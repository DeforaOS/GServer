/* $Id$ */
/* Copyright (c) 2012-2016 Pierre Pronchery <khorben@defora.org> */
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
typedef enum _GServerVideoProto
{
	GSERVER_VIDEO_PROTO_0 = 0,
	GSERVER_VIDEO_PROTO_1d,
	GSERVER_VIDEO_PROTO_1i,
	GSERVER_VIDEO_PROTO_1u,
	GSERVER_VIDEO_PROTO_2u,
	GSERVER_VIDEO_PROTO_3f,
	GSERVER_VIDEO_PROTO_3i,
	GSERVER_VIDEO_PROTO_3u,
	GSERVER_VIDEO_PROTO_4d,
	GSERVER_VIDEO_PROTO_4f,
	GSERVER_VIDEO_PROTO_4i,
	GSERVER_VIDEO_PROTO_4u,
	GSERVER_VIDEO_PROTO_4ub,
	GSERVER_VIDEO_PROTO_4us
} GServerVideoProto;

typedef enum _GServerVideoProto0
{
	GSERVER_VIDEO_PROTO0_glEnd = 0,
	GSERVER_VIDEO_PROTO0_glFlush,
	GSERVER_VIDEO_PROTO0_glLoadIdentity,
	GSERVER_VIDEO_PROTO0_SwapBuffers
} GServerVideoProto0;
# define GSERVER_VIDEO_PROTO0_LAST GSERVER_VIDEO_PROTO0_SwapBuffers
# define GSERVER_VIDEO_PROTO0_COUNT (GSERVER_VIDEO_PROTO0_LAST + 1)

typedef enum _GServerVideoProto1d
{
	GSERVER_VIDEO_PROTO1d_glClearDepth = 0
} GServerVideoProto1d;
# define GSERVER_VIDEO_PROTO1d_LAST GSERVER_VIDEO_PROTO1d_glClearDepth
# define GSERVER_VIDEO_PROTO1d_COUNT (GSERVER_VIDEO_PROTO1d_LAST + 1)

typedef enum _GServerVideoProto1i
{
	GSERVER_VIDEO_PROTO1i_glBegin = 0,
	GSERVER_VIDEO_PROTO1i_glClear
} GServerVideoProto1i;
# define GSERVER_VIDEO_PROTO1i_LAST GSERVER_VIDEO_PROTO1i_glClear
# define GSERVER_VIDEO_PROTO1i_COUNT (GSERVER_VIDEO_PROTO1i_LAST + 1)

typedef enum _GServerVideoProto1u
{
	GSERVER_VIDEO_PROTO1u_glDepthFunc = 0,
	GSERVER_VIDEO_PROTO1u_glDisable,
	GSERVER_VIDEO_PROTO1u_glDisableClientState,
	GSERVER_VIDEO_PROTO1u_glEnable,
	GSERVER_VIDEO_PROTO1u_glEnableClientState,
	GSERVER_VIDEO_PROTO1u_glIsEnabled
} GServerVideoProto1u;
# define GSERVER_VIDEO_PROTO1u_LAST GSERVER_VIDEO_PROTO1u_glIsEnabled
# define GSERVER_VIDEO_PROTO1u_COUNT (GSERVER_VIDEO_PROTO1u_LAST + 1)

typedef enum _GServerVideoProto2u
{
	GSERVER_VIDEO_PROTO2u_glColorMaterial = 0,
	GSERVER_VIDEO_PROTO2u_glHint
} GServerVideoProto2u;
# define GSERVER_VIDEO_PROTO2u_LAST GSERVER_VIDEO_PROTO2u_glHint
# define GSERVER_VIDEO_PROTO2u_COUNT (GSERVER_VIDEO_PROTO1u_LAST + 1)

typedef enum _GServerVideoProto3f
{
	GSERVER_VIDEO_PROTO3f_glColor3f = 0,
	GSERVER_VIDEO_PROTO3f_glTranslatef,
	GSERVER_VIDEO_PROTO3f_glVertex3f
} GServerVideoProto3f;
# define GSERVER_VIDEO_PROTO3f_LAST GSERVER_VIDEO_PROTO3f_glVertex3f
# define GSERVER_VIDEO_PROTO3f_COUNT (GSERVER_VIDEO_PROTO3f_LAST + 1)

typedef enum _GServerVideoProto3i
{
	GSERVER_VIDEO_PROTO3i_glColor3i = 0,
	GSERVER_VIDEO_PROTO3i_glVertex3i
} GServerVideoProto3i;
# define GSERVER_VIDEO_PROTO3i_LAST GSERVER_VIDEO_PROTO3i_glVertex3i
# define GSERVER_VIDEO_PROTO3i_COUNT (GSERVER_VIDEO_PROTO3i_LAST + 1)

