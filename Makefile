CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

MAIN  := $(BLDD)/main.o

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
ALL_FUNCF := $(filter-out $(MAIN) $(AUX), $(ALL_OBJF))

TEST_SRC := $(shell find $(TSTD) -type f -name *.c)

INC := -I $(INCD)

CFLAGS := -g -O2 -Wall -Werror -Wno-unused-variable -Wno-unused-function -MMD
COLORF := -DCOLOR
DFLAGS := -g -DDEBUG -DCOLOR
PRINT_STAMENTS := -DERROR -DSUCCESS -DWARN -DINFO

STD := -std=gnu11
LIBS := -lm

CFLAGS += $(STD)

EXEC := fenc

.PHONY: clean all setup debug

all: setup $(EXEC) 

debug: CFLAGS += $(DFLAGS) $(PRINT_STAMENTS) $(COLORF)
debug: all

setup: $(BIND) $(BLDD)
$(BIND):
		mkdir -p $(BIND)
$(BLDD):
		mkdir -p $(BLDD)

$(EXEC): $(ALL_OBJF)
		$(CC) $^ -o $@ $(LIBS)

$(BLDD)/%.o: $(SRCD)/%.c
		$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
		rm -rf $(BLDD) $(BIND)

.PRECIOUS: $(BLDD)/*.d
-include $(BLDD)/*.d