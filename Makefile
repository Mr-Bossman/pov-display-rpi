IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR=lib
SRCDIR=src
LIBS=

_DEPS = spi.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ =  spi.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(OBJ): $(SRCDIR)/*.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
