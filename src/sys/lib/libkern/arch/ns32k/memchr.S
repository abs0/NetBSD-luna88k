/*	$NetBSD: memchr.S,v 1.3 2005/02/26 22:58:56 perry Exp $	*/

/*
 * Written by Matthias Pfaller, 1996
 * Public domain.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: memchr.S,v 1.3 2005/02/26 22:58:56 perry Exp $")
#endif

/*
 * void *
 * memchr (const void *b, int c, size_t len)
 *	locate the first occurrence of c in string b.
 */

KENTRY(memchr, 12)
	enter	[r4],0
	movd	B_ARG2,r0
	movb	B_ARG1,r4
	movd	B_ARG0,r1
	skpsb	u
	bfc	0f
	movd	r1,r0
0:	exit	[r4]
	ret	ARGS
