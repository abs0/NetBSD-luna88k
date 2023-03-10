/* $NetBSD: byte_swap_4.S,v 1.4 2000/06/14 06:49:01 cgd Exp $ */

/*
 * Copyright (c) 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
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

#include <machine/asm.h>

/*
 * Byte-swap a 4-byte quantity.  (Convert 0x01234567 to 0x67452301.)
 *
 * Argument is an unsigned 4-byte integer (u_int32_t).
 */
LEAF(__bswap32, 1)		/* a0 contains 0x01234567 */
XLEAF(htonl, 1)
XLEAF(ntohl, 1)
	insbl	a0, 3, t0	/* t0 = 0x67       */
	extbl	a0, 1, t1	/* t1 = 0x      45 */
	extbl	a0, 2, t2	/* t2 = 0x      23 */
	extbl	a0, 3, t3	/* t3 = 0x      01 */
	sll	t1, 16, t1	/* t1 = 0x  45     */
	sll	t2, 8, t2	/* t2 = 0x    23   */
	or	t3, t0, v0	/* v0 = 0x67    01 */
	or	t1, t2, t1	/* t1 = 0x  4523   */
	or	t1, v0, v0	/* v0 = 0x67452301 */
	RET
END(__bswap32)
