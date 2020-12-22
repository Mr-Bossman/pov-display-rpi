#ifndef GPIO_H
#define GPIO_H

#define IN 0
#define OUT 1

extern int GPIORead(int pin);
extern void GPIOPinmode(int pin, int dir);
extern int GPIOInit();

#endif