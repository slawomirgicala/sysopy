CC = gcc -Wall -std=c99

.PHONY: clean

all: trucker worker loader



trucker: trucker.c queue.h
	$(CC) -o trucker trucker.c

worker: worker.c queue.h
	$(CC) -o worker worker.c

loader: loader.c queue.h
	$(CC) -o loader loader.c

clean:
	rm -f trucker worker