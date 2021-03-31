# Miniature version of the provided CS241 Makefiles

TESTERS_DIR=testers
TESTERS_EXE_DIR=testers_exe

TESTERS=$(filter %.c, $(shell find $(TESTERS_DIR) -type f 2>/dev/null))
EXES=$(patsubst %.c,%,$(foreach tester,$(TESTERS),$(patsubst testers/%,testers_exe/%,$(tester))))

CC=clang
INCLUDES=-I./includes/
WARNINGS= -Wall -Wextra -Werror -Wno-error=unused-parameter -Wmissing-declarations -Wmissing-variable-declarations

CFLAGS_COMMON=$(INCLUDES) $(WARNINGS) -std=c99 -D_GNU_SOURCE
CFLAGS_RELEASE=$(CFLAGS_COMMON) -O2
CFLAGS_DEBUG=$(CFLAGS_COMMON) -O0 -g -DDEBUG

LD=clang
PROVIDED_LIBRARIES:=$(shell find libs/ -type f -name '*.a' 2>/dev/null)
PROVIDED_LIBRARIES:=$(PROVIDED_LIBRARIES:libs/lib%.a=%)
LDFLAGS = -Llibs/ $(foreach lib,$(PROVIDED_LIBRARIES),-l$(lib)) -lm

all: $(EXES)

$(TESTERS_EXE_DIR):
	@mkdir -p $@

$(TESTERS_EXE_DIR)/%: $(TESTERS_DIR)/%.c
	$(LD) $^ -o $@ $(LDFLAGS)

.PHONY: scan
scan: $(TESTERS_EXE_DIR)/stack_scan
	$(TESTERS_EXE_DIR)/stack_scan

.PHONY: scan_mreplace
scan_mreplace: $(TESTERS_EXE_DIR)/stack_scan
	./mreplace $(TESTERS_EXE_DIR)/stack_scan

.PHONY: scan_both
scan_both: $(TESTERS_EXE_DIR)/stack_scan
	@echo ----------------------------BELOW IS mreplace-------------------
	./mreplace $(TESTERS_EXE_DIR)/stack_scan
	@echo ----------------------------BELOW IS GLIBC-------------------
	$(TESTERS_EXE_DIR)/stack_scan


.PHONY: clean
clean:
	rm -rf $(EXES)

