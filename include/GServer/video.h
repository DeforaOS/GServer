/* $Id$ */
/* Copyright (c) 2012-2017 Pierre Pronchery <khorben@defora.org> */
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



#ifndef GRAPHICS_GSERVER_VIDEO_H
# define GRAPHICS_GSERVER_VIDEO_H

# include <stdint.h>
# include <System.h>
# include "gserver.h"
# include "platform.h"


/* Video */
/* types */
typedef enum _GServerVideoCall
{
	GSERVER_VIDEO_CALL_0 = 0,
	GSERVER_VIDEO_CALL_1d,
	GSERVER_VIDEO_CALL_1f,
	GSERVER_VIDEO_CALL_1i,
	GSERVER_VIDEO_CALL_2f,
	GSERVER_VIDEO_CALL_2i,
	GSERVER_VIDEO_CALL_3b,
	GSERVER_VIDEO_CALL_3d,
	GSERVER_VIDEO_CALL_3f,
	GSERVER_VIDEO_CALL_3i,
	GSERVER_VIDEO_CALL_3s,
	GSERVER_VIDEO_CALL_4b,
	GSERVER_VIDEO_CALL_4d,
	GSERVER_VIDEO_CALL_4f,
	GSERVER_VIDEO_CALL_4i,
	GSERVER_VIDEO_CALL_4s
} GServerVideoCall;

typedef enum _GServerVideoCall0
{
	GSERVER_VIDEO_CALL0_glEnd = 0,
	GSERVER_VIDEO_CALL0_glEndList,
	GSERVER_VIDEO_CALL0_glFlush,
	GSERVER_VIDEO_CALL0_glLoadIdentity,
	GSERVER_VIDEO_CALL0_SwapBuffers
} GServerVideoCall0;
# define GSERVER_VIDEO_CALL0_LAST GSERVER_VIDEO_CALL0_SwapBuffers
# define GSERVER_VIDEO_CALL0_COUNT (GSERVER_VIDEO_CALL0_LAST + 1)

typedef enum _GServerVideoCall1d
{
	GSERVER_VIDEO_CALL1d_glClearDepth = 0
} GServerVideoCall1d;
# define GSERVER_VIDEO_CALL1d_LAST GSERVER_VIDEO_CALL1d_glClearDepth
# define GSERVER_VIDEO_CALL1d_COUNT (GSERVER_VIDEO_CALL1d_LAST + 1)

typedef enum _GServerVideoCall1f
{
	GSERVER_VIDEO_CALL1f_glClearIndex = 0,
	GSERVER_VIDEO_CALL1f_glPointSize
} GServerVideoCall1f;
# define GSERVER_VIDEO_CALL1f_LAST GSERVER_VIDEO_CALL1f_glPointSize
# define GSERVER_VIDEO_CALL1f_COUNT (GSERVER_VIDEO_CALL1f_LAST + 1)

typedef enum _GServerVideoCall1i
{
	GSERVER_VIDEO_CALL1i_glActiveTexture = 0,
	GSERVER_VIDEO_CALL1i_glArrayElement,
	GSERVER_VIDEO_CALL1i_glBegin,
	GSERVER_VIDEO_CALL1i_glBlendEquation,
	GSERVER_VIDEO_CALL1i_glCallList,
	GSERVER_VIDEO_CALL1i_glClear,
	GSERVER_VIDEO_CALL1i_glClearStencil,
	GSERVER_VIDEO_CALL1i_glClientActiveTexture,
	GSERVER_VIDEO_CALL1i_glCompileShader,
	GSERVER_VIDEO_CALL1i_glDepthFunc,
	GSERVER_VIDEO_CALL1i_glDisable,
	GSERVER_VIDEO_CALL1i_glDisableClientState,
	GSERVER_VIDEO_CALL1i_glEnable,
	GSERVER_VIDEO_CALL1i_glEnableClientState,
	GSERVER_VIDEO_CALL1i_glIsEnabled,
	GSERVER_VIDEO_CALL1i_glShadeModel
} GServerVideoCall1i;
# define GSERVER_VIDEO_CALL1i_LAST GSERVER_VIDEO_CALL1i_glShadeModel
# define GSERVER_VIDEO_CALL1i_COUNT (GSERVER_VIDEO_CALL1i_LAST + 1)

typedef enum _GServerVideoCall2f
{
	GSERVER_VIDEO_CALL2f_glPolygonOffset = 0,
	GSERVER_VIDEO_CALL2f_glTexCoord2f
} GServerVideoCall2f;
# define GSERVER_VIDEO_CALL2f_LAST GSERVER_VIDEO_CALL2f_glTexCoord2f
# define GSERVER_VIDEO_CALL2f_COUNT (GSERVER_VIDEO_CALL2f_LAST + 1)

