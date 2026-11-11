PROG = mimetype
PREFIX = /usr/local

CC = cc
INCS = $$(pkg-config --cflags gio-2.0)
LIBS = $$(pkg-config --libs gio-2.0)

CPPFLAGS = $(INCS)
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -O2
LDFLAGS = -s $(LIBS)

SRC = $(PROG).c
OBJ = $(SRC:.c=.o)

all: $(PROG)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<
	chmod 644 $@

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)
	chmod 755 $@

clean:
	rm -f $(PROG) $(OBJ)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(PROG) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(PROG)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROG)

.PHONY: all clean install uninstall
