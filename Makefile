build:
	gcc -o exec_name gpioTest.c -Wall -pthread -lpigpio -lrt
run:
	sudo ./exec_name