typedef enum _GServerVideoCall2i
{
	GSERVER_VIDEO_CALL2i_glAttachShader = 0,
	GSERVER_VIDEO_CALL2i_glBeginQuery,
	GSERVER_VIDEO_CALL2i_glBindBuffer,
	GSERVER_VIDEO_CALL2i_glBindTexture,
	GSERVER_VIDEO_CALL2i_glBlendEquationSeparate,
	GSERVER_VIDEO_CALL2i_glBlendFunc,
	GSERVER_VIDEO_CALL2i_glColorMaterial,
	GSERVER_VIDEO_CALL2i_glFogi,
	GSERVER_VIDEO_CALL2i_glHint,
	GSERVER_VIDEO_CALL2i_glLightModeli,
	GSERVER_VIDEO_CALL2i_glVertex2i
} GServerVideoCall2i;
# define GSERVER_VIDEO_CALL2i_LAST GSERVER_VIDEO_CALL2i_glVertex2i
# define GSERVER_VIDEO_CALL2i_COUNT (GSERVER_VIDEO_CALL1i_LAST + 1)

typedef enum _GServerVideoCall3b
{
	GSERVER_VIDEO_CALL3b_glColor3b = 0,
	GSERVER_VIDEO_CALL3b_glColor3ub
} GServerVideoCall3b;
# define GSERVER_VIDEO_CALL3b_LAST GSERVER_VIDEO_CALL3b_glColor3ub
# define GSERVER_VIDEO_CALL3b_COUNT (GSERVER_VIDEO_CALL3b_LAST + 1)

typedef enum _GServerVideoCall3d
{
	GSERVER_VIDEO_CALL3d_glColor3d = 0,
	GSERVER_VIDEO_CALL3d_glTranslated
} GServerVideoCall3d;
# define GSERVER_VIDEO_CALL3d_LAST GSERVER_VIDEO_CALL3d_glTranslated
# define GSERVER_VIDEO_CALL3d_COUNT (GSERVER_VIDEO_CALL3d_LAST + 1)

typedef enum _GServerVideoCall3f
{
	GSERVER_VIDEO_CALL3f_glColor3f = 0,
	GSERVER_VIDEO_CALL3f_glNormal3f,
	GSERVER_VIDEO_CALL3f_glTranslatef,
	GSERVER_VIDEO_CALL3f_glVertex3f
} GServerVideoCall3f;
# define GSERVER_VIDEO_CALL3f_LAST GSERVER_VIDEO_CALL3f_glVertex3f
# define GSERVER_VIDEO_CALL3f_COUNT (GSERVER_VIDEO_CALL3f_LAST + 1)

typedef enum _GServerVideoCall3i
{
	GSERVER_VIDEO_CALL3i_glColor3i = 0,
	GSERVER_VIDEO_CALL3i_glColor3ui,
	GSERVER_VIDEO_CALL3i_glDrawArrays,
	GSERVER_VIDEO_CALL3i_glLighti,
	GSERVER_VIDEO_CALL3i_glTexGeni,
	GSERVER_VIDEO_CALL3i_glTexParameteri,
	GSERVER_VIDEO_CALL3i_glVertex3i
} GServerVideoCall3i;
# define GSERVER_VIDEO_CALL3i_LAST GSERVER_VIDEO_CALL3i_glVertex3i
# define GSERVER_VIDEO_CALL3i_COUNT (GSERVER_VIDEO_CALL3i_LAST + 1)

typedef enum _GServerVideoCall3s
{
	GSERVER_VIDEO_CALL3s_glColor3s = 0,
	GSERVER_VIDEO_CALL3s_glColor3us
} GServerVideoCall3s;
# define GSERVER_VIDEO_CALL3s_LAST GSERVER_VIDEO_CALL3s_glColor3us
# define GSERVER_VIDEO_CALL3s_COUNT (GSERVER_VIDEO_CALL3s_LAST + 1)

typedef enum _GServerVideoCall4b
{
	GSERVER_VIDEO_CALL4b_glColor4b = 0,
	GSERVER_VIDEO_CALL4b_glColor4ub
} GServerVideoCall4b;
# define GSERVER_VIDEO_CALL4b_LAST GSERVER_VIDEO_CALL4b_glColor4ub
# define GSERVER_VIDEO_CALL4b_COUNT (GSERVER_VIDEO_CALL4b_LAST + 1)

typedef enum _GServerVideoCall4d
{
	GSERVER_VIDEO_CALL4d_glColor4d = 0
} GServerVideoCall4d;
# define GSERVER_VIDEO_CALL4d_LAST GSERVER_VIDEO_CALL4d_glColor4d
# define GSERVER_VIDEO_CALL4d_COUNT (GSERVER_VIDEO_CALL4d_LAST + 1)

