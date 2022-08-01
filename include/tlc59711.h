#include "common.h"
#ifndef TLC59711_H
#define TLC59711_H
#include "spi.h"

void tlc59711_create(const uint16_t data[12], uint8_t output[28]);
int tlc59711_init(const char *device);

#endif