/*	$OpenBSD: m188_machdep.c,v 1.22 2006/05/08 14:36:10 miod Exp $	*/
/*
 * Copyright (c) 1998, 1999, 2000, 2001 Steve Murphree, Jr.
 * Copyright (c) 1996 Nivas Madhur
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
 *      This product includes software developed by Nivas Madhur.
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
 *
 */
/*
 * Copyright (c) 1999 Steve Murphree, Jr.
 * Copyright (c) 1995 Theo de Raadt
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1995 Nivas Madhur
 * Copyright (c) 1994 Gordon W. Ross
 * Copyright (c) 1993 Adam Glass
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
 *
 *	@(#)clock.c	8.1 (Berkeley) 6/11/93
 */
/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/errno.h>

#include <uvm/uvm_extern.h>

#include <machine/asm_macro.h>
#include <machine/cmmu.h>
#include <machine/cpu.h>
#include <machine/reg.h>
#include <machine/trap.h>

#include <machine/m88100.h>
#include <machine/mvme188.h>

#include <mvme88k/dev/sysconreg.h>

#include <mvme88k/mvme88k/clockvar.h>

void	m188_reset(void);
u_int	safe_level(u_int mask, u_int curlevel);

void	m188_bootstrap(void);
void	m188_ext_int(u_int, struct trapframe *);
u_int	m188_getipl(void);
void	m188_init_clocks(void);
vaddr_t	m188_memsize(void);
u_int	m188_raiseipl(u_int);
u_int	m188_setipl(u_int);
void	m188_startup(void);

/*
 * The MVME188 interrupt arbiter has 25 orthogonal interrupt sources.
 * We fold this model in the 8-level spl model this port uses, enforcing
 * priorities manually with the interrupt masks.
 */

/*
 * interrupt status register for each CPU.
 */
unsigned int *volatile int_mask_reg[] = {
	(unsigned int *)MVME188_IEN0,
	(unsigned int *)MVME188_IEN1,
	(unsigned int *)MVME188_IEN2,
	(unsigned int *)MVME188_IEN3
};

unsigned int m188_curspl[] = {0, 0, 0, 0};

/*
 * external interrupt masks per spl.
 */
const unsigned int int_mask_val[INT_LEVEL] = {
	MASK_LVL_0,
	MASK_LVL_1,
	MASK_LVL_2,
	MASK_LVL_3,
	MASK_LVL_4,
	MASK_LVL_5,
	MASK_LVL_6,
	MASK_LVL_7
};

/*
 * Figure out how much memory is available, by querying the MBus registers.
 *
 * For every 4MB segment, ask the MBus address decoder which device claimed
 * the range. Since memory is packed at low addresses, we will hit all memory
 * boards in order until reaching either a VME space or a non-claimed space.
 *
 * As a safety measure, we never check for more than 256MB - the 188 can
 * only have up to 4 memory boards, which theoretically can not be larger
 * than 64MB, and I am not aware of third-party larger memory boards.
 */
vaddr_t
m188_memsize()
{
	unsigned int pgnum;
	int32_t rmad;

#define	MVME188_MAX_MEMORY	((4 * 64) / 4)	/* 4 64MB boards */
	for (pgnum = 0; pgnum <	MVME188_MAX_MEMORY; pgnum++) {
		*(volatile int32_t *)MVME188_RMAD = (pgnum << 22);
		rmad = *(volatile int32_t *)MVME188_RMAD;

		if (rmad & 0x04)	/* not a memory board */
			break;
	}

	return (pgnum << 22);
}

void
m188_startup()
{
}

void
m188_bootstrap()
{
	extern struct cmmu_p cmmu8820x;

	cmmu = &cmmu8820x;
	md_interrupt_func_ptr = m188_ext_int;
	md_getipl = m188_getipl;
	md_setipl = m188_setipl;
	md_raiseipl = m188_raiseipl;
	md_init_clocks = m188_init_clocks;

	/* clear and disable all interrupts */
	*(volatile u_int32_t *)MVME188_IENALL = 0;

	/* supply a vector base for m188ih */
	*(volatile u_int8_t *)MVME188_VIRQV = M188_IVEC;
}

void
m188_reset()
{
	volatile int cnt;

	/* clear and disable all interrupts */
	*(volatile u_int32_t *)MVME188_IENALL = 0;

	if ((*(volatile u_int8_t *)MVME188_GLOBAL1) & M188_SYSCON) {
		/* Force a complete VMEbus reset */
		*(volatile u_int32_t *)MVME188_GLBRES = 1;
	} else {
		/* Force only a local reset */
		*(volatile u_int8_t *)MVME188_GLOBAL1 |= M188_LRST;
	}

	*(volatile u_int32_t *)MVME188_UCSR |= 0x2000;	/* clear SYSFAIL */
	for (cnt = 0; cnt < 5*1024*1024; cnt++)
		;
	*(volatile u_int32_t *)MVME188_UCSR |= 0x2000;	/* clear SYSFAIL */

	printf("reset failed\n");
}

