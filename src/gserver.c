/* $Id$ */
/* Copyright (c) 2010-2020 Pierre Pronchery <khorben@defora.org> */
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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <System.h>
#include <System/Marshall.h>
#include "GServer/video.h"
#include "../data/GServer.h"
#include "platform.h"
#include "gserver.h"
#include "../config.h"

#ifndef PREFIX
# define PREFIX		"/usr/local"
#endif
#ifndef LIBDIR
# define LIBDIR		PREFIX "/lib"
#endif

#ifdef DEBUG
# define DEBUG_INTERFACE() fprintf(stderr, "DEBUG: %s()\n", __func__)
# define DEBUG_INTERFACE1i(x) fprintf(stderr, "DEBUG: %s(0x%x)\n", __func__, x)
# define DEBUG_INTERFACE2d(x, y) \
	fprintf(stderr, "DEBUG: %s(%.1f, %.1f)\n", __func__, x, y)
# define DEBUG_INTERFACE2f(x, y) DEBUG_INTERFACE2d(x, y)
# define DEBUG_INTERFACE2i(x, y) \
	fprintf(stderr, "DEBUG: %s(0x%x, 0x%x)\n", __func__, x, y)
# define DEBUG_INTERFACE3b(x, y, z) DEBUG_INTERFACE3i(x, y, z)
# define DEBUG_INTERFACE3d(x, y, z) \
	fprintf(stderr, "DEBUG: %s(%.1f, %.1f, %.1f)\n", __func__, x, y, z)
# define DEBUG_INTERFACE3f(x, y, z) DEBUG_INTERFACE3d(x, y, z)
# define DEBUG_INTERFACE3i(x, y, z) \
	fprintf(stderr, "DEBUG: %s(0x%x, 0x%x, 0x%x)\n", __func__, x, y, z)
# define DEBUG_INTERFACE3s(x, y, z) DEBUG_INTERFACE3i(x, y, z)
# define DEBUG_INTERFACE4b(x, y, z, t) DEBUG_INTERFACE4i(x, y, z, t)
# define DEBUG_INTERFACE4d(x, y, z, t) \
	fprintf(stderr, "DEBUG: %s(%.1f, %.1f, %.1f, %.1f)\n", \
			__func__, x, y, z, t)
# define DEBUG_INTERFACE4f(x, y, z, t) DEBUG_INTERFACE4d(x, y, z, t)
# define DEBUG_INTERFACE4i(x, y, z, t) \
	fprintf(stderr, "DEBUG: %s(0x%x, 0x%x, 0x%x, 0x%x)\n", \
			__func__, x, y, z, t)
# define DEBUG_INTERFACE4s(x, y, z, t) DEBUG_INTERFACE4i(x, y, z, t)
#else
# define DEBUG_INTERFACE()
# define DEBUG_INTERFACE1i(x)
# define DEBUG_INTERFACE2d(x, y)
# define DEBUG_INTERFACE2f(x, y)
# define DEBUG_INTERFACE2i(x, y)
# define DEBUG_INTERFACE3b(x, y, z)
# define DEBUG_INTERFACE3d(x, y, z)
# define DEBUG_INTERFACE3f(x, y, z)
# define DEBUG_INTERFACE3i(x, y, z)
# define DEBUG_INTERFACE3s(x, y, z)
# define DEBUG_INTERFACE4b(x, y, z, t)
# define DEBUG_INTERFACE4d(x, y, z, t)
# define DEBUG_INTERFACE4f(x, y, z, t)
# define DEBUG_INTERFACE4i(x, y, z, t)
# define DEBUG_INTERFACE4s(x, y, z, t)
#endif


/* GServer */
/* private */
/* types */
typedef struct _GServerCall GServerCall;
typedef struct _GServerClient GServerClient;

struct _App
{
	GServerPlatform * platform;

	Event * event;
	int event_own;
	AppServer * appserver;
	int loop;
	MarshallCallback * calls;

