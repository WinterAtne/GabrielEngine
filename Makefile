NAME = Engine
EXTENSION = .out
VERSION = 0.0.0

# Variables
SHELL=/bin/sh
CC=clang
CLIB=-ldl -lglfw -lm


# Directories & Files
BUILD=build
BIN=bin
LIB=lib
SRC_DIR=src
RESOURCES=resources

EXE=$(BIN)/$(NAME)$(EXTENSION)

SRC += $(wildcard src/*.c)               # list of source files
SRC += $(wildcard lib/*.c)               # list of source files
OBJS = $(patsubst %.c, %.o, $(SRC)) # list of object files

# Rules
.PHONY:all

all:$(EXE)

%.o: %.c
	$(CC) -c $< -I$(SRC_DIR) -I$(LIB) -o $@

$(EXE): $(OBJS)
	$(CC) $(CLIB) -I$(SRC_DIR) -I$(LIB) $(OBJS) -o $@ 

run: $(EXE)
	./$(EXE)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)
	mkdir $(BUILD) $(BIN) $(BUILD)/$(SRC_DIR) $(BUILD)/$(LIB) $(BIN)/$(RESOURCES)
