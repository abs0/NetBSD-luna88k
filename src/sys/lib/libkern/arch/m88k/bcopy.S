/* $NetBSD$ */
/* $OpenBSD: subr.S,v 1.6 2005/05/15 16:23:45 miod Exp $	*/
/*
 * Mach Operating System
 * Copyright (c) 1993-1992 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * Copyright (c) 1996 Nivas Madhur
 * Copyright (c) 1998 Steve Murphree, Jr.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON AND OMRON ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON AND OMRON DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

#include <sys/cdefs.h>
#include <machine/asm.h>

/*
 * void bcopy(source, destination, count)
 *
 * copy count bytes of data from source to destination
 * Don Harper (don@omron.co.jp), Omron Corporation.
 *
 */

ENTRY(bcopy)
ENTRY(ovbcopy)
	bcnd	eq0,r4,_ASM_LABEL(bcopy_out)	/* nothing to do if == 0 */
/*
 *	check position of source and destination data
 */
	cmp 	r9,r2,r3	/* compare source address to destination */
	bb1	eq,r9,_ASM_LABEL(bcopy_out)	/* nothing to do if equal */
	bb1	lo,r9,_ASM_LABEL(bcopy_reverse)	/* reverse copy if src < dest */
/*
 *	source address is greater than destination address, copy forward
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,_ASM_LABEL(f_byte_copy)	/* copy bytes for small data length */
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(_ASM_LABEL(f_strat))
	or	r12,r12,lo16(_ASM_LABEL(f_strat))
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */


/*
 * Copy three bytes from src to destination then copy words
 */
ASLOCAL(f_3byte_word_copy)
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	addu	r2,r2,3		/* increment source pointer */
	addu	r3,r3,3		/* increment destination pointer */
	br.n	_ASM_LABEL(f_word_copy)	/* copy full words */
	 subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
ASLOCAL(f_1half_word_copy)
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	addu	r2,r2,2		/* increment source pointer */
	addu	r3,r3,2		/* increment destination pointer */
	br.n	_ASM_LABEL(f_word_copy)	/* copy full words */
	 subu	r4,r4,2		/* decrement remaining length */

/*
 * Copy 1 byte from src to destination then copy words
 */
