#define chips 2
#define degreesIn 360

#include "common.h"
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include "display.h"
#include "render.h"


int main(int argc, char *argv[])
{
    bool go = true;
    bool swap = false;
    uint16_t buffer[3][degreesIn][rings] = {0};

    std::thread dis(display,&go,buffer,&swap);
    std::thread ren(render16,argv[1],&go,buffer,std::stoi(argv[2]),&swap);
    while(1){}
}
