#include "common.h"
#include "spi.h"
#include "tlc59711.h"

int main(int argc, char *argv[])
{
    uint16_t test[12] = {0};
    tlc59711_init("/dev/spidev1.1");
    while(1){
        int ret;
        if((ret = tlc59711_send(test)) < 0 ){
            exit(ret);
        }
    }
	return 0;
}
