/*	$NetBSD: __sigsetops14.c,v 1.2 2003/08/07 16:42:45 agc Exp $	*/

/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)sigsetops.c	8.1 (Berkeley) 6/4/93
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)sigsetops.c	8.1 (Berkeley) 6/4/93";
#else
__RCSID("$NetBSD: __sigsetops14.c,v 1.2 2003/08/07 16:42:45 agc Exp $");
#endif
#endif /* LIBC_SCCS and not lint */

#define	__LIBC12_SOURCE__

#include <errno.h>
#include <signal.h>

#undef sigemptyset
#undef sigfillset
#undef sigaddset
#undef sigdelset
#undef sigismember

int
__sigemptyset14(set)
	sigset_t *set;
{

	__sigemptyset(set);
	return (0);
}

int
__sigfillset14(set)
	sigset_t *set;
{

	__sigfillset(set);
	return (0);
}

int
__sigaddset14(set, signo)
	sigset_t *set;
	int signo;
{
	if (signo <= 0 || signo >= NSIG) {
		errno = EINVAL;
		return -1;
	}
	__sigaddset(set, signo);
	return (0);
}

int
__sigdelset14(set, signo)
	sigset_t *set;
	int signo;
{
	if (signo <= 0 || signo >= NSIG) {
		errno = EINVAL;
		return -1;
	}
	__sigdelset(set, signo);
	return (0);
}

int
__sigismember14(set, signo)
	const sigset_t *set;
	int signo;
{
	if (signo <= 0 || signo >= NSIG) {
		errno = EINVAL;
		return -1;
	}
	return (__sigismember(set, signo));
}
