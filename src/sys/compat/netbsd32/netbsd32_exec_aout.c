/*	$NetBSD: netbsd32_exec_aout.c,v 1.20 2005/02/26 23:10:21 perry Exp $	*/
/*	from: NetBSD: exec_aout.c,v 1.15 1996/09/26 23:34:46 cgd Exp */

/*
 * Copyright (c) 1998, 2001 Matthew R. Green.
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1993, 1994 Christopher G. Demetriou
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christopher G. Demetriou.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: netbsd32_exec_aout.c,v 1.20 2005/02/26 23:10:21 perry Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/vnode.h>
#include <sys/exec.h>
#include <sys/exec_aout.h>
#include <sys/resourcevar.h>
#include <sys/signal.h>
#include <sys/signalvar.h>

#include <compat/netbsd32/netbsd32.h>
#ifndef EXEC_AOUT
#define EXEC_AOUT
#endif
#include <compat/netbsd32/netbsd32_exec.h>

#include <machine/frame.h>
#include <machine/netbsd32_machdep.h>

int netbsd32_copyinargs __P((struct exec_package *, struct ps_strings *,
			     void *, size_t, const void *, const void *));

/*
 * exec_netbsd32_makecmds(): Check if it's an netbsd32 a.out format
 * executable.
 *
 * Given a proc pointer and an exec package pointer, see if the referent
 * of the epp is in netbsd32 a.out format.  Check 'standard' magic
 * numbers for this architecture.
 *
 * This function, in the former case, or the hook, in the latter, is
 * responsible for creating a set of vmcmds which can be used to build
 * the process's vm space and inserting them into the exec package.
 */

int
exec_netbsd32_makecmds(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	netbsd32_u_long midmag, magic;
	u_short mid;
	int error;
	struct netbsd32_exec *execp = epp->ep_hdr;

	if (epp->ep_hdrvalid < sizeof(struct netbsd32_exec))
		return ENOEXEC;

	midmag = (netbsd32_u_long)ntohl(execp->a_midmag);
	mid = (midmag >> 16) & 0x3ff;
	magic = midmag & 0xffff;

	midmag = mid << 16 | magic;

	/* this is already needed by setup_stack() */
	epp->ep_flags |= EXEC_32;

	switch (midmag) {
	case (NETBSD32_MID_MACHINE << 16) | ZMAGIC:
		error = netbsd32_exec_aout_prep_zmagic(p, epp);
		break;
	case (NETBSD32_MID_MACHINE << 16) | NMAGIC:
		error = netbsd32_exec_aout_prep_nmagic(p, epp);
		break;
	case (NETBSD32_MID_MACHINE << 16) | OMAGIC:
		error = netbsd32_exec_aout_prep_omagic(p, epp);
		break;
	default:
		/* Invalid magic */
		error = ENOEXEC;
		break;
	}

	if (error) {
		kill_vmcmds(&epp->ep_vmcmds);
		epp->ep_flags &= ~EXEC_32;
	}
	return error;
}

/*
 * netbsd32_exec_aout_prep_zmagic(): Prepare a 'native' ZMAGIC binary's
 * exec package
 *
 * First, set of the various offsets/lengths in the exec package.
 *
 * Then, mark the text image busy (so it can be demand paged) or error
 * out if this is not possible.  Finally, set up vmcmds for the
 * text, data, bss, and stack segments.
 */

int
netbsd32_exec_aout_prep_zmagic(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	struct netbsd32_exec *execp = epp->ep_hdr;
	int error;

	epp->ep_taddr = AOUT_LDPGSZ;
	epp->ep_tsize = execp->a_text;
	epp->ep_daddr = epp->ep_taddr + execp->a_text;
	epp->ep_dsize = execp->a_data + execp->a_bss;
	epp->ep_entry = execp->a_entry;
	epp->ep_vm_minaddr = VM_MIN_ADDRESS;
	epp->ep_vm_maxaddr = VM_MAXUSER_ADDRESS32;

	error = vn_marktext(epp->ep_vp);
	if (error)
		return (error);

	/* set up command for text segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_pagedvn, execp->a_text,
	    epp->ep_taddr, epp->ep_vp, 0, VM_PROT_READ|VM_PROT_EXECUTE);

	/* set up command for data segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_pagedvn, execp->a_data,
	    epp->ep_daddr, epp->ep_vp, execp->a_text,
	    VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	/* set up command for bss segment */
	if (execp->a_bss > 0)
		NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_zero, execp->a_bss,
		    epp->ep_daddr + execp->a_data, NULLVP, 0,
		    VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	return (*epp->ep_esch->es_setup_stack)(p, epp);
}

