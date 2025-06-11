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
SRC=src
RESOURCES=resources

EXE=$(BIN)/$(NAME)$(EXTENSION)

SRC_C += $(wildcard $(addsuffix /*.c, $(SRC)))
SRC_C += $(wildcard $(addsuffix /*.c, $(LIB)))
OBJ := $(patsubst %.c, %.o, $(SRC_C))

# Rules
.PHONY:all

all:$(OBJ) $(EXE)

%.o: %.c
	$(CC) -c $< -I$(SRC) -I$(LIB) -o $(BUILD)/$@

$(EXE): $(OBJ)
	$(CC) $(CLIB) -I$(SRC) -I$(LIB) $(addprefix $(BUILD)/, $(OBJ)) -o $@ 
	cp $(RESOURCES) $(BIN) -ru

run: $(EXE)
	./$(EXE)

clean:
	rm -rf $(BUILD)
	rm -rf $(BIN)
	mkdir $(BUILD) $(BIN) $(BUILD)/$(SRC) $(BUILD)/$(LIB) $(BIN)/$(RESOURCES)
