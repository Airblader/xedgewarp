TARGET = xedgewarp
SDIR = src
IDIR = include
ODIR = obj

INSTALL = install
PREFIX = /usr/bin

MANDIR = /usr/share/man/man1

CC = gcc
CFLAGS += -I$(IDIR)
CFLAGS += -std=gnu99
CFLAGS += -Wall -Wundef -Wshadow
LIBS = $(shell pkg-config --libs xcb xcb-randr xcb-aux)

INCS = $(wildcard $(IDIR)/*.h)
SRCS = $(wildcard $(SDIR)/*.c)
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

MANS = man/xedgewarp.1

.PHONY: all
all: clean $(TARGET) mans

.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(INCS)
	$(CC) -D'__VERSION="$(shell git describe --all --long --always)"' $(CFLAGS) -o $@ -c $<

.PHONY: install
install: all
	$(INSTALL) -m 0755 $(TARGET) $(DESTDIR)$(PREFIX)/
	$(INSTALL) -m 0644 man/xedgewarp.1 $(DESTDIR)$(MANDIR)/xedgewarp.1

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/$(TARGET)
	$(RM) $(DESTDIR)$(MANDIR)/xedgewarp.1

.PHONY: mans
mans: $(MANS)

$(MANS): %.1: %.man
	a2x -f manpage $<

.PHONY: test
test: all
	@cd test/; \
	perl run.pl; \
	cd ../

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
	$(RM) man/*.1 man/*.xml
	$(RM) test/GLOB*
