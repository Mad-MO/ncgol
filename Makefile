
run: ncgol
	./ncgol

distclean: clean

clean:
	rm -f ncgol ncgol.o

ncgol: ncgol.o Makefile
	gcc -o ncgol ncgol.o -lncurses

ncgol.o: ncgol.c Makefile
	gcc -c ncgol.c
