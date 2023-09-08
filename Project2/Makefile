CC = gcc
CFLAGS = -Wall -Werror

all: project2

project2: project2.o student2.o
	$(CC) $(CFLAGS) -o project2 project2.o student2.o

project2.o: project2.c project2.h
	$(CC) $(CFLAGS) -c project2.c

student2.o: student2.c project2.h
	$(CC) $(CFLAGS) -c student2.c

clean:
	rm -f project2 *.o

