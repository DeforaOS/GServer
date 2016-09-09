/* $Id$ */
/* Copyright (c) 2010-2016 Pierre Pronchery <khorben@defora.org> */
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
# define DEBUG_INTERFACE1u(x) DEBUG_INTERFACE1i(x)
# define DEBUG_INTERFACE3f(x, y, z) fprintf(stderr, \
		"DEBUG: %s(%.1f, %.1f, %.1f)\n", __func__, *x, *y, *z)
# define DEBUG_INTERFACE3i(x, y, z) fprintf(stderr, \
		"DEBUG: %s(%d, %d, %d)\n", __func__, x, y, z)
# define DEBUG_INTERFACE4f(x, y, z, t) fprintf(stderr, \
		"DEBUG: %s(%.1f, %.1f, %.1f, %.1f)\n", __func__, *x, *y, *z, *t)
#else
# define DEBUG_INTERFACE()
# define DEBUG_INTERFACE1i(x)
# define DEBUG_INTERFACE1u(x)
# define DEBUG_INTERFACE3f(x, y, z)
# define DEBUG_INTERFACE3i(x, y, z)
# define DEBUG_INTERFACE4f(x, y, z, t)
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
	GServerVideoProto type;
	unsigned int func;
	union
	{
		struct
		{
			double x;
		} _1d;
		struct
		{
			int32_t x;
		} _1i;
		struct
		{
			uint32_t x;
		} _1u;
		struct
		{
			float x;
			float y;
			float z;
		} _3f;
		struct
		{
			int32_t x;
			int32_t y;
			int32_t z;
		} _3i;
		struct
		{
			float x;
			float y;
			float z;
			float t;
		} _4f;
	} args;
};

struct _GServerClient
{
	AppServerClient * asc;
	GServerCall * calls;
	size_t calls_cnt;
};


/* variables */
static GServer * _gserver = NULL;


/* prototypes */
static void _gserver_client_calls(GServer * gserver, GServerClient * client);
static GServerClient * _gserver_get_client(GServer * gserver, void * id);

/* queue */
static GServerCall * _gserver_queue(GServer * gserver, AppServerClient * asc,
		GServerVideoProto type, unsigned int func);
static int _gserver_queue0(GServer * gserver, AppServerClient * asc,
		GServerVideoProto0 func);
static int _gserver_queue1d(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1d func, double x);
static int _gserver_queue1i(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1i func, int32_t x);
static int _gserver_queue1u(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1u func, uint32_t x);
static int _gserver_queue3f(GServer * gserver, AppServerClient * asc,
		GServerVideoProto3f func, float x, float y, float z);
static int _gserver_queue3i(GServer * gserver, AppServerClient * asc,
		GServerVideoProto3i func, int32_t x, int32_t y, int32_t z);
static int _gserver_queue4f(GServer * gserver, AppServerClient * asc,
		GServerVideoProto4f func, float x, float y, float z, float t);


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
	gserver->video_helper.gserver = gserver;
	gserver->video_helper.get_event = gserver_get_event;
	gserver->video_helper.get_platform = gserver_get_platform;
	gserver->video_helper.refresh = gserver_refresh;
	if((gserver->appserver = appserver_new_event(gserver, options,
					"GServer", NULL, gserver->event))
			!= NULL
			&& _init_video(gserver) == 0)
	{
		gserver->loop = 1;
		return 0;
	}
	if(gserver->appserver != NULL)
		appserver_delete(gserver->appserver);
	if(gserver->event_own != 0)
		event_delete(gserver->event);
	gserverplatform_delete(gserver->platform);
	return -1;
}

static int _init_video(GServer * gserver)
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
	if(_gserver == gserver)
		_gserver = NULL;
	_delete_video(gserver);
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

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	gserver->video_plugin->proto1i(gserver->video_plugin,
			GSERVER_VIDEO_PROTO1i_glClear,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gserver->video_plugin->proto0(gserver->video_plugin,
			GSERVER_VIDEO_PROTO0_glLoadIdentity);
	for(i = 0; i < gserver->clients_cnt; i++)
		_gserver_client_calls(gserver, &gserver->clients[i]);
	gserver->video_plugin->proto0(gserver->video_plugin,
			GSERVER_VIDEO_PROTO0_SwapBuffers);
}


