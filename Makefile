NAME = Engine
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
RESOURCES_DIR := resources

SUB_DIRS := $(BIN_DIR) $(OUT_EXE_DIR) $(OUT_EXE_DIR)/$(RESOURCES_DIR) $(OUT_LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/$(SRC_DIR) $(OBJ_DIR)/$(LIB_DIR)

MAIN_FILE := $(SRC_DIR)/main.c
MAIN_FILE_OUT := $(OBJ_DIR)/$(SRC_DIR)/main.o
SRC := $(filter-out $(MAIN_FILE), $(wildcard $(SRC_DIR)/*.c $(LIB_DIR)/*.c))
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(SRC)))

INC := -I$(SRC_DIR) -I$(LIB_DIR)

EXE := $(OUT_EXE_DIR)/$(NAME)$(EXTENSION)
SLIB := $(OUT_LIB_DIR)/$(SLIB_PREFIX)$(NAME)$(SLIB_EXTENTION)

# Rules
.PHONY:default
default: $(EXE) $(RESOURCES_DIR)
.PHONY:lib
lib: $(SLIB)
.PHONY:all
all: default lib

$(OBJ_DIR)/%.o: %.c
	$(CC) $(INC) -c $< -o $@

$(EXE): $(OBJ) $(MAIN_FILE_OUT)
	$(CC) $(CLIB) $(INC) $(OBJ) $(MAIN_FILE_OUT) -o $@

# TODO make this run only when files are changed
# TODO make this a dependency of $(EXE)
.PHONY:$(RESOURCES_DIR)
$(RESOURCES_DIR):
	cp -ru $(RESOURCES_DIR) $(BIN_DIR)

run: all
	./$(EXE)

$(SLIB): $(OBJ)
	ar -rcs $@ $(OBJ) --record-libdeps \"$($CLIB)\"

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)
	rm -f $(SRC_DIR)/*.o $(LIB_DIR)/*.o
	mkdir -p $(SUB_DIRS)
