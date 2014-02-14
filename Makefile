#CC=arm-linux-gnueabi-gcc
CC=gcc
CFLAGS=-c -Wall -Wextra -pedantic -Wunused -Wdeprecated --std=gnu99
LDFLAGS=-lrt

SOURCES= time.c
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=time

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
