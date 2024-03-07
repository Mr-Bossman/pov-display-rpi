#ifndef SPI_H
#define SPI_H
#include "common.h"

typedef struct spiMode
{
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;
} spiMode;

int spi_init(spiMode *modes, const char *device);
void spi_deinit();
int transfer(uint8_t tx[], uint8_t rx[], unsigned int size);

#endif