/*
 * netbsd32_exec_aout_prep_nmagic(): Prepare a 'native' NMAGIC binary's
 * exec package
 */

int
netbsd32_exec_aout_prep_nmagic(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	struct netbsd32_exec *execp = epp->ep_hdr;
	long bsize, baddr;

	epp->ep_taddr = AOUT_LDPGSZ;
	epp->ep_tsize = execp->a_text;
	epp->ep_daddr = roundup(epp->ep_taddr + execp->a_text, AOUT_LDPGSZ);
	epp->ep_dsize = execp->a_data + execp->a_bss;
	epp->ep_entry = execp->a_entry;
	epp->ep_vm_minaddr = VM_MIN_ADDRESS;
	epp->ep_vm_maxaddr = VM_MAXUSER_ADDRESS32;

	/* set up command for text segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_readvn, execp->a_text,
	    epp->ep_taddr, epp->ep_vp, sizeof(struct netbsd32_exec),
	    VM_PROT_READ|VM_PROT_EXECUTE);

	/* set up command for data segment */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_readvn, execp->a_data,
	    epp->ep_daddr, epp->ep_vp, execp->a_text + sizeof(struct netbsd32_exec),
	    VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	/* set up command for bss segment */
	baddr = roundup(epp->ep_daddr + execp->a_data, PAGE_SIZE);
	bsize = epp->ep_daddr + epp->ep_dsize - baddr;
	if (bsize > 0)
		NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_zero, bsize, baddr,
		    NULLVP, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	return (*epp->ep_esch->es_setup_stack)(p, epp);
}

/*
 * netbsd32_exec_aout_prep_omagic(): Prepare a 'native' OMAGIC binary's
 * exec package
 */

int
netbsd32_exec_aout_prep_omagic(p, epp)
	struct proc *p;
	struct exec_package *epp;
{
	struct netbsd32_exec *execp = epp->ep_hdr;
	long dsize, bsize, baddr;

	epp->ep_taddr = AOUT_LDPGSZ;
	epp->ep_tsize = execp->a_text;
	epp->ep_daddr = epp->ep_taddr + execp->a_text;
	epp->ep_dsize = execp->a_data + execp->a_bss;
	epp->ep_entry = execp->a_entry;
	epp->ep_vm_minaddr = VM_MIN_ADDRESS;
	epp->ep_vm_maxaddr = VM_MAXUSER_ADDRESS32;

	/* set up command for text and data segments */
	NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_readvn,
	    execp->a_text + execp->a_data, epp->ep_taddr, epp->ep_vp,
	    sizeof(struct netbsd32_exec), VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	/* set up command for bss segment */
	baddr = roundup(epp->ep_daddr + execp->a_data, PAGE_SIZE);
	bsize = epp->ep_daddr + epp->ep_dsize - baddr;
	if (bsize > 0)
		NEW_VMCMD(&epp->ep_vmcmds, vmcmd_map_zero, bsize, baddr,
		    NULLVP, 0, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE);

	/*
	 * Make sure (# of pages) mapped above equals (vm_tsize + vm_dsize);
	 * obreak(2) relies on this fact. Both `vm_tsize' and `vm_dsize' are
	 * computed (in execve(2)) by rounding *up* `ep_tsize' and `ep_dsize'
	 * respectively to page boundaries.
	 * Compensate `ep_dsize' for the amount of data covered by the last
	 * text page.
	 */
	dsize = epp->ep_dsize + execp->a_text - roundup(execp->a_text,
							PAGE_SIZE);
	epp->ep_dsize = (dsize > 0) ? dsize : 0;
	return (*epp->ep_esch->es_setup_stack)(p, epp);
}