/* interface */
#define GSERVER_PROTO0(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc) \
{ \
	DEBUG_INTERFACE(); \
	_gserver_queue0(gserver, asc, GSERVER_VIDEO_PROTO0_ ## func); \
}
#define GSERVER_PROTO1d(type, func) \
	type GServer_ ## func(GServer * gserver, AppServerClient * asc, double x) \
{ \
	DEBUG_INTERFACE(); \
	_gserver_queue1d(gserver, asc, GSERVER_VIDEO_PROTO1d_ ## func, x); \
}
#define GSERVER_PROTO1i(type, func, type1) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, type1 x) \
{ \
	DEBUG_INTERFACE1i(x); \
	_gserver_queue1i(gserver, asc, GSERVER_VIDEO_PROTO1i_ ## func, x); \
}
#define GSERVER_PROTO1u(type, func, type1) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, type1 x) \
{ \
	DEBUG_INTERFACE1u(x); \
	_gserver_queue1u(gserver, asc, GSERVER_VIDEO_PROTO1u_ ## func, x); \
}
#define GSERVER_PROTO3f(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, float x, float y, float z) \
{ \
	DEBUG_INTERFACE3f(x, y, z); \
	_gserver_queue3f(gserver, asc, GSERVER_VIDEO_PROTO3f_ ## func, x, y, z); \
}
#define GSERVER_PROTO3i(type, func, type1, type2, type3) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, int32_t x, int32_t y, int32_t z) \
{ \
	DEBUG_INTERFACE3i(x, y, z); \
	_gserver_queue3i(gserver, asc, GSERVER_VIDEO_PROTO3i_ ## func, x, y, z); \
}
#define GSERVER_PROTO4f(type, func) \
	type GServer_ ## func (GServer * gserver, AppServerClient * asc, float x, float y, float z, float t) \
{ \
	DEBUG_INTERFACE4f(x, y, z, t); \
	_gserver_queue4f(gserver, asc, GSERVER_VIDEO_PROTO4f_ ## func, x, y, z, t); \
}

/* proto0 */
GSERVER_PROTO0(void, glEnd)
GSERVER_PROTO0(void, glLoadIdentity)
GSERVER_PROTO0(void, glFlush)
GSERVER_PROTO0(void, SwapBuffers)

/* proto1d */
GSERVER_PROTO1d(void, glClearDepth)

/* proto1i */
GSERVER_PROTO1i(void, glBegin, uint32_t)
GSERVER_PROTO1i(void, glClear, uint32_t)

/* proto1u */
GSERVER_PROTO1u(void, glEnable, uint32_t)
/* FIXME really returns bool */
GSERVER_PROTO1u(bool, glIsEnabled, uint32_t)

/* proto3f */
GSERVER_PROTO3f(void, glColor3f)
GSERVER_PROTO3f(void, glTranslatef)
GSERVER_PROTO3f(void, glVertex3f)

/* proto3i */
GSERVER_PROTO3i(void, glColor3i, int32_t, int32_t, int32_t)
GSERVER_PROTO3i(void, glVertex3i, int32_t, int32_t, int32_t)

/* proto4f */
GSERVER_PROTO4f(void, glClearColor)
GSERVER_PROTO4f(void, glRotatef)


/* private */
/* functions */
/* gserver_client_calls */
static void _gserver_client_calls(GServer * gserver, GServerClient * client)
{
	size_t i;
	GServerCall * call;
	GServerVideoPlugin * vp = gserver->video_plugin;

	for(i = 0; i < client->calls_cnt; i++)
	{
		call = &client->calls[i];
		switch(call->type)
		{
			case GSERVER_VIDEO_PROTO_0:
				vp->proto0(vp, call->func);
				break;
			case GSERVER_VIDEO_PROTO_1d:
				vp->proto1d(vp, call->func, call->args._1d.x);
				break;
			case GSERVER_VIDEO_PROTO_1i:
				vp->proto1i(vp, call->func, call->args._1i.x);
				break;
			case GSERVER_VIDEO_PROTO_1u:
				vp->proto1u(vp, call->func, call->args._1u.x);
				break;
			case GSERVER_VIDEO_PROTO_3f:
				vp->proto3f(vp, call->func, call->args._3f.x,
						call->args._3f.y,
						call->args._3f.z);
				break;
			case GSERVER_VIDEO_PROTO_3i:
				vp->proto3i(vp, call->func, call->args._3i.x,
						call->args._3i.y,
						call->args._3i.z);
				break;
			case GSERVER_VIDEO_PROTO_4f:
				vp->proto4f(vp, call->func, call->args._4f.x,
						call->args._4f.y,
						call->args._4f.z,
						call->args._4f.t);
				break;
		}
	}
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


/* gserver_queue */
static GServerCall * _gserver_queue(GServer * gserver, AppServerClient * asc,
		GServerVideoProto type, unsigned int func)
{
	GServerCall * ret = NULL;
	GServerClient * gsc;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s(%p, %u, %u)\n", __func__, (void *)gserver,
			type, func);
#endif
	if((gsc = _gserver_get_client(gserver, asc)) == NULL)
		return NULL;
	if((ret = realloc(gsc->calls, sizeof(*ret) * (gsc->calls_cnt + 1)))
			== NULL)
		return NULL;
	gsc->calls = ret;
	ret = &gsc->calls[gsc->calls_cnt++];
	ret->type = type;
	ret->func = func;
#if 0 /* XXX probably hurts performance without protecting anything */
	memset(&gsc->args, 0, sizeof(gsc->args));
#endif
	return ret;
}


/* gserver_queue0 */
static int _gserver_queue0(GServer * gserver, AppServerClient * asc,
		GServerVideoProto0 func)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_0, func))
			== NULL)
		return -1;
	/* FIXME intercept SwapBuffers() and glClear() */
	return 0;
}


/* gserver_queue1d */
static int _gserver_queue1d(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1d func, double x)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_1d, func))
			== NULL)
		return -1;
	gsc->args._1d.x = x;
	return 0;
}


