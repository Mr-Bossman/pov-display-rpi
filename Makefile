BUILD_DIR = out
CXX = g++
CC = g++
CFLAGS = $(shell pkg-config --cflags opencv4) -fopenmp
LIBS = $(shell pkg-config --libs opencv4) -fopenmp

C_SOURCES = src/display.c src/GPIO.c src/spi.c src/tlc59711.c
CXX_SOURCES = src/main.cpp src/render.cpp
C_INCLUDES = -Iinclude

CFLAGS += $(C_INCLUDES) -O3
CXXFLAGS = $(CFLAGS) -std=c++11

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

all: clean pov

run: all
	./pov test.mp4 30 0

clean:
	rm -f pov
clean-all: clean
	rm -f out/*

$(BUILD_DIR)/%.o: %.c
	${CC} -g -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	${CXX} -g -c $(CXXFLAGS) $< -o $@

pov: $(OBJECTS)
	${CXX} -g -o pov $(OBJECTS) $(LIBS)