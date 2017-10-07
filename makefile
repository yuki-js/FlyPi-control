# Referred http://www.ie.u-ryukyu.ac.jp/~e085739/c.makefile.tuts.html

# Program filename and object files
PROGRAM = flypi
OBJS = main.o sensor.o control.o socket.o config.o
LIBS = -lpigpio -lrt -lm

CC = gcc
CFLAGS = -Wall -O1 

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) -o $(PROGRAM) $^

main.o: src/main.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

sensor.o: src/sensor.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

control.o: src/control.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

socket.o: src/socket.c
	$(CC) $(CFLAGS) $(LIBS) -c $<

config.o: src/config.c
	$(CC) $(CFLAGS) $(LIBS) -c $<


config.c : src/config.h
main.c : src/packetBits.h


.PHONY: clean
clean:
	rm -rf $(OBJS) $(PROGRAM)
