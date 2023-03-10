/*	$OpenBSD: cpu.h,v 1.15 2006/05/08 14:36:09 miod Exp $ */
/*
 * Copyright (c) 1996 Nivas Madhur
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
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
 */

#ifndef __M88K_CPU_H__
#define __M88K_CPU_H__

#if defined(_KERNEL_OPT)
#include "opt_multiprocessor.h"
#include "opt_lockdebug.h"
#endif

/*
 * CTL_MACHDEP definitinos.
 */
#define	CPU_CONSDEV	1	/* dev_t: console terminal device */
#define	CPU_MAXID	2	/* number of valid machdep ids */

#define	CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
}

#ifdef _KERNEL

#include <machine/frame.h>
#include <machine/pcb.h>
#include <machine/psl.h>
#include <machine/intr.h>
#include <sys/sched.h>

#if defined(MULTIPROCESSOR)
#if !defined(MAX_CPUS) || MAX_CPUS > 4
#undef	MAX_CPUS
#define	MAX_CPUS	4
#endif
#else
#undef	MAX_CPUS
#define	MAX_CPUS	1
#endif

#ifndef _LOCORE

extern u_int max_cpus;

#include <sys/cpu_data.h>

/*
 * Per-CPU data structure
 */

struct cpu_info {
	struct cpu_data ci_data;		/* MI per-cpu data */

	u_int	ci_alive;			/* nonzero if CPU present */

	struct lwp *ci_curlwp;			/* current thread... */
	struct pcb *ci_curpcb;			/* ...and its pcb */

	u_int	ci_cpuid;			/* cpu number */
	u_int	ci_primary;			/* set if master cpu */
	u_int	ci_pfsr_i0, ci_pfsr_i1;		/* instruction... */
	u_int	ci_pfsr_d0, ci_pfsr_d1;		/* ... and data CMMU PFSRs */

	int	ci_want_resched;		/* need_resched() invoked */

	struct pcb *ci_idle_pcb;		/* idle pcb (and stack) */

	u_int	ci_intrdepth;			/* interrupt depth */

	volatile int ci_ddb_state;		/* ddb status */
#define	CI_DDB_RUNNING	0
#define	CI_DDB_ENTERDDB	1
#define	CI_DDB_INDDB	2
};

extern cpuid_t master_cpu;
extern struct cpu_info m88k_cpus[MAX_CPUS];

#define	CPU_INFO_ITERATOR	cpuid_t
#define	CPU_INFO_FOREACH(cii, ci) \
	for ((cii) = 0; (cii) < MAX_CPUS; (cii)++) \
		if (((ci) = &m88k_cpus[cii])->ci_alive != 0)
#define	CPU_INFO_UNIT(ci)	((ci)->ci_cpuid)

#if defined(MULTIPROCESSOR)

#define	curcpu() \
({									\
	struct cpu_info *cpuptr;					\
									\
	__asm__ __volatile__ ("ldcr %0, cr17" : "=r" (cpuptr));		\
	cpuptr;								\
})

#define	CPU_IS_PRIMARY(ci)	((ci)->ci_primary != 0)

void	cpu_boot_secondary_processors(void);

#else	/* MULTIPROCESSOR */

#define	curcpu()	(&m88k_cpus[0])
#define	CPU_IS_PRIMARY(ci)	1

#endif	/* MULTIPROCESSOR */

void	set_cpu_number(cpuid_t);

/*
 * The md code may hardcode this in some very specific situations.
 */
#if !defined(cpu_number)
#define	cpu_number()		curcpu()->ci_cpuid
#endif

#define	curpcb			curcpu()->ci_curpcb

#endif /* _LOCORE */

/*
 * definitions of cpu-dependent requirements
 * referenced in generic code
 */
#define	cpu_exec(p)		do { /* nothing */ } while (0)
#define	cpu_wait(p)		do { /* nothing */ } while (0)
#define	cpu_swapin(p)		do { /* nothing */ } while (0)
#define	cpu_swapout(p)		do { /* nothing */ } while (0)
#define cpu_proc_fork(p1, p2)	/* nothing */

#if defined(MULTIPROCESSOR)
#include <sys/lock.h>
#include <sys/mplock.h>
#endif

/*
 * Arguments to hardclock and gatherstats encapsulate the previous
 * machine state in an opaque clockframe. CLKF_INTR is only valid
 * if the process is in kernel mode. Clockframe is really trapframe,
 * so pointer to clockframe can be safely cast into a pointer to
 * trapframe.
 */
struct clockframe {
	struct trapframe tf;
};

/* All the CLKF_* macros take a struct clockframe * as an argument. */

/*
 * CLKF_USERMODE: Return TRUE/FALSE (1/0) depending on whether the
 * frame came from USR mode or not.
 */
#define	CLKF_USERMODE(framep)	(((framep)->tf.tf_epsr & PSR_MODE) == 0)

/*
 * CLKF_PC: Extract the program counter from a clockframe
 */
#define	CLKF_PC(framep)		((framep)->tf.tf_sxip & XIP_ADDR)

/*
 * CLKF_INTR: True if we took the interrupt from inside another
 * interrupt handler.
 */
#define	CLKF_INTR(framep) \
	(((struct cpu_info *)(framep)->tf.tf_cpu)->ci_intrdepth > 1)

/*
 * CLKF_BASEPRI: True if we were at spl0 before the interrupt.
 */
#define CLKF_BASEPRI(framep)	((((struct trapframe *)(framep))->tf_epsr & PSR_IND) == 0)

/*
 * LWP_PC: Find out the program counter for the given lwp.
 */
/* XXX - missing LWP_PC - TKM */

/*
 * Get interrupt glue.
 */
#include <machine/intr.h>

#define SIR_NET		1
#define SIR_CLOCK	2

void	softintr_dispatch(void);
#ifdef MULTIPROCESSOR
extern void setsoftint(int);
extern int clrsoftint(int);
#else
extern int ssir;
#define setsoftint(x)	(ssir |= (x))
#define	clrsoftint(x)	\
({									\
	int tmpsir = ssir & (x);					\
	ssir ^= tmpsir;							\
	tmpsir;								\
})
#endif	/* MULTIPROCESSOR */

#define setsoftnet()	setsoftint(SIR_NET)
#define setsoftclock()	setsoftint(SIR_CLOCK)

#define	aston(p)	((p)->p_md.md_astpending = 1)

/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */
#define	need_resched(ci) \
do {									\
	ci->ci_want_resched = 1;					\
	if (curlwp != NULL)					\
		aston(curlwp->l_proc);				\
} while (0)

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the m88k, request an ast to send us
 * through trap(), marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	((p)->p_flag |= P_OWEUPC, aston(p))

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */
#define	signotify(p)		aston(p)

int	badaddr(vaddr_t addr, int size);
void	nmihand(void *);

#endif /* _KERNEL */
#endif /* __M88K_CPU_H__ */
