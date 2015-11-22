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



#ifndef GSERVER_PLATFORM_H
# define GSERVER_PLATFORM_H

# include <System/string.h>


/* GServerPlatform */
/* public */
/* accessors */
GServerPlatform * gserverplatform_new(void);
void gserverplatform_delete(GServerPlatform * platform);

/* accessors */
String const * gserverplatform_get_driver(GServerPlatform * platform,
		String const * subsystem);

#endif /* !GSERVER_PLATFORM_H */
