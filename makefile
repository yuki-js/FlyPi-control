# Referred http://www.ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html

# Program filename and object files
PROGRAM = flypi
OBJS = main.o sensor.o control.o
LIBS = -lpigpio -lrt  -lm

CC = gcc
CFLAGS = -Wall -O1

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $^

main.o: src/main.c
	$(CC) $(CFLAGS) -c $<

sensor.o: src/sensor.c
	$(CC) $(CFLAGS) -c $<

control.o: src/control.c
	$(CC) $(CFLAGS) -c $<

main.c : src/config.h
main.c : src/packetBits.h


.PHONY: clean
clean:
	rm -rf $(OBJS) $(PROGRAM)
