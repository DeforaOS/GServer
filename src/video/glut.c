/* $Id$ */
/* Copyright (c) 2010-2017 Pierre Pronchery <khorben@defora.org> */
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
#include <GL/glut.h>
#include "GServer/video.h"
#include "../../config.h"


/* GLUT */
/* private */
/* types */
typedef struct _GLUTVideo
{
	unsigned int width;
	unsigned int height;
} GLUTVideo;


/* prototypes */
static int _glut_init(GServerVideoPlugin * plugin);
static void _glut_destroy(GServerVideoPlugin * plugin);

static void _glut_display(void);
static void _glut_idle(void);


/* public */
/* variables */
GServerVideoPlugin video_plugin =
{
	NULL,
	"GLUT",
	_glut_init,
	_glut_destroy,
	NULL
};


/* private */
/* functions */
/* glut_init */
static int _glut_init(GServerVideoPlugin * plugin)
{
	GLUTVideo * glut;
	GServerPlatform * platform;
	int argc = 1;
	char * argv[] = { "GServer", NULL };

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	if((glut = object_new(sizeof(*glut))) == NULL)
		return 1;
	plugin->priv = glut;
	platform = plugin->helper->get_platform(plugin->helper->gserver);
	glut->width = gserverplatform_get_video_width(platform);
	glut->height = gserverplatform_get_video_height(platform);
	glutInit(&argc, argv);
	glutInitWindowSize(glut->width, glut->height);
	glutCreateWindow(PACKAGE " GLUT");
	glutDisplayFunc(_glut_display);
	glutIdleFunc(_glut_idle);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glViewport(0, 0, glut->width, glut->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(glut->height == 0)
		glut->height = 1;
	gluPerspective(45.0, (GLfloat)glut->width / (GLfloat)glut->height, 0.1,
			100.0);
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glutMainLoop();
	return 0;
}


/* glut_destroy */
static void _glut_destroy(GServerVideoPlugin * plugin)
{
	GLUTVideo * glut = plugin->priv;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: %s()\n", __func__);
#endif
	/* FIXME take care of the rest */
	object_delete(glut);
}


/* glut_display */
static void _glut_display(void)
{
	video_plugin.helper->refresh(video_plugin.helper->gserver);
}


/* glut_idle */
static int _idle_timeout(void * data);

static void _glut_idle(void)
{
	struct timeval tv;
	GServerPlatform * platform;
	Event * event;

	platform = video_plugin.helper->get_platform(
			video_plugin.helper->gserver);
	event = video_plugin.helper->get_event(video_plugin.helper->gserver);
#ifdef DEBUG
	tv.tv_sec = 1;
	tv.tv_usec = 0;
#else
	tv.tv_sec = 0;
	tv.tv_usec = 1000000 / gserverplatform_get_video_refresh_rate(platform);
#endif
	event_register_timeout(event, &tv, _idle_timeout, event);
	event_loop(event);
}

static int _idle_timeout(void * data)
{
	Event * event = data;

	event_loop_quit(event);
	return 1;
}
