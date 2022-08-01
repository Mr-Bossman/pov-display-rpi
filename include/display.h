#ifndef DISPLAY_H
#define DISPLAY_H
#include "common.h"
#ifndef CHIPS
#define CHIPS 2
#define DEGREESIN 360
#endif

void display(bool *go, const uint16_t lester[3][DEGREESIN][CHIPS * 12], bool *swap);

#endif