# By Hongyi (Franklin) Huang

CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits -O2
SRC_DIR = -I../rs232

objects = Rs232.o uartDriver.o uartLoopback.o

all: fesvr

fesvr : Rs232.o uartDrifesvrver.o
	$(CC) $(SRC_DIR) Rs232.o fesvr.o -o fesvr

uartLoopback : Rs232.o uartLoopback.o
	$(CC) $(SRC_DIR) Rs232.o uartLoopback.o -o uartLoopback

fesvr.o : fesvr.cpp ../rs232/Rs232.h
	$(CC) $(CFLAGS) $(SRC_DIR) -c fesvr.cpp -o fesvr.o

uartLoopback.o : uartLoopback.cpp ../rs232/Rs232.h
	$(CC) $(CFLAGS) $(SRC_DIR) -c uartLoopback.cpp -o uartLoopback.o

Rs232.o : ../rs232/Rs232.h ../rs232/Rs232.cpp
	$(CC) $(CFLAGS) $(SRC_DIR) -c ../rs232/Rs232.cpp -o Rs232.o

clean :
	$(RM) fesvr uartLoopback $(objects)