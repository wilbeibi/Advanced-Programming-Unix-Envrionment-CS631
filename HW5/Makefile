CC=gcc
CFLAGS=-g -std=gnu99 -Wall -pedantic-errors
OBJS = aed.o
LIBS = -lcrypto

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
aed: $(OBJS)
	$(CC) -o $@ $< $(LIBS) -Wall

clean:
	rm -rf *.o aed
