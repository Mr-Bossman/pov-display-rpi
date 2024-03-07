// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Kent Gibson <warthog618@gmail.com>

#include "common.h"
#include <gpiod.h>
#include "gpio_ctl.h"

struct gpiod_line_request *request_input_line(const char *chip_path,
						     unsigned int offset,
						     const char *consumer,
						     struct gpiod_line_settings *settings) {
	struct gpiod_line_request *request = NULL;
	struct gpiod_request_config *req_cfg = NULL;
	struct gpiod_line_config *line_cfg;
	struct gpiod_chip *chip;
	int ret;

	if (!settings)
		settings = gpiod_line_settings_new();

	chip = gpiod_chip_open(chip_path);
	if (!chip)
		goto free_settings;

	line_cfg = gpiod_line_config_new();
	if (!line_cfg)
		goto close_chip;

	ret = gpiod_line_config_add_line_settings(line_cfg, &offset, 1,
						  settings);
	if (ret)
		goto free_line_config;

	if (consumer) {
		req_cfg = gpiod_request_config_new();
		if (!req_cfg)
			goto free_line_config;

		gpiod_request_config_set_consumer(req_cfg, consumer);
	}

	request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

	gpiod_request_config_free(req_cfg);

free_line_config:
	gpiod_line_config_free(line_cfg);

close_chip:
	gpiod_chip_close(chip);

free_settings:
	gpiod_line_settings_free(settings);
	settings = NULL;
	return request;
}

struct gpiod_edge_event * read_event(struct gpiod_line_request *request,
				     struct gpiod_edge_event_buffer *event_buffer) {
	int err;

	err = gpiod_line_request_read_edge_events(request, event_buffer, 1);
	if (err == -1) {
		fprintf(stderr, "error reading edge events: %s\n",
			strerror(errno));
		return NULL;
	}
	return gpiod_edge_event_buffer_get_event(event_buffer, 0);
}
