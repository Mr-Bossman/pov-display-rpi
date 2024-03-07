#include "common.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "spi.h"

static spiMode _mode;
static int fd;

void spi_deinit() {
	close(fd);
	fd = -1;
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
	if (ret == -1)
		fprintf(stderr, "Can't send spi message: %s.\n",
			strerror(errno));
	return ret;
}

int spi_init(spiMode *modes, const char *device) {
	_mode = *modes;
	int ret;
	fd = open(device, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "could not open: %s, error: %s\n", device,
			strerror(errno));
		return fd;
	}
	ret = ioctl(fd, SPI_IOC_WR_MODE, &(modes->mode));
	if (ret == -1) {
		fprintf(stderr, "Can't set spi mode: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	ret = ioctl(fd, SPI_IOC_RD_MODE, &(modes->mode));
	if (ret == -1) {
		fprintf(stderr, "Can't get spi mode: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &(modes->bits));
	if (ret == -1) {
		fprintf(stderr, "Can't set bits per word: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &(modes->bits));
	if (ret == -1) {
		fprintf(stderr, "Can't get bits per word: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &(modes->speed));
	if (ret == -1) {
		fprintf(stderr, "Can't set max speed HZ: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &(modes->speed));
	if (ret == -1) {
		fprintf(stderr, "Can't get max speed HZ: %s.\n",
			strerror(errno));
		goto close_spi;
	}
	printf("spi mode: %d\n", modes->mode);
	printf("bits per word: %d\n", modes->bits);
	printf("max speed: %d Hz (%d KHz)\n", modes->speed, modes->speed / 1000);
	return fd;

	close_spi:
		spi_deinit();
	return -1;
}
