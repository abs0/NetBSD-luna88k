/*	$NetBSD: sigsuspend.S,v 1.5 1998/12/02 01:01:04 thorpej Exp $	*/

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

#include "SYS.h"

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: sigsuspend.S,v 1.5 1998/12/02 01:01:04 thorpej Exp $")
#endif

WARN_REFERENCES(sigsuspend, \
    "warning: reference to compatibility sigsuspend(); include <signal.h> for correct reference")

ENTRY(sigsuspend)
	movd	0(S_ARG0),S_ARG0	/* indirect to mask arg */
	SYSTRAP(compat_13_sigsuspend13)
	bcs	_ASM_LABEL(cerror)
	movqd	0,r0			/* shouldn t happen */
	ret	0
