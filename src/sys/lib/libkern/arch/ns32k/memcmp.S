/*	$NetBSD: memcmp.S,v 1.2 2005/02/26 22:58:56 perry Exp $	*/

/*
 * Written by Matthias Pfaller, 1996
 * Public domain.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: memcmp.S,v 1.2 2005/02/26 22:58:56 perry Exp $")
#endif

/*
 * int
 * memcmp (const void *b1, const void *b2, size_t len)
 */
KENTRY(memcmp, 12)
	enter	[],0
	movd	B_ARG2,r0
	movd	B_ARG0,r1
	movd	B_ARG1,r2

	lshd	-2,r0
	cmpsd
	movqd	4,r0
	bne	1f
	movqd	3,r0
	andd	B_ARG2,r0
1:	cmpsb
	bne	3f
2:	exit	[]
	ret	ARGS

3:	movzbd	0(r1),r0
	movzbd	0(r2),r1
	subd	r1,r0
	exit	[]
	ret	ARGS
