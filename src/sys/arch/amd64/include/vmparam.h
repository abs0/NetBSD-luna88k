/*	$NetBSD: vmparam.h,v 1.7.4.1 2005/09/18 20:09:51 tron Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	@(#)vmparam.h	5.9 (Berkeley) 5/12/91
 */

#ifndef _VMPARAM_H_
#define _VMPARAM_H_

#include <sys/tree.h>

/*
 * Machine dependent constants for 386.
 */

/*
 * Page size on the amd64 s not variable in the traditional sense.
 * We override the PAGE_* definitions to compile-time constants.
 */
#define	PAGE_SHIFT	12
#define	PAGE_SIZE	(1 << PAGE_SHIFT)
#define	PAGE_MASK	(PAGE_SIZE - 1)

/*
 * USRSTACK is the top (end) of the user stack. Immediately above the
 * user stack resides the user structure, which is UPAGES long and contains
 * the kernel stack.
 *
 * Immediately after the user structure is the page table map, and then
 * kernal address space.
 */
#define	USRSTACK	VM_MAXUSER_ADDRESS

#define USRSTACK32	VM_MAXUSER_ADDRESS32

/*
 * Virtual memory related constants, all in bytes
 */
#define	MAXTSIZ		(64*1024*1024)		/* max text size */
#ifndef DFLDSIZ
#define	DFLDSIZ		(256*1024*1024)		/* initial data size limit */
#endif
#ifndef MAXDSIZ
#define	MAXDSIZ		(8L*1024*1024*1024)	/* max data size */
#endif
#ifndef	DFLSSIZ
#define	DFLSSIZ		(2*1024*1024)		/* initial stack size limit */
#endif
#ifndef	MAXSSIZ
#define	MAXSSIZ		(32*1024*1024)		/* max stack size */
#endif

/*
 * 32bit memory related constants.
 */

#define MAXTSIZ32	(64*1024*1024)
#ifndef DFLDSIZ32
#define	DFLDSIZ32	(256*1024*1024)		/* initial data size limit */
#endif
#ifndef MAXDSIZ32
#define	MAXDSIZ32	(2L*1024*1024*1024)	/* max data size */
#endif
#ifndef	DFLSSIZ32
#define	DFLSSIZ32	(2*1024*1024)		/* initial stack size limit */
#endif
#ifndef	MAXSSIZ32
#define	MAXSSIZ32	(32*1024*1024)		/* max stack size */
#endif

/*
 * Size of shared memory map
 */
#ifndef SHMMAXPGS
#define SHMMAXPGS	2048
#endif

/*
 * Size of User Raw I/O map
 */
#define	USRIOSIZE 	300

/*
 * Mach derived constants
 */

/* user/kernel map constants */
#define VM_MIN_ADDRESS		0
#define VM_MAXUSER_ADDRESS	0x00007f8000000000
#define VM_MAX_ADDRESS		0x00007fbfdfeff000
#define VM_MIN_KERNEL_ADDRESS	0xffff800000000000
#define VM_MAX_KERNEL_ADDRESS	0xffff800100000000

#define VM_MAXUSER_ADDRESS32	0xfffff000

/*
 * XXXfvdl we have plenty of KVM now, remove this.
 */
#ifndef VM_MAX_KERNEL_BUF
#define VM_MAX_KERNEL_BUF	(384 * 1024 * 1024)
#endif

/* virtual sizes (bytes) for various kernel submaps */
#define VM_PHYS_SIZE		(USRIOSIZE*PAGE_SIZE)

#define VM_PHYSSEG_MAX		5	/* 1 "hole" + 4 free lists */
#define VM_PHYSSEG_STRAT	VM_PSTRAT_BIGFIRST
#define VM_PHYSSEG_NOADD		/* can't add RAM after vm_mem_init */

#define	VM_NFREELIST		2
#define	VM_FREELIST_DEFAULT	0
#define	VM_FREELIST_FIRST16	1

#define __HAVE_PMAP_PHYSSEG

#define __HAVE_VM_PAGE_MD
#define VM_MDPAGE_INIT(pg)                                      \
        memset(&(pg)->mdpage, 0, sizeof((pg)->mdpage));         \
        simple_lock_init(&(pg)->mdpage.mp_pvhead.pvh_lock);     \
        SPLAY_INIT(&(pg)->mdpage.mp_pvhead.pvh_root);

struct pv_entry;

struct pv_head {
        struct simplelock pvh_lock;     /* locks every pv in this tree */
        SPLAY_HEAD(pvtree, pv_entry) pvh_root;
                                        /* head of tree (locked by pvh_lock) */
};

struct vm_page_md {
        struct pv_head mp_pvhead;
        int mp_attrs;
};

/*
 * pmap specific data stored in the vm_physmem[] array
 */
struct pmap_physseg {
	struct pv_head *pvhead;		/* pv_head array */
	unsigned char *attrs;		/* attrs array */
};

#endif /* _VMPARAM_H_ */
