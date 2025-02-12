
# File:    Makefile
# Author:  Martin Ochs
# License: MIT



ifeq ($(shell uname -s), Darwin)
  CC     = gcc
  LDLIBS = -lncurses
endif
ifeq ($(shell uname -s), Linux)
  CC     = gcc
  LDLIBS = -lncursesw
endif



run: ncgol
	./ncgol

distclean: clean

clean:
	rm -f ncgol ncgol.o

ncgol: ncgol.o Makefile
	$(CC) -o ncgol ncgol.o $(LDLIBS)

ncgol.o: ncgol.c Makefile
	$(CC) -c ncgol.c

