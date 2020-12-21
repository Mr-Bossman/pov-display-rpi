#include "tlc59711.h"
static uint8_t command[4];
static int fd = 0;

static void CommandInit(uint8_t _command[4]);

extern int tlc59711_init(const char *device)
{
    CommandInit(command); // need to set modes
    spiMode modes = {
        .mode = 0,
        .bits = 8,
        .speed = 20000000,
        .delay = 0};

    int fd = spi_init(&modes, device);
    return fd;
} 
extern int tlc59711_send(const uint16_t data[12])
{
    uint8_t rx[24]; // throw away
   	uint8_t tx[24];

    int ret;
    if((ret = transfer(command,rx,4)) < 0){
        return ret;
    }


    // 12 channels per TLC59711
    for (int8_t c = 11; c >= 0; c--) {

        // 16 bits per channel, send MSB first
        tx[c << 1] = data[c] >> 8;
        tx[(c<< 1)+1] = data[c];
    }
    return transfer(tx,rx,24);
}


static void CommandInit(uint8_t _command[4])
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
    _command[0] = commands >> 24;
    _command[1] = commands >> 16;
    _command[2] = commands >> 8;
    _command[3] = commands;
}
