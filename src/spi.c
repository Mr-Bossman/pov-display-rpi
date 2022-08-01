#include "spi.h"
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static spiMode _mode;
static int fd;

void spi_deinit() {
	close(fd);
}

int transfer(uint8_t tx[], uint8_t rx[], unsigned int size) {
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = size,
		.speed_hz = _mode.speed,
		.delay_usecs = _mode.delay,
		.bits_per_word = _mode.bits};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		printf("can't send spi message\n");
	return ret;
}

int spi_init(spiMode *modes, const char *device) {
	_mode = *modes;
	int ret;
	fd = open(device, O_RDWR);
	if (fd < 0) {
		printf("could not open %s\n", device);
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
	printf("max speed: %d Hz (%d KHz)\n", modes->speed, modes->speed / 1000);
	return fd;
}
