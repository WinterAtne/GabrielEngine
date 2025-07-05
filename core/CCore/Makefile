NAME = CCore
EXE_EXTENSION := .out
SLIB_PREFIX := lib
SLIB_EXTENTION := .a
VERSION := 0.0.0

# Variables
SHELL = /bin/sh
CC = clang
CLIB = -ldl -lglfw -lm

# Directories & Files
OBJ_DIR := obj
BIN_DIR := bin
OUT_EXE_DIR := $(BIN_DIR)/exe
OUT_LIB_DIR := $(BIN_DIR)/lib
LIB_DIR := lib
SRC_DIR := src
TST_DIR := test
OPT_DIR := options

SUB_DIRS := $(BIN_DIR) $(OUT_EXE_DIR)  $(OUT_LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/$(SRC_DIR) $(OBJ_DIR)/$(LIB_DIR) $(OBJ_DIR)/$(TST_DIR) $(OBJ_DIR)/$(OPT_DIR)

MAIN_FILE := $(TST_DIR)/main.c
MAIN_FILE_OUT := $(OBJ_DIR)/$(TST_DIR)/main.o
SRC := $(wildcard $(SRC_DIR)/*.c $(LIB_DIR)/*.c $(OPT_DIR)/*.c)
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(SRC)))

INC := -I$(SRC_DIR) -I$(LIB_DIR) -I$(OPT_DIR)

EXE := $(OUT_EXE_DIR)/$(NAME)$(EXTENSION)
SLIB := $(OUT_LIB_DIR)/$(SLIB_PREFIX)$(NAME)$(SLIB_EXTENTION)

# Rules
.PHONY:default
default: all
.PHONY:lib
lib: $(SLIB)
.PHONY:all
all: $(EXE) lib

$(OBJ_DIR)/%.o: %.c
	$(CC) $(INC) -c $< -o $@

$(EXE): $(OBJ) $(MAIN_FILE_OUT)
	$(CC) $(CLIB) $(INC) $(OBJ) $(MAIN_FILE_OUT) -o $@

run: default
	./$(EXE)

$(SLIB): $(OBJ)
	ar -rcs $@ $(OBJ) --record-libdeps \"$($CLIB)\"

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.o $(OPT_DIR)/*.o
	mkdir -p $(SUB_DIRS)
