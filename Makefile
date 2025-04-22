
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
		  $(BUILD)/debug_output.o \
          $(BUILD)/end_det.o \
          $(BUILD)/grid.o \
		  $(BUILD)/patterns.o



build: ncgol

runtest: clear distclean ncgol
	@echo "--- Version ---"
	$(BIN)/ncgol -v
	@echo "--- Help ---"
	$(BIN)/ncgol -h
	@echo "--- Run ---"
	$(BIN)/ncgol --nowait --speed 9 --init random --charstyle braille --mode next
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
	$(CC) -o $@ -c $< -D WITH_DEBUG_OUTPUT
