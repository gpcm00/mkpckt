CC := gcc
CFLAGS := -Werror -std=c17 -Iinc
TARGET := mkpckt

ifeq ($(DEBUG), true)
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -Wall -O2
endif

SRCDIR := src
OBJDIR := obj

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ) main.c | $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ main.c $(OBJ)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean

