# Try to guess the target platform and ABI
ifeq ($(shell uname -s),Darwin)
ABI := macho
else
ABI := elf
endif
ARCH := $(shell uname -m)
PLATFORM ?= $(ARCH)_sysv_$(ABI)

.PHONY: default all run clean

default: run

TARGETS := simple full

all: $(TARGETS)

run: all
	@for x in $(TARGETS); do \
		printf "\nRUNNING $$x\n"; \
		./$$x && echo TEST OK || echo TEST FAILED; \
	done; \
	echo ALL TESTS COMPLETED OK

%: %.c
	$(CC) $(CFLAGS) -I../inc/ ../src/asm/*$(PLATFORM)*.S $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGETS)