typedef enum _GServerVideoCall4f
{
	GSERVER_VIDEO_CALL4f_glClearAccum = 0,
	GSERVER_VIDEO_CALL4f_glClearColor,
	GSERVER_VIDEO_CALL4f_glColor4f,
	GSERVER_VIDEO_CALL4f_glRotatef
} GServerVideoCall4f;
# define GSERVER_VIDEO_CALL4f_LAST GSERVER_VIDEO_CALL4f_glRotatef
# define GSERVER_VIDEO_CALL4f_COUNT (GSERVER_VIDEO_CALL4f_LAST + 1)

typedef enum _GServerVideoCall4i
{
	GSERVER_VIDEO_CALL4i_glBlendFuncSeparate = 0,
	GSERVER_VIDEO_CALL4i_glColor4i,
	GSERVER_VIDEO_CALL4i_glColor4ui,
	GSERVER_VIDEO_CALL4i_glColorMask,
	GSERVER_VIDEO_CALL4i_glScissor,
	GSERVER_VIDEO_CALL4i_glViewport
} GServerVideoCall4i;
# define GSERVER_VIDEO_CALL4i_LAST GSERVER_VIDEO_CALL4i_glViewport
# define GSERVER_VIDEO_CALL4i_COUNT (GSERVER_VIDEO_CALL4i_LAST + 1)

typedef enum _GServerVideoCall4s
{
	GSERVER_VIDEO_CALL4s_glColor4s = 0,
	GSERVER_VIDEO_CALL4s_glColor4us
} GServerVideoCall4s;
# define GSERVER_VIDEO_CALL4s_LAST GSERVER_VIDEO_CALL4s_glColor4us
# define GSERVER_VIDEO_CALL4s_COUNT (GSERVER_VIDEO_CALL4s_LAST + 1)

/* GServerVideoPlugin */
typedef struct _GServerVideoPlugin GServerVideoPlugin;

typedef struct _GServerVideoPluginHelper
{
	GServer * gserver;
	char const * (*config_get)(GServer * gserver, char const * section,
			char const * variable);
	Event * (*get_event)(GServer * gserver);
	GServerPlatform * (*get_platform)(GServer * gserver);
	void (*refresh)(GServer * gserver);
} GServerVideoPluginHelper;

struct _GServerVideoPlugin
{
	GServerVideoPluginHelper * helper;
	char const * name;
	int (*init)(GServerVideoPlugin * plugin);
	void (*destroy)(GServerVideoPlugin * plugin);
	void (*call0)(GServerVideoPlugin * plugin, GServerVideoCall0 func);
	void (*call1d)(GServerVideoPlugin * plugin, GServerVideoCall1d func,
			double x);
	void (*call1f)(GServerVideoPlugin * plugin, GServerVideoCall1f func,
			float x);
	void (*call1i)(GServerVideoPlugin * plugin, GServerVideoCall1i func,
			uint32_t x);
	void (*call2f)(GServerVideoPlugin * plugin, GServerVideoCall2f func,
			float x, float y);
	void (*call2i)(GServerVideoPlugin * plugin, GServerVideoCall2i func,
			uint32_t x, uint32_t y);
	void (*call3b)(GServerVideoPlugin * plugin, GServerVideoCall3b func,
			uint8_t x, uint8_t y, uint8_t z);
	void (*call3d)(GServerVideoPlugin * plugin, GServerVideoCall3d func,
			double x, double y, double z);
	void (*call3f)(GServerVideoPlugin * plugin, GServerVideoCall3f func,
			float x, float y, float z);
	void (*call3i)(GServerVideoPlugin * plugin, GServerVideoCall3i func,
			uint32_t x, uint32_t y, uint32_t z);
	void (*call3s)(GServerVideoPlugin * plugin, GServerVideoCall3s func,
			uint16_t x, uint16_t y, uint16_t z);
	void (*call4b)(GServerVideoPlugin * plugin, GServerVideoCall4b func,
			uint8_t x, uint8_t y, uint8_t z, uint8_t t);
	void (*call4d)(GServerVideoPlugin * plugin, GServerVideoCall4d func,
			double x, double y, double z, double t);
	void (*call4f)(GServerVideoPlugin * plugin, GServerVideoCall4f func,
			float x, float y, float z, float t);
	void (*call4i)(GServerVideoPlugin * plugin, GServerVideoCall4i func,
			uint32_t x, uint32_t y, uint32_t z, uint32_t t);
	void (*call4s)(GServerVideoPlugin * plugin, GServerVideoCall4s func,
			uint16_t x, uint16_t y, uint16_t z, uint16_t t);
	void * priv;
};

#endif /* !GRAPHICS_GSERVER_VIDEO_H */
