/*	$NetBSD: portald.h,v 1.7 2005/02/09 13:57:57 xtraeme Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
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
 *
 *	from: Id: portald.h,v 1.1 1992/05/25 21:43:09 jsp Exp
 *	@(#)portald.h	8.1 (Berkeley) 6/5/93
 */

#include <sys/cdefs.h>
#include <miscfs/portal/portal.h>

/*
 * Meta-chars in an RE.  Paths in the config file containing
 * any of these characters will be matched using regexec, other
 * paths will be prefix-matched.
 */
#define RE_CHARS ".|()[]*+?\\^$"

typedef struct qelem qelem;

struct qelem {
	qelem *q_forw;
	qelem *q_back;
};

typedef struct provider provider;
struct provider {
	const char *pr_match;
	int (*pr_func)(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);
};
extern provider providers[];

/*
 * Portal providers
 */
extern int portal_exec(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);
extern int portal_file(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);
extern int portal_tcp(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);
extern int portal_rfilter(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);
extern int portal_wfilter(struct portal_cred *,
				char *key, const char **v, int so, int *fdp);

/*
 * Global functions
 */
extern void activate(qelem *q, int so);
extern const char **conf_match(qelem *q, char *key);
extern void conf_read(qelem *q, char *conf);
extern int lose_credentials(struct portal_cred *);
