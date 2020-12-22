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
    uint64_t delay =0;
    uint16_t buffer[degreesIn][rings] = {0};
    std::thread dis(display,&go,buffer,&delay);
    std::thread ren(render16,argv[1],&go,buffer,std::stoi(argv[2]),&delay);
    while(1){}
}