/*
 * return next safe spl to reenable interrupts.
 */
u_int
safe_level(u_int mask, u_int curlevel)
{
	int i;

	for (i = curlevel; i < INT_LEVEL; i++)
		if ((int_mask_val[i] & mask) == 0)
			return (i);

	return (INT_LEVEL - 1);
}

u_int
m188_getipl(void)
{
	return m188_curspl[cpu_number()];
}

u_int
m188_setipl(u_int level)
{
	u_int mask, curspl;
	int cpu = cpu_number();

	curspl = m188_curspl[cpu];

	mask = int_mask_val[level];
#ifdef MULTIPROCESSOR
	if (cpu != master_cpu)
		mask &= ~SLAVE_MASK;
#endif

	*int_mask_reg[cpu] = mask;
	m188_curspl[cpu] = level;

	return curspl;
}

u_int
m188_raiseipl(u_int level)
{
	u_int mask, curspl;
	int cpu = cpu_number();

	curspl = m188_curspl[cpu];
	if (curspl < level) {
		mask = int_mask_val[level];
#ifdef MULTIPROCESSOR
		if (cpu != master_cpu)
			mask &= ~SLAVE_MASK;
#endif

		*int_mask_reg[cpu] = mask;
		m188_curspl[cpu] = level;
	}
	return curspl;
}

/*
 * Device interrupt handler for MVME188
 */

/*
 * Hard coded vector table for onboard devices and hardware failure
 * interrupts.
 */
const unsigned int obio_vec[32] = {
	0,		/* SWI0 */
	0,		/* SWI1 */
	0,		/* SWI2 */
	0,		/* SWI3 */
	0,		/* VME1 */
	0,
	0,		/* VME2 */
	0,		/* SIGLPI */	/* no vector, but always masked */
	0,		/* LMI */	/* no vector, but always masked */
	0,
	0,		/* VME3 */
	0,
	0,		/* VME4 */
	0,
	0,		/* VME5 */
	0,
	0,		/* SIGHPI */	/* no vector, but always masked */
	SYSCV_SCC,	/* DI */
	0,
	0,		/* VME6 */
	SYSCV_SYSF,	/* SF */
	SYSCV_TIMER2,	/* CIOI */
	0,
	0,		/* VME7 */
	0,		/* SWI4 */
	0,		/* SWI5 */
	0,		/* SWI6 */
	0,		/* SWI7 */
	SYSCV_TIMER1,	/* DTI */
	0,		/* ARBTO */	/* no vector, but always masked */
	SYSCV_ACF,	/* ACF */
	SYSCV_ABRT	/* ABORT */
};

#define VME_VECTOR_MASK		0x1ff 	/* mask into VIACK register */
#define VME_BERR_MASK		0x100 	/* timeout during VME IACK cycle */

