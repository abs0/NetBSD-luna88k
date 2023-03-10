/*	$NetBSD: profile.h,v 1.8 2003/08/07 16:29:00 agc Exp $	*/

/*
 * Copyright (c) 1992, 1993
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
 *	@(#)profile.h	8.1 (Berkeley) 6/11/93
 */

/*	pc532 version, 5/15/94.
 */

/* profiles ... */
__inline void _mcount __P((u_long, u_long));
void mcount(void);

#define	_MCOUNT_DECL __inline void _mcount

#define	MCOUNT \
extern void mcount(void) __asm__("mcount");				\
void									\
mcount(void)								\
{									\
	int selfpc, frompcindex;					\
	/*								\
	 * find the return address for mcount,				\
	 * and the return address for mcount's caller.			\
	 *								\
	 * selfpc = pc pushed by mcount call				\
	 */								\
	__asm__("movd 4(fp),%0" : "=r" (selfpc));			\
	/*								\
	 * frompcindex = pc pushed by call into self.			\
	 */								\
	__asm__("movd 4(0(fp)),%0" : "=r" (frompcindex));		\
	_mcount(frompcindex, selfpc);					\
}

#ifdef _KERNEL
/*
 * Note that we assume splhigh() and splx() cannot call mcount()
 * recursively.
 */
#define	MCOUNT_ENTER	s = splhigh()
#define	MCOUNT_EXIT	splx(s)
#endif /* _KERNEL */
