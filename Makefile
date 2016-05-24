CFLAGS= -lpthread

all: clean ep2

ep2: main.o
	gcc -o ep2 main.o $(CFLAGS)

main.o: main.c
	gcc -o main.o -c main.c $(CFLAGS)
	
clean: 
	rm -f ep2
	rm -f *.o
