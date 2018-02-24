COMPILE := gcc -std=gnu11
EXEC := yemalloc
CFLAGS := -Wall -Werror
DFLAGS := -g -DDEBUG

SRCD := src
BLDD := build
BIND := bin
INCD := include

SRCF := $(shell find $(SRCD) -type f -name *.c)
OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(SRCF:.c=.o))

.PHONY: clean all setup debug
.DEFAULT: clean all

all: setup $(EXEC)

debug: CFLAGS += $(DFLAGS)
debug: all

clean:
	rm -rf $(BIND) $(BLDD)

setup:
	mkdir -p bin build

$(EXEC): $(OBJF)
	$(COMPILE) $(CFLAGS) -I$(INCD) $^ -o $(BIND)/$@ 

$(BLDD)/%.o: $(SRCD)/%.c
	$(COMPILE) $(CFLAGS) -I$(INCD) -c $< -o $@
