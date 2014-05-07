/* $Id$ */
/* Copyright (c) 2010-2014 Pierre Pronchery <khorben@defora.org> */
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



#include <unistd.h>
#include <stdio.h>
#include <System.h>
#include "gserver.h"
#include "../config.h"

#ifndef PROGNAME
# define PROGNAME PACKAGE
#endif


/* functions */
/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " [-R]\n", stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int o;
	AppServerOptions options = 0;
	GServer * gserver;

	while((o = getopt(argc, argv, "R")) != -1)
		switch(o)
		{
			case 'R':
				options = ASO_REGISTER;
				break;
			default:
				return _usage();
		}
	if(optind != argc)
		return _usage();
	if((gserver = gserver_new(options, NULL)) == NULL)
		return error_print("GServer");
	while(gserver_loop(gserver) == 0);
	gserver_delete(gserver);
	return 0;
}
