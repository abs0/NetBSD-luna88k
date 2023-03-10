/* $OpenBSD: subr.S,v 1.11 2005/05/15 16:23:45 miod Exp $	*/
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

#include "assym.h"

#include <sys/cdefs.h>
#include <sys/errno.h>
#include <machine/asm.h>
#include <machine/trap.h>


/*
 * Copy specified amount of data from user space into the kernel
 * copyin(from, to, len)
 *	r2 == from (user source address)
 *	r3 == to (kernel destination address)
 *	r4 == length
 * (r1=return addr)
 */

#define	SRC	r2
#define	DEST	r3
#define	LEN	r4

ENTRY(copyin)
	/* set up fault handler */
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	or.u	r5,   r0,   hi16(_ASM_LABEL(Lciflt))
	or	r5,   r5,   lo16(_ASM_LABEL(Lciflt))
	st	r5,   r6,   PCB_ONFAULT	/* pcb_onfault = Lciflt */

#if 0
	bcnd	ne0,  LEN,  1f		/* XXX optimize len = 0 case */
	or	r2,   r0,   0
	br	_ASM_LABEL(Lcidone)
1:	bcnd	lt0,  LEN,  _ASM_LABEL(Lciflt)	/* EFAULT if len < 0 */
#endif

	/* If it's a small length (less than 8), then do byte-by-byte */
	cmp	r9,   LEN,  8
	bb1	lt,   r9,   _ASM_LABEL(copyin_byte_only)

	/* If they're not aligned similarly, use byte only... */
	xor	r9,   SRC,  DEST
	mask	r8,   r9,   0x3
	bcnd	ne0,  r8,   _ASM_LABEL(copyin_byte_only)

	/*
	 * At this point, we don't know if they're word aligned or not,
	 * but we know that what needs to be done to one to align
	 * it is what's needed for the other.
	 */
	bb1	0,    SRC,  _ASM_LABEL(copyin_left_align_to_halfword)
ASLOCAL(copyin_left_aligned_to_halfword)
	bb1	1,    SRC,  _ASM_LABEL(copyin_left_align_to_word)
ASLOCAL(copyin_left_aligned_to_word)
	bb1	0,    LEN,  _ASM_LABEL(copyin_right_align_to_halfword)
ASLOCAL(copyin_right_aligned_to_halfword)
	bb1	1,    LEN,  _ASM_LABEL(copyin_right_align_to_word)
ASLOCAL(copyin_right_aligned_to_word)

	/* At this point, both SRC and DEST are aligned to a word */
	/* boundary, and LEN is an even multiple of 4.            */
	bb1.n	2,    LEN,  _ASM_LABEL(copyin_right_align_to_doubleword)
	 or	r7,   r0,   4

ASLOCAL(copyin_right_aligned_to_doubleword)
#ifdef ERRATA__XXX_USR
	NOP
	ld.usr	r5,   SRC,  r0
	NOP
	NOP
	NOP
	ld.usr	r6,   SRC,  r7
	NOP
	NOP
	NOP
#else
	ld.usr	r5,   SRC,  r0
	ld.usr	r6,   SRC,  r7
#endif
	subu	LEN,  LEN,  8
	st	r5,   DEST, r0
	addu	SRC,  SRC,  8
	st	r6,   DEST, r7
	bcnd.n	ne0,  LEN,  _ASM_LABEL(copyin_right_aligned_to_doubleword)
	 addu	DEST, DEST, 8
	br.n	_ASM_LABEL(Lcidone)
	 or	r2, r0, r0	/* successful return */

	/***************************************************/

ASLOCAL(copyin_left_align_to_halfword)
#ifdef ERRATA__XXX_USR
	NOP
	ld.b.usr	r5,   SRC,  r0
	NOP
	NOP
	NOP
#else
	ld.b.usr	r5,   SRC,  r0
#endif
	subu	LEN,  LEN,  1
	st.b	r5,   DEST, r0
	addu	SRC,  SRC,  1
	br.n	_ASM_LABEL(copyin_left_aligned_to_halfword)
	 addu	DEST, DEST, 1

ASLOCAL(copyin_left_align_to_word)
#ifdef ERRATA__XXX_USR
	NOP
	ld.h.usr	r5,   SRC,  r0
	NOP
	NOP
	NOP
#else
	ld.h.usr	r5,   SRC,  r0
