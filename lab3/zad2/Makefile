CC = gcc -Wall

.PHONY: clean clean_arch

monitor:
	$(CC) -o monitor main.c

tester:
	$(CC) -o tester tester.c

clean:
	rm -f monitor tester

clean_arch:
	rm -rf ./archive/*

