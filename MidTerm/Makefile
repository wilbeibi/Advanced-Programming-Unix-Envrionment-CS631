# -Wall in CFLAGS
CFLAGS=-g -std=c99 -D_BSD_SOURCE -Wall -pedantic-errors 
ls: ls.o print.o cmp.o
	gcc -o ls ls.o print.o cmp.o -lbsd
ls.o: ls.c ls.h cmp.h
	gcc -c ${CFLAGS} ls.c
print.o: print.c print.h ls.h
	gcc -c ${CFLAGS} print.c
cmp.o: cmp.c cmp.h ls.h
		gcc -c ${CFLAGS} cmp.c

clean:
	rm *.o ls
