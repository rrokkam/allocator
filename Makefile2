COMPILE := gcc -std=gnu11
EXEC := yemalloc
CFLAGS := -Wall -Werror
DFLAGS := -g -DDEBUG
TFLAGS := -lcriterion

BLDD := build
BIND := bin
INCD := include
SRCD := src
TSTD := tests

TST_EXEC = $(EXEC)_tests

SRCF := $(shell find $(SRCD) -type f -name *.c)
TSTF := $(shell find $(TSTD) -type f -name *.c)

SRC_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(SRCF:.c=.o))
TST_OBJF := $(patsubst $(TSTD)/%,$(BLDD)/%,$(TSTF:.c=.o))

ALL_OBJF := $(SRC_OBJF) $(TST_OBJF)

.PHONY: all clean debug setup
.DEFAULT: all clean

all: setup $(EXEC) $(TST_EXEC)

clean:
	rm -rf $(BIND) $(BLDD)

debug: CFLAGS += $(DFLAGS)
debug: all

setup:
	mkdir -p bin build

$(EXEC): $(SRC_OBJF)
	$(COMPILE) $(CFLAGS) -I$(INCD) $^ -o $(BIND)/$@ 

$(TST_EXEC): $(filter-out build/main.o, $(ALL_OBJF))
	$(COMPILE) $(CFLAGS) -I$(INCD) $(TST_OBJF) $(TST_SRCF) -o $(BIND)/$(TST_EXEC) $(TFLAGS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(COMPILE) $(CFLAGS) -I$(INCD) -c $< -o $@

$(BLDD)/%.o: $(TSTD)/%.c
	$(COMPILE) $(CFLAGS) -I$(INCD) -c $< -o $@
