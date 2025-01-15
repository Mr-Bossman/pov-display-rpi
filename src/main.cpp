#include "common.h"
#include <iostream>
#include <sys/resource.h>
#include <thread>
#include <signal.h>
#include "render.h"

extern "C" {
#include "display.h"
}

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
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset); //clears the cpuset
	CPU_SET(1, &cpuset); //set CPU 0 on cpuset
	CPU_SET(2, &cpuset); //set CPU 0 on cpuset
	CPU_SET(3, &cpuset); //set CPU 0 on cpuset
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
	setpriority(PRIO_PROCESS, 0, 20);
	render16(argv[1], &go, buffer, std::stoi(argv[2]), &swap,(std::stoi(argv[3]))?true:false);
	go = false;
#ifndef DESKTOP_TEST
	dis.join();
#endif
	std::cout << std::endl;
	return 0;
}