void
m188_ext_int(u_int v, struct trapframe *eframe)
{
	int cpu = cpu_number();
	unsigned int cur_mask, ign_mask;
	unsigned int level, old_spl;
	struct intrhand *intr;
	intrhand_t *list;
	int ret, intbit;
	vaddr_t ivec;
	u_int vec;
	int unmasked = 0;
#ifdef DIAGNOSTIC
	static int problems = 0;
#endif

	cur_mask = ISR_GET_CURRENT_MASK(cpu);
	ign_mask = 0;
	old_spl = m188_curspl[cpu];
	eframe->tf_mask = old_spl;

	if (cur_mask == 0) {
		/*
		 * Spurious interrupts - may be caused by debug output clearing
		 * DUART interrupts.
		 */
		flush_pipeline();
		goto out;
	}

	uvmexp.intrs++;

	/*
	 * We want to service all interrupts marked in the IST register
	 * They are all valid because the mask would have prevented them
	 * from being generated otherwise.  We will service them in order of
	 * priority.
	 */
	do {
		level = safe_level(cur_mask, old_spl);

#ifdef DIAGNOSTIC
		if (level != IPL_ABORT && level <= old_spl) {
			int i;

			printf("safe level %d <= old level %d\n", level, old_spl);
			printf("cur_mask = 0x%b\n", cur_mask, IST_STRING);
			for (i = 0; i < 4; i++)
				printf("IEN%d = 0x%b  ", i, *int_mask_reg[i], IST_STRING);
			printf("\nCPU0 spl %d  CPU1 spl %d  CPU2 spl %d  CPU3 spl %d\n",
			       m188_curspl[0], m188_curspl[1],
			       m188_curspl[2], m188_curspl[3]);
			for (i = 0; i < INT_LEVEL; i++)
				printf("int_mask[%d] = 0x%08x\n", i, int_mask_val[i]);
			printf("--CPU %d halted--\n", cpu_number());
			setipl(IPL_ABORT);
			for(;;) ;
		}
#endif

		setipl(level);

		/*
		 * Do not enable interrupts yet if we know, from cur_mask,
		 * that we have not cleared enough conditions yet.
		 * For now, only the timer interrupt requires its condition
		 * to be cleared before interrupts are enabled.
		 */
		if (unmasked == 0 && (cur_mask & IRQ_DTI) == 0) {
			set_psr(get_psr() & ~PSR_IND);
			unmasked = 1;
		}

		/* generate IACK and get the vector */

		/*
		 * This is tricky.  If you don't catch all the
		 * interrupts, you die. Game over. Insert coin...
		 * XXX smurph
		 */

		/* find the first bit set in the current mask */
		intbit = ff1(cur_mask);
		if (OBIO_INTERRUPT_MASK & (1 << intbit)) {
			vec = obio_vec[intbit];
			if (vec == 0) {
				panic("unknown onboard interrupt: mask = 0x%b",
				    1 << intbit, IST_STRING);
			}
			vec += SYSCON_VECT;
		} else if (HW_FAILURE_MASK & (1 << intbit)) {
			vec = obio_vec[intbit];
			if (vec == 0) {
				panic("unknown hardware failure: mask = 0x%b",
				    1 << intbit, IST_STRING);
			}
			vec += SYSCON_VECT;
		} else if (VME_INTERRUPT_MASK & (1 << intbit)) {
			ivec = MVME188_VIRQLV + (level << 2);
			vec = *(volatile u_int32_t *)ivec & VME_VECTOR_MASK;
			if (vec & VME_BERR_MASK) {
				/*
				 * This could be a self-inflicted interrupt.
				 * Except that we never write to VIRQV, so
				 * such things do not happen.

				u_int src = 0x07 &
				    *(volatile u_int32_t *)MVME188_VIRQLV;
				if (src == 0)
					vec = 0xff &
					    *(volatile u_int32_t *)MVME188_VIRQV;
				else

				 */
				{
					printf("%s: timeout getting VME "
					    "interrupt vector, "
					    "level %d, mask 0x%b\n",
					    __func__, level,
					   cur_mask, IST_STRING); 
					ign_mask |=  1 << intbit;
					continue;
				}
			}
			if (vec == 0) {
				panic("%s: invalid VME interrupt vector, "
				    "level %d, mask 0x%b",
				    __func__, level, cur_mask, IST_STRING);
			}
		} else {
			panic("%s: unexpected interrupt source, "
			    "level %d, mask 0x%b",
			    __func__, level, cur_mask, IST_STRING);
		}

		list = &intr_handlers[vec];
		if (SLIST_EMPTY(list)) {
			printf("%s: spurious interrupt, "
			    "level %d, vec 0x%x, mask 0x%b\n",
			    __func__, level, vec, cur_mask, IST_STRING);
			ign_mask |=  1 << intbit;
		} else {
			/*
			 * Walk through all interrupt handlers in the chain
			 * for the given vector, calling each handler in turn,
			 * till some handler returns a value != 0.
			 */
			ret = 0;
			SLIST_FOREACH(intr, list, ih_link) {
				if (intr->ih_wantframe != 0)
					ret = (*intr->ih_fn)((void *)eframe);
				else
					ret = (*intr->ih_fn)(intr->ih_arg);
				if (ret != 0) {
					intr->ih_count.ec_count++;
					break;
				}
			}
			if (ret == 0) {
				printf("%s: unclaimed interrupt, "
				    "level %d, vec 0x%x, mask 0x%b\n",
				    __func__, level, vec, cur_mask, IST_STRING);
				ign_mask |=  1 << intbit;
				continue;
			}
		}
	} while (((cur_mask = ISR_GET_CURRENT_MASK(cpu)) & ~ign_mask) != 0);

#ifdef DIAGNOSTIC
	if (ign_mask != 0) {
		if (++problems >= 10)
			panic("%s: broken interrupt behaviour", __func__);
	} else
		problems = 0;
#endif

	/*
	 * process any remaining data access exceptions before
	 * returning to assembler
	 */
	set_psr(get_psr() | PSR_IND);
out:
	if (eframe->tf_dmt0 & DMT_VALID)
		m88100_trap(T_DATAFLT, eframe);

	/*
	 * Restore the mask level to what it was when the interrupt
	 * was taken.
	 */
	m188_setipl(eframe->tf_mask);
}

