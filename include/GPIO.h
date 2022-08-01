#ifndef GPIO_H
#define GPIO_H

#define IN 0
#define OUT 1

int GPIORead(int pin);
void GPIOPinmode(int pin, int dir);
int GPIOInit();
int unmapG();

#endif