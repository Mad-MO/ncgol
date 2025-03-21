
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

runtest: clear distclean ncgol
	@echo "--- Version ---"
	$(BIN)/ncgol -v
	@echo "--- Help ---"
	$(BIN)/ncgol -h
	@echo "--- Run ---"
	$(BIN)/ncgol --nowait --speed 9 --charstyle braille --init random --mode loop
	@echo "--- End ---"

clear:
	@clear

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
