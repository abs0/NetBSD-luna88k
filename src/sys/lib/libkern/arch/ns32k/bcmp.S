/*	$NetBSD: bcmp.S,v 1.3 2005/02/26 22:58:56 perry Exp $	*/

/*
 * Written by Matthias Pfaller, 1996
 * Public domain.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: bcmp.S,v 1.3 2005/02/26 22:58:56 perry Exp $")
#endif

/*
 * int
 * bcmp (const void *b1, const void *b2, size_t len)
 */
KENTRY(bcmp, 12)
	enter	[],0
	movd	B_ARG2,r0
	movd	B_ARG0,r1
	movd	B_ARG1,r2
	lshd	-2,r0
	cmpsd
	bne	0f
	movqd	3,r0
	andd	B_ARG2,r0
	cmpsb
0:	exit	[]
	ret	ARGS
