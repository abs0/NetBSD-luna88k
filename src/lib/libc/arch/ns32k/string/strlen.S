/*	$NetBSD: strlen.S,v 1.3 1998/04/03 22:58:10 matthias Exp $	*/

/* 
 * Written by Randy Hyde, 1993
 * Public domain.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: strlen.S,v 1.3 1998/04/03 22:58:10 matthias Exp $")
#endif

/*
 * size_t
 * strlen (char *s)
 *	compute the length of the string s.
 */

KENTRY(strlen, 4)
	enter	[r3,r4],0
	movd	B_ARG0,r0

	/*
	 * First begin by seeing if we can doubleword align the
	 * pointer. The following code aligns the pointer in r0.
	 */

	movqd	3,r3
	andd	r0,r3

0:	casew	1f(pc)[r3:w]
1:	.word	5f-0b
	.word	2f-0b
	.word	3f-0b
	.word	4f-0b

	.align	2,0xa2
2:	cmpqb	0,0(r0) ; beq 7f
	cmpqb	0,1(r0) ; beq 8f
	cmpqb	0,2(r0) ; beq 9f
	addqd	3,r0
	br	5f

	.align	2,0xa2
3:	cmpqb	0,0(r0) ; beq 7f
	cmpqb	0,1(r0) ; beq 8f
	addqd	2,r0
	br	5f

	.align	2,0xa2
4:	cmpqb	0,0(r0) ; beq 7f
	addqd	1,r0

	/*
	 * Okay, when we get down here r0 points at a double word
	 * algined source block of bytes.
	 * This guy processes four bytes at a time and checks for the
	 * zero terminating byte amongst the bytes in the double word.
	 * This algorithm is de to Dave Rand.
	 *
	 * Sneaky test for zero amongst four bytes:
	 *
	 *	 xxyyzztt
	 *	-01010101
	 *	---------
	 *	 aabbccdd
	 *  bic  xxyyzztt
	 *	---------
	 *       eeffgghh   ee=0x80 if xx=0, ff=0x80 if yy=0, etc. 
	 *
	 *		This whole result will be zero if there
	 *		was no zero byte, it will be non-zero if
	 *		there is a zero byte present.
	 */
   
5:	movd	0x01010101,r2		/* Magic number to use */
	movd	0x80808080,r3		/* Another magic number. */
	addqd	-4,r0

	.align	2,0xa2
0:	movd	4(r0),r1		/* Get next double word. */
	addqd	4,r0			/* Advance pointer. */
	movd	r1,r4			/* Save for storage later. */
	subd	r2,r1			/* Gets borrow if byte = 0. */
	bicd	r4,r1			/* Clear original bits. */
	andd	r3,r1			/* See if borrow occurred. */
	cmpqd	0,r1
	beq	0b			/* See if this DWORD contained a 0. */

	/*
	 * At this point, r0 points at a double word which
	 * contains a zero byte. Count the bytes up to the
	 * zero.
	 */

1:	cmpqb	0,0(r0) ; beq 7f
	cmpqb	0,1(r0) ; beq 8f
	cmpqb	0,2(r0) ; beq 9f
	addqd	3,r0			/* Must be in fourth byte. */
	
7:	subd	B_ARG0,r0
	exit	[r3,r4]
	ret	ARGS

8:	addqd	1,r0
	subd	B_ARG0,r0
	exit	[r3,r4]
	ret	ARGS

9:	addqd	2,r0
	subd	B_ARG0,r0
	exit	[r3,r4]
	ret	ARGS
