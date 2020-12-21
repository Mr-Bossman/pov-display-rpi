./make.sh
./main
git clean -f -d 

g++ -o main tlc59711.o spi.o main.o
g++ -o spi.o  src/spi.c -Iinclude -c
g++ -o tlc59711.o  src/tlc59711.c -Iinclude -c

g++ -o main.o  src/main.cpp -Iinclude -c
