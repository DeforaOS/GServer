SUBDIRS	= video
TARGETS	= $(OBJDIR)../data/GServer.h $(OBJDIR)GServer
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
BINDIR	= $(PREFIX)/bin
SBINDIR	= $(PREFIX)/sbin
CC	= cc
CPPFLAGSF= -I ../include
CPPFLAGS= -I $(PREFIX)/include
CFLAGSF	= -W `pkg-config --cflags libApp`
CFLAGS	= -Wall -g -O2 -pedantic
LDFLAGSF= `pkg-config --libs libApp` -Wl,--export-dynamic
LDFLAGS	= 
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: subdirs $(TARGETS)

subdirs:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE)) || exit; done

$(OBJDIR)../data/GServer.h: ../data/GServer.interface
	../data/appbroker.sh -P "$(PREFIX)" -- "$(OBJDIR)../data/GServer.h"

GServer_OBJS = $(OBJDIR)gserver.o $(OBJDIR)main.o
GServer_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS) `pkg-config --cflags gl`
GServer_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)GServer: $(GServer_OBJS)
	$(CC) -o $(OBJDIR)GServer $(GServer_OBJS) $(GServer_LDFLAGS)

$(OBJDIR)gserver.o: gserver.c gserver.h ../data/GServer.h ../config.h
	$(CC) $(GServer_CFLAGS) -o $(OBJDIR)gserver.o -c gserver.c

$(OBJDIR)main.o: main.c gserver.h ../config.h
	$(CC) $(GServer_CFLAGS) -o $(OBJDIR)main.o -c main.c

clean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) clean) || exit; done
	$(RM) -- $(../data/GServer.h_OBJS) $(GServer_OBJS)
	../data/appbroker.sh -c -P "$(PREFIX)" -- "../data/GServer.h"

distclean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) distclean) || exit; done
	$(RM) -- $(../data/GServer.h_OBJS) $(GServer_OBJS)
	../data/appbroker.sh -c -P "$(PREFIX)" -- "../data/GServer.h"
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) install) || exit; done

uninstall:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) uninstall) || exit; done

.PHONY: all subdirs clean distclean install uninstall
