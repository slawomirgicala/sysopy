CC = gcc -Wall -std=c99

.PHONY: clean

all: trucker worker



trucker: trucker.c queue.h
	$(CC) -o trucker trucker.c

worker: worker.c queue.h
	$(CC) -o worker worker.c

clean:
	rm -f trucker worker