	/* plugins */
	/* video */
	GServerVideoPluginHelper video_helper;
	void * video_handle;
	GServerVideoPlugin * video_plugin;

	/* clients */
	GServerClient * clients;
	size_t clients_cnt;
};

struct _GServerCall
{
	GServerVideoCall type;
	MarshallCallback callback;
	Variable ** args;
	size_t args_cnt;
};

typedef struct _GServerOpenGLCall
{
	String const * name;
	GServerVideoCall type;
} GServerOpenGLCall;

struct _GServerClient
{
	AppServerClient * asc;
	GServerCall * calls;
	size_t calls_cnt;
};


/* constants */
const GServerOpenGLCall _gserver_calls[] =
{
	/* GSERVER_VIDEO_CALL0 */
	{ "glEnd",		GSERVER_VIDEO_CALL_0	},
	{ "glEndList",		GSERVER_VIDEO_CALL_0	},
	{ "glFlush",		GSERVER_VIDEO_CALL_0	},
	{ "glLoadIdentity",	GSERVER_VIDEO_CALL_0	},
	{ "SwapBuffers",	GSERVER_VIDEO_CALL_0	},
	/* GSERVER_VIDEO_CALL1d */
	{ "glClearDepth",	GSERVER_VIDEO_CALL_1d	},
	/* GSERVER_VIDEO_CALL1f */
	{ "glClearIndex",	GSERVER_VIDEO_CALL_1f	},
	{ "glPointSize",	GSERVER_VIDEO_CALL_1f	},
	/* GSERVER_VIDEO_CALL1i */
	{ "glActiveTexture",	GSERVER_VIDEO_CALL_1i	},
	{ "glArrayElement",	GSERVER_VIDEO_CALL_1i	},
	{ "glBegin",		GSERVER_VIDEO_CALL_1i	},
	{ "glBlendEquation",	GSERVER_VIDEO_CALL_1i	},
	{ "glCallList",		GSERVER_VIDEO_CALL_1i	},
	{ "glClear",		GSERVER_VIDEO_CALL_1i	},
	{ "glClearStencil",	GSERVER_VIDEO_CALL_1i	},
	{ "glClientActiveTexture",GSERVER_VIDEO_CALL_1i	},
#if 0
	{ "glCompileShader",	GSERVER_VIDEO_CALL_1i	},
#endif
	{ "glDepthFunc",	GSERVER_VIDEO_CALL_1i	},
	{ "glDisable",		GSERVER_VIDEO_CALL_1i	},
	{ "glDisableClientState",GSERVER_VIDEO_CALL_1i	},
	{ "glEnable",		GSERVER_VIDEO_CALL_1i	},
	{ "glEnableClientState",GSERVER_VIDEO_CALL_1i	},
	{ "glIsEnabled",	GSERVER_VIDEO_CALL_1i	},
	{ "glShadeModel",	GSERVER_VIDEO_CALL_1i	},
	/* GSERVER_VIDEO_CALL2f */
	{ "glPolygonOffset",	GSERVER_VIDEO_CALL_2f	},
	{ "glTexCoord2f",	GSERVER_VIDEO_CALL_2f	},
	/* GSERVER_VIDEO_CALL2i */
	{ "glAttachShader",	GSERVER_VIDEO_CALL_2i	},
	{ "glBeginQuery",	GSERVER_VIDEO_CALL_2i	},
	{ "glBindBuffer",	GSERVER_VIDEO_CALL_2i	},
	{ "glBindTexture",	GSERVER_VIDEO_CALL_2i	},
	{ "glBlendEquationSeparate",GSERVER_VIDEO_CALL_2i	},
#if 0
	{ "glBindTexture",	GSERVER_VIDEO_CALL_2i	},
#endif
	{ "glBlendFunc",	GSERVER_VIDEO_CALL_2i	},
	{ "glColorMaterial",	GSERVER_VIDEO_CALL_2i	},
	{ "glFogi",		GSERVER_VIDEO_CALL_2i	},
	{ "glHint",		GSERVER_VIDEO_CALL_2i	},
	{ "glLightModeli",	GSERVER_VIDEO_CALL_2i	},
	{ "glVertex2i",		GSERVER_VIDEO_CALL_2i	},
	/* GSERVER_VIDEO_CALL_3b */
	{ "glColor3b",		GSERVER_VIDEO_CALL_3b	},
	{ "glColor3ub",		GSERVER_VIDEO_CALL_3b	},
	/* GSERVER_VIDEO_CALL_3d */
	{ "glColor3d",		GSERVER_VIDEO_CALL_3d	},
	{ "glTranslate3d",	GSERVER_VIDEO_CALL_3d	},
	/* GSERVER_VIDEO_CALL_3f */
	{ "glColor3f",		GSERVER_VIDEO_CALL_3f	},
	{ "glLightf",		GSERVER_VIDEO_CALL_3f	},
	{ "glNormal3f",		GSERVER_VIDEO_CALL_3f	},
	{ "glTranslate3f",	GSERVER_VIDEO_CALL_3f	},
	{ "glVertex3f",		GSERVER_VIDEO_CALL_3f	},
	/* GSERVER_VIDEO_CALL_3i */
	{ "glColor3i",		GSERVER_VIDEO_CALL_3i	},
	{ "glColor3ui",		GSERVER_VIDEO_CALL_3i	},
	{ "glDrawArrays",	GSERVER_VIDEO_CALL_3i	},
	{ "glLighti",		GSERVER_VIDEO_CALL_3i	},
	{ "glTexGeni",		GSERVER_VIDEO_CALL_3i	},
	{ "glTexParameteri",	GSERVER_VIDEO_CALL_3i	},
	{ "glVertex3i",		GSERVER_VIDEO_CALL_3i	},
	/* GSERVER_VIDEO_CALL_3s */
	{ "glColor3s",		GSERVER_VIDEO_CALL_3s	},
	{ "glColor3us",		GSERVER_VIDEO_CALL_3s	},
	/* GSERVER_VIDEO_CALL_4b */
	{ "glColor4b",		GSERVER_VIDEO_CALL_4d	},
	{ "glColor4ub",		GSERVER_VIDEO_CALL_4d	},
	/* GSERVER_VIDEO_CALL_4d */
	{ "glColor4d",		GSERVER_VIDEO_CALL_4d	},
	/* GSERVER_VIDEO_CALL_4f */
	{ "glClearAccum",	GSERVER_VIDEO_CALL_4f	},
	{ "glClearColor",	GSERVER_VIDEO_CALL_4f	},
	{ "glColor4f",		GSERVER_VIDEO_CALL_4f	},
	{ "glRotatef",		GSERVER_VIDEO_CALL_4f	}
};


