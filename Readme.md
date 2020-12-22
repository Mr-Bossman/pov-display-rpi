./make.sh
./main
git clean -f -d 

g++ -o main tlc59711.o spi.o main.o
g++ -o spi.o  src/spi.c -Iinclude -c
g++ -o tlc59711.o  src/tlc59711.c -Iinclude -c

g++ -o main.o  src/main.cpp -Iinclude -c 
g++ src/spi.c src/main.cpp src/tlc59711.c src/GPIO.c src/display.c -lpthread -Wl,--no-as-needed -Wall -std=c++11 -Iinclude -o main -O3