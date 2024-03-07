BUILD_DIR = out
CXX = g++
CC = gcc
CXXFLAGS = $(shell pkg-config --cflags opencv4) -fopenmp
CFLAGS = $(shell pkg-config --cflags libgpiod)
LIBS = $(shell pkg-config --libs opencv4 libgpiod) -fopenmp

C_SOURCES = src/display.c src/gpio_ctl.c src/spi.c src/tlc59711.c
CXX_SOURCES = src/main.cpp src/render.cpp
C_INCLUDES = -Iinclude

CXXFLAGS += $(C_INCLUDES) -fopenmp -g -O3 -std=c++11 -Wall -Wextra -pedantic
CFLAGS += $(C_INCLUDES) -fopenmp -g -O3 -std=c11 -Wall -Wextra -pedantic

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

all: pov

run: all
	./pov test.mp4 30 0

clean:
	rm -f out/*

$(BUILD_DIR)/%.o: %.c
	${CC} -g -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	${CXX} -g -c $(CXXFLAGS) $< -o $@

pov: $(OBJECTS)
	${CXX} -g -o pov $(OBJECTS) $(LIBS)
