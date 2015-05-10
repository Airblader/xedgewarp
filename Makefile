TARGET = xedgewarp
SDIR = src
IDIR = include
ODIR = obj

CC = gcc
CFLAGS += -I$(IDIR)
CFLAGS += -std=gnu99
CFLAGS += -Wall -Wundef -Wshadow
LIBS = -lxcb -lxcb-util -lxcb-randr

INCS = $(wildcard $(IDIR)/*.h)
SRCS = $(wildcard $(SDIR)/*.c)
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

.PHONY: all
all: clean $(TARGET)

.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(INCS)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
