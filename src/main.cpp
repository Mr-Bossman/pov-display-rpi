#include "common.h"
#include <iostream>
#include <thread>
#include <signal.h>
#include "display.h"
#include "GPIO.h"
#include "render.h"
#include "spi.h"

static bool go = true;

void sigm(int sig) {
	(void) sig;
	go = false;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sigm);
	if(argc < 1) {
		printf("argv must have program name!\n");
		exit(-1);  
	}
	if(argc != 4){
		printf("%s (path to video file) (fps) (fit type)\n",argv[0]);
		exit(-1);
	}
	bool swap = false;
	uint16_t buffer[3][DEGREESIN][RINGS] = {0};

#ifndef DESKTOP_TEST
	std::thread dis(display, &go, buffer, &swap);
#endif
	render16(argv[1], &go, buffer, std::stoi(argv[2]), &swap,(std::stoi(argv[3]))?true:false);
#ifndef DESKTOP_TEST
	dis.join();
#endif
	std::cout << std::endl;
	return 0;
}
