
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



build: ncgol

run: ncgol
	$(BIN)/ncgol

distclean: clean
	@rm -vf $(BIN)/*

clean:
	@rm -vf $(BUILD)/*

ncgol: $(BIN)/ncgol

$(BIN)/ncgol: $(BUILD)/*.o
	@mkdir -vp $(BIN)
	$(CC) -o $@ $< $(LDLIBS)

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -vp $(BUILD)
	$(CC) -o $@ -c $<
