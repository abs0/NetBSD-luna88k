/*	$NetBSD: __sigaction14_sigtramp.c,v 1.4 2004/03/23 16:19:41 drochner Exp $	*/

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#include <sys/types.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>

#include "extern.h"

#ifdef __weak_alias
__weak_alias(__sigaction14, __libc_sigaction14)
#endif

int
__libc_sigaction14(int sig, const struct sigaction *act, struct sigaction *oact)
{
	extern const int __sigtramp_sigcontext_1[];
	extern const int __sigtramp_siginfo_2[];
	int rv;

	/*
	 * If no sigaction, use the "default" trampoline since it won't
	 * be used.
	 */
	if (act == NULL)
		return  __sigaction_sigtramp(sig, act, oact, NULL, 0);

	/*
	 * We select the non-SA_SIGINFO trampoline if SA_SIGINFO is not
	 * set in the sigaction.
	 */
	if ((act->sa_flags & SA_SIGINFO) == 0) {
		int sav = errno;
		rv =  __sigaction_sigtramp(sig, act, oact,
		    __sigtramp_sigcontext_1, 1);
		if (rv >= 0 || errno != EINVAL)
			return rv;
		errno = sav;
	}

	/*
	 * If SA_SIGINFO was specified or the compatibility trampolines
	 * can't be used, use the siginfo trampoline.
	 */
	return __sigaction_sigtramp(sig, act, oact, __sigtramp_siginfo_2, 2);
}