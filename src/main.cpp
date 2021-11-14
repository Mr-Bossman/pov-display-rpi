#include "common.h"
#include <iostream> // std::cout
#include <thread>   // std::thread
#include <signal.h>
#include "display.h"
#include "GPIO.h"
#include "render.h"
#include "spi.h"

static bool go = true;
void sigm(int sig){
    go = false;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigm);
    if(argc != 4){
        printf("./exe (pathto video file) (fps) (fit type)\n \"(\" isnt included (fps) and (fit type) is an int");
        exit(-1);
    }
    bool swap = false;
    uint16_t buffer[3][degreesIn][rings] = {0};

    std::thread dis(display, &go, buffer, &swap);
    render16(argv[1], &go, buffer, std::stoi(argv[2]), &swap,(std::stoi(argv[3]))?true:false);
    dis.join();
    unmapG();
    spi_deinit();
    return 0;
}
