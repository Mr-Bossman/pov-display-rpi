#include "common.h"
#include <fcntl.h>
#include <sys/mman.h>

#define BCM2708_PERI_BASE        0xFE000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define BLOCK_SIZE (4*1024)

static int  mem_fd;
static void *gpio_map;

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

int GPIORead(int pin){
	return GET_GPIO(pin);
}

void GPIOPinmode(int pin,int dir){
	INP_GPIO(pin);
	if(dir)
		OUT_GPIO(pin);
}

int unmapG(){
	return munmap(gpio_map, BLOCK_SIZE);
}

int GPIOInit()
{
	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		printf("can't open /dev/mem \n");
		return -1;
	}

	/* mmap GPIO */
	gpio_map = mmap(
		NULL,             //Any adddress in our space will do
		BLOCK_SIZE,       //Map length
		PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
		MAP_SHARED,       //Shared with other processes
		mem_fd,           //File to map
		GPIO_BASE         //Offset to GPIO peripheral
	);
	close(mem_fd); //No need to keep mem_fd open after mmap

	if (gpio_map == MAP_FAILED) {
		printf("mmap error %lu\n", (uint64_t)gpio_map);//errno also set!
		return -1;
	}

	// Always use volatile pointer!
	gpio = (volatile unsigned *)gpio_map;
	return 0;
}
