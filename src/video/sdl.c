/* $Id$ */
/* Copyright (c) 2016-2017 Pierre Pronchery <khorben@defora.org> */
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


/* public */
/* variables */
GServerVideoPlugin video_plugin =
{
	NULL,
	"SDL",
	_sdl_init,
	_sdl_destroy,
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