#endif
	subu	LEN,  LEN,  2
	st.h	r5,   DEST, r0
	addu	SRC,  SRC,  2
	br.n	_ASM_LABEL(copyin_left_aligned_to_word)
	 addu	DEST, DEST, 2

ASLOCAL(copyin_right_align_to_halfword)
	subu	LEN,  LEN,  1
#ifdef ERRATA__XXX_USR
	NOP
	ld.b.usr	r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.b.usr	r5,   SRC,  LEN
#endif
	br.n	_ASM_LABEL(copyin_right_aligned_to_halfword)
	 st.b	r5,   DEST, LEN

ASLOCAL(copyin_right_align_to_word)
	subu	LEN,  LEN,  2
#ifdef ERRATA__XXX_USR
	NOP
	ld.h.usr	r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.h.usr	r5,   SRC,  LEN
#endif
	br.n	_ASM_LABEL(copyin_right_aligned_to_word)
	 st.h	r5,   DEST, LEN

ASLOCAL(copyin_right_align_to_doubleword)
	subu	LEN,  LEN,  4
#ifdef ERRATA__XXX_USR
	NOP
	ld.usr	r5,   SRC,  LEN
	NOP
	NOP
	NOP
#else
	ld.usr	r5,   SRC,  LEN
#endif
	bcnd.n	ne0,  LEN, _ASM_LABEL(copyin_right_aligned_to_doubleword)
	 st	r5,   DEST, LEN
	br.n	_ASM_LABEL(Lcidone)
   	 or	r2, r0, r0	/* successful return */

ASLOCAL(copyin_byte_only)
	bcnd	eq0, LEN, 2f
1:
	subu	LEN, LEN, 1
#ifdef ERRATA__XXX_USR
	NOP
	ld.b.usr	r5, SRC, LEN
	NOP
	NOP
	NOP
#else
	ld.b.usr	r5, SRC, LEN
#endif
	bcnd.n	ne0, LEN, 1b
	 st.b	r5, DEST, LEN
2:
	br.n	_ASM_LABEL(Lcidone)
	 or	r2, r0, r0	/* successful return */

ASLOCAL(Lcidone)
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	jmp.n	r1
	 st	r0,r6,PCB_ONFAULT

ASLOCAL(Lciflt)
	br.n	_ASM_LABEL(Lcidone)
	 or	r2, r0, EFAULT	/* return fault */

#undef	SRC
#undef	DEST
#undef	LEN
/*######################################################################*/
/*######################################################################*/

/*
 * Copy a null terminated string from the user space to the kernel
 * address space.
 *
 * copyinstr(from, to, maxlen, &lencopied)
 * r2 == from
 * r3 == to
 * r4 == maxlen
 * r5 == len actually transferred (includes the terminating NUL!!!)
 * r6 & r7 - used as temporaries
 */
#define	SRC	r2
#define	DEST	r3
#define	CNT	r4
#define	LEN	r5

ENTRY(copyinstr)

	/* setup fault handler */
	ldcr	r6,   CPU
	ld	r7,   r6,   CI_CURPCB
	or.u	r6,   r0,   hi16(_ASM_LABEL(Lcisflt))
	or	r6,   r6,   lo16(_ASM_LABEL(Lcisflt))
	st	r6,   r7,   PCB_ONFAULT
	or	r6,   r0,   0
	bcnd	lt0,  CNT,  _ASM_LABEL(Lcisflt)
	bcnd	eq0,  CNT,  _ASM_LABEL(Lcistoolong)
1:
#ifdef ERRATA__XXX_USR
	NOP
	ld.bu.usr	r7,   SRC,  r6
	NOP
	NOP
	NOP
#else
	ld.bu.usr	r7,   SRC,  r6
#endif
	st.b	r7, DEST, r6
	bcnd.n	eq0,  r7, 2f		/* all done */
	 addu	r6,   r6, 1
	cmp	r7,   r6, CNT
	bb1	lt,   r7, 1b

ASLOCAL(Lcistoolong)
	or	r2,   r0, ENAMETOOLONG	/* overflow */

ASLOCAL(Lcisnull)
	bcnd	eq0,r6, _ASM_LABEL(Lcisdone)	/* do not attempt to clear last byte */
					/* if we did not write to the string */
	subu	r6,   r6, 1
	st.b	r0, DEST, r6		/* clear last byte */
	br.n	_ASM_LABEL(Lcisdone)
	 addu	r6,   r6, 1
