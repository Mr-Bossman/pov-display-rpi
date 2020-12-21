#include "common.h"
#include "spi.h"
#include "tlc59711.h"
#include "GPIO.h"


int main(int argc, char *argv[])
{
    GPIOInit(1,IN);

    uint16_t test[12] = {0};
    tlc59711_init("/dev/spidev0.1");
    while(1){
        /*int ret;
        if((ret = tlc59711_send(test)) < 0 ){
            exit(ret);
        }*/
        printf("%d\n",GPIORead(1));
    }
	return 0;
}