/* prototypes */
/* accessors */
static GServerClient * _gserver_get_client(GServer * gserver, void * id);
static MarshallCallback _gserver_get_call(GServer * gserver,
		String const * name);

/* useful */
static int _gserver_call(GServer * gserver, Variable * ret,
		char const * name, size_t args_cnt, Variable ** args);
static void _gserver_client_calls(GServer * gserver, GServerClient * client);

/* queue */
static int _gserver_queue(GServer * gserver, AppServerClient * asc,
		GServerVideoCall type, char const * name, ...);


/* public */
/* functions */
/* gserver_new */
static int _new_init(AppServerOptions options, GServer * gserver,
		Event * event);
static int _new_init_opengl(GServer * gserver);
static int _new_init_video(GServer * gserver);

GServer * gserver_new(AppServerOptions options, Event * event)
{
	GServer * gserver;

	if((gserver = object_new(sizeof(*gserver))) == NULL)
		return NULL;
	if(_new_init(options, gserver, event) != 0)
	{
		object_delete(gserver);
		return NULL;
	}
	return gserver;
}

static int _new_init(AppServerOptions options, GServer * gserver, Event * event)
{
	if((gserver->platform = gserverplatform_new()) == NULL)
		return -1;
	gserver->video_handle = NULL;
	gserver->video_plugin = NULL;
	gserver->clients = NULL;
	gserver->clients_cnt = 0;
	if((gserver->event = event) != NULL)
		gserver->event_own = 0;
	else if((gserver->event = event_new()) == NULL)
	{
		gserverplatform_delete(gserver->platform);
		return -1;
	}
	else
		gserver->event_own = 1;
	gserver->calls = NULL;
	gserver->video_helper.gserver = gserver;
	gserver->video_helper.get_event = gserver_get_event;
	gserver->video_helper.get_platform = gserver_get_platform;
	gserver->video_helper.refresh = gserver_refresh;
	if((gserver->appserver = appserver_new_event(gserver, options,
					"GServer", NULL, gserver->event))
			!= NULL
			&& _new_init_video(gserver) == 0
			&& _new_init_opengl(gserver) == 0)
	{
		gserver->loop = 1;
		return 0;
	}
	if(gserver->calls != NULL)
		object_delete(gserver->calls);
	if(gserver->appserver != NULL)
		appserver_delete(gserver->appserver);
	if(gserver->event_own != 0)
		event_delete(gserver->event);
	gserverplatform_delete(gserver->platform);
	return -1;
}