ASLOCAL(f_1byte_word_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */
ASLOCAL(f_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(f_byte_copy)	/* not enough left, copy bytes */
	ld	r6,r2,0		/* load first word */
	ld	r7,r2,4		/* load second word */
	ld	r8,r2,8		/* load third word */
	ld	r9,r2,12	/* load fourth word */
	st	r6,r3,0		/* store first word */
	st	r7,r3,4		/* store second word */
	st 	r8,r3,8		/* store third word */
	st 	r9,r3,12	/* store fourth word */
	addu	r2,r2,16	/* increment source pointer */
	addu	r3,r3,16	/* increment destination pointer */
	br.n	_ASM_LABEL(f_word_copy)	/* branch to copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(f_1byte_half_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

ASLOCAL(f_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(f_byte_copy)	/* not enough left, copy bytes */
	ld.hu	r6,r2,0		/* load first half-word */
	ld.hu	r7,r2,2		/* load second half-word */
	ld.hu	r8,r2,4		/* load third half-word */
	ld.hu	r9,r2,6		/* load fourth half-word */
	ld.hu	r10,r2,8	/* load fifth half-word */
	ld.hu	r11,r2,10	/* load sixth half-word */
	ld.hu	r12,r2,12	/* load seventh half-word */
	ld.hu	r13,r2,14	/* load eighth half-word */
	st.h	r6,r3,0		/* store first half-word */
	st.h	r7,r3,2		/* store second half-word */
	st.h 	r8,r3,4		/* store third half-word */
	st.h 	r9,r3,6		/* store fourth half-word */
	st.h	r10,r3,8	/* store fifth half-word */
	st.h	r11,r3,10	/* store sixth half-word */
	st.h 	r12,r3,12	/* store seventh half-word */
	st.h 	r13,r3,14	/* store eighth half-word */
	addu	r2,r2,16	/* increment source pointer */
	addu	r3,r3,16	/* increment destination pointer */
	br.n	_ASM_LABEL(f_half_copy)	/* branch to copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(f_byte_copy)
	bcnd	eq0,r4,_ASM_LABEL(bcopy_out)	/* branch if nothing left to copy */
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	br.n	_ASM_LABEL(f_byte_copy)	/* branch for next byte */
	 subu	r4,r4,1		/* decrement remaining length */

/*
 *	source address is less than destination address, copy in reverse
 */
ASLOCAL(bcopy_reverse)
/*
 * start copy pointers at end of data
 */
	addu	r2,r2,r4	/* start source at end of data */
	addu	r3,r3,r4	/* start destination at end of data */
/*
 * check for short data
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,_ASM_LABEL(r_byte_copy)	/* copy bytes for small data length */
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(_ASM_LABEL(r_strat))
	or	r12,r12,lo16(_ASM_LABEL(r_strat))
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */

/*
 * Copy three bytes from src to destination then copy words
 */
ASLOCAL(r_3byte_word_copy)
	subu	r2,r2,3		/* decrement source pointer */
	subu	r3,r3,3		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	br.n	_ASM_LABEL(r_word_copy)	/* copy full words */
	 subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
ASLOCAL(r_1half_word_copy)
	subu	r2,r2,2		/* decrement source pointer */
	subu	r3,r3,2		/* decrement destination pointer */
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	br.n	_ASM_LABEL(r_word_copy)	/* copy full words */
	 subu	r4,r4,2		/* decrement remaining length */

/*
 * Copy 1 byte from src to destination then copy words
 */
ASLOCAL(r_1byte_word_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */
ASLOCAL(r_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(r_byte_copy)	/* not enough left, copy bytes */
	subu	r2,r2,16	/* decrement source pointer */
	subu	r3,r3,16	/* decrement destination pointer */
	ld	r6,r2,0		/* load first word */
	ld	r7,r2,4		/* load second word */
	ld	r8,r2,8		/* load third word */
	ld	r9,r2,12	/* load fourth word */
	st	r6,r3,0		/* store first word */
	st	r7,r3,4		/* store second word */
	st 	r8,r3,8		/* store third word */
	st 	r9,r3,12	/* store fourth word */
	br.n	_ASM_LABEL(r_word_copy)	/* branch to copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(r_1byte_half_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

ASLOCAL(r_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(r_byte_copy)	/* not enough left, copy bytes */
	subu	r2,r2,16	/* decrement source pointer */
	subu	r3,r3,16	/* decrement destination pointer */
	ld.hu	r6,r2,0		/* load first half-word */
	ld.hu	r7,r2,2		/* load second half-word */
	ld.hu	r8,r2,4		/* load third half-word */
	ld.hu	r9,r2,6		/* load fourth half-word */
	ld.hu	r10,r2,8	/* load fifth half-word */
	ld.hu	r11,r2,10	/* load sixth half-word */
	ld.hu	r12,r2,12	/* load seventh half-word */
	ld.hu	r13,r2,14	/* load eighth half-word */
	st.h	r6,r3,0		/* store first half-word */
	st.h	r7,r3,2		/* store second half-word */
	st.h 	r8,r3,4		/* store third half-word */
	st.h 	r9,r3,6		/* store fourth half-word */
	st.h	r10,r3,8	/* store fifth half-word */
	st.h	r11,r3,10	/* store sixth half-word */
	st.h 	r12,r3,12	/* store seventh half-word */
	st.h 	r13,r3,14	/* store eighth half-word */
	br.n	_ASM_LABEL(r_half_copy)	/* branch to copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(r_byte_copy)
	bcnd	eq0,r4,_ASM_LABEL(bcopy_out)	/* branch if nothing left to copy */
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	br.n	_ASM_LABEL(r_byte_copy)	/* branch for next byte */
	 subu	r4,r4,1		/* decrement remaining length */

ASLOCAL(bcopy_out)
	jmp	r1		/* all done, return to caller */

	data
	align	4
ASLOCAL(f_strat)
	word	_ASM_LABEL(f_word_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_half_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_3byte_word_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_1byte_half_copy)
	word	_ASM_LABEL(f_half_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_1half_word_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_1byte_half_copy)
	word	_ASM_LABEL(f_byte_copy)
	word	_ASM_LABEL(f_1byte_word_copy)

ASLOCAL(r_strat)
	word	_ASM_LABEL(r_word_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_half_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_1byte_word_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_1byte_half_copy)
	word	_ASM_LABEL(r_half_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_1half_word_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_1byte_half_copy)
	word	_ASM_LABEL(r_byte_copy)
	word	_ASM_LABEL(r_3byte_word_copy)

	text
