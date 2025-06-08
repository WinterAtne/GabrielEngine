NAME = Engine
EXTENSION = .out
VERSION = 0.0.0

# Variables
SHELL=/bin/sh
CC=clang
CLIB=-ldl -lglfw


# Directories & Files
BUILD=build
BIN=bin
LIB=lib
SRC=src

EXE=$(BIN)/$(NAME)$(EXTENSION)

SRC_C += $(wildcard $(addsuffix /*.c, $(SRC)))
SRC_C += $(wildcard $(addsuffix /*.c, $(LIB)))
OBJ := $(patsubst %.c, %.o, $(SRC_C))

# Rules
.PHONY:all

all:$(OBJ) $(EXE)

%.o: %.c
	$(CC) -c $< -o $(BUILD)/$@

$(EXE): $(OBJ)
	$(CC) $(CLIB) $(addprefix $(BUILD)/, $(OBJ)) -o $@ 

run: $(EXE)
	./$(EXE)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)
	mkdir $(BUILD) $(BIN) $(BUILD)/$(SRC) $(BUILD)/$(LIB)
