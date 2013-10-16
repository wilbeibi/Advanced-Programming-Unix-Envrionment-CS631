TARGET = tcpm
CC = gcc
CFLAGS = -g -Wall -pedantic-errors
OBJECTS = tcpm.o

default: $(TARGET)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@ -lbsd

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ -lbsd

clean:
	rm -f *.o
	rm -f $(TARGET)
