/* $Id$ */
/* Copyright (c) 2010 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Graphics GServer */
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */



#ifdef DEBUG
# include <stdio.h>
#endif
#include <string.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include "video.h"


/* GLX */
/* private */
/* prototypes */
static int _glx_init(void);
static void _glx_destroy(void);
static void _glx_swap_buffers(void);


/* variables */
static Display * _display;
static Window _window;


/* public */
/* variables */
VideoPlugin video_plugin =
{
	_glx_init,
	_glx_destroy,
	_glx_swap_buffers
};


/* private */
/* functions */
/* glx_init */
static int _glx_init(void)
{
	int screen;
	int attributes[] = { GLX_RGBA, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4,
		GLX_BLUE_SIZE, 4, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
	XVisualInfo * vi;
	GLXContext context;
	XSetWindowAttributes attr;
	Atom wdelete;
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	Window wdummy;
	unsigned int bdummy;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	_display = XOpenDisplay(0);
	screen = DefaultScreen(_display);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() screen=%d\n", __func__, screen);
#endif
	if((vi = glXChooseVisual(_display, screen, attributes)) == NULL)
	{
		attributes[(sizeof(attributes) / sizeof(*attributes)) - 2]
			= None;
		vi = glXChooseVisual(_display, screen, attributes);
	}
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() got visual\n", __func__);
#endif
	context = glXCreateContext(_display, vi, 0, GL_TRUE);
	memset(&attr, 0, sizeof(attr));
	attr.colormap = XCreateColormap(_display, RootWindow(_display,
				vi->screen), vi->visual, AllocNone);
	attr.border_pixel = 0;
	attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
		| StructureNotifyMask;
	_window = XCreateWindow(_display, RootWindow(_display, vi->screen),
			0, 0, 640, 480, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap | CWEventMask, &attr);
	wdelete = XInternAtom(_display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(_display, _window, &wdelete, 1);
	XSetStandardProperties(_display, _window, "GServer GLX", "GServer GLX",
			None, NULL, 0, NULL);
	XMapRaised(_display, _window);
	glXMakeCurrent(_display, _window, context);
	XGetGeometry(_display, _window, &wdummy, &x, &y, &width, &height,
			&bdummy, &depth);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() (%d,%d) (%ux%u@%u)\n", __func__, x, y,
			width, height, depth);
#endif
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(height == 0)
		height = 1;
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glXSwapBuffers(_display, _window);
	return 0;
}


/* glx_destroy */
static void _glx_destroy(void)
{
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
}


/* glx_swap_buffers */
static void _glx_swap_buffers(void)
{
#if 0
	XEvent event;

	while(XPending(_display) > 0)
	{
		XNextEvent(_display, &event);
	}
#endif
	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)640 / (GLfloat)480, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(3.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();
	glXSwapBuffers(_display, _window);
}
