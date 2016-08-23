/* $Id$ */
/* Copyright (c) 2016 Pierre Pronchery <khorben@defora.org> */
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
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "GServer/video.h"
#include "../../config.h"


/* SDL */
/* private */
/* types */
typedef struct _SDLVideo
{
	SDL_VideoInfo const * info;
	unsigned int width;
	unsigned int height;
} SDLVideo;


/* prototypes */
static int _sdl_init(GServerVideoPlugin * plugin);
static void _sdl_destroy(GServerVideoPlugin * plugin);

static void _sdl_proto0(GServerVideoPlugin * plugin, GServerVideoProto0 func);
static void _sdl_proto1d(GServerVideoPlugin * plugin, GServerVideoProto1d func,
		double x);
static void _sdl_proto1i(GServerVideoPlugin * plugin, GServerVideoProto1i func,
		int32_t x);
static void _sdl_proto3f(GServerVideoPlugin * plugin, GServerVideoProto3f func,
		float x, float y, float z);
static void _sdl_proto3i(GServerVideoPlugin * plugin, GServerVideoProto3i func,
		int32_t x, int32_t y, int32_t z);
static void _sdl_proto4f(GServerVideoPlugin * plugin, GServerVideoProto4f func,
		float x, float y, float z, float t);

static void _sdl_swap_buffers(void);


/* variables */
static void (*_sdl_func0[GSERVER_VIDEO_PROTO0_COUNT])(void) =
{
	glEnd,
	glFlush,
	glLoadIdentity,
	_sdl_swap_buffers
};

static void (*_sdl_func1d[GSERVER_VIDEO_PROTO1d_COUNT])(double) =
{
	glClearDepth
};

static void (*_sdl_func1i[GSERVER_VIDEO_PROTO1i_COUNT])(uint32_t) =
{
	glBegin,
	glClear
};

static void (*_sdl_func3f[GSERVER_VIDEO_PROTO3f_COUNT])(float, float, float) =
{
	glColor3f,
	glTranslatef,
	glVertex3f
};

static void (*_sdl_func3i[GSERVER_VIDEO_PROTO3i_COUNT])(int32_t, int32_t,
		int32_t) =
{
	glColor3i,
	glVertex3i
};

static void (*_sdl_func4f[GSERVER_VIDEO_PROTO4f_COUNT])(float, float, float,
		float) =
{
	glClearColor,
	glRotatef
};


/* public */
/* variables */
GServerVideoPlugin video_plugin =
{
	NULL,
	"SDL",
	_sdl_init,
	_sdl_destroy,
	_sdl_proto0,
	_sdl_proto1d,
	_sdl_proto1i,
	_sdl_proto3f,
	_sdl_proto3i,
	_sdl_proto4f,
	NULL
};


/* private */
/* functions */
/* sdl_init */
static int _sdl_init(GServerVideoPlugin * plugin)
{
	GServerPlatform * platform;
	SDLVideo * sdl;
	unsigned int depth;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	if((sdl = object_new(sizeof(*sdl))) == NULL)
		return 1;
	plugin->priv = sdl;
	platform = plugin->helper->get_platform(plugin->helper->gserver);
	if(SDL_Init(SDL_INIT_VIDEO) < 0
			|| (sdl->info = SDL_GetVideoInfo()) == NULL)
	{
		object_delete(sdl);
		return 1;
	}
	sdl->width = gserverplatform_get_video_width(platform);
	sdl->height = gserverplatform_get_video_height(platform);
	depth = sdl->info->vfmt->BitsPerPixel;
#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s() (%ux%u@%u)\n", __func__, sdl->width,
			sdl->height, depth);
#endif
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if(SDL_SetVideoMode(sdl->width, sdl->height, depth,
				SDL_OPENGL | SDL_FULLSCREEN) == 0)
	{
		object_delete(sdl);
		return 1;
	}
	glShadeModel(GL_SMOOTH);
#ifdef DEBUG
	fprintf(stderr, "DEBUG: glClearColor(%.1f, %.1f, %.1f, %.1f)\n", 0.0,
			0.0, 0.0, 0.0);
#endif
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glViewport(0, 0, sdl->width, sdl->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(sdl->height == 0)
		sdl->height = 1;
	gluPerspective(45.0, (GLfloat)sdl->width / (GLfloat)sdl->height, 0.1,
			100.0);
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SDL_GL_SwapBuffers();
	return 0;
}


/* sdl_destroy */
static void _sdl_destroy(GServerVideoPlugin * plugin)
{
	SDLVideo * sdl = plugin->priv;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	SDL_Quit();
	object_delete(sdl);
}


/* functions */
/* sdl_proto0 */
static void _sdl_proto0(GServerVideoPlugin * plugin, GServerVideoProto0 func)
{
	if(func == GSERVER_VIDEO_PROTO0_SwapBuffers)
	{
		SDL_GL_SwapBuffers();
		return;
	}
	_sdl_func0[func]();
}


/* sdl_proto1d */
static void _sdl_proto1d(GServerVideoPlugin * plugin, GServerVideoProto1d func,
		double x)
{
	_sdl_func1d[func](x);
}


/* sdl_proto1i */
static void _sdl_proto1i(GServerVideoPlugin * plugin, GServerVideoProto1i func,
		int32_t x)
{
	_sdl_func1i[func](x);
}


/* sdl_proto3f */
static void _sdl_proto3f(GServerVideoPlugin * plugin, GServerVideoProto3f func,
		float x, float y, float z)
{
	_sdl_func3f[func](x, y, z);
}


/* sdl_proto3i */
static void _sdl_proto3i(GServerVideoPlugin * plugin, GServerVideoProto3i func,
		int32_t x, int32_t y, int32_t z)
{
	_sdl_func3i[func](x, y, z);
}


/* sdl_proto4f */
static void _sdl_proto4f(GServerVideoPlugin * plugin, GServerVideoProto4f func,
		float x, float y, float z, float t)
{
	_sdl_func4f[func](x, y, z, t);
}


/* sdl_swap_buffers */
static void _sdl_swap_buffers(void)
{
	/* no need to do anything */
}
