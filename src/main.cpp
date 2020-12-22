#include "common.h"
#include <iostream> // std::cout
#include <thread>   // std::thread
#include "display.h"
#include "render.h"

int main(int argc, char *argv[])
{
    if(argc != 4){
        printf("./exe (pathto video file) (fps) (fit type)\n \"(\" isnt included (fps) and (fit type) is an int");
        exit(-1);
    }
    bool go = true;
    bool swap = false;
    uint16_t buffer[3][degreesIn][rings] = {0};

    std::thread dis(display, &go, buffer, &swap);
    std::thread ren(render16, argv[1], &go, buffer, std::stoi(argv[2]), &swap,(std::stoi(argv[3]))?true:false);
    while (1)
    {
    }
}
