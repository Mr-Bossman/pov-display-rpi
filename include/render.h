#ifndef RENDER_H
#define RENDER_H
#include "common.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
#ifndef CHIPS
#define CHIPS 2
#define DEGREESIN 360
#endif
#define RINGS CHIPS * 12

int render(char *argv, bool *go, cv::Vec3b *buffer[DEGREESIN][RINGS], bool fits);
int render16(char *argv, bool *go, uint16_t buf[3][DEGREESIN][RINGS], uint64_t fps, bool *swap, bool fits);

#endif