/*
 * Clock routines
 */

void	m188_cio_init(unsigned);
u_int	read_cio(int);
void	write_cio(int, u_int);

int	m188_clockintr(void *);
int	m188_statintr(void *);

struct simplelock m188_cio_lock;

#define	CIO_LOCK	simple_lock(&m188_cio_lock)
#define	CIO_UNLOCK	simple_unlock(&m188_cio_lock)

/*
 * Notes on the MVME188 clock usage:
 *
 * We have two sources for timers:
 * - two counter/timers in the DUART (MC68681/MC68692)
 * - three counter/timers in the Zilog Z8536
 *
 * However:
 * - Z8536 CT#3 is reserved as a watchdog device; and its input is
 *   user-controllable with jumpers on the SYSCON board, so we can't
 *   really use it.
 * - When using the Z8536 in timer mode, it _seems_ like it resets at
 *   0xffff instead of the initial count value...
 * - Despite having per-counter programmable interrupt vectors, the
 *   SYSCON logic forces fixed vectors for the DUART and the Z8536 timer
 *   interrupts.
 * - The DUART timers keep counting down from 0xffff even after
 *   interrupting, and need to be manually stopped, then restarted, to
 *   resume counting down the initial count value.
 *
 * Also, while the Z8536 has a very reliable 4MHz clock source, the
 * 3.6864MHz clock source of the DUART timers does not seem to be correct.
 *
 * As a result, clock is run on a Z8536 counter, kept in counter mode and
 * retriggered every interrupt, while statclock is run on a DUART counter,
 * but in practice runs at an average 96Hz instead of the expected 100Hz.
 *
 * It should be possible to run statclock on the Z8536 counter #2, but
 * this would make interrupt handling more tricky, in the case both
 * counters interrupt at the same time...
 */

#define	DART_ISR		0xfff82017	/* interrupt status */
#define	DART_IVR		0xfff82033	/* interrupt vector */
#define	DART_STARTC		0xfff8203b	/* start counter cmd */
#define	DART_STOPC		0xfff8203f	/* stop counter cmd */
#define	DART_ACR		0xfff82013	/* auxiliary control */
#define	DART_CTUR		0xfff8201b	/* counter/timer MSB */
#define	DART_CTLR		0xfff8201f	/* counter/timer LSB */
#define	DART_OPCR		0xfff82037	/* output port config*/

void
m188_init_clocks(void)
{
	volatile u_int8_t imr;
	int statint, minint;

	simple_lock_init(&m188_cio_lock);

#ifdef DIAGNOSTIC
	if (1000000 % hz) {
		printf("cannot get %d Hz clock; using 100 Hz\n", hz);
		hz = 100;
	}
#endif
	tick = 1000000 / hz;

	m188_cio_init(tick);

	if (stathz == 0)
		stathz = hz;
#ifdef DIAGNOSTIC
	if (1000000 % stathz) {
		printf("cannot get %d Hz statclock; using 100 Hz\n", stathz);
		stathz = 100;
	}
#endif
	profhz = stathz;		/* always */

	/*
	 * The DUART runs at 3.6864 MHz, CT#1 will run in PCLK/16 mode.
	 */
	statint = (3686400 / 16) / stathz;
	minint = statint / 2 + 100;
	while (statvar > minint)
		statvar >>= 1;
	statmin = statint - (statvar >> 1);

	/* clear the counter/timer output OP3 while we program the DART */
	*(volatile u_int8_t *)DART_OPCR = 0x00;
	/* set interrupt vec */
	*(volatile u_int8_t *)DART_IVR = SYSCON_VECT + SYSCV_TIMER1;
	/* do the stop counter/timer command */
	imr = *(volatile u_int8_t *)DART_STOPC;
	/* set counter/timer to counter mode, PCLK/16 */
	*(volatile u_int8_t *)DART_ACR = 0x30;
	*(volatile u_int8_t *)DART_CTUR = (statint >> 8);
	*(volatile u_int8_t *)DART_CTLR = (statint & 0xff);
	/* set the counter/timer output OP3 */
	*(volatile u_int8_t *)DART_OPCR = 0x04;
	/* give the start counter/timer command */
	imr = *(volatile u_int8_t *)DART_STARTC;

	clock_ih.ih_fn = m188_clockintr;
	clock_ih.ih_arg = 0;
	clock_ih.ih_wantframe = 1;
	clock_ih.ih_ipl = IPL_CLOCK;
	sysconintr_establish(SYSCV_TIMER2, &clock_ih, "clock");

	statclock_ih.ih_fn = m188_statintr;
	statclock_ih.ih_arg = 0;
	statclock_ih.ih_wantframe = 1;
	statclock_ih.ih_ipl = IPL_CLOCK;
	sysconintr_establish(SYSCV_TIMER1, &statclock_ih, "stat");
}

