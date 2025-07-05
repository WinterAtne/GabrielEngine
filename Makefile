NAME := Engine
VERSION := 0.0.0

MAKE := make
MAKE_CLEAN := $(MAKE) clean
GO := go
GO_CLEAN := $(GO) clean -cache
SUB_MAKE := core/CCore
SUB_MAKE_CLEAN := $(addsuffix clean, $(SUB_MAKE))

.PHONY: all
all: run $(SUB_MAKE)

run:
	$(GO) run .

clean:
	$(GO_CLEAN)

$(SUB_MAKE): FORCE
	$(MAKE) -C $@


FORCE:
