/*      $NetBSD: functions.c,v 1.12 2005/03/09 22:09:36 kleink Exp $       */

/*
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Copyright (c) 2000 Tim Rightnour <garbled@NetBSD.org>
 * Copyright (c) 2000 Hubert Feyrer <hubertf@NetBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/utsname.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "sushi.h"
#include "functions.h"

#ifndef NETBSD_PKG_BASE
#define NETBSD_PKG_BASE	"ftp://ftp.NetBSD.org/pub/NetBSD/packages"
#endif

extern int scripting;
extern int logging;
extern FILE *logfile;
extern FILE *script;
extern nl_catd catalog;

char *ftp_base(int);
void cleanup(void);

/* Required by libinstall */
void	cleanup(void){ ; }
int	ftp_cmd(const char *, const char *);
void	ftp_stop(void);

func_record func_map[] = 
{
	{ "ftp_pkglist", ftp_pkglist },
	{ "ftp_pkgcats", ftp_pkgcats },
	{ "script_do", script_do },
	{ "log_do", log_do },
	{(char *)NULL, (char **(*)(char *))NULL},
};

/*ARGSUSED*/
char **
log_do(char *what)
{
	int i;
	time_t tloc;

	i = logging;

	if (logging == 1)
		logging = 0;
	else if (logging == 0)
		logging = 1;

	time(&tloc);

	if (logging && i == 0) { /* open */
		logfile = fopen(LOGFILE_NAME, "w");
		if (logfile == NULL)
			bailout("fopen %s: %s", LOGFILE_NAME,  strerror(errno));
		fprintf(logfile, "%s: %s\n",
			catgets(catalog, 4, 3, "Log started at"),
			asctime(localtime(&tloc)));
		fflush(logfile);
	} else {  /* close */
		if (logfile == NULL)
			bailout("fopen %s: %s", LOGFILE_NAME,  strerror(errno));
		fprintf(logfile, "%s: %s\n",
			catgets(catalog, 4, 10, "Log ended at"),
			asctime(localtime(&tloc)));
		fflush(logfile);
		fclose(logfile);
	}
	return (NULL); /* XXX */
}

/*ARGSUSED*/
char **
script_do(char *what)
{
	int i;
	time_t tloc;

	i = scripting;

	if (scripting == 0)
		scripting = 1;
	else if (scripting == 1)
		scripting = 0;

	time(&tloc);

	if (scripting && i == 0) { /* open */
		script = fopen(SCRIPTFILE_NAME, "w");
		if (script == NULL)
			bailout("fopen %s: %s", SCRIPTFILE_NAME,
			    strerror(errno));
		fprintf(script, "#!/bin/sh\n");
		fprintf(script, "# %s: %s\n",
			catgets(catalog, 4, 4, "Script started at"),
			asctime(localtime(&tloc)));
		fflush(script);
	} else { /* close */
		if (script == NULL)
			bailout("fopen %s: %s", SCRIPTFILE_NAME,
			    strerror(errno));
		fprintf(script, "# %s: %s\n",
			catgets(catalog, 4, 11, "Script ended at"),
			asctime(localtime(&tloc)));
		fflush(script);
		fclose(script);
	}
	return (NULL); /* XXX */
}

/*
 *   Return list of packages available at the given URL
 *   or NULL on error. Returned pointer can be free(3)d
 *   later.
 */
char **
ftp_pkglist(char *subdir)
{
	int rc, tfd;
	char tmpname[FILENAME_MAX];
	char buf[FILENAME_MAX];
	char url[FILENAME_MAX];
	char **list;
	FILE *f;
	int nlines;

	extern int ftp_start(char *);		/* pkg_install/lib stuff */
	extern int Verbose;			/* pkg_install/lib stuff */
	Verbose = 0; /* debugging */

	/* ftp(1) must have a trailing '/' for directories */
	snprintf(url, sizeof(url), "%s/%s/", ftp_base(0), subdir);

	/*
	 * Start FTP coprocess
	 */
	rc = ftp_start(url);
	if (rc == -1)
		bailout(catgets(catalog, 1, 3, "ftp_start failed"));

	/*
	 * Generate tmp file
	 */
	strlcpy(tmpname, TMPFILE_NAME, sizeof(tmpname));
	tfd = mkstemp(tmpname);
	if (tfd == -1)
		bailout("mkstemp: %s", strerror(errno));

	close(tfd); /* We don't need the file descriptor, but will use 
		       the file in a second */
	/*
	 * Setup & run the command for ftp(1)
	 */
	(void)snprintf(buf, sizeof(buf), "nlist *.tgz %s\n", tmpname);
	rc = ftp_cmd(buf, "\n(550|226).*\n"); /* catch errors */
	if (rc != 226) {
		unlink(tmpname);	/* remove clutter */
		bailout(catgets(catalog, 1, 4, "nlist failed"));
	}

	f = fopen(tmpname, "r");
	if (f == NULL)
		bailout("fopen: %s", strerror(errno));

	/* Read through file once to find out how many lines it has */
	nlines = 0;
	while (fgets(buf, sizeof(buf), f) != NULL)
		nlines++;
	rewind(f);

	list = malloc((nlines + 1) * sizeof(char *));
	if (list == NULL)
		bailout("malloc: %s", strerror(errno));

	/* alloc space for each line now */
	nlines = 0;
	while (fgets(buf, sizeof(buf), f) != NULL) {
		list[nlines] = strdup(buf);
		/* XXX 5 to get .tgz */
		list[nlines][strlen(list[nlines])-5] = '\0';
		nlines++;
	}
	list[nlines] = NULL;

	fclose(f);
	unlink(tmpname);

	/*
	 * Stop FTP coprocess
	 */
	ftp_stop();

	return (list);
}

