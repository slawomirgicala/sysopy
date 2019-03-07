CMP=gcc -Wall -std=c99

all: liblibrary_static.a liblibrary_shared.so clean

liblibrary_static.a: liblibrary_static.o
	ar rc $@ $^
liblibrary_static.o: library.c library.h
	$(CMP) -I. -c -o $@ $<
liblibrary_shared.so: liblibrary_shared.o
	$(CMP) -shared -o $@ $<
liblibrary_shared.o: library.c library.h
	$(CMP) -fPIC -I. -c -o  $@ $<

clean:
	 rm -f *.o
clean_out:
	 rm -f *.so *.a
