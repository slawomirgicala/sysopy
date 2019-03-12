CMP = gcc -Wall -std=c99



liblibrary_static.a: liblibrary_static.o
	ar rc $@ $^

liblibrary_static.o: library.c library.h
	$(CMP) -I. -c -o $@ $<


test: liblibrary_static.a
	 $(CMP) -o test main.c $< -I .

clean:
	 rm -f *.o
clean_out:
	 rm -f *.so *.a test

