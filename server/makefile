CC=gcc
CFLAGS=-g -Wall
TARGET=server
OBJS=server.o
 
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

server.o : server.c
	$(CC) -c -o server.o server.c

clean:
	rm $(OBJECT) $(TARGET)