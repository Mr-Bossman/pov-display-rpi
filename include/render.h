#ifndef RENDER_H
#define RENDER_H
#include "common.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
#ifndef chips
    #define chips 2
    #define degreesIn 360
#endif
#define rings chips*12

#ifndef fits
    #define fits true
#endif

extern int render(char *argv,bool *go,cv::Vec3b* buffer[degreesIn][rings]);
extern int render16(char *argv,bool *go,uint16_t buf[degreesIn][rings]);
#endif