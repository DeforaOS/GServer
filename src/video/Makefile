TARGETS	= $(OBJDIR)glut.so $(OBJDIR)glx.so $(OBJDIR)vesa.so
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
LIBDIR	= $(PREFIX)/lib
CC	= cc
CPPFLAGSF= -I ../../include
CPPFLAGS=
CFLAGSF	= -W `pkg-config --cflags libApp`
CFLAGS	= -Wall -g -O2 -fPIC -pedantic
LDFLAGSF= -W `pkg-config --libs libApp`
AR	= ar
RANLIB	= ranlib
CCSHARED= $(CC) -shared
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

glut_OBJS = $(OBJDIR)glut.o
glut_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
glut_LDFLAGS = $(LDFLAGSF) $(LDFLAGS) `pkg-config --libs glut`

$(OBJDIR)glut.so: $(glut_OBJS)
	$(CCSHARED) -o $(OBJDIR)glut.so $(glut_OBJS) $(glut_LDFLAGS)

glx_OBJS = $(OBJDIR)glx.o
glx_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
glx_LDFLAGS = $(LDFLAGSF) $(LDFLAGS) `pkg-config --libs glu`

$(OBJDIR)glx.so: $(glx_OBJS)
	$(CCSHARED) -o $(OBJDIR)glx.so $(glx_OBJS) $(glx_LDFLAGS)

vesa_OBJS = $(OBJDIR)vbe.o $(OBJDIR)vesa.o
vesa_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
vesa_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)vesa.so: $(vesa_OBJS)
	$(CCSHARED) -o $(OBJDIR)vesa.so $(vesa_OBJS) $(vesa_LDFLAGS)

$(OBJDIR)glut.o: glut.c
	$(CC) $(glut_CFLAGS) `pkg-config --cflags glut` -o $(OBJDIR)glut.o -c glut.c

$(OBJDIR)glx.o: glx.c
	$(CC) $(glx_CFLAGS) `pkg-config --cflags glu` -o $(OBJDIR)glx.o -c glx.c

$(OBJDIR)vbe.o: vbe.c
	$(CC) $(vesa_CFLAGS) -o $(OBJDIR)vbe.o -c vbe.c

$(OBJDIR)vesa.o: vesa.c
	$(CC) $(vesa_CFLAGS) -o $(OBJDIR)vesa.o -c vesa.c

clean:
	$(RM) -- $(glut_OBJS) $(glx_OBJS) $(vesa_OBJS)

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	$(MKDIR) $(DESTDIR)$(PREFIX)/lib/GServer/video
	$(INSTALL) -m 0755 $(OBJDIR)glut.so $(DESTDIR)$(PREFIX)/lib/GServer/video/glut.so
	$(MKDIR) $(DESTDIR)$(PREFIX)/lib/GServer/video
	$(INSTALL) -m 0755 $(OBJDIR)glx.so $(DESTDIR)$(PREFIX)/lib/GServer/video/glx.so
	$(MKDIR) $(DESTDIR)$(PREFIX)/lib/GServer/video
	$(INSTALL) -m 0755 $(OBJDIR)vesa.so $(DESTDIR)$(PREFIX)/lib/GServer/video/vesa.so

uninstall:
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/GServer/video/glut.so
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/GServer/video/glx.so
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/GServer/video/vesa.so

.PHONY: all clean distclean install uninstall
