/*	$NetBSD: uucpd.c,v 1.23 2003/08/07 09:46:53 agc Exp $	*/

/*
 * Copyright (c) 1985 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Adams.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1985 The Regents of the University of California.\n\
 All rights reserved.\n");
#if 0
static char sccsid[] = "from: @(#)uucpd.c	5.10 (Berkeley) 2/26/91";
#else
__RCSID("$NetBSD: uucpd.c,v 1.23 2003/08/07 09:46:53 agc Exp $");
#endif
#endif /* not lint */

/*
 * 4.2BSD TCP/IP server for uucico
 * uucico's TCP channel causes this server to be run at the remote end.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/syslog.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <util.h>
#include <time.h>
#ifdef SUPPORT_UTMP
#include <utmp.h>
#endif
#ifdef SUPPORT_UTMPX
#include <utmpx.h>
#endif

#include "pathnames.h"

struct	sockaddr_storage hisctladdr;
int hisaddrlen = sizeof hisctladdr;
int mypid;

char Logname[64], Username[64];
char *nenv[] = {
	Logname,
	Username,
	NULL,
};
int	dolog;

extern char **environ;

void dologout __P((void));
void dologin __P((struct passwd *, struct sockaddr *));
void doit __P((struct sockaddr *));
int readline __P((char *, int));
int main __P((int, char **));

int
main(argc, argv)
	int argc;
	char **argv;
{
	int	ch;

	while ((ch = getopt(argc, argv, "l")) != -1)
		switch (ch) {
		case 'l':
			dolog = 1;
			break;
		default:
			exit(1);
		}

	if (dolog)
		openlog("uucpd", LOG_PID, LOG_AUTH);
	environ = nenv;
	close(1);
	close(2);
	dup(0);
	dup(0);
	hisaddrlen = sizeof (hisctladdr);
	if (getpeername(0, (struct sockaddr *)&hisctladdr, &hisaddrlen) < 0) {
		fprintf(stderr, "%s: ", argv[0]);
		perror("getpeername");
		if (dolog)
			syslog(LOG_ERR, "getpeername failed: %m");
		exit(1);
	}
	switch (fork()) {
	case -1:
		break;
	case 0:
		doit((struct sockaddr *)&hisctladdr);
		break;
	default:
		dologout();
	}
	exit(1);
}

void
doit(sa)
	struct sockaddr *sa;
{
	char user[64], passwd[64];
	char *xpasswd = NULL;	/* XXX gcc */
	struct passwd *pw;

	alarm(60);
	do {
		printf("login: ");
		fflush(stdout);
		if (readline(user, sizeof user) < 0) {
			fprintf(stderr, "user read\n");
			if (dolog)
				syslog(LOG_ERR, "user read failed: %m");
			return;
		}
	} while (user[0] == '\0');
	/* ensure username is NUL-terminated */
	user[sizeof(user)-1] = '\0';
	pw = getpwnam(user);
	if (pw == NULL || (pw->pw_passwd && *pw->pw_passwd != '\0')) {
		printf("Password: ");
		fflush(stdout);
		if (readline(passwd, sizeof passwd) < 0) {
			fprintf(stderr, "passwd read\n");
			if (dolog)
				syslog(LOG_ERR, "passwd read failed: %m");
			return;
		}
		if (pw != NULL)
			xpasswd = crypt(passwd, pw->pw_passwd);

		if (pw == NULL || strcmp(xpasswd, pw->pw_passwd)) {
			fprintf(stderr, "Login incorrect.\n");
			if (dolog)
				syslog(LOG_ERR, "incorrect password from %s",
				    user);
			return;
		}
	} else
		(void)crypt("dummy password", "PA");	/* must always crypt */
	if (strcmp(pw->pw_shell, _PATH_UUCICO)) {
		fprintf(stderr, "Login incorrect.\n");
			if (dolog)
				syslog(LOG_ERR, "incorrect shell for %s", user);
		return;
	}
	alarm(0);
	snprintf(Logname, sizeof(Logname), "LOGNAME=%s", user);
	snprintf(Username, sizeof(Username), "USER=%s", user);
	dologin(pw, sa);
	if (initgroups(pw->pw_name, pw->pw_gid) < 0 ||
	    setgid(pw->pw_gid) < 0 ||
	    chdir(pw->pw_dir) < 0 ||
	    setsid() < 0 ||
	    setlogin(user) < 0 ||
	    setuid(pw->pw_uid) < 0) {
		fprintf(stderr, "Could not set permissions.\n");
		if (dolog)
			syslog(LOG_ERR,"couldn't set user %s's permissions: %m",
			    user);
		return;
	}
	if (dolog)
		syslog(LOG_INFO, "%s has logged in", user);
	execl(_PATH_UUCICO, "uucico", (char *)0);
	perror("uucico server: execl");
	if (dolog)
		syslog(LOG_ERR, "execl failed: %m");
}