static int _new_init_opengl(GServer * gserver)
{
	Plugin * plugin;
	const size_t cnt = sizeof(_gserver_calls) / sizeof(*_gserver_calls);
	size_t i;

	if((gserver->calls = object_new(cnt * sizeof(*gserver->calls))) == NULL)
		return -1;
	if((plugin = plugin_new_self()) == NULL)
		return -1;
	for(i = 0; i < cnt; i++)
		if((gserver->calls[i] = (MarshallCallback)plugin_lookup(
						plugin, _gserver_calls[i].name))
				== NULL)
			break;
	plugin_delete(plugin);
	return (i == cnt) ? 0 : -1;
}

static int _new_init_video(GServer * gserver)
{
	String const subsystem[] = "video";
	GServerPlatform * platform = gserver->platform;

	if((gserver->video_handle = plugin_new(LIBDIR, PACKAGE, subsystem,
					gserverplatform_get_driver(platform,
						subsystem))) == NULL)
		return 1;
	if((gserver->video_plugin = plugin_lookup(gserver->video_handle,
					"video_plugin")) == NULL)
	{
		plugin_delete(gserver->video_handle);
		gserver->video_handle = NULL;
		return 1;
	}
	gserver->video_plugin->helper = &gserver->video_helper;
	gserver->video_plugin->init(gserver->video_plugin);
	return 0;
}


/* gserver_delete */
static void _delete_video(GServer * gserver);

void gserver_delete(GServer * gserver)
{
	_delete_video(gserver);
	if(gserver->calls != NULL)
		object_delete(gserver->calls);
	if(gserver->appserver != NULL)
		appserver_delete(gserver->appserver);
	if(gserver->event != NULL)
		event_delete(gserver->event);
	if(gserver->platform != NULL)
		gserverplatform_delete(gserver->platform);
	object_delete(gserver);
}

static void _delete_video(GServer * gserver)
{
	if(gserver->video_plugin != NULL)
		gserver->video_plugin->destroy(gserver->video_plugin);
	if(gserver->video_handle != NULL)
		plugin_delete(gserver->video_handle);
}


/* accessors */
/* gserver_get_event */
Event * gserver_get_event(GServer * gserver)
{
	return gserver->event;
}


