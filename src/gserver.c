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



#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "video/video.h"
#include "../data/GServer.h"
#include "gserver.h"
#include "../config.h"

#ifdef DEBUG
# define DEBUG_INTERFACE() fprintf(stderr, "DEBUG: %s()\n", __func__)
# define DEBUG_INTERFACE1i(x) fprintf(stderr, "DEBUG: %s(0x%x)\n", __func__, x)
# define DEBUG_INTERFACE3f(x, y, z) fprintf(stderr, \
		"DEBUG: %s(%.1f, %.1f, %.1f)\n", __func__, *x, *y, *z)
# define DEBUG_INTERFACE3i(x, y, z) fprintf(stderr, \
		"DEBUG: %s(%d, %d, %d)\n", __func__, x, y, z)
# define DEBUG_INTERFACE4f(x, y, z, t) fprintf(stderr, \
		"DEBUG: %s(%.1f, %.1f, %.1f, %.1f)\n", __func__, *x, *y, *z, *t)
#else
# define DEBUG_INTERFACE()
# define DEBUG_INTERFACE1i(x)
# define DEBUG_INTERFACE3f(x, y, z)
# define DEBUG_INTERFACE3i(x, y, z)
# define DEBUG_INTERFACE4f(x, y, z, t)
#endif


/* GServer */
/* private */
/* types */
struct _GServer
{
	Event * event;
	int event_own;
	AppServer * appserver;
	int loop;

	/* plugins */
	/* video */
	VideoPluginHelper video_helper;
	void * video_handle;
	VideoPlugin * video_plugin;
};


/* variables */
static GServer * _gserver = NULL;


/* public */
/* functions */
/* gserver_new */
static int _new_init(AppServerOptions options, GServer * gserver,
		Event * event);
static int _init_video(GServer * gserver);

GServer * gserver_new(AppServerOptions options, Event * event)
{
	GServer * gserver;

	if((gserver = object_new(sizeof(*gserver))) == NULL)
		return NULL;
	_gserver = gserver;
	if(_new_init(options, gserver, event) != 0)
	{
		object_delete(gserver);
		return NULL;
	}
	return gserver;
}

static int _new_init(AppServerOptions options, GServer * gserver, Event * event)
{
	gserver->video_handle = NULL;
	gserver->video_plugin = NULL;
	if((gserver->event = event) != NULL)
		gserver->event_own = 0;
	else if((gserver->event = event_new()) == NULL)
		return -1;
	else
		gserver->event_own = 1;
	gserver->video_helper.gserver = gserver;
	gserver->video_helper.get_event = gserver_get_event;
	if((gserver->appserver = appserver_new_event("GServer", options,
					gserver->event)) != NULL
			&& _init_video(gserver) == 0)
	{
		gserver->loop = 1;
		return 0;
	}
	if(gserver->appserver != NULL)
		appserver_delete(gserver->appserver);
	if(gserver->event_own != 0)
		event_delete(gserver->event);
	return -1;
}

static int _init_video(GServer * gserver)
	/* FIXME ask Hardware what to load instead of hard-coding glx */
{
	char const filename[] = PREFIX "/lib/" PACKAGE "/video/glx.so";

	if((gserver->video_handle = dlopen(filename, RTLD_LAZY)) == NULL)
		return error_set_code(1, "%s: %s", filename, dlerror());
	if((gserver->video_plugin = dlsym(gserver->video_handle,
					"video_plugin")) == NULL)
	{
		error_set_code(1, "%s: %s", filename, dlerror());
		dlclose(gserver->video_handle);
		return 1;
	}
	gserver->video_plugin->helper = &gserver->video_helper;
	gserver->video_plugin->init(gserver->video_plugin);
	return 0;
}


/* gserver_delete */
static void _destroy_video(GServer * gserver);

void gserver_delete(GServer * gserver)
{
	if(_gserver == gserver)
		_gserver = NULL;
	_destroy_video(gserver);
	if(gserver->appserver != NULL)
		appserver_delete(gserver->appserver);
	if(gserver->event != NULL)
		event_delete(gserver->event);
	object_delete(gserver);
}

static void _destroy_video(GServer * gserver)
{
	if(gserver->video_plugin != NULL)
		gserver->video_plugin->destroy(gserver->video_plugin);
	if(gserver->video_handle != NULL)
		dlclose(gserver->video_handle);
}


/* accessors */
/* gserver_get_event */
Event * gserver_get_event(GServer * gserver)
{
	return gserver->event;
}


/* useful */
/* gserver_loop */
int gserver_loop(GServer * gserver)
{
	int ret = 0;

	while(gserver->loop == 1)
		ret |= event_loop(gserver->event);
	return ret;
}


/* interface */
#define GSERVER_PROTO0(func) \
	void GServer_ ## func (void) \
{ \
	DEBUG_INTERFACE(); \
	_gserver->video_plugin->proto0(_gserver->video_plugin, NULL, \
			VIDEO_PROTO0_ ## func); \
}
#define GSERVER_PROTO1d(func) \
	void GServer_ ## func(double * x) \
{ \
	DEBUG_INTERFACE(); \
	_gserver->video_plugin->proto1d(_gserver->video_plugin, NULL, \
			VIDEO_PROTO1d_ ## func, *x); \
}
#define GSERVER_PROTO1i(func, type1) \
	void GServer_ ## func (type1 x) \
{ \
	DEBUG_INTERFACE1i(x); \
	_gserver->video_plugin->proto1i(_gserver->video_plugin, NULL, \
			VIDEO_PROTO1i_ ## func, x); \
}
#define GSERVER_PROTO3f(func) \
	void GServer_ ## func (float * x, float * y, float * z) \
{ \
	DEBUG_INTERFACE3f(x, y, z); \
	_gserver->video_plugin->proto3f(_gserver->video_plugin, NULL, \
			VIDEO_PROTO3f_ ## func, *x, *y, *z); \
}
#define GSERVER_PROTO3i(func, type1, type2, type3) \
	void GServer_ ## func (int32_t x, int32_t y, int32_t z) \
{ \
	DEBUG_INTERFACE3i(x, y, z); \
	_gserver->video_plugin->proto3i(_gserver->video_plugin, NULL, \
			VIDEO_PROTO3i_ ## func, x, y, z); \
}
#define GSERVER_PROTO4f(func) \
	void GServer_ ## func (float * x, float * y, float * z, float * t) \
{ \
	DEBUG_INTERFACE4f(x, y, z, t); \
	_gserver->video_plugin->proto4f(_gserver->video_plugin, NULL, \
			VIDEO_PROTO4f_ ## func, *x, *y, *z, *t); \
}

/* proto0 */
GSERVER_PROTO0(glEnd)
GSERVER_PROTO0(glLoadIdentity)
GSERVER_PROTO0(glFlush)
GSERVER_PROTO0(SwapBuffers)

/* proto1d */
GSERVER_PROTO1d(glClearDepth)

/* proto1i */
GSERVER_PROTO1i(glBegin, uint32_t)
GSERVER_PROTO1i(glClear, uint32_t)

/* proto3f */
GSERVER_PROTO3f(glColor3f)
GSERVER_PROTO3f(glTranslatef)
GSERVER_PROTO3f(glVertex3f)

/* proto3i */
GSERVER_PROTO3i(glColor3i, int32_t, int32_t, int32_t)
GSERVER_PROTO3i(glVertex3i, int32_t, int32_t, int32_t)

/* proto4f */
GSERVER_PROTO4f(glClearColor)
GSERVER_PROTO4f(glRotatef)
