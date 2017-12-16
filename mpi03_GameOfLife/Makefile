TARGET = GameOfLife
LIBS =
CC = mpiicc
CFLAGS = -g -Wall -std=c99

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET):
	$(CC) $(CFLAGS) $(LIBS) -o $@ $@_Task3.c RLE_Parsing.c

run:
	ccsalloc -I --res=rset=1:ncpus=8:mpiprocs=8 impi -- ./GameOfLife TestFile.rle 10 1
clean:
	-rm -f *.o *.ppm *.err *.out
	-rm -f $(TARGET)

