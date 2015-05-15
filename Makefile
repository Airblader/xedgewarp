TARGET = xedgewarp
SDIR = src
IDIR = include
ODIR = obj

INSTALL = install
PREFIX = /usr/bin

CC = gcc
CFLAGS += -I$(IDIR)
CFLAGS += -std=gnu99
CFLAGS += -Wall -Wundef -Wshadow
LIBS = $(shell pkg-config --libs xcb xcb-randr xcb-aux)

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

.PHONY: install
install: all
	$(INSTALL) -m 0755 $(TARGET) $(DESTDIR)$(PREFIX)/

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/$(TARGET)

.PHONY: test
test: all
	@cd test/; \
	perl run.pl; \
	cd ../

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
	$(RM) test/GLOB*
