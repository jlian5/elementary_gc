# Miniature version of the provided CS241 Makefiles

TESTERS_DIR=testers
TESTERS_EXE_DIR=testers_exe
OBJS_DIR=.objs

# get the name of all files inside testers/
TESTERS=$(filter %.c, $(shell find $(TESTERS_DIR) -type f 2>/dev/null))
# fix the name of the files inside testers/ by removing the .c and changing the directory to be testers_exe/
EXES=$(patsubst %.c,%,$(foreach tester,$(TESTERS),$(patsubst testers/%,testers_exe/%,$(tester))))

OBJS= gc.o

CC=clang
# make it so that you can #include "set.h"  and #include "gc.h" directly
INCLUDES=-I./includes/ -I./
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

$(OBJS_DIR):
	@mkdir -p $@

$(TESTERS_EXE_DIR)/%: $(TESTERS_EXE_DIR) $(OBJS_DIR)/gc.o $(OBJS_DIR)/%.o
	$(LD) $(word 2,$^) $(word 3,$^) -o $@ $(LDFLAGS)

$(OBJS_DIR)/gc.o: $(OBJS_DIR)
	$(CC) $(CFLAGS_DEBUG) -c gc.c -o $@

$(OBJS_DIR)/%.o: $(OBJS_DIR) $(TESTERS_DIR)/%.c
	$(CC) $(CFLAGS_DEBUG) -c $(word 2,$^) -o $@

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
	@echo ----------------------------BELOW IS GLIBC----------------------
	$(TESTERS_EXE_DIR)/stack_scan


.PHONY: clean
clean:
	rm -rf $(TESTERS_EXE_DIR) $(OBJS_DIR)

