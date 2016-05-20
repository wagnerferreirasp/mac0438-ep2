CFLAGS= -lpthread

all: clean EP2

EP2: main.o
	gcc -o EP1 main.o $(CFLAGS)

main.o: main.c
	gcc -o main.o -c main.c $(CFLAGS)
	
clean: 
	rm -f EP2
	rm -f *.o
