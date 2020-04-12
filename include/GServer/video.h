/* $Id$ */
/* Copyright (c) 2012-2020 Pierre Pronchery <khorben@defora.org> */
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
	void * priv;
};

#endif /* !GRAPHICS_GSERVER_VIDEO_H */
