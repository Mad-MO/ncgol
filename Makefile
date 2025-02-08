
run: ncgol
	./ncgol

ncgol: ncgol.o Makefile
	gcc -o ncgol ncgol.o -lncurses

ncgol.o: ncgol.c Makefile
	gcc -c ncgol.c