int
readline(p, n)
	char *p;
	int n;
{
	char c;

	while (n-- > 0) {
		if (read(0, &c, 1) <= 0)
			return(-1);
		c &= 0177;
		if (c == '\r') {
			*p = '\0';
			return(0);
		}
		if (c != '\n')
			*p++ = c;
	}
	return (-1);
}

/* Note that SCPYN is only used on strings that may not be nul terminated */
#define	SCPYN(a, b)	strncpy(a, b, sizeof (a))

#ifdef SUPPORT_UTMP
struct utmp utmp;
#endif
#ifdef SUPPORT_UTMPX
struct utmpx utmpx;
#endif

void
dologout()
{
#if defined(SUPPORT_UTMP) || defined(SUPPORT_UTMPX)
	int status;
	char line[MAXPATHLEN];
	pid_t pid;

	while ((pid = wait(&status)) > 0) {
		snprintf(line, sizeof(line), "uucp%.4d", pid);
#ifdef SUPPORT_UTMPX
		if (logoutx(line, status, DEAD_PROCESS))
			logwtmpx(line, "", "", status, DEAD_PROCESS);
#endif
#ifdef SUPPORT_UTMP
		if (logout(line))
			logwtmp(line, "", "");
#endif
	}
#endif
}

/*
 * Record login in wtmp file.
 */
void
dologin(pw, sa)
	struct passwd *pw;
	struct sockaddr *sa;
{
#if defined(SUPPORT_UTMP) || defined(SUPPORT_UTMPX)
	char hbuf[NI_MAXHOST];
	struct timeval tv;
	char line[MAXPATHLEN];
	pid_t pid = getpid();

	/* hack, but must be unique and no tty line */
	snprintf(line, sizeof(line), "uucp%.4d", pid = getpid());
	(void)gettimeofday(&tv, NULL);
	if (getnameinfo(sa, sa->sa_len, hbuf, sizeof(hbuf), NULL, 0, 0))
		(void)strlcpy(hbuf, "?", sizeof(hbuf));
#endif
#ifdef SUPPORT_UTMPX
	SCPYN(utmpx.ut_line, line);
	SCPYN(utmpx.ut_name, pw->pw_name);
	SCPYN(utmpx.ut_host, hbuf);
	if (strlen(line) > sizeof(utmpx.ut_id)) {
		SCPYN(utmpx.ut_id, line + strlen(line) - sizeof(utmpx.ut_id));
	} else {
		SCPYN(utmpx.ut_id, line);
	}
	utmpx.ut_tv = tv;
	utmpx.ut_type = USER_PROCESS;
	utmpx.ut_pid = pid;
	(void)memcpy(&utmpx.ut_ss, sa, sizeof(*sa));
	loginx(&utmpx);
	logwtmpx(line, pw->pw_name, hbuf, 0, USER_PROCESS);
#endif
#ifdef SUPPORT_UTMP
	SCPYN(utmp.ut_line, line);
	SCPYN(utmp.ut_name, pw->pw_name);
	SCPYN(utmp.ut_host, hbuf);
	utmp.ut_time = (time_t)tv.tv_sec;
	login(&utmp);
	logwtmp(line, pw->pw_name, hbuf);
#endif
}