/* gserver_queue1i */
static int _gserver_queue1i(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1i func, int32_t x)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_1i, func))
			== NULL)
		return -1;
	gsc->args._1i.x = x;
	return 0;
}


/* gserver_queue1u */
static int _gserver_queue1u(GServer * gserver, AppServerClient * asc,
		GServerVideoProto1u func, uint32_t x)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_1u, func))
			== NULL)
		return -1;
	gsc->args._1u.x = x;
	return 0;
}


/* gserver_queue3f */
static int _gserver_queue3f(GServer * gserver, AppServerClient * asc,
		GServerVideoProto3f func, float x, float y, float z)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_3f, func))
			== NULL)
		return -1;
	gsc->args._3f.x = x;
	gsc->args._3f.y = y;
	gsc->args._3f.z = z;
	return 0;
}


/* gserver_queue3i */
static int _gserver_queue3i(GServer * gserver, AppServerClient * asc,
		GServerVideoProto3i func, int32_t x, int32_t y, int32_t z)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_3i, func))
			== NULL)
		return -1;
	gsc->args._3i.x = x;
	gsc->args._3i.y = y;
	gsc->args._3i.z = z;
	return 0;
}


/* gserver_queue4f */
static int _gserver_queue4f(GServer * gserver, AppServerClient * asc,
		GServerVideoProto4f func, float x, float y, float z, float t)
{
	GServerCall * gsc;

	if((gsc = _gserver_queue(gserver, asc, GSERVER_VIDEO_PROTO_4f, func))
			== NULL)
		return -1;
	gsc->args._4f.x = x;
	gsc->args._4f.y = y;
	gsc->args._4f.z = z;
	gsc->args._4f.t = t;
	return 0;
}
