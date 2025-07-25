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

#--- C stuff ---#
CCORE := ccore
CCORE_OUT_LIB := $(OUT_LIB_DIR)/lib$(CCORE).a

CC = clang
INC := -I$(CCORE_SRC_DIR) -I$(CCORE_LIB_DIR) -I$(CCORE_OPT_DIR)
CLIB := -ldl -lglfw -lm
CFLAGS_RELEASE := -Wall -Werror
CFLAGS_DEBUG := -fsanitize=address -g -O0
CFLAGS := $(CFLAGS_RELEASE)

CCORE_SRCS := $(wildcard $(CCORE_SRC_DIR)/*.c $(CCORE_LIB_DIR)/*.c $(CCORE_OPT_DIR)/*.c)
CCORE_OBJS := $(addprefix $(OBJ_DIR)/, $(patsubst %.c,%.o,$(CCORE_SRCS)))

#--- Go stuff ---#
GO := go
GO_RUN := $(GO) run
GO_BUILD := $(GO) build
GO_DEBUG_FLAGS := -asan -gcflags="all=-N -l"
GO_RUN_DEBUG := $(GO_RUN) $(GO_DEBUG_FLAGS)
GO_BUILD_DEBUG := $(GO_BUILD) $(GO_DEBUG_FLAGS)
GO_TEST := $(GO) test $(GO_DEBUG_FLAGS)
GO_CLEAN := $(GO) clean -cache

GO_OUT_EXE := $(OUT_CMD_DIR)/$(NAME)
GO_OUT_EXE_DEBUG := $(OUT_CMD_DIR)/$(NAME)

#--- Other CMDS ---#
MAKE := make
MKDIR := mkdir -p
RMRF := rm -rf
AR := ar -rcs
DELVE := dlv exec

#--- Targets ---#
.PHONY: default
default: debug_run

.PHONY: debug_run
debug_run: debug_ccore
	CGO_CFLAGS="$(CFLAGS_DEBUG)" $(GO_RUN_DEBUG) .

.PHONY: debug_build
debug_build: $(GO_OUT_EXE_DEBUG)

.PHONY: debug
debug: $(GO_OUT_EXE_DEBUG)
	$(DELVE) $<

.PHONY: test
test: debug_ccore
	$(GO_TEST) ./...

.PHONY: test_cover
test_cover: debug_ccore
	$(GO_TEST) -cover ./...

$(GO_OUT_EXE_DEBUG): debug_ccore
	CGO_CFLAGS="$(CFLAGS_DEBUG)" $(GO_BUILD_DEBUG) -o $@ .

.PHONY: debug_ccore
debug_ccore: CFLAGS+=$(CFLAGS_DEBUG)
debug_ccore: $(CCORE_OUT_LIB)

$(CCORE_OUT_LIB): $(CCORE_OBJS)
	$(AR) $@ $(CCORE_OBJS) --record-libdeps \"$($CLIB)\"
	$(MAKE) go_clean

$(OBJ_DIR)/%.o: %.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean: go_clean
	$(RMRF) $(OUT_DIRS)

	$(MKDIR) $(OUT_DIRS)

.PHONY: go_clean 
go_clean:
	$(GO_CLEAN)

