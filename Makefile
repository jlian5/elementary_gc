# Miniature version of the provided CS241 Makefiles

EXES=gc test

OBJS_DIR=.objs

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

.PHONY: all
.PHONY: release
.PHONY: debug

all: release

release: test

debug: clean test-debug

# create the objects directory if not there
$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

# compile to exec-debug.o in OBJS_DIR
$(OBJS_DIR)/%-debug.o: %.c | $(OBJS_DIR)
	$(CC) $(CFLAGS_DEBUG) $< -o $@

$(OBJS_DIR)/%-release.o: %.c | $(OBJS_DIR)
	$(CC) $(CFLAGS_RELEASE) $< -o $@

# actual executables
test-debug: $(OBJS_DIR)/test-debug.o $(OBJS_DIR)/gc-debug.o
	$(LD) $^ $(LDFLAGS) -o $@

test: $(OBJS_DIR)/test-release.o $(OBJS_DIR)/gc-release.o
	$(LD) $^ $(LDFLAGS) -o $@

# stack_scan: 
# 	$(LD) -g stack_scan.c -o stack_scan

.PHONY: clean
clean:
	rm -rf $(EXES)

