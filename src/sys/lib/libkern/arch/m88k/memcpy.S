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
 * Gcc 2 generates calls to memcpy for bcopies of unknown size. memcpy can
 * simply be implemented as ovbcopy but the src (r2, r3) and dst args need to
 * be switched.
 */
/*
 * void memcpy(dest, source, count)
 *
 */
ENTRY(memcpy)
	or 	r5, r0, r2	/* dst -> tmp */
	or	r2, r0, r3	/* src -> 1st arg */
	br.n	_C_LABEL(ovbcopy)
	 or	r3, r0, r5	/* dst -> 2nd arg */