/*
 *   Return list of package categories available at the given URL
 *   or NULL on error. Returned pointer can be free(3)d
 *   later.
 */
/* ARGSUSED */
char **
ftp_pkgcats(char *subdir)
{
	int rc, tfd;
	char tmpname[FILENAME_MAX];
	char buf[FILENAME_MAX];
	char url[FILENAME_MAX];
	char **list;
	FILE *f;
	int nlines;

	extern int ftp_start(char *);		/* pkg_install/lib stuff */
	extern int Verbose;			/* pkg_install/lib stuff */
	Verbose = 0; /* debugging */

	/* ftp(1) must have a trailing '/' for directories */
	snprintf(url, sizeof(url), "%s/", ftp_base(0));

	/*
	 * Start FTP coprocess
	 */
	rc = ftp_start(url);
	if (rc == -1)
		bailout(catgets(catalog, 1, 3, "ftp_start failed"));

	/*
	 * Generate tmp file
	 */
	strlcpy(tmpname, TMPFILE_NAME, sizeof(tmpname));
	tfd = mkstemp(tmpname);
	if (tfd == -1)
		bailout("mkstemp: %s", strerror(errno));

	close(tfd); /* We don't need the file descriptor, but will use 
		       the file in a second */
	/*
	 * Setup & run the command for ftp(1)
	 */
	(void)snprintf(buf, sizeof(buf), "ls -1F %s\n", tmpname);
	rc = ftp_cmd(buf, "\n(550|226).*\n"); /* catch errors */
	if (rc != 226) {
		unlink(tmpname);	/* remove clutter */
		bailout(catgets(catalog, 1, 4, "nlist failed"));
	}

	f = fopen(tmpname, "r");
	if (f == NULL)
		bailout("fopen: %s", strerror(errno));

	/* Read through file once to find out how many lines it has */
	nlines = 0;
	while (fgets(buf, sizeof(buf), f) != NULL)
		nlines++;
	rewind(f);

	list = malloc((nlines + 1) * sizeof(char *));
	if (list == NULL)
		bailout("malloc: %s", strerror(errno));

	/* alloc space for each line now */
	nlines = 0;
	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (buf[strlen(buf) - 2] == '/') {
			list[nlines] = malloc(strlen(buf) - 1);
			if (list[nlines] == NULL)
				bailout("malloc: %s", strerror(errno));
			strlcpy(list[nlines], buf, strlen(buf) - 1);
			nlines++;
		}
	}
	list[nlines] = NULL;

	fclose(f);
	unlink(tmpname);

	/*
	 * Stop FTP coprocess
	 */
	ftp_stop();

	return (list);
}

/*
 *	Return patch where binary packages for this OS version/arch
 *	are expected. If mirror is NULL, ftp.NetBSD.org is used.
 *	If it's set, it's assumed to be the URL where the OS version
 *	dirs are, e.g. ftp://ftp.NetBSD.org/pub/NetBSD/packages.
 *	If $PKG_PATH is set, is returned unchanged, overriding everything.
 *	In any case, a trailing '/' is *not* passed.
 *	See also Appendix B of /usr/pkgsrc/Packages.txt.
 *
 *	The returned pointer will be overwritten on next call of
 *	this function.
 */
char *
ftp_base(int truename)
{
	char *pkg_path, *p;
	struct utsname un;
	static char buf[256];
	int rc;

	pkg_path = getenv("PKG_PATH");
	if (pkg_path != NULL)
		return (pkg_path);

	strlcpy(buf, NETBSD_PKG_BASE, sizeof(buf));

	rc = uname(&un);
	if (rc == -1)
		bailout("uname: %s", strerror(errno));

	strlcat(buf, "/", sizeof(buf));
	if (!truename) {
		if ((p = strchr(un.release, '_')) != NULL) {
			/* -stable or -rc, e.g. 2.0_STABLE */
			*p = '\0';
		} else if ((p = strstr(un.release, "99.")) != NULL) {
			/* -current, e.g. 2.99.14 */
			*p++ = '0';
			*p = '\0';
		} else if (strspn(un.release, ".") > 1) {
			/* security release, e.g. 2.0.1 */
			p = strrchr(un.release, '.');
			*p = '\0';
		}
	}
	strlcat(buf, un.release, sizeof(buf));
	strlcat(buf, "/", sizeof(buf));
	strlcat(buf, un.machine, sizeof(buf));	/* sysctl hw.machine_arch? */

	return (buf);
}
