/* $Id$ */
/* Copyright (c) 2015 Pierre Pronchery <khorben@defora.org> */
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
#include <limits.h>
#include <System.h>
#include "GServer/platform.h"
#include "../config.h"

#ifndef VENDOR
# define VENDOR		"DeforaOS"
#endif


/* GServerPlatform */
/* private */
/* types */
struct _GServerPlatform
{
	Config * config;
};


/* prototypes */
/* accessors */
static int _gserverplatform_get_integer(GServerPlatform * platform,
		String const * section, String const * name, int fallback);
static String const * _gserverplatform_get_string(GServerPlatform * platform,
		String const * section, String const * name,
		String const * fallback);


/* public */
/* functions */
/* gserverplatform_new */
GServerPlatform * gserverplatform_new(void)
{
	GServerPlatform * platform;

	if((platform = object_new(sizeof(*platform))) == NULL)
		return NULL;
	if((platform->config = config_new()) == NULL)
	{
		object_delete(platform->config);
		return NULL;
	}
	/* XXX report errors */
	config_load_preferences_system(platform->config, VENDOR, PACKAGE,
			"platform.conf");
	return platform;
}


/* gserverplatform_delete */
void gserverplatform_delete(GServerPlatform * platform)
{
	config_delete(platform->config);
	object_delete(platform);
}


/* accessors */
/* gserverplatform_get_driver */
String const * gserverplatform_get_driver(GServerPlatform * platform,
		String const * subsystem)
{
	struct
	{
		String const * subsystem;
		String const * fallback;
	} fallback[] =
	{
		{ "video",	"glx"	}
	};
	size_t i;

	for(i = 0; i < sizeof(fallback) / sizeof(*fallback); i++)
		if(string_compare(fallback[i].subsystem, subsystem) == 0)
			return _gserverplatform_get_string(platform, "drivers",
					subsystem, fallback[i].fallback);
	return _gserverplatform_get_string(platform, "drivers", subsystem,
			NULL);
}


/* gserverplatform_get_video_depth */
int gserverplatform_get_video_depth(GServerPlatform * platform)
{
	return _gserverplatform_get_integer(platform, "video", "depth", 24);
}


/* gserverplatform_get_video_height */
int gserverplatform_get_video_height(GServerPlatform * platform)
{
	return _gserverplatform_get_integer(platform, "video", "height", 480);
}


/* gserverplatform_get_video_refresh_rate */
int gserverplatform_get_video_refresh_rate(GServerPlatform * platform)
{
	return _gserverplatform_get_integer(platform, "video", "refresh_rate",
			60);
}


/* gserverplatform_get_video_width */
int gserverplatform_get_video_width(GServerPlatform * platform)
{
	return _gserverplatform_get_integer(platform, "video", "width", 640);
}


/* private */
/* functions */
/* gserverplatform_get_integer */
static int _gserverplatform_get_integer(GServerPlatform * platform,
		String const * section, String const * name, int fallback)
{
	String const * p;
	int d;

	if((p = config_get(platform->config, section, name)) == NULL
			|| (d = strtol(p, NULL, 0)) == 0)
		return fallback;
	return d;
}


/* gserverplatform_get_string */
static String const * _gserverplatform_get_string(GServerPlatform * platform,
		String const * section, String const * name,
		String const * fallback)
{
	String const * s;

	if((s = config_get(platform->config, section, name)) == NULL)
		return fallback;
	return s;
}