2:					/* all done */
	or	r2,   r0, 0

ASLOCAL(Lcisdone)
	bcnd	eq0, LEN, 3f
	st	r6, r0, LEN
3:
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	jmp.n	r1
	 st	r0,r6,PCB_ONFAULT	/* clear the handler */

ASLOCAL(Lcisflt)
	br.n	_ASM_LABEL(Lcisnull)
	 or	r2, r0, EFAULT		/* return fault */

#undef	SRC
#undef	DEST
#undef	CNT
#undef	LEN

/*
 * Copy specified amount of data from kernel to the user space
 * Copyout(from, to, len)
 *	r2 == from (kernel source address)
 *	r3 == to (user destination address)
 *	r4 == length
 */

#define	SRC	r2
#define	DEST	r3
#define	LEN	r4

ENTRY(copyout)
	/* setup fault handler */
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	or.u	r5,   r0,   hi16(_ASM_LABEL(Lcoflt))
	or	r5,   r5,   lo16(_ASM_LABEL(Lcoflt))
	st	r5,   r6,   PCB_ONFAULT	/* pcb_onfault = Lcoflt */
#if 0
	bcnd	ne0,  LEN,  1f		/* XXX optimize len = 0 case */
	or	r2,   r0,   0
	br	_ASM_LABEL(Lcodone)
1:	bcnd	lt0,  LEN,  _ASM_LABEL(Lcoflt)	/* EFAULT if len < 0 */
#endif
	/* If it's a small length (less than 8), then do byte-by-byte */
	cmp	r9,   LEN,  8
	bb1	lt,   r9,   _ASM_LABEL(copyout_byte_only)

	/* If they're not aligned similarly, use byte only... */
	xor	r9,   SRC,  DEST
	mask	r8,   r9,   0x3
	bcnd	ne0,  r8,   _ASM_LABEL(copyout_byte_only)

	/*
	 * At this point, we don't know if they're word aligned or not,
	 * but we know that what needs to be done to one to align
	 * it is what's needed for the other.
	 */
	bb1	0,    SRC,  _ASM_LABEL(copyout_left_align_to_halfword)
ASLOCAL(copyout_left_aligned_to_halfword)
	bb1	1,    SRC,  _ASM_LABEL(copyout_left_align_to_word)
ASLOCAL(copyout_left_aligned_to_word)
	bb1	0,    LEN,  _ASM_LABEL(copyout_right_align_to_halfword)
ASLOCAL(copyout_right_aligned_to_halfword)
	bb1	1,    LEN,  _ASM_LABEL(copyout_right_align_to_word)
ASLOCAL(copyout_right_aligned_to_word)

	/*
	 * At this point, both SRC and DEST are aligned to a word
	 * boundary, and LEN is an even multiple of 4.
	 */
	bb1.n	2,    LEN,  _ASM_LABEL(copyout_right_align_to_doubleword)
	 or	r7,   r0,   4

ASLOCAL(copyout_right_aligned_to_doubleword)
	ld 	r5,   SRC,  r0
	ld    	r6,   SRC,  r7
	subu	LEN,  LEN,  8
#ifdef ERRATA__XXX_USR
	NOP
	st.usr	r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.usr	r5,   DEST, r0
#endif
	addu	SRC,  SRC,  8
#ifdef ERRATA__XXX_USR
	NOP
	st.usr	r6,   DEST, r7
	NOP
	NOP
	NOP
#else
	st.usr	r6,   DEST, r7
#endif
	bcnd.n	ne0,  LEN,  _ASM_LABEL(copyout_right_aligned_to_doubleword)
	 addu	DEST, DEST, 8
	or	r2, r0, r0	/* successful return */
	br	_ASM_LABEL(Lcodone)

	/***************************************************/
ASLOCAL(copyout_left_align_to_halfword)
	ld.b	r5,   SRC,  r0
	subu	LEN,  LEN,  1
#ifdef ERRATA__XXX_USR
	NOP
	st.b.usr	r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.b.usr	r5,   DEST, r0
#endif
	addu	SRC,  SRC,  1
	br.n	_ASM_LABEL(copyout_left_aligned_to_halfword)
	 addu	DEST, DEST, 1