/* gserver_get_platform */
GServerPlatform * gserver_get_platform(GServer * gserver)
{
	return gserver->platform;
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


/* gserver_refresh */
void gserver_refresh(GServer * gserver)
{
	size_t i;
	uint32_t u = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	Variable * v;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	v = variable_new(VT_UINT32, &u);
	_gserver_call(gserver, NULL, "glClear", 1, &v);
	_gserver_call(gserver, NULL, "glLoadIdentity", 0, NULL);
	for(i = 0; i < gserver->clients_cnt; i++)
		_gserver_client_calls(gserver, &gserver->clients[i]);
	_gserver_call(gserver, NULL, "SwapBuffers", 0, NULL);
}


/* interface */
#define GSERVER_PROTO0(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc) \
{ \
	DEBUG_INTERFACE(); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_0, \
			"" # func); \
}
#define GSERVER_PROTO1d(type, func) \
	type GServer_ ## func(GServer * gserver, AppServerClient * asc, \
			double x) \
{ \
	DEBUG_INTERFACE(); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_1d, \
			"" # func, x); \
}
#define GSERVER_PROTO1f(type, func) \
	type GServer_ ## func(GServer * gserver, AppServerClient * asc, \
			float x) \
{ \
	DEBUG_INTERFACE(); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_1f, \
			"" # func, x); \
}
#define GSERVER_PROTO1i(type, func, type1) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x) \
{ \
	DEBUG_INTERFACE1i(x); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_1i, \
			"" # func, x); \
}
#define GSERVER_PROTO2f(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			float x, float y) \
{ \
	DEBUG_INTERFACE2f(x, y); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_2f, \
			"" # func, x, y); \
}
#define GSERVER_PROTO2i(type, func, type1, type2) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y) \
{ \
	DEBUG_INTERFACE2i(x, y); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_2i, \
			"" # func, x, y); \
}
#define GSERVER_PROTO3b(type, func, type1, type2, type3) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z) \
{ \
	DEBUG_INTERFACE3b(x, y, z); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_3b, \
			"" # func, x, y, z); \
}
#define GSERVER_PROTO3d(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			double x, double y, double z) \
{ \
	DEBUG_INTERFACE3d(x, y, z); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_3d, \
			"" # func, x, y, z); \
}
#define GSERVER_PROTO3f(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			float x, float y, float z) \
{ \
	DEBUG_INTERFACE3f(x, y, z); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_3f, \
			"" # func, x, y, z); \
}
#define GSERVER_PROTO3i(type, func, type1, type2, type3) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z) \
{ \
	DEBUG_INTERFACE3i(x, y, z); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_3i, \
			"" # func, x, y, z); \
}
#define GSERVER_PROTO3s(type, func, type1, type2, type3) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z) \
{ \
	DEBUG_INTERFACE3s(x, y, z); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_3s, \
			"" # func, x, y, z); \
}
#define GSERVER_PROTO4b(type, func, type1, type2, type3, type4) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z, type4 t) \
{ \
	DEBUG_INTERFACE4b(x, y, z, t); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_4b, \
			"" # func, x, y, z, t); \
}
#define GSERVER_PROTO4d(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			double x, double y, double z, double t) \
{ \
	DEBUG_INTERFACE4d(x, y, z, t); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_4d, \
			"" # func, x, y, z, t); \
}
#define GSERVER_PROTO4f(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			float x, float y, float z, float t) \
{ \
	DEBUG_INTERFACE4f(x, y, z, t); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_4f, \
			"" # func, x, y, z, t); \
}
#define GSERVER_PROTO4i(type, func, type1, type2, type3, type4) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z, type4 t) \
{ \
	DEBUG_INTERFACE4i(x, y, z, t); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_4i, \
			"" # func, x, y, z, t); \
}
#define GSERVER_PROTO4s(type, func, type1, type2, type3, type4) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, \
			type1 x, type2 y, type3 z, type4 t) \
{ \
	DEBUG_INTERFACE4s(x, y, z, t); \
	_gserver_queue(gserver, asc, GSERVER_VIDEO_CALL_4s, \
			"" # func, x, y, z, t); \
}

/* proto0 */
GSERVER_PROTO0(void, glEnd)
GSERVER_PROTO0(void, glEndList)
GSERVER_PROTO0(void, glFlush)
GSERVER_PROTO0(void, glLoadIdentity)
GSERVER_PROTO0(void, SwapBuffers)

/* proto1d */
GSERVER_PROTO1d(void, glClearDepth)

/* proto1f*/
GSERVER_PROTO1f(void, glClearIndex)
GSERVER_PROTO1f(void, glPointSize)

/* proto1i */
GSERVER_PROTO1i(void, glActiveTexture, uint32_t)
GSERVER_PROTO1i(void, glArrayElement, int32_t)
GSERVER_PROTO1i(void, glBegin, uint32_t)
GSERVER_PROTO1i(void, glBlendEquation, uint32_t)
GSERVER_PROTO1i(void, glCallList, uint32_t)
GSERVER_PROTO1i(void, glClear, uint32_t)
GSERVER_PROTO1i(void, glClearStencil, int32_t)
GSERVER_PROTO1i(void, glClientActiveTexture, uint32_t)
#if 0
GSERVER_PROTO1i(void, glCompileShader, uint32_t)
#endif
GSERVER_PROTO1i(void, glDepthFunc, uint32_t)
GSERVER_PROTO1i(void, glDisable, uint32_t)
GSERVER_PROTO1i(void, glDisableClientState, uint32_t)
GSERVER_PROTO1i(void, glEnable, uint32_t)
GSERVER_PROTO1i(void, glEnableClientState, uint32_t)
/* FIXME really returns bool */
GSERVER_PROTO1i(bool, glIsEnabled, uint32_t)
GSERVER_PROTO1i(void, glShadeModel, uint32_t)

/* proto2f */
GSERVER_PROTO2f(void, glPolygonOffset)
GSERVER_PROTO2f(void, glTexCoord2f)

/* proto2i */
#if 0
GSERVER_PROTO2i(void, glAttachShader, uint32_t, uint32_t)
GSERVER_PROTO2i(void, glBeginQuery, uint32_t, uint32_t)
GSERVER_PROTO2i(void, glBindBuffer, uint32_t, uint32_t)
#endif
GSERVER_PROTO2i(void, glBindTexture, uint32_t, uint32_t)
#if 0
GSERVER_PROTO2i(void, glBlendEquationSeparate, uint32_t, uint32_t)
#endif
GSERVER_PROTO2i(void, glBlendFunc, uint32_t, uint32_t)
GSERVER_PROTO2i(void, glColorMaterial, uint32_t, uint32_t)
GSERVER_PROTO2i(void, glFogi, uint32_t, int32_t)
GSERVER_PROTO2i(void, glHint, uint32_t, uint32_t)
GSERVER_PROTO2i(void, glLightModeli, uint32_t, int32_t)
GSERVER_PROTO2i(void, glVertex2i, int32_t, int32_t)

/* proto3b */
GSERVER_PROTO3b(void, glColor3b, int8_t, int8_t, int8_t)
GSERVER_PROTO3b(void, glColor3ub, uint8_t, uint8_t, uint8_t)

/* proto3d */
GSERVER_PROTO3d(void, glColor3d)
GSERVER_PROTO3d(void, glTranslated)

/* proto3f */
GSERVER_PROTO3f(void, glColor3f)
GSERVER_PROTO3f(void, glNormal3f)
GSERVER_PROTO3f(void, glTranslatef)
GSERVER_PROTO3f(void, glVertex3f)

/* proto3i */
GSERVER_PROTO3i(void, glColor3i, int32_t, int32_t, int32_t)
GSERVER_PROTO3i(void, glColor3ui, uint32_t, uint32_t, uint32_t)
GSERVER_PROTO3i(void, glDrawArrays, uint32_t, int32_t, int32_t)
GSERVER_PROTO3i(void, glLighti, uint32_t, uint32_t, int32_t)
GSERVER_PROTO3i(void, glTexGeni, uint32_t, uint32_t, int32_t)
GSERVER_PROTO3i(void, glTexParameteri, uint32_t, uint32_t, uint32_t)
GSERVER_PROTO3i(void, glVertex3i, int32_t, int32_t, int32_t)

/* proto3s */
GSERVER_PROTO3s(void, glColor3s, int16_t, int16_t, int16_t)
GSERVER_PROTO3s(void, glColor3us, uint16_t, uint16_t, uint16_t)

/* proto4b */
GSERVER_PROTO4b(void, glColor4b, int8_t, int8_t, int8_t, int8_t)
GSERVER_PROTO4b(void, glColor4ub, uint8_t, uint8_t, uint8_t, uint8_t)

/* proto4d */
GSERVER_PROTO4d(void, glColor4d)

/* proto4f */
GSERVER_PROTO4f(void, glClearAccum)
GSERVER_PROTO4f(void, glClearColor)
GSERVER_PROTO4f(void, glColor4f)
GSERVER_PROTO4f(void, glRotatef)

/* proto4i */
#if 0
GSERVER_PROTO4i(void, glBlendFuncSeparate, uint32_t, uint32_t, uint32_t,
		uint32_t)
#endif
GSERVER_PROTO4i(void, glColor4i, int32_t, int32_t, int32_t, int32_t)
GSERVER_PROTO4i(void, glColor4ui, uint32_t, uint32_t, uint32_t, uint32_t)
GSERVER_PROTO4i(void, glColorMask, bool, bool, bool, bool)
GSERVER_PROTO4i(void, glScissor, int32_t, int32_t, int32_t, int32_t)
GSERVER_PROTO4i(void, glViewport, int32_t, int32_t, int32_t, int32_t)

/* proto4s */
GSERVER_PROTO4s(void, glColor4s, int16_t, int16_t, int16_t, int16_t)
GSERVER_PROTO4s(void, glColor4us, uint16_t, uint16_t, uint16_t, uint16_t)


/* private */
/* functions */
/* accessors */
static MarshallCallback _gserver_get_call(GServer * gserver,
		String const * name)
{
	const size_t cnt = sizeof(_gserver_calls) / sizeof(*_gserver_calls);
	size_t i;

	for(i = 0; i < cnt; i++)
		if(strcmp(_gserver_calls[i].name, name) == 0)
			return gserver->calls[i];
	/* XXX report the error */
	return NULL;
}


/* gserver_get_client */
static GServerClient * _gserver_get_client(GServer * gserver,
		AppServerClient * asc)
{
	GServerClient * ret;
	size_t i;

	for(i = 0; i < gserver->clients_cnt; i++)
		if(gserver->clients[i].asc == asc)
			return &gserver->clients[i];
	if((ret = realloc(gserver->clients, sizeof(*ret) * (gserver->clients_cnt
						+ 1))) == NULL)
		return NULL;
	gserver->clients = ret;
	ret = &gserver->clients[gserver->clients_cnt++];
	ret->asc = asc;
	ret->calls = NULL;
	ret->calls_cnt = 0;
	return ret;
}


/* useful */
/* gserver_call */
static int _gserver_call(GServer * gserver, Variable * ret,
		char const * name, size_t args_cnt, Variable ** args)
{
	MarshallCallback callback;

	if((callback = _gserver_get_call(gserver, name)) != NULL)
		return marshall_call(ret, callback, args_cnt, args);
	return -1;
}


/* gserver_client_calls */
static void _gserver_client_calls(GServer * gserver, GServerClient * client)
{
	size_t i;
	GServerCall * call;

	/* FIXME place the calls in context (windows...) */
	for(i = 0; i < client->calls_cnt; i++)
	{
		call = &client->calls[i];
		marshall_call(NULL, call->callback, call->args_cnt, call->args);
	}
}


/* queue */
/* gserver_queue */
static int _queue_error(GServerCall * call);

static int _gserver_queue(GServer * gserver, AppServerClient * asc,
		GServerVideoCall type, char const * name, ...)
{
	va_list ap;
	GServerCall * call;
	GServerClient * gsc;
	size_t i = 0;
	Variable * v;
	uint8_t u8;
	double d;
	float f;
	uint16_t u16;
	uint32_t u32;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, %u, %u)\n", __func__, (void *)gserver,
			type, name);
