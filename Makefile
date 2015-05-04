TARGET = xedgewarp

SDIR = src
IDIR = include
ODIR = obj

LIBS = -lxcb -lxcb-util -lxcb-randr

CC = gcc
CFLAGS += -I$(IDIR)
CFLAGS += -std=c99
CFLAGS += -Wall -Wundef -Wshadow -pedantic

_INCS = globals.h randr.h types.h
INCS = $(patsubst %,$(IDIR)/%,$(_INCS))

_SRCS = randr.c xedgewarp.c
SRCS = $(patsubst %,$(SDIR)/%,$(_SRCS))

_OBJS = $(_SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

.PHONY: all
all: $(TARGET)

.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(INCS)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
