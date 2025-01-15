#define _GNU_SOURCE
#include "common.h"
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <gpiod.h>
#include <unistd.h>
#include "display.h"
#include "gpio_ctl.h"
#include "spi.h"
#include "tlc59711.h"


static int disp_line(const uint16_t line[CHIPS * 12]);

static int setupIO(struct gpiod_line_request **request,
		   struct gpiod_edge_event_buffer **event_buffer);

static void freeIO(struct gpiod_line_request *request,
		   struct gpiod_edge_event_buffer *event_buffer);

static uint64_t getDelay(struct gpiod_edge_event *event, uint64_t *last);

static int display_lines(bool *go, struct gpiod_line_request * request, uint64_t delay,
	const uint16_t polar_frames[DEGREESIN][CHIPS * 12]);

static uint64_t nanos() {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return ((uint64_t)now.tv_sec) * 1000000000 + ((uint64_t)now.tv_nsec);
}

static inline void set_affinity(void) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset); //clears the cpuset
	CPU_SET(0, &cpuset); //set CPU 0 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
	setpriority(PRIO_PROCESS, 0, 20);
}

struct data{
	const uint16_t (*lines)[72];
	int ind;
} val;

static void* do_line(void * none){
	(void)none;
	while(1) {
		int tmp = val.ind;
		while(tmp == val.ind) usleep(1);
		tmp = val.ind;
		disp_line(val.lines[tmp]);
	}
	return NULL;
}
void display(bool *go, const uint16_t polar_frames[3][DEGREESIN][CHIPS * 12], bool *swap) {

	uint64_t delay = 1, last = nanos();
	int p = 0, err, deg;
	struct gpiod_edge_event *event;
	struct gpiod_edge_event_buffer *event_buffer;
	struct gpiod_line_request *request;

	set_affinity();
	err = setupIO(&request, &event_buffer);
	if (err == -1)
		return;

	val.ind = 0;
	pthread_t pid;
	pthread_create(&pid, NULL, &do_line, NULL);

	while (*go) { // main loop
		val.lines = &*(polar_frames[p]);

		deg = display_lines(go, request, delay, polar_frames[p]);
		// Error goto FreeIO
		if(deg == -1)
			break;
		event = read_event(request, event_buffer);
		if(!event) {
			fprintf(stderr, "Failed to read event: %s\n",
				strerror(errno));
			// Error goto FreeIO
			break;
		}

		delay = getDelay(event, &last);
		if (!(*swap)) {
			if (p == 2)
				p = 0;
			else
				p++;
			*swap = true;
		}
	}
	freeIO(request, event_buffer);
	*go = false;
}

static uint64_t getDelay(struct gpiod_edge_event *event, uint64_t *last) {
	uint64_t tmp = *last;
	*last = gpiod_edge_event_get_timestamp_ns(event);
	return (*last - tmp) / DEGREESIN;
}

static int display_lines(bool *go, struct gpiod_line_request * request, uint64_t delay,
	const uint16_t polar_frames[DEGREESIN][CHIPS * 12]) {
	int deg, edge;
	uint64_t start = nanos(),ndelay;
	for (deg = 0; deg < DEGREESIN; deg++) { // go thorugh degrees
		// sleep between lines
		ndelay = (nanos()-start);
                ndelay = (delay*(deg+1) < ndelay)? 0 : (delay*(deg+1) - ndelay);
		edge = gpiod_line_request_wait_edge_events(request, ndelay);
		if (edge == -1) {
			fprintf(stderr, "gpiod_line_request_wait_edge_events error: %s\n",
				strerror(errno));
			return -1;
		}
		val.ind = deg;
		// Event is pending
		if (edge == 1)
			return deg;
		// Need to exit
		if(!*go)
			return -1;

	}
	while (1) {
		edge = gpiod_line_request_wait_edge_events(request, 1);
		if (edge == -1) {
			fprintf(stderr, "gpiod_line_request_wait_edge_events error: %s\n",
				strerror(errno));
			return -1;
		}
		// Event is pending
		if (edge == 1)
				return deg;
		// Need to exit
		if(!*go)
			return -1;
	}
	return deg;
}

static int disp_line(const uint16_t line[CHIPS * 12]) {
	uint8_t data[CHIPS * 28];
	uint16_t tmp[12];
	for (int i = 0; i < CHIPS; i++) {
		for (int b = 0; b < 12; b++)
			tmp[11 - b] = line[(i * 12) + b];
		tlc59711_create(tmp, data + (28 * i));
	}
	return transfer(data, NULL, CHIPS * 28);
}

static int setupIO(struct gpiod_line_request **request,
		   struct gpiod_edge_event_buffer **event_buffer) {
	int err;
	static const int event_buf_size = 1;
	static const int pin = 0;
	struct gpiod_line_settings *settings;

	err = tlc59711_init("/dev/spidev0.1");
	if(err == -1)
		return err;

	settings = gpiod_line_settings_new();
	if (!settings)
		goto close_spi;

	err |= gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
	err |= gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_FALLING);
	err |= gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
//	err |= gpiod_line_settings_set_event_clock(settings, GPIOD_LINE_CLOCK_REALTIME);
//	gpiod_line_settings_set_debounce_period_us(settings, 1);

	if(err == -1) {
		fprintf(stderr, "failed to set gpio settings: %s\n",
			strerror(errno));
		gpiod_line_settings_free(settings);
		goto close_spi;
	}

	*request = request_input_line("/dev/gpiochip0", pin,
				     "async-watch-line-value",
				     settings);
	if (!*request) {
		fprintf(stderr, "failed to request line: %s\n",
			strerror(errno));
		goto close_spi;
	}

	*event_buffer = gpiod_edge_event_buffer_new(event_buf_size);
	if (!*event_buffer) {
		fprintf(stderr, "failed to create event buffer: %s\n",
			strerror(errno));
		goto free_request;
	}

	return 0;

	free_request:
		gpiod_line_request_release(*request);
		*request = NULL;
	close_spi:
		spi_deinit();
	return -1;
}

static void freeIO(struct gpiod_line_request *request,
		   struct gpiod_edge_event_buffer *event_buffer) {
	gpiod_edge_event_buffer_free(event_buffer);
	gpiod_line_request_release(request);
	request = NULL;
	spi_deinit();
}
