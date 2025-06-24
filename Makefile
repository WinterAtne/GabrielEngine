NAME = Engine
EXTENSION = .out
VERSION = 0.0.0

# Variables
SHELL = /bin/sh
CC = clang
CLIB = -ldl -lglfw -lm

# Directories & Files
OBJ_DIR := obj
BIN_DIR := bin
LIB_DIR := lib
SRC_DIR := src
RESOURCES_DIR := resources

SRC := $(wildcard $(SRC_DIR)/*.c $(LIB_DIR)/*.c)
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(SRC)))

INC := -I$(SRC_DIR) -I$(LIB_DIR)

EXE := $(BIN_DIR)/$(NAME)$(EXTENSION)

# Rules
.PHONY:all
all: $(EXE) $(RESOURCES_DIR)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(INC) -c $< -o $@

$(EXE): $(OBJ)
	$(CC) $(CLIB) $(INC) $(OBJ) -o $@

# It would be nice if this only had to run when it changed
.PHONY:$(RESOURCES_DIR)
$(RESOURCES_DIR):
	cp -ru $(RESOURCES_DIR) $(BIN_DIR)

run: all
	./$(EXE)

clean:
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.o
	mkdir -p $(BIN_DIR) $(OBJ_DIR)/$(SRC_DIR) $(OBJ_DIR)/$(LIB_DIR) $(BIN_DIR)/$(RESOURCES_DIR)
