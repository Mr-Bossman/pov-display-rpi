#include "spi.h"
#include "common.h"
#define chips 6
#define degreesIn 360

spiSend
readPin
getTime
void tlc59711(const uint32_t& command, const uint16_t data[12]);

uint32_t CommandInit();

void lines(const uint32_t& command,uint16_t data[12*chips]);

void getDelay();

int main(int argc, char *argv[]){
    uint32_t command = CommandInit();
    uint16_t pwmbuffer[12*chips];
    int delay = 0, last = 0;
    went_back = true;
    while(1){ // main loop
            for(int deg = 0; deg < degreesIn; deg++){ // go thorugh degrees
                while(last+((delay)*deg) > getTime()){ // sleep between lines
                    if(readPin())
                        went_back = true; // we can now wait for the next edge
                    if(!readPin() && went_back) // we are still in the loop but we need to exit
                        goto end;
                } // sleep between lines
                for(uint8_t i=0; i<72; i++) {
                    pwmbuffer[i]= lester[deg][i];
                }
                lines(command,pwmbuffer); 
            }
            end:
            while(readPin()); // wait till it goes low if we exited the loop early
            getDelay(delay , last);
            went_back = false; //make shure we trigger on the rising edge
        }
    }
}
void getDelay(int &delay,int &last){
    uint64_t tmp = last;
    last = getTime()
    delay = (tmp - last)/degreesIn;
}
void tlc59711(const uint32_t& command, const uint16_t data[12]){

    spiSend(commands >> 24);
    spiSend(commands >> 16);
    spiSend(commands >> 8);
    spiSend(commands);

    // 12 channels per TLC59711
    for (int8_t c = 11; c >= 0; c--) {
        // 16 bits per channel, send MSB first
        spiSend(pwmbuffer[c] >> 8);
        spiSend(pwmbuffer[c]);
    }
}


uint32_t CommandInit(){
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
    return command;
}
void lines(const uint32_t& command,uint16_t data[chips][12]){
    for(int i = 0; i < chips;i++)
        tlc59711(data[i]);
}
