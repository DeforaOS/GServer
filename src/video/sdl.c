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

static void _sdl_call0(GServerVideoPlugin * plugin, GServerVideoCall0 func);
static void _sdl_call1d(GServerVideoPlugin * plugin, GServerVideoCall1d func,
		double x);
static void _sdl_call1i(GServerVideoPlugin * plugin, GServerVideoCall1i func,
		uint32_t x);
static void _sdl_call2f(GServerVideoPlugin * plugin, GServerVideoCall2f func,
		float x, float y);
static void _sdl_call2i(GServerVideoPlugin * plugin, GServerVideoCall2i func,
		uint32_t x, uint32_t y);
static void _sdl_call3b(GServerVideoPlugin * plugin, GServerVideoCall3b func,
		uint8_t x, uint8_t y, uint8_t z);
static void _sdl_call3d(GServerVideoPlugin * plugin, GServerVideoCall3d func,
		double x, double y, double z);
static void _sdl_call3f(GServerVideoPlugin * plugin, GServerVideoCall3f func,
		float x, float y, float z);
static void _sdl_call3i(GServerVideoPlugin * plugin, GServerVideoCall3i func,
		uint32_t x, uint32_t y, uint32_t z);
static void _sdl_call3s(GServerVideoPlugin * plugin, GServerVideoCall3s func,
		uint16_t x, uint16_t y, uint16_t z);
static void _sdl_call4b(GServerVideoPlugin * plugin, GServerVideoCall4b func,
		uint8_t x, uint8_t y, uint8_t z, uint8_t t);
static void _sdl_call4d(GServerVideoPlugin * plugin, GServerVideoCall4d func,
		double x, double y, double z, double t);
static void _sdl_call4f(GServerVideoPlugin * plugin, GServerVideoCall4f func,
		float x, float y, float z, float t);
static void _sdl_call4i(GServerVideoPlugin * plugin, GServerVideoCall4i func,
		uint32_t x, uint32_t y, uint32_t z, uint32_t t);
static void _sdl_call4s(GServerVideoPlugin * plugin, GServerVideoCall4s func,
		uint16_t x, uint16_t y, uint16_t z, uint16_t t);

static void _sdl_swap_buffers(void);


/* variables */
static void (*_sdl_func0[GSERVER_VIDEO_CALL0_COUNT])(void) =
{
	glEnd,
	glFlush,
	glLoadIdentity,
	_sdl_swap_buffers
};

static void (*_sdl_func1d[GSERVER_VIDEO_CALL1d_COUNT])(double) =
{
	glClearDepth
};

static void (*_sdl_func1i[GSERVER_VIDEO_CALL1i_COUNT])(uint32_t) =
{
	glBegin,
	glClear,
	glDepthFunc,
	glDisable,
	glDisableClientState,
	glEnable,
	glEnableClientState,
	glIsEnabled,
	glShadeModel
};

static void (*_sdl_func2f[GSERVER_VIDEO_CALL2f_COUNT])(float, float) =
{
	glPolygonOffset,
	glTexCoord2f
};

static void (*_sdl_func2i[GSERVER_VIDEO_CALL2i_COUNT])(uint32_t, uint32_t) =
{
	glBindTexture,
	glColorMaterial,
	glFogi,
	glHint,
	glLightModeli,
	glVertex2i
};

static void (*_sdl_func3b[GSERVER_VIDEO_CALL3b_COUNT])(uint8_t, uint8_t,
		uint8_t) =
{
	glColor3b,
	glColor3ub
};

static void (*_sdl_func3d[GSERVER_VIDEO_CALL3d_COUNT])(double, double,
		double) =
{
	glColor3d,
	glTranslated
};

static void (*_sdl_func3f[GSERVER_VIDEO_CALL3f_COUNT])(float, float, float) =
{
	glColor3f,
	glNormal3f,
	glTranslatef,
	glVertex3f
};

static void (*_sdl_func3i[GSERVER_VIDEO_CALL3i_COUNT])(uint32_t, uint32_t,
		uint32_t) =
{
	glColor3i,
	glColor3ui,
	glDrawArrays,
	glLighti,
	glTexGeni,
	glTexParameteri,
	glVertex3i
};

static void (*_sdl_func3s[GSERVER_VIDEO_CALL3s_COUNT])(uint16_t, uint16_t,
		uint16_t) =
{
	glColor3s,
	glColor3us
};

static void (*_sdl_func4b[GSERVER_VIDEO_CALL4b_COUNT])(uint8_t, uint8_t,
		uint8_t, uint8_t) =
{
	glColor4b,
	glColor4ub
};

static void (*_sdl_func4d[GSERVER_VIDEO_CALL4d_COUNT])(double, double, double,
		double) =
{
	glColor4d
};

static void (*_sdl_func4f[GSERVER_VIDEO_CALL4f_COUNT])(float, float, float,
		float) =
{
	glClearColor,
	glRotatef
};

