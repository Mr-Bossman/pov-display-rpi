#ifndef GPIO_H
#define GPIO_H
#include <gpiod.h>

struct gpiod_edge_event * read_event(struct gpiod_line_request *request,
				     struct gpiod_edge_event_buffer *event_buffer);

struct gpiod_line_request *request_input_line(const char *chip_path,
						     unsigned int offset,
						     const char *consumer,
						     struct gpiod_line_settings *settings);
#endif
