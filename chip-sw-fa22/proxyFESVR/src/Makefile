# By Hongyi (Franklin) Huang

CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Wshadow -Wformat-nonliteral -Wformat-security -Wtype-limits -g
SRC_DIR = -I../rs232
# -O2
objects = Rs232.o uartLoopback.o tsi.o fesvr.o main.o

all: fesvr uartLoopback

fesvr : Rs232.o fesvr.o tsi.o main.o
	$(CC) $(SRC_DIR) Rs232.o tsi.o fesvr.o main.o -o fesvr

uartLoopback : Rs232.o uartLoopback.o
	$(CC) $(SRC_DIR) Rs232.o uartLoopback.o -o uartLoopback

main.o : tsi.o fesvr.o
	$(CC) $(CFLAGS) $(SRC_DIR) -c main.cpp -o main.o

fesvr.o : fesvr.cpp tsi.o
	$(CC) $(CFLAGS) $(SRC_DIR) -c fesvr.cpp -o fesvr.o

tsi.o : tsi.cpp Rs232.o
	$(CC) $(CFLAGS) $(SRC_DIR) -c tsi.cpp -o tsi.o

uartLoopback.o : uartLoopback.cpp ../rs232/Rs232.o
	$(CC) $(CFLAGS) $(SRC_DIR) -c uartLoopback.cpp -o uartLoopback.o

Rs232.o : ../rs232/Rs232.o ../rs232/Rs232.cpp
	$(CC) $(CFLAGS) $(SRC_DIR) -c ../rs232/Rs232.cpp -o Rs232.o

clean :
	$(RM) fesvr uartLoopback $(objects)