CC=gcc
CFLAGS=-g -Wall
TARGET=client
OBJS=client.o
 
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

client.o : client.c
	$(CC) -c -o client.o client.c

clean:
	rm $(OBJECT) $(TARGET)