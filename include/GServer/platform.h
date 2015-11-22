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



#ifndef GRAPHICS_GSERVER_PLATFORM_H
# define GRAPHICS_GSERVER_PLATFORM_H


/* GServerPlatform */
/* protected */
/* types */
typedef struct _GServerPlatform GServerPlatform;


/* public */
/* accessors */
int gserverplatform_get_video_refresh_rate(GServerPlatform * platform);

int gserverplatform_get_video_depth(GServerPlatform * platform);
int gserverplatform_get_video_height(GServerPlatform * platform);
int gserverplatform_get_video_width(GServerPlatform * platform);

#endif /* !GRAPHICS_GSERVER_PLATFORM_H */
