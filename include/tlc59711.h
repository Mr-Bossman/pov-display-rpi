#include "common.h"
#ifndef TLC59711_H
#define TLC59711_H
#include "spi.h"
extern void tlc59711_send(const uint16_t data[12]);
extern int tlc59711_init(const char *device);
#endif