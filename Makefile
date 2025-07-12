NAME := Gabriel
VERSION := 0.0.0

#--- Directories ---#
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

OUT_LIB_DIR := $(BIN_DIR)/lib
OUT_CMD_DIR := $(BIN_DIR)/cmd

CCORE_DIR := core/CCore
CCORE_CMD_DIR := $(CCORE_DIR)/test
CCORE_LIB_DIR := $(CCORE_DIR)/lib
CCORE_OPT_DIR := $(CCORE_DIR)/options
CCORE_SRC_DIR := $(CCORE_DIR)/src
OUT_OBJ_DIRS  := $(addprefix $(OBJ_DIR)/, $(CCORE_DIR) $(CCORE_CMD_DIR) $(CCORE_LIB_DIR) $(CCORE_OPT_DIR) $(CCORE_SRC_DIR))

OUT_DIRS := $(OUT_BIN_DIR) $(OUT_LIB_DIR) $(OUT_CMD_DIR) $(OUT_OBJ_DIR) $(OUT_OBJ_DIRS)

#--- C stuff --- *#
CCORE := ccore
CCORE_OUT_LIB := $(OUT_LIB_DIR)/lib$(CCORE).a

CC = clang
INC := -I$(CCORE_SRC_DIR) -I$(CCORE_LIB_DIR) -I$(CCORE_OPT_DIR)
CLIB := -ldl -lglfw -lm
CFLAGS_RELEASE := -Wall
# CFLAGS_DEBUG := -fsanitize=address -g -O0 -Wall

CCORE_SRCS := $(wildcard $(CCORE_SRC_DIR)/*.c $(CCORE_LIB_DIR)/*.c $(CCORE_OPT_DIR)/*.c)
CCORE_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(CCORE_SRCS)))

#--- Go stuff ---#
GO := go
GO_CLEAN := $(GO) clean -cache

#--- Other CMDS ---#
MAKE := make
MKDIR := mkdir -p
RMRF := rm -rf
AR := ar -rcs

#--- Targets ---#
.PHONY: default
default: run

.PHONY: run
run: $(CCORE_OUT_LIB)
	$(GO) run .

$(CCORE_OUT_LIB): $(CCORE_OBJS)
	$(AR) $@ $(CCORE_OBJS) --record-libdeps \"$($CLIB)\"
	$(MAKE) goclean

$(OBJ_DIR)/%.o: %.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean: goclean
	$(RMRF) $(OUT_DIRS)

	$(MKDIR) $(OUT_DIRS)

.PHONY: goclean 
goclean:
	$(GO_CLEAN)
