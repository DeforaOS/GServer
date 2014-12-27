PACKAGE	= GServer
VERSION	= 0.0.0
SUBDIRS	= data include src
RM	= rm -f
LN	= ln -f
TAR	= tar
MKDIR	= mkdir -m 0755 -p


all: subdirs

subdirs:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE)) || exit; done

clean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) clean) || exit; done

distclean:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) distclean) || exit; done

dist:
	$(RM) -r -- $(OBJDIR)$(PACKAGE)-$(VERSION)
	$(LN) -s -- "$$PWD" $(OBJDIR)$(PACKAGE)-$(VERSION)
	@cd $(OBJDIR). && $(TAR) -czvf $(OBJDIR)$(PACKAGE)-$(VERSION).tar.gz -- \
		$(PACKAGE)-$(VERSION)/data/Makefile \
		$(PACKAGE)-$(VERSION)/data/GServer.interface \
		$(PACKAGE)-$(VERSION)/data/appbroker.sh \
		$(PACKAGE)-$(VERSION)/data/project.conf \
		$(PACKAGE)-$(VERSION)/include/GServer.h \
		$(PACKAGE)-$(VERSION)/include/Makefile \
		$(PACKAGE)-$(VERSION)/include/project.conf \
		$(PACKAGE)-$(VERSION)/include/GServer/gserver.h \
		$(PACKAGE)-$(VERSION)/include/GServer/video.h \
		$(PACKAGE)-$(VERSION)/include/GServer/Makefile \
		$(PACKAGE)-$(VERSION)/include/GServer/project.conf \
		$(PACKAGE)-$(VERSION)/src/gserver.c \
		$(PACKAGE)-$(VERSION)/src/main.c \
		$(PACKAGE)-$(VERSION)/src/Makefile \
		$(PACKAGE)-$(VERSION)/src/gserver.h \
		$(PACKAGE)-$(VERSION)/src/project.conf \
		$(PACKAGE)-$(VERSION)/src/video/glut.c \
		$(PACKAGE)-$(VERSION)/src/video/glx.c \
		$(PACKAGE)-$(VERSION)/src/video/vbe.c \
		$(PACKAGE)-$(VERSION)/src/video/vesa.c \
		$(PACKAGE)-$(VERSION)/src/video/Makefile \
		$(PACKAGE)-$(VERSION)/src/video/vbe.h \
		$(PACKAGE)-$(VERSION)/src/video/project.conf \
		$(PACKAGE)-$(VERSION)/COPYING \
		$(PACKAGE)-$(VERSION)/Makefile \
		$(PACKAGE)-$(VERSION)/TODO \
		$(PACKAGE)-$(VERSION)/config.h \
		$(PACKAGE)-$(VERSION)/project.conf
	$(RM) -- $(OBJDIR)$(PACKAGE)-$(VERSION)

distcheck: dist
	$(TAR) -xzvf $(OBJDIR)$(PACKAGE)-$(VERSION).tar.gz
	$(MKDIR) -- $(PACKAGE)-$(VERSION)/objdir
	$(MKDIR) -- $(PACKAGE)-$(VERSION)/destdir
	(cd "$(PACKAGE)-$(VERSION)" && $(MAKE) OBJDIR="$$PWD/objdir/")
	(cd "$(PACKAGE)-$(VERSION)" && $(MAKE) OBJDIR="$$PWD/objdir/" DESTDIR="$$PWD/destdir" install)
	(cd "$(PACKAGE)-$(VERSION)" && $(MAKE) OBJDIR="$$PWD/objdir/" DESTDIR="$$PWD/destdir" uninstall)
	(cd "$(PACKAGE)-$(VERSION)" && $(MAKE) OBJDIR="$$PWD/objdir/" distclean)
	(cd "$(PACKAGE)-$(VERSION)" && $(MAKE) dist)
	$(RM) -r -- $(PACKAGE)-$(VERSION)

install:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) install) || exit; done

uninstall:
	@for i in $(SUBDIRS); do (cd "$$i" && $(MAKE) uninstall) || exit; done

.PHONY: all subdirs clean distclean dist distcheck install uninstall
