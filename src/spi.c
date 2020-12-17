/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

typedef struct spiMode{
	uint8_t mode;
 	uint8_t bits;
 	uint32_t speed;
	uint16_t delay;
}spiMode;
static spiMode _mode;


extern int transfer(int fd,uint8_t tx[], uint8_t rx[],size_t size)
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = size,
		.delay_usecs = _mode.delay,
		.speed_hz =  _mode.speed,
		.bits_per_word =  _mode.bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		printf("can't send spi message");
	return ret;	
}
extern int spi_init(spiMode* modes,const char *device ){
	_mode = *modes;
	int fd , ret;
	fd = open(device, O_RDWR);
	if (fd < 0) {
		printf("could not open %s\n",device);
		return fd;
	}
	ret = ioctl(fd, SPI_IOC_WR_MODE, &(modes->mode));
	if (ret == -1) {
		printf("can't set spi mode\n");
		return ret;
	}
	ret = ioctl(fd, SPI_IOC_RD_MODE, &(modes->mode));
	if (ret == -1) {
		printf("can't get spi mode\n");
		return ret;
	}
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &(modes->bits));
	if (ret == -1) {
		printf("can't set bits per word\n");
		return ret;
	}
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &(modes->bits));
	if (ret == -1) {
		printf("can't get bits per word\n");
		return ret;
	}
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &(modes->speed));
	if (ret == -1) {
		printf("can't set max speed hz\n");
		return ret;
	}
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &(modes->speed));
	if (ret == -1) {
		printf("can't get max speed hz\n");
		return ret;
	}
	printf("spi mode: %d\n", modes->mode);
	printf("bits per word: %d\n", modes->bits);
	printf("max speed: %d Hz (%d KHz)\n", modes->speed, modes->speed/1000);
}
void spi_deinit(int fd){
		close(fd);
}

int main(int argc, char *argv[])
{

	spiMode modes = {
 		.mode= 0,
 		.bits = 8,
 		.speed = 500000,
		.delay = 0
	};
	uint8_t tx[12] = {1,2,3,4,5,6,7,8,9,0};
	uint8_t rx[12];
	int fd = spi_init(&modes,"/dev/spidev1.1");
	if(fd < 0)exit(fd);

	transfer(fd,tx,rx,12);

	close(fd);

	return 0;
}
