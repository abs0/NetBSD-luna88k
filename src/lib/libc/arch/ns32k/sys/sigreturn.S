/*	$NetBSD: sigreturn.S,v 1.9 2004/03/26 22:15:03 drochner Exp $	*/

/* 
 * Copyright (c) 1992 Helsinki University of Technology
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * HELSINKI UNIVERSITY OF TECHNOLOGY ALLOWS FREE USE OF THIS SOFTWARE IN
 * ITS "AS IS" CONDITION. HELSINKI UNIVERSITY OF TECHNOLOGY DISCLAIMS ANY
 * LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE
 * USE OF THIS SOFTWARE.
 */
/*
 * HISTORY
 * 29-Apr-92  Johannes Helander (jvh) at Helsinki University of Technology
 *	Created.
 */

#ifdef GPROF
/*
 * We must preserve the state of the registers as the user has set them up.
 */
# define _SAVELIST r0,r1,r2
#endif
#include "SYS.h"

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: sigreturn.S,v 1.9 2004/03/26 22:15:03 drochner Exp $")
#endif

WARN_REFERENCES(sigreturn, \
    "warning: reference to compatibility sigreturn()")

PSEUDO(sigreturn,compat_13_sigreturn13)
