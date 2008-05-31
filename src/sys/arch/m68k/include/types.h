/*	$NetBSD: types.h,v 1.22 2004/01/18 18:23:19 martin Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	7.9 (Berkeley) 3/9/91
 */
#ifndef	_M68K_TYPES_H_
#define	_M68K_TYPES_H_

#include <sys/cdefs.h>
#include <sys/featuretest.h>
#include <m68k/int_types.h>

#if defined(_KERNEL)
typedef struct label_t {		/* consistent with HP-UX */
	int val[15];
} label_t;
#endif

/* NB: This should probably be if defined(_KERNEL) */
#if defined(_NETBSD_SOURCE)
typedef	unsigned long	vm_offset_t;
typedef	unsigned long	vm_size_t;

typedef vm_offset_t	paddr_t;
typedef vm_size_t	psize_t;
typedef vm_offset_t	vaddr_t;
typedef vm_size_t	vsize_t;
#endif

typedef int		register_t;

typedef	__volatile unsigned char __cpu_simple_lock_t;

#define	__SIMPLELOCK_LOCKED	0x80	/* result of `tas' insn */
#define	__SIMPLELOCK_UNLOCKED	0

#define	__HAVE_SYSCALL_INTERN
#define	__HAVE_MD_RUNQUEUE

#if defined(_KERNEL)
#define	__HAVE_RAS
#endif

#endif	/* !_M68K_TYPES_H_ */