typedef enum _GServerVideoProto3u
{
	GSERVER_VIDEO_PROTO3i_glColor3ui = 0
} GServerVideoProto3u;
# define GSERVER_VIDEO_PROTO3u_LAST GSERVER_VIDEO_PROTO3ui_glColor3ui
# define GSERVER_VIDEO_PROTO3u_COUNT (GSERVER_VIDEO_PROTO3ui_LAST + 1)

typedef enum _GServerVideoProto4d
{
	GSERVER_VIDEO_PROTO4d_glColor4d = 0
} GServerVideoProto4d;
# define GSERVER_VIDEO_PROTO4d_LAST GSERVER_VIDEO_PROTO4d_glColor4d
# define GSERVER_VIDEO_PROTO4d_COUNT (GSERVER_VIDEO_PROTO4d_LAST + 1)

typedef enum _GServerVideoProto4f
{
	GSERVER_VIDEO_PROTO4f_glClearColor = 0,
	GSERVER_VIDEO_PROTO4f_glColor4f,
	GSERVER_VIDEO_PROTO4f_glRotatef
} GServerVideoProto4f;
# define GSERVER_VIDEO_PROTO4f_LAST GSERVER_VIDEO_PROTO4f_glRotatef
# define GSERVER_VIDEO_PROTO4f_COUNT (GSERVER_VIDEO_PROTO4f_LAST + 1)

typedef enum _GServerVideoProto4i
{
	GSERVER_VIDEO_PROTO4i_glColor4i = 0
} GServerVideoProto4i;
# define GSERVER_VIDEO_PROTO4i_LAST GSERVER_VIDEO_PROTO4i_glColor4i
# define GSERVER_VIDEO_PROTO4i_COUNT (GSERVER_VIDEO_PROTO4i_LAST + 1)

typedef enum _GServerVideoProto4u
{
	GSERVER_VIDEO_PROTO4u_glColor4ui = 0
} GServerVideoProto4u;
# define GSERVER_VIDEO_PROTO4u_LAST GSERVER_VIDEO_PROTO4ui_glColor4ui
# define GSERVER_VIDEO_PROTO4u_COUNT (GSERVER_VIDEO_PROTO4ui_LAST + 1)

typedef enum _GServerVideoProto4ub
{
	GSERVER_VIDEO_PROTO4ub_glColor4ub = 0
} GServerVideoProto4ub;
# define GSERVER_VIDEO_PROTO4ub_LAST GSERVER_VIDEO_PROTO4ub_glColor4ub
# define GSERVER_VIDEO_PROTO4ub_COUNT (GSERVER_VIDEO_PROTO4ub_LAST + 1)

typedef enum _GServerVideoProto4us
{
	GSERVER_VIDEO_PROTO4us_glColor4us = 0
} GServerVideoProto4us;
# define GSERVER_VIDEO_PROTO4us_LAST GSERVER_VIDEO_PROTO4us_glColor4us
# define GSERVER_VIDEO_PROTO4us_COUNT (GSERVER_VIDEO_PROTO4us_LAST + 1)

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
	void (*proto0)(GServerVideoPlugin * plugin, GServerVideoProto0 func);
	void (*proto1d)(GServerVideoPlugin * plugin, GServerVideoProto1d func,
			double x);
	void (*proto1i)(GServerVideoPlugin * plugin, GServerVideoProto1i func,
			int32_t x);
	void (*proto1u)(GServerVideoPlugin * plugin, GServerVideoProto1u func,
			uint32_t x);
	void (*proto2u)(GServerVideoPlugin * plugin, GServerVideoProto2u func,
			uint32_t x, uint32_t y);
	void (*proto3f)(GServerVideoPlugin * plugin, GServerVideoProto3f func,
			float x, float y, float z);
	void (*proto3i)(GServerVideoPlugin * plugin, GServerVideoProto3i func,
			int32_t x, int32_t y, int32_t z);
	void (*proto3u)(GServerVideoPlugin * plugin, GServerVideoProto3u func,
			uint32_t x, uint32_t y, uint32_t z);
	void (*proto4d)(GServerVideoPlugin * plugin, GServerVideoProto4d func,
			double x, double y, double z, double t);
	void (*proto4f)(GServerVideoPlugin * plugin, GServerVideoProto4f func,
			float x, float y, float z, float t);
	void (*proto4i)(GServerVideoPlugin * plugin, GServerVideoProto4i func,
			int32_t x, int32_t y, int32_t z, int32_t t);
	void (*proto4u)(GServerVideoPlugin * plugin, GServerVideoProto4u func,
			uint32_t x, uint32_t y, uint32_t z, uint32_t t);
	void (*proto4ub)(GServerVideoPlugin * plugin, GServerVideoProto4ub func,
			uint8_t x, uint8_t y, uint8_t z, uint8_t t);
	void (*proto4us)(GServerVideoPlugin * plugin, GServerVideoProto4us func,
			uint16_t x, uint16_t y, uint16_t z, uint16_t t);
	void * priv;
};

#endif /* !GRAPHICS_GSERVER_VIDEO_H */