static void (*_sdl_func4i[GSERVER_VIDEO_CALL4i_COUNT])(uint32_t, uint32_t,
		uint32_t, uint32_t) =
{
	glColor4i,
	glColor4ui,
	glScissor,
	glViewport
};

static void (*_sdl_func4s[GSERVER_VIDEO_CALL4s_COUNT])(uint16_t, uint16_t,
		uint16_t, uint16_t) =
{
	glColor4s,
	glColor4us
};


/* public */
/* variables */
GServerVideoPlugin video_plugin =
{
	NULL,
	"SDL",
	_sdl_init,
	_sdl_destroy,
	_sdl_call0,
	_sdl_call1d,
	_sdl_call1i,
	_sdl_call2f,
	_sdl_call2i,
	_sdl_call3b,
	_sdl_call3d,
	_sdl_call3f,
	_sdl_call3i,
	_sdl_call3s,
	_sdl_call4b,
	_sdl_call4d,
	_sdl_call4f,
	_sdl_call4i,
	_sdl_call4s,
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
/* sdl_call0 */
static void _sdl_call0(GServerVideoPlugin * plugin, GServerVideoCall0 func)
{
	if(func == GSERVER_VIDEO_CALL0_SwapBuffers)
	{
		SDL_GL_SwapBuffers();
		return;
	}
	_sdl_func0[func]();
}


/* sdl_call1d */
static void _sdl_call1d(GServerVideoPlugin * plugin, GServerVideoCall1d func,
		double x)
{
	(void) plugin;

	_sdl_func1d[func](x);
}


/* sdl_call1i */
static void _sdl_call1i(GServerVideoPlugin * plugin, GServerVideoCall1i func,
		uint32_t x)
{
	(void) plugin;

	_sdl_func1i[func](x);
}


/* sdl_call2f */
static void _sdl_call2f(GServerVideoPlugin * plugin, GServerVideoCall2f func,
		float x, float y)
{
	(void) plugin;

	_sdl_func2f[func](x, y);
}


/* sdl_call2i */
static void _sdl_call2i(GServerVideoPlugin * plugin, GServerVideoCall2i func,
		uint32_t x, uint32_t y)
{
	(void) plugin;

	_sdl_func2i[func](x, y);
}


/* sdl_call3b */
static void _sdl_call3b(GServerVideoPlugin * plugin, GServerVideoCall3b func,
		uint8_t x, uint8_t y, uint8_t z)
{
	(void) plugin;

	_sdl_func3b[func](x, y, z);
}


/* sdl_call3d */
static void _sdl_call3d(GServerVideoPlugin * plugin, GServerVideoCall3d func,
		double x, double y, double z)
{
	(void) plugin;

	_sdl_func3d[func](x, y, z);
}


/* sdl_call3f */
static void _sdl_call3f(GServerVideoPlugin * plugin, GServerVideoCall3f func,
		float x, float y, float z)
{
	(void) plugin;

	_sdl_func3f[func](x, y, z);
}


/* sdl_call3i */
static void _sdl_call3i(GServerVideoPlugin * plugin, GServerVideoCall3i func,
		uint32_t x, uint32_t y, uint32_t z)
{
	(void) plugin;

	_sdl_func3i[func](x, y, z);
}


/* sdl_call3s */
static void _sdl_call3s(GServerVideoPlugin * plugin, GServerVideoCall3s func,
		uint16_t x, uint16_t y, uint16_t z)
{
	(void) plugin;

	_sdl_func3s[func](x, y, z);
}


/* sdl_call4b */
static void _sdl_call4b(GServerVideoPlugin * plugin, GServerVideoCall4b func,
		uint8_t x, uint8_t y, uint8_t z, uint8_t t)
{
	(void) plugin;

	_sdl_func4b[func](x, y, z, t);
}


/* sdl_call4d */
static void _sdl_call4d(GServerVideoPlugin * plugin, GServerVideoCall4d func,
		double x, double y, double z, double t)
{
	(void) plugin;

	_sdl_func4d[func](x, y, z, t);
}


/* sdl_call4f */
static void _sdl_call4f(GServerVideoPlugin * plugin, GServerVideoCall4f func,
		float x, float y, float z, float t)
{
	(void) plugin;

	_sdl_func4f[func](x, y, z, t);
}


/* sdl_call4i */
static void _sdl_call4i(GServerVideoPlugin * plugin, GServerVideoCall4i func,
		uint32_t x, uint32_t y, uint32_t z, uint32_t t)
{
	(void) plugin;

	_sdl_func4i[func](x, y, z, t);
}


/* sdl_call4s */
static void _sdl_call4s(GServerVideoPlugin * plugin, GServerVideoCall4s func,
		uint16_t x, uint16_t y, uint16_t z, uint16_t t)
{
	(void) plugin;

	_sdl_func4s[func](x, y, z, t);
}


/* sdl_swap_buffers */
static void _sdl_swap_buffers(void)
{
	/* no need to do anything */
}
