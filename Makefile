TARGET = xedgewarp
VERSION = 1.2
SDIR = src
IDIR = include
ODIR = obj

INSTALL = install
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

CC = gcc
CFLAGS += -I$(IDIR)
CFLAGS += -std=gnu99 -fcommon
CFLAGS += -Wall -Wundef -Wshadow -Wformat-security
LIBS = $(shell pkg-config --libs xcb xcb-randr xcb-aux x11 x11-xcb xi)

INCS = $(wildcard $(IDIR)/*.h)
SRCS = $(wildcard $(SDIR)/*.c)
OBJS = $(patsubst %,$(ODIR)/%,$(notdir $(SRCS:.c=.o)))

MANS = man/xedgewarp.1

.NOTPARALLEL:

.PHONY: all
all: clean $(TARGET) mans

.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c $(INCS)
	$(CC) -D'__VERSION="${VERSION}"' $(CFLAGS) -o $@ -c $<

.PHONY: install
install: $(TARGET)
	$(INSTALL) -Dm 0755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	$(INSTALL) -Dm 0644 man/xedgewarp.1 $(DESTDIR)$(MANDIR)/xedgewarp.1

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(TARGET)
	$(RM) $(DESTDIR)$(MANDIR)/xedgewarp.1

.PHONY: mans
mans: $(MANS)

$(MANS): %.1: %.man
	a2x --no-xmllint -f manpage $<

.PHONY: test
test: clean $(TARGET)
	@cd test/; \
	xvfb-run ./run.pl; \
	cd ../

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
	$(RM) man/*.1 man/*.xml
	$(RM) test/GLOB*
