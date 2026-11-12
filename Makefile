PROG = mimetype
VERSION = 1.0.0
PREFIX = /usr/local

CC = cc
PKGS = gio-2.0

CPPFLAGS = $$(pkg-config --cflags $(PKGS))
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -O2
LDFLAGS = -s $$(pkg-config --libs $(PKGS))

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
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROG)
	cp -f $(PROG) $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(PROG)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(PROG)

.PHONY: all clean install uninstall
