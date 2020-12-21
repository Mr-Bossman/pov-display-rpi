#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "common.h"

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

static int fd;

static int GPIOExport(int pin)
{
    #define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

    #define DIRECTION_MAX 35
	char path[DIRECTION_MAX];

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	return(0);
}

extern int GPIORead(int pin)
{
    #define VALUE_MAX 30
	char path[VALUE_MAX];

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDWR);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-3);
	}
    

    
    char value_str[3];
	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}
    	close(fd);


	return(atoi(value_str));
}
extern int GPIOWrite(int pin, int value)
{
    #define VALUE_MAX 30
	char path[VALUE_MAX];

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDWR);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-3);
	}
    


	static const char s_values_str[] = "01";

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}
	return(0);
}
extern int GPIOClose(int pin)
{

	close(fd);
    return GPIOUnexport(pin);
}
extern int GPIOInit(int pin,int dir)
{
    if (-1 == GPIOExport(pin))
	    return(-1);
    if (-1 == GPIODirection(pin, dir) || -1 == GPIODirection(pin, dir))
	   	return(-2);
    return 0;
}