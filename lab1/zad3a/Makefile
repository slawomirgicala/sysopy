CC = gcc -Wall -std=c11 -ggdb

.PHONY: test_st test_sh test_dy

all: static shared clean

static:
	$(CC) -c library.c
	ar rcs library.a library.o

shared:
	$(CC) -c -fPIC library.c
	$(CC) -shared -fPIC -o library.so library.o

clean:
	rm -f *.o *.so *.a test*

test_static:
	make static
	$(CC) -o test_st main.c library.a -I .
	make test_st

test_dynamic:
	make shared
	$(CC) -o test_dy main_dynamic.c -I . -ldl
	make test_dy

test_shared:
	make shared
	$(CC) -o test_sh main.c -L ./ -Wl,-rpath=./ library.so -I .
	make test_sh


test_st:
	./test_st c 1000 s /home s /home/slawomir/Pulpit/tmp ad 100000 r 0

test_sh:
	./test_sh c 1000 s /home s /home/slawomir/Pulpit/tmp ad 100000 r 0

test_dy:
	./test_dy c 1000 s /home s /home/slawomir/Pulpit/tmp ad 100000 r 0