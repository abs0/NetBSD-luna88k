/*	$NetBSD: byte_swap_2.S,v 1.2 1999/02/01 21:32:35 matthias Exp $	*/

/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * Copyright (c) 1992 Helsinki University of Technology
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON AND HELSINKI UNIVERSITY OF TECHNOLOGY ALLOW FREE USE
 * OF THIS SOFTWARE IN ITS "AS IS" CONDITION.  CARNEGIE MELLON AND
 * HELSINKI UNIVERSITY OF TECHNOLOGY DISCLAIM ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * 11-May-92  Johannes Helander (jvh) at Helsinki University of Technology
 *	Created.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: byte_swap_2.S,v 1.2 1999/02/01 21:32:35 matthias Exp $")
#endif

KENTRY(__bswap16, 4)
	movzwd	S_ARG0,r0
	rotw	8,r0
	ret	ARGS
ALTENTRY(ntohs,__bswap16)
ALTENTRY(htons,__bswap16)
