
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
	$(BIN)/ncgol --help
	$(BIN)/ncgol -n -s2 -c braille -i ilove8bit

runsmall: ncgol
	@if [ "$$TERM_PROGRAM" = "Apple_Terminal" ]; then printf '\e[8;25;80t'; fi
	@if [ "$$TERM_PROGRAM" = "Linux" ];          then resize -s 25 80;      fi # TODO: check if this works
	$(BIN)/ncgol --help
	$(BIN)/ncgol -ns2
	@if [ "$$TERM_PROGRAM" = "Apple_Terminal" ]; then printf '\e[8;50;160t'; fi
	@if [ "$$TERM_PROGRAM" = "Linux" ];          then resize -s 50 160;      fi # TODO: check if this works

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