#endif
	if((gsc = _gserver_get_client(gserver, asc)) == NULL)
		return -1;
	if((call = realloc(gsc->calls, sizeof(*call) * (gsc->calls_cnt + 1)))
			== NULL)
		return -1;
	gsc->calls = call;
	call = &gsc->calls[gsc->calls_cnt];
	call->type = type;
	call->callback = _gserver_get_call(gserver, name);
#if 0 /* XXX probably hurts performance without protecting anything */
	memset(&gsc->args, 0, sizeof(gsc->args));
#endif
	va_start(ap, name);
	switch(type)
	{
		case GSERVER_VIDEO_CALL_0:
			/* FIXME intercept SwapBuffers() and glClear() */
			call->args_cnt = 0;
			call->args = NULL;
			break;
		case GSERVER_VIDEO_CALL_1d:
			call->args_cnt = 1;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			break;
		case GSERVER_VIDEO_CALL_1f:
			call->args_cnt = 1;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_FLOAT, &f);
			break;
		case GSERVER_VIDEO_CALL_1i:
			call->args_cnt = 1;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			break;
		case GSERVER_VIDEO_CALL_2f:
			call->args_cnt = 2;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_FLOAT, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_FLOAT, &f);
			break;
		case GSERVER_VIDEO_CALL_2i:
			call->args_cnt = 2;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			break;
		case GSERVER_VIDEO_CALL_3b:
			call->args_cnt = 3;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			break;
		case GSERVER_VIDEO_CALL_3d:
			call->args_cnt = 3;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			break;
		case GSERVER_VIDEO_CALL_3f:
			call->args_cnt = 3;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			break;
		case GSERVER_VIDEO_CALL_3i:
			call->args_cnt = 3;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			break;
		case GSERVER_VIDEO_CALL_3s:
			call->args_cnt = 3;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			break;
		case GSERVER_VIDEO_CALL_4b:
			call->args_cnt = 4;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			u8 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_BOOL, &u8);
			break;
		case GSERVER_VIDEO_CALL_4d:
			call->args_cnt = 4;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			d = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &d);
			break;
		case GSERVER_VIDEO_CALL_4f:
			call->args_cnt = 4;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			f = va_arg(ap, double);
			call->args[i++] = variable_new(VT_DOUBLE, &f);
			break;
		case GSERVER_VIDEO_CALL_4i:
			call->args_cnt = 4;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			u32 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT32, &u32);
			break;
		case GSERVER_VIDEO_CALL_4s:
			call->args_cnt = 4;
			if((call->args = object_new(sizeof(v) * call->args_cnt))
					== NULL)
				return _queue_error(call);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			u16 = va_arg(ap, uint32_t);
			call->args[i++] = variable_new(VT_UINT16, &u16);
			break;
	}
	va_end(ap);
	for(i = 0; i < call->args_cnt; i++)
		if(call->args[i] == NULL)
			return _queue_error(call);
	gsc->calls_cnt++;
	return 0;
}

static int _queue_error(GServerCall * call)
{
	size_t i;

	for(i = 0; i < call->args_cnt; i++)
		if(call->args[i] != NULL)
			variable_delete(call->args[i]);
	object_delete(call->args);
	return -1;
}
