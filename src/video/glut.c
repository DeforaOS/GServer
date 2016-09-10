/* $Id$ */
/* Copyright (c) 2010-2015 Pierre Pronchery <khorben@defora.org> */
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

static void _glut_call0(GServerVideoPlugin * plugin, GServerVideoCall0 func);
static void _glut_call1d(GServerVideoPlugin * plugin, GServerVideoCall1d func,
		double x);
static void _glut_call1f(GServerVideoPlugin * plugin, GServerVideoCall1f func,
		float x);
static void _glut_call1i(GServerVideoPlugin * plugin, GServerVideoCall1i func,
		uint32_t x);
static void _glut_call2f(GServerVideoPlugin * plugin, GServerVideoCall2f func,
		float x, float y);
static void _glut_call2i(GServerVideoPlugin * plugin, GServerVideoCall2i func,
		uint32_t x, uint32_t y);
static void _glut_call3b(GServerVideoPlugin * plugin, GServerVideoCall3b func,
		uint8_t x, uint8_t y, uint8_t z);
static void _glut_call3d(GServerVideoPlugin * plugin, GServerVideoCall3d func,
		double x, double y, double z);
static void _glut_call3f(GServerVideoPlugin * plugin, GServerVideoCall3f func,
		float x, float y, float z);
static void _glut_call3i(GServerVideoPlugin * plugin, GServerVideoCall3i func,
		uint32_t x, uint32_t y, uint32_t z);
static void _glut_call3s(GServerVideoPlugin * plugin, GServerVideoCall3s func,
		uint16_t x, uint16_t y, uint16_t z);
static void _glut_call4b(GServerVideoPlugin * plugin, GServerVideoCall4b func,
		uint8_t x, uint8_t y, uint8_t z, uint8_t t);
static void _glut_call4d(GServerVideoPlugin * plugin, GServerVideoCall4d func,
		double x, double y, double z, double t);
static void _glut_call4f(GServerVideoPlugin * plugin, GServerVideoCall4f func,
		float x, float y, float z, float t);
static void _glut_call4i(GServerVideoPlugin * plugin, GServerVideoCall4i func,
		uint32_t x, uint32_t y, uint32_t z, uint32_t t);
static void _glut_call4s(GServerVideoPlugin * plugin, GServerVideoCall4s func,
		uint16_t x, uint16_t y, uint16_t z, uint16_t t);

static void _glut_swap_buffers(void);


/* variables */
static void (*_glut_func0[GSERVER_VIDEO_CALL0_COUNT])(void) =
{
	glEnd,
	glEndList,
	glFlush,
	glLoadIdentity,
	_glut_swap_buffers
};

static void (*_glut_func1d[GSERVER_VIDEO_CALL1d_COUNT])(double) =
{
	glClearDepth
};

static void (*_glut_func1f[GSERVER_VIDEO_CALL1f_COUNT])(float) =
{
	glPointSize
};

static void (*_glut_func1i[GSERVER_VIDEO_CALL1i_COUNT])(uint32_t) =
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

static void (*_glut_func2f[GSERVER_VIDEO_CALL2f_COUNT])(float, float) =
{
	glPolygonOffset,
	glTexCoord2f
};

static void (*_glut_func2i[GSERVER_VIDEO_CALL2i_COUNT])(uint32_t, uint32_t) =
{
	glBindTexture,
	glColorMaterial,
	glFogi,
	glHint,
	glLightModeli,
	glVertex2i
};

static void (*_glut_func3b[GSERVER_VIDEO_CALL3b_COUNT])(uint8_t, uint8_t,
		uint8_t) =
{
	glColor3b,
	glColor3ub
};

static void (*_glut_func3d[GSERVER_VIDEO_CALL3d_COUNT])(double, double,
		double) =
{
	glColor3d,
	glTranslated
};

static void (*_glut_func3f[GSERVER_VIDEO_CALL3f_COUNT])(float, float, float) =
{
	glColor3f,
	glNormal3f,
	glTranslatef,
	glVertex3f
};

