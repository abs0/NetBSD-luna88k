/************************************************************************
* Copyright 1995 by Wietse Venema.  All rights reserved.  Some individual
* files may be covered by other copyrights.
*
* This material was originally written and compiled by Wietse Venema at
* Eindhoven University of Technology, The Netherlands, in 1990, 1991,
* 1992, 1993, 1994 and 1995.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that this entire copyright notice
* is duplicated in all such copies.
*
* This software is provided "as is" and without any expressed or implied
* warranties, including, without limitation, the implied warranties of
* merchantibility and fitness for any particular purpose.
************************************************************************/
/* Author: Wietse Venema <wietse@wzv.win.tue.nl> */

#include "bsd_locl.h"

__RCSID("$KTH-KRB: sysv_default.c,v 1.12 2001/06/04 14:08:41 assar Exp $"
      "$NetBSD: sysv_default.c,v 1.1.1.4 2002/09/12 12:22:04 joda Exp $");

#include "sysv_default.h"

 /*
  * Default values for stuff that can be read from the defaults file. The
  * SunOS 5.1 documentation is incomplete and often disagrees with reality.
  */

static char default_umask_value[] = "022";

char   *default_console	= 0;
char   *default_altsh	= "YES";
char   *default_passreq	= "NO";
char   *default_timezone= 0;
char   *default_hz	= 0;
char   *default_path	= _PATH_DEFPATH;
char   *default_supath	= _PATH_DEFSUPATH;
char   *default_ulimit	= 0;
char   *default_timeout	= "180";
char   *default_umask	= default_umask_value;
char   *default_sleep	= "4";
char   *default_maxtrys	= "5";

static struct sysv_default {
    char  **valptr;
    char   *prefix;
    int     prefix_len;
} defaults[] = {
    {&default_console,	"CONSOLE=",	sizeof("CONSOLE=") -1},
    {&default_altsh,	"ALTSHELL=",	sizeof("ALTSHELL=") -1},
    {&default_passreq,	"PASSREQ=",	sizeof("PASSREQ=") -1},
    {&default_timezone,	"TIMEZONE=",	sizeof("TIMEZONE=") -1},
    {&default_hz,	"HZ=",		sizeof("HZ=") -1},
    {&default_path,	"PATH=",	sizeof("PATH=") -1},
    {&default_supath,	"SUPATH=",	sizeof("SUPATH=") -1},
    {&default_ulimit,	"ULIMIT=",	sizeof("ULIMIT=") -1},
    {&default_timeout,	"TIMEOUT=",	sizeof("TIMEOUT=") -1},
    {&default_umask,	"UMASK=",	sizeof("UMASK=") -1},
    {&default_sleep,	"SLEEPTIME=",	sizeof("SLEEPTIME=") -1},
    {&default_maxtrys,	"MAXTRYS=",	sizeof("MAXTRYS=") -1},
    {0},
};

#define trim(s) { \
	char   *cp = s + strlen(s); \
	while (cp > s && isspace((unsigned char)cp[-1])) \
	    cp--; \
	*cp = 0; \
}

/* sysv_defaults - read login defaults file */

void
sysv_defaults()
{
    struct sysv_default *dp;
    FILE   *fp;
    char    buf[BUFSIZ];

    if ((fp = fopen(_PATH_ETC_DEFAULT_LOGIN, "r"))) {

	/* Stupid quadratic algorithm. */

	while (fgets(buf, sizeof(buf), fp)) {

	    /* Skip comments and blank lines. */

	    if (buf[0] == '#')
		continue;
	    trim(buf);
	    if (buf[0] == 0)
		continue;

	    /* Assign defaults from file. */

#define STREQN(x,y,l) (x[0] == y[0] && strncmp(x,y,l) == 0)

	    for (dp = defaults; dp->valptr; dp++) {
		if (STREQN(buf, dp->prefix, dp->prefix_len)) {
		    if ((*(dp->valptr) = strdup(buf + dp->prefix_len)) == 0) {
			warnx("Insufficient memory resources - try later.");
			sleepexit(1);
		    }
		    break;
		}
	    }
	}
	fclose(fp);
    }
}
