#include "tlc59711.h"
static uint32_t command = 0;
static int fd = 0;

static uint32_t CommandInit();

extern int tlc59711_init(const char *device)
{
    command = CommandInit(); // need to set modes
    spiMode modes = {
        .mode = 0,
        .bits = 8,
        .speed = 500000,
        .delay = 0};

    int fd = spi_init(&modes, device);
    return fd;
} 
extern void tlc59711_send(const uint16_t data[12])
{
    uint8_t rx[24]; // throw away
   	uint8_t tx[24];

    tx[0] = command >> 24;
    tx[1] = command >> 16;
    tx[2] = command >> 8;
    tx[3] = command;
    transfer(tx,rx,4);


    // 12 channels per TLC59711
    for (int8_t c = 11; c >= 0; c--) {

        // 16 bits per channel, send MSB first
        //spiSend(data[c] >> 8);
        //spiSend(data[c]);
        tx[c << 1] = 0xFF;
    }
    transfer(tx,rx,24);
}


static uint32_t CommandInit()
{
    uint32_t commands = 0;
    commands = 0x25;
    commands <<= 5;
    commands |= 0x16;
    commands <<= 7;
    commands |= 0x7F;
    commands <<= 7;
    commands |= 0x7F;
    commands <<= 7;
    commands |= 0x7F;
    return commands;
}
