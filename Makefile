
# File:    Makefile
# Author:  Martin Ochs
# License: MIT

BUILD = ./build
BIN   = ./bin
SRC   = ./src

ifeq ($(shell uname -s), Darwin)
  CC     = gcc
  LDLIBS = -lncurses
endif
ifeq ($(shell uname -s), Linux)
  CC     = gcc
  LDLIBS = -lncursesw
endif

OBJECTS = $(BUILD)/ncgol.o \
          $(BUILD)/grid.o \
		  $(BUILD)/patterns.o



build: ncgol

run: ncgol
	$(BIN)/ncgol -v
	$(BIN)/ncgol -h
	$(BIN)/ncgol -n -s2 -c block -i simkin -m loop

distclean: clean
	@rm -vf $(BIN)/*

clean:
	@rm -vf $(BUILD)/*

ncgol: $(BIN)/ncgol

$(BIN)/ncgol: $(OBJECTS)
	@mkdir -vp $(BIN)
	$(CC) -o $@ $^ $(LDLIBS)

$(BUILD)/%.o: $(SRC)/%.c $(SRC)/*.h Makefile
	@mkdir -vp $(BUILD)
	$(CC) -o $@ -c $<
