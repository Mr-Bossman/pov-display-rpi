#include "common.h"
#ifndef SPI_H
#define SPI_H
typedef struct spiMode
{
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;
} spiMode;
extern int spi_init(spiMode *modes, const char *device);
extern void spi_deinit();
extern int transfer(uint8_t tx[], uint8_t rx[], unsigned int size);
#endif