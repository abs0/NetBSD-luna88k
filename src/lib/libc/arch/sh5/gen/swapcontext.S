/*	$NetBSD: swapcontext.S,v 1.3 2003/03/24 14:29:34 scw Exp $	*/

/*
 * Copyright 2003 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Steve C. Woodford for Wasabi Systems, Inc.
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
 *      This product includes software developed for the NetBSD Project by
 *      Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "SYS.h"

#if defined(LIBC_SCCS) && !defined(lint)
	RCSID("$NetBSD: swapcontext.S,v 1.3 2003/03/24 14:29:34 scw Exp $")
#endif /* LIBC_SCCS && !lint */

ENTRY(swapcontext)
	or	r2, r63, r4		/* Preserve oucp in r4 across syscall */
	or	r18, r63, r5		/* Preserve return address similarly */
	SYSTRAP(getcontext)		/* Note: preserves ucp(r3) & oucp(r4) */
	pt/u	1f, tr0
	bne/u	r0, r63, tr0
	st.q	r4, 40, r5		/* Adjust saved program counter */
	st.q	r4, 72, r63		/* Arrange for return value of 0 */
	or	r3, r63, r2		/* Fetch 'ucp' */
	SYSTRAP(setcontext)
	/*NOTREACHED*/
	brk				/* But just in case ... */

1:	PIC_PROLOGUE
	PIC_PTAL(_C_LABEL(__cerror), r0, tr0)
	blink	tr0, r63
