NAME = Engine
EXTENSION = .out
VERSION = 0.0.0

# Variables
SHELL=/bin/sh
CC=clang
CLIB=-ldl -lglfw -lm

# Directories & Files
BIN_DIR=bin
LIB_DIR=lib
SRC_DIR=src
RESOURCES_DIR=resources

EXE=$(BIN_DIR)/$(NAME)$(EXTENSION)

SRC += $(wildcard $(SRC_DIR)/*.c)
SRC += $(wildcard $(LIB_DIR)/*.c)
INC += -I$(SRC_DIR) -I$(LIB_DIR)

# Rules
.PHONY:all
all: $(EXE) $(RESOURCES_DIR)

# TODO: this should output .o files into a build_dir
$(EXE): $(SRC)
	$(CC) $(CLIB) $(INC) $(SRC) -o $@

# It would be nice if this only had to run when it changed
.PHONY:$(RESOURCES_DIR)
$(RESOURCES_DIR):
	cp -ru $(RESOURCES_DIR) $(BIN_DIR)

run: all
	./$(EXE)

clean:
	rm -rf $(BIN_DIR)
	mkdir $(BIN_DIR) $(BIN_DIR)/$(RESOURCES_DIR)