ASLOCAL(copyout_left_align_to_word)
	ld.h	r5,   SRC,  r0
	subu	LEN,  LEN,  2
#ifdef ERRATA__XXX_USR
	NOP
	st.h.usr	r5,   DEST, r0
	NOP
	NOP
	NOP
#else
	st.h.usr	r5,   DEST, r0
#endif
	addu	SRC,  SRC,  2
	br.n	_ASM_LABEL(copyout_left_aligned_to_word)
	 addu	DEST, DEST, 2

ASLOCAL(copyout_right_align_to_halfword)
	subu	LEN,  LEN,  1
	ld.b	r5,   SRC,  LEN
#ifdef ERRATA__XXX_USR
	NOP
	st.b.usr	r5,   DEST, LEN
	NOP
	NOP
	NOP
	br	_ASM_LABEL(copyout_right_aligned_to_halfword)
#else
	br.n	_ASM_LABEL(copyout_right_aligned_to_halfword)
	 st.b.usr	r5,   DEST, LEN
#endif

ASLOCAL(copyout_right_align_to_word)
	subu	LEN,  LEN,  2
	ld.h	r5,   SRC,  LEN
#ifdef ERRATA__XXX_USR
	NOP
	st.h.usr	r5,   DEST, LEN
	NOP
	NOP
	NOP
	br	_ASM_LABEL(copyout_right_aligned_to_word)
#else
	br.n	_ASM_LABEL(copyout_right_aligned_to_word)
	 st.h.usr	r5,   DEST, LEN
#endif

ASLOCAL(copyout_right_align_to_doubleword)
	subu	LEN,  LEN,  4
	ld	r5,   SRC,  LEN
#ifdef ERRATA__XXX_USR
	NOP
	st.usr	r5,   DEST, LEN
	NOP
	NOP
	NOP
	bcnd	ne0,  LEN, _ASM_LABEL(copyout_right_aligned_to_doubleword)
#else
	bcnd.n	ne0,  LEN, _ASM_LABEL(copyout_right_aligned_to_doubleword)
	 st.usr	r5,   DEST, LEN
#endif
	br.n	_ASM_LABEL(Lcodone)
	 or	r2, r0, r0	/* successful return */

ASLOCAL(copyout_byte_only)
	bcnd	eq0, LEN, 2f
1:
	subu	LEN, LEN, 1
	ld.b	r5, SRC, LEN
#ifdef ERRATA__XXX_USR
	NOP
	st.b.usr	r5, DEST, LEN
	NOP
	NOP
	NOP
	bcnd	ne0, LEN, 1b
#else
	bcnd.n	ne0, LEN, 1b
	 st.b.usr	r5, DEST, LEN
#endif

2:
	br.n	_ASM_LABEL(Lcodone)
	 or	r2, r0, r0	/* successful return */

ASLOCAL(Lcodone)
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	jmp.n	r1
	 st	r0,r6,PCB_ONFAULT	/* clear the handler */

ASLOCAL(Lcoflt)
	br.n	_ASM_LABEL(Lcodone)
	 or	r2, r0, EFAULT	/* return fault */

#undef	SRC
#undef	DEST
#undef	LEN

/*
 * Copy a null terminated string from the kernel space to the user
 * address space.
 *
 * copyoutstr(from, to, maxlen, &lencopied)
 * r2 == from
 * r3 == to
 * r4 == maxlen that can be copied
 * r5 == len actually copied (including the terminating NUL!!!)
 */

#define	SRC	r2
#define	DEST	r3
#define	CNT	r4
#define	LEN	r5

ENTRY(copyoutstr)
	/* setup fault handler */
	ldcr	r6,   CPU
	ld	r7,   r6,   CI_CURPCB
	or.u	r6,   r0,   hi16(_ASM_LABEL(Lcosflt))
	or	r6,   r6,   lo16(_ASM_LABEL(Lcosflt))
	st	r6,   r7,   PCB_ONFAULT
	bcnd	lt0,  CNT,  _ASM_LABEL(Lcosflt)
	bcnd	eq0,  CNT,  _ASM_LABEL(Lcosdone)
	or	r6,   r0,   0
1:
	ld.bu	r7,   SRC,  r6
#ifdef ERRATA__XXX_USR
	NOP
	st.b.usr	r7,   DEST,  r6
	NOP
	NOP
	NOP
#else
	st.b.usr	r7,   DEST,  r6