int
m188_clockintr(void *eframe)
{
	CIO_LOCK;
	write_cio(CIO_CSR1, CIO_GCB | CIO_CIP);  /* Ack the interrupt */

	hardclock(eframe);

	/* restart counter */
	write_cio(CIO_CSR1, CIO_GCB | CIO_TCB | CIO_IE);
	CIO_UNLOCK;

	return (1);
}

int
m188_statintr(void *eframe)
{
	volatile u_int8_t tmp;
	u_long newint, r, var;

	/* stop counter and acknowledge interrupt */
	tmp = *(volatile u_int8_t *)DART_STOPC;
	tmp = *(volatile u_int8_t *)DART_ISR;

	statclock((struct clockframe *)eframe);

	/*
	 * Compute new randomized interval.  The intervals are uniformly
	 * distributed on [statint - statvar / 2, statint + statvar / 2],
	 * and therefore have mean statint, giving a stathz frequency clock.
	 */
	var = statvar;
	do {
		r = random() & (var - 1);
	} while (r == 0);
	newint = statmin + r;

	/* setup new value and restart counter */
	*(volatile u_int8_t *)DART_CTUR = (newint >> 8);
	*(volatile u_int8_t *)DART_CTLR = (newint & 0xff);
	tmp = *(volatile u_int8_t *)DART_STARTC;

	return (1);
}

/* Write CIO register */
void
write_cio(int reg, u_int val)
{
	int s;
	volatile int i;
	volatile u_int32_t * cio_ctrl = (volatile u_int32_t *)CIO_CTRL;

	s = splclock();
	CIO_LOCK;

	i = *cio_ctrl;				/* goto state 1 */
	*cio_ctrl = 0;				/* take CIO out of RESET */
	i = *cio_ctrl;				/* reset CIO state machine */

	*cio_ctrl = (reg & 0xff);		/* select register */
	*cio_ctrl = (val & 0xff);		/* write the value */

	CIO_UNLOCK;
	splx(s);
}

/* Read CIO register */
u_int
read_cio(int reg)
{
	int c, s;
	volatile int i;
	volatile u_int32_t * cio_ctrl = (volatile u_int32_t *)CIO_CTRL;

	s = splclock();
	CIO_LOCK;

	/* select register */
	*cio_ctrl = (reg & 0xff);
	/* delay for a short time to allow 8536 to settle */
	for (i = 0; i < 100; i++)
		;
	/* read the value */
	c = *cio_ctrl;
	CIO_UNLOCK;
	splx(s);
	return (c & 0xff);
}

/*
 * Initialize the CTC (8536)
 * Only the counter/timers are used - the IO ports are un-comitted.
 */
void
m188_cio_init(unsigned period)
{
	volatile int i;

	CIO_LOCK;

	/* Start by forcing chip into known state */
	read_cio(CIO_MICR);
	write_cio(CIO_MICR, CIO_MICR_RESET);	/* Reset the CTC */
	for (i = 0; i < 1000; i++)	 	/* Loop to delay */
		;

	/* Clear reset and start init seq. */
	write_cio(CIO_MICR, 0x00);

	/* Wait for chip to come ready */
	while ((read_cio(CIO_MICR) & CIO_MICR_RJA) == 0)
		;

	/* Initialize the 8536 for real */
	write_cio(CIO_MICR,
	    CIO_MICR_MIE /* | CIO_MICR_NV */ | CIO_MICR_RJA | CIO_MICR_DLC);
	write_cio(CIO_CTMS1, CIO_CTMS_CSC);	/* Continuous count */
	write_cio(CIO_PDCB, 0xff);		/* set port B to input */

	period <<= 1;	/* CT#1 runs at PCLK/2, hence 2MHz */
	write_cio(CIO_CT1MSB, period >> 8);
	write_cio(CIO_CT1LSB, period);
	/* enable counter #1 */
	write_cio(CIO_MCCR, CIO_MCCR_CT1E | CIO_MCCR_PBE);
	write_cio(CIO_CSR1, CIO_GCB | CIO_TCB | CIO_IE);

	CIO_UNLOCK;
}
