#include "common.h"
#include "spi.h"
#include "tlc59711.h"
#include "GPIO.h"
#include "display.h"
#include <time.h>

inline static int readPin()
{
    return GPIORead(0);
}
static uint64_t nanos()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((uint64_t)now.tv_sec) * 1000000000 + ((uint64_t)now.tv_nsec);
}

static uint16_t pwmbuffer[chips][12];

static void lines(uint16_t line[chips*12]);

static void getDelay(uint64_t *delay, uint64_t *last);

extern void display(bool *go, const uint16_t lester[3][degreesIn][chips * 12], bool *swap)
{
    GPIOInit();
    GPIOPinmode(0, IN);
    tlc59711_init("/dev/spidev0.1");
    uint64_t delay = 0, last = 0;
    bool went_back = true;
    int p = 0;
    while (*go)
    { // main loop
        for (int deg = 0; deg < degreesIn; deg++)
        { // go thorugh degrees
            while (last + ((delay)*deg) > nanos())
            { // sleep between lines
                if (readPin())
                    went_back = true;        // we can now wait for the next edge
                if (!readPin() && went_back) // we are still in the loop but we need to exit
                    goto end;
            } // sleep between lines

            lines(lester[p][deg]);
        }
    end:
        while (readPin())
            ; // wait till it goes low if we exited the loop early
        getDelay(&delay, &last);
        went_back = false; //make shure we trigger on the rising edge
        printf("%llu\n", delay);
        if (p == 2)
            p = 0;
        else
            p++;
        *swap = true;
    }
}
static void getDelay(uint64_t *delay, uint64_t *last)
{
    uint64_t tmp = *last;
    *last = nanos();
    *delay = (*last - tmp) / degreesIn;
}
static void lines(uint16_t line[chips*12])
{
    uint8_t data[chips*28];
    uint8_t tmp[12];
    for (int i = 0; i < chips; i++){
                for(int b = 0; b < 12;b++)
                tmp[11 - b] = line[i];

        tlc59711_create(tmp, data + (28 * i));
    }
    transfer(data, NULL, chips * 28);
}
