#ifndef DISPLAY_H
#define DISPLAY_H
#include "common.h"
#ifndef chips
#define chips 2
#define degreesIn 360
#endif
extern void display(bool *go, const uint16_t lester[3][degreesIn][chips * 12], bool *swap);
#endif