#endif
	bcnd.n	eq0,  r7, 2f		/* all done */
	 addu	r6,   r6, 1
	cmp	r7,   r6, CNT
	bb1	lt,   r7, 1b
	br.n	_ASM_LABEL(Lcosdone)
	 or	r2,   r0, ENAMETOOLONG
2:
	br.n	_ASM_LABEL(Lcosdone)
	 or	r2,   r0, 0

ASLOCAL(Lcosflt)
	br.n	_ASM_LABEL(Lcosdone)
	 or	r2, r0, EFAULT

ASLOCAL(Lcosdone)
	bcnd	eq0, LEN, 3f
	st	r6, r0, LEN
3:
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	jmp.n	r1
	 st	r0,r6,PCB_ONFAULT	/* clear the handler */

#undef	SRC
#undef	DEST
#undef	CNT
#undef	LEN

/*######################################################################*/

/*
 * kcopy(const void *src, void *dst, size_t len);
 *
 * Copy len bytes from src to dst, aborting if we encounter a page fault.
 */
ENTRY(kcopy)
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	or.u	r5,   r0,   hi16(_ASM_LABEL(kcopy_fault))
	or	r5,   r5,   lo16(_ASM_LABEL(kcopy_fault))
	st	r5,   r6,   PCB_ONFAULT		/* pcb_onfault = kcopy_fault */
	bcnd	le0,r4,_ASM_LABEL(kcopy_out)	/* nothing to do if <= 0 */
/*
 *	check position of source and destination data
 */
	cmp 	r9,r2,r3	/* compare source address to destination */
	bb1	eq,r9,_ASM_LABEL(kcopy_out)	/* nothing to do if equal */
	bb1	lo,r9,_ASM_LABEL(kcopy_reverse)	/* reverse copy if src < dest */
/*
 *	source address is greater than destination address, copy forward
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,_ASM_LABEL(kf_byte_copy)	/* copy bytes for small length */
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(_ASM_LABEL(kf_strat))
	or	r12,r12,lo16(_ASM_LABEL(kf_strat))
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */

/*
 * Copy three bytes from src to destination then copy words
 */
ASLOCAL(kf_3byte_word_copy)
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	addu	r2,r2,3		/* increment source pointer */
	addu	r3,r3,3		/* increment destination pointer */
	br.n	_ASM_LABEL(kf_word_copy)	/* copy full words */
	 subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
ASLOCAL(kf_1half_word_copy)
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	addu	r2,r2,2		/* increment source pointer */
	addu	r3,r3,2		/* increment destination pointer */
	br.n	_ASM_LABEL(kf_word_copy)	/* copy full words */
	 subu	r4,r4,2		/* decrement remaining length */

/*
 * Copy 1 byte from src to destination then copy words
 */
