#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

extern int GPIORead(int pin);
extern int GPIOWrite(int pin, int value);
extern int GPIOClose(int pin);
extern int GPIOInit(int pin,int dir);