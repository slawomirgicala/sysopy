CC = gcc -Wall -std=c99

.PHONY: clean

all: trucker



trucker: trucker.c
	$(CC) -o trucker trucker.c

client: client.c
	$(CC) -o client client.c

clean:
	rm -f trucker