ASLOCAL(kf_1byte_word_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */
ASLOCAL(kf_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(kf_byte_copy) 	/* not enough left, copy bytes */
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
	br.n	_ASM_LABEL(kf_word_copy)	/* copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(kf_1byte_half_copy)
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

ASLOCAL(kf_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(kf_byte_copy)	/* not enough left, copy bytes */
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
	br.n	_ASM_LABEL(kf_half_copy)	/* copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(kf_byte_copy)
	bcnd	eq0,r4,_ASM_LABEL(kcopy_out)	/* branch if nothing left to copy */
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	addu	r2,r2,1		/* increment source pointer */
	addu	r3,r3,1		/* increment destination pointer */
	br.n	_ASM_LABEL(kf_byte_copy)	/* branch for next byte */
	 subu	r4,r4,1		/* decrement remaining length */

/*
 *	source address is less than destination address, copy in reverse
 */
ASLOCAL(kcopy_reverse)
/*
 * start copy pointers at end of data
 */
	addu	r2,r2,r4	/* start source at end of data */
	addu	r3,r3,r4	/* start destination at end of data */
/*
 * check for short data
 */
	cmp 	r9,r4,16	/* see if we have at least 16 bytes */
	bb1	lt,r9,_ASM_LABEL(kr_byte_copy)	/* copy bytes for small data length */
/*
 *	determine copy strategy based on alignment of source and destination
 */
	mask	r6,r2,3		/* get 2 low order bits of source address */
	mask	r7,r3,3		/* get 2 low order bits of destintation addr */
	mak	r6,r6,0<4>	/* convert source bits to table offset */
	mak	r7,r7,0<2>	/* convert destination bits to table offset */
	or.u	r12,r0,hi16(_ASM_LABEL(kr_strat))
	or	r12,r12,lo16(_ASM_LABEL(kr_strat))
	addu	r6,r6,r7	/* compute final table offset for strategy */
	ld	r12,r12,r6	/* load the strategy routine */
	jmp	r12		/* branch to strategy routine */

/*
 * Copy three bytes from src to destination then copy words
 */
ASLOCAL(kr_3byte_word_copy)
	subu	r2,r2,3		/* decrement source pointer */
	subu	r3,r3,3		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	ld.bu	r7,r2,1		/* load byte from source */
	ld.bu	r8,r2,2		/* load byte from source */
	st.b	r6,r3,0		/* store byte to destination */
	st.b	r7,r3,1		/* store byte to destination */
	st.b	r8,r3,2		/* store byte to destination */
	br.n	_ASM_LABEL(kr_word_copy)	/* copy full words */
	 subu	r4,r4,3		/* decrement length */

/*
 * Copy 1 halfword from src to destination then copy words
 */
ASLOCAL(kr_1half_word_copy)
	subu	r2,r2,2		/* decrement source pointer */
	subu	r3,r3,2		/* decrement destination pointer */
	ld.hu	r6,r2,0		/* load half-word from source */
	st.h	r6,r3,0		/* store half-word to destination */
	br.n	_ASM_LABEL(kr_word_copy)	/* copy full words */
	 subu	r4,r4,2		/* decrement remaining length */

/*
 * Copy 1 byte from src to destination then copy words
 */
ASLOCAL(kr_1byte_word_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to word copy */
/*
 * Copy as many full words as possible, 4 words per loop
 */
ASLOCAL(kr_word_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(kr_byte_copy)	/* not enough left, copy bytes */
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
	br.n	_ASM_LABEL(kr_word_copy)	/* copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(kr_1byte_half_copy)
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load 1 byte from source */
	st.b	r6,r3,0		/* store 1 byte to destination */
	subu	r4,r4,1		/* decrement remaining length */
	/* fall through to half copy */

ASLOCAL(kr_half_copy)
	cmp	r10,r4,16	/* see if we have 16 bytes remaining */
	bb1	lo,r10,_ASM_LABEL(kr_byte_copy)	/* not enough left, copy bytes */
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
	br.n	_ASM_LABEL(kr_half_copy)	/* copy another block */
	 subu	r4,r4,16	/* decrement remaining length */

ASLOCAL(kr_byte_copy)
	bcnd	eq0,r4,_ASM_LABEL(kcopy_out)	/* branch if nothing left to copy */
	subu	r2,r2,1		/* decrement source pointer */
	subu	r3,r3,1		/* decrement destination pointer */
	ld.bu	r6,r2,0		/* load byte from source */
	st.b	r6,r3,0		/* store byte in destination */
	br.n	_ASM_LABEL(kr_byte_copy)	/* branch for next byte */
	 subu	r4,r4,1		/* decrement remaining length */

ASLOCAL(kcopy_out)
	or	r2,   r0,   0		/* return success */
ASLOCAL(kcopy_out_fault)
	ldcr	r5,   CPU
	ld	r6,   r5,   CI_CURPCB
	st	r0,r6,PCB_ONFAULT	/* clear the handler */
	jmp	r1		/* all done, return to caller */

ASLOCAL(kcopy_fault)
	or	r2,   r0,   EFAULT	/* return fault */
	br	_ASM_LABEL(kcopy_out_fault)

	data
	align	4
ASLOCAL(kf_strat)
	word	_ASM_LABEL(kf_word_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_half_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_3byte_word_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_1byte_half_copy)
	word	_ASM_LABEL(kf_half_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_1half_word_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_1byte_half_copy)
	word	_ASM_LABEL(kf_byte_copy)
	word	_ASM_LABEL(kf_1byte_word_copy)

ASLOCAL(kr_strat)
	word	_ASM_LABEL(kr_word_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_half_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_1byte_word_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_1byte_half_copy)
	word	_ASM_LABEL(kr_half_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_1half_word_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_1byte_half_copy)
	word	_ASM_LABEL(kr_byte_copy)
	word	_ASM_LABEL(kr_3byte_word_copy)

/*######################################################################*/
