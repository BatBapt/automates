CC = gcc
CFLAGS = -Wall -g

all: af mygrep 

af: af.o afd.o afn.o abr.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

example_with_debug:
	./mygrep '(a+c)*.(a+b)*.c' 'accbbc' 'd'
	make compilo
	./compilo

example_without_debug:
	./mygrep '(a+c)*.(a+b)*.c' 'accbca' 'n'
	make compilo
	./compilo

mygrep: mygrep.o compregex.o
	$(CC) $^ -o mygrep

mygrep.o: mygrep.c
	$(CC) $^ mygrep.c -c

compregex.o: compregex.c compregex.h
	$(CC) compregex.c -c

compilo: compilo.o afd.o afn.o abr.o compregex.o
	$(CC) $(CFLAGS) $^ -o compilo

compilo.o: compilo.c afn.h afd.h compregex.h
	$(CC) $^ compilo.c -c

clean:
	rm -f *.o *~