static void (*_glut_func3i[GSERVER_VIDEO_CALL3i_COUNT])(uint32_t, uint32_t,
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

static void (*_glut_func3s[GSERVER_VIDEO_CALL3s_COUNT])(uint16_t, uint16_t,
		uint16_t) =
{
	glColor3s,
	glColor3us
};

static void (*_glut_func4b[GSERVER_VIDEO_CALL4b_COUNT])(uint8_t, uint8_t,
		uint8_t, uint8_t) =
{
	glColor4b,
	glColor4ub
};

static void (*_glut_func4d[GSERVER_VIDEO_CALL4d_COUNT])(double, double, double,
		double) =
{
	glColor4d
};

static void (*_glut_func4f[GSERVER_VIDEO_CALL4f_COUNT])(float, float, float,
		float) =
{
	glClearColor,
	glRotatef
};

static void (*_glut_func4i[GSERVER_VIDEO_CALL4i_COUNT])(uint32_t, uint32_t,
		uint32_t, uint32_t) =
{
	glColor4i,
	glColor4ui,
	glScissor,
	glViewport
};

static void (*_glut_func4s[GSERVER_VIDEO_CALL4s_COUNT])(uint16_t, uint16_t,
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
	"GLUT",
	_glut_init,
	_glut_destroy,
	_glut_call0,
	_glut_call1d,
	_glut_call1f,
	_glut_call1i,
	_glut_call2f,
	_glut_call2i,
	_glut_call3b,
	_glut_call3d,
	_glut_call3f,
	_glut_call3i,
	_glut_call3s,
	_glut_call4b,
	_glut_call4d,
	_glut_call4f,
	_glut_call4i,
	_glut_call4s,
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


/* glut_call0 */
static void _glut_call0(GServerVideoPlugin * plugin, GServerVideoCall0 func)
{
	(void) plugin;

	if(func == GSERVER_VIDEO_CALL0_SwapBuffers)
	{
		glutSwapBuffers();
		return;
	}
	_glut_func0[func]();
}


/* glut_call1d */
static void _glut_call1d(GServerVideoPlugin * plugin, GServerVideoCall1d func,
		double x)
{
	(void) plugin;

	_glut_func1d[func](x);
}


/* glut_call1f */
static void _glut_call1f(GServerVideoPlugin * plugin, GServerVideoCall1f func,
		float x)
{
	(void) plugin;

	_glut_func1f[func](x);
}


/* glut_call1i */
static void _glut_call1i(GServerVideoPlugin * plugin, GServerVideoCall1i func,
		uint32_t x)
{
	(void) plugin;

	_glut_func1i[func](x);
}


/* glut_call2f */
static void _glut_call2f(GServerVideoPlugin * plugin, GServerVideoCall2f func,
		float x, float y)
{
	(void) plugin;

	_glut_func2f[func](x, y);
}


/* glut_call2i */
static void _glut_call2i(GServerVideoPlugin * plugin, GServerVideoCall2i func,
		uint32_t x, uint32_t y)
{
	(void) plugin;

	_glut_func2i[func](x, y);
}


/* glut_call3b */
static void _glut_call3b(GServerVideoPlugin * plugin, GServerVideoCall3b func,
		uint8_t x, uint8_t y, uint8_t z)
{
	(void) plugin;

	_glut_func3b[func](x, y, z);
}


/* glut_call3d */
static void _glut_call3d(GServerVideoPlugin * plugin, GServerVideoCall3d func,
		double x, double y, double z)
{
	(void) plugin;

	_glut_func3d[func](x, y, z);
}


/* glut_call3f */
static void _glut_call3f(GServerVideoPlugin * plugin, GServerVideoCall3f func,
		float x, float y, float z)
{
	(void) plugin;

	_glut_func3f[func](x, y, z);
}


/* glut_call3i */
static void _glut_call3i(GServerVideoPlugin * plugin, GServerVideoCall3i func,
		uint32_t x, uint32_t y, uint32_t z)
{
	(void) plugin;

	_glut_func3i[func](x, y, z);
}


/* glut_call3s */
static void _glut_call3s(GServerVideoPlugin * plugin, GServerVideoCall3s func,
		uint16_t x, uint16_t y, uint16_t z)
{
	(void) plugin;

	_glut_func3s[func](x, y, z);
}


/* glut_call4b */
static void _glut_call4b(GServerVideoPlugin * plugin, GServerVideoCall4b func,
		uint8_t x, uint8_t y, uint8_t z, uint8_t t)
{
	(void) plugin;

	_glut_func4b[func](x, y, z, t);
}


/* glut_call4d */
static void _glut_call4d(GServerVideoPlugin * plugin, GServerVideoCall4d func,
		double x, double y, double z, double t)
{
	(void) plugin;

	_glut_func4d[func](x, y, z, t);
}


/* glut_call4f */
static void _glut_call4f(GServerVideoPlugin * plugin, GServerVideoCall4f func,
		float x, float y, float z, float t)
{
	(void) plugin;

	_glut_func4f[func](x, y, z, t);
}


/* glut_call4i */
static void _glut_call4i(GServerVideoPlugin * plugin, GServerVideoCall4i func,
		uint32_t x, uint32_t y, uint32_t z, uint32_t t)
{
	(void) plugin;

	_glut_func4i[func](x, y, z, t);
}


/* glut_call4s */
static void _glut_call4s(GServerVideoPlugin * plugin, GServerVideoCall4s func,
		uint16_t x, uint16_t y, uint16_t z, uint16_t t)
{
	(void) plugin;

	_glut_func4s[func](x, y, z, t);
}


/* glut_swap_buffers */
static void _glut_swap_buffers(void)
{
	/* no need to do anything */
}
