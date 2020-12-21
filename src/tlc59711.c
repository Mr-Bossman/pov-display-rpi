#include "tlc59711.h"
static uint8_t command[4];
static void CommandInit(uint8_t _command[4]);

extern int tlc59711_init(const char *device)
{
    CommandInit(command); // need to set modes
    spiMode modes = {
        .mode = 0,
        .bits = 8,
        .speed = 12000000,
        .delay = 0};

    int fd = spi_init(&modes, device);
    return fd;
} 

extern void tlc59711_create(const uint16_t data[12],uint8_t output[28])
{
    memcpy(output,command,4);
    // 12 channels per TLC59711
    for (int8_t c = 11; c >= 0; c--) {

        // 16 bits per channel, send MSB first
        output[(c << 1)+4] = data[c] >> 8;
        output[((c<< 1)+1)+4] = data[c];
    }
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
