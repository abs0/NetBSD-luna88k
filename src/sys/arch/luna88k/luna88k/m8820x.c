/*	$OpenBSD: m8820x.c,v 1.13 2006/10/30 14:32:27 aoyama Exp $	*/
/*
 * Copyright (c) 2004, Miodrag Vallat.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 2001 Steve Murphree, Jr.
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

#include <sys/param.h>
#include <sys/systm.h>

#include <uvm/uvm_extern.h>

#include <machine/asm_macro.h>
#include <machine/board.h>
#include <machine/cmmu.h>
#include <machine/cpu.h>
#include <machine/m8820x.h>

/*
 * This routine sets up the CPU/CMMU configuration.
 */
void
m8820x_setup_board_config()
{
	struct m8820x_cmmu *cmmu;
	u_int num;

	m8820x_cmmu[0].cmmu_regs = (void *)CMMU_I0;
	m8820x_cmmu[1].cmmu_regs = (void *)CMMU_D0;
	m8820x_cmmu[2].cmmu_regs = (void *)CMMU_I1;
	m8820x_cmmu[3].cmmu_regs = (void *)CMMU_D1;
	m8820x_cmmu[4].cmmu_regs = (void *)CMMU_I2;
	m8820x_cmmu[5].cmmu_regs = (void *)CMMU_D2;
	m8820x_cmmu[6].cmmu_regs = (void *)CMMU_I3;
	m8820x_cmmu[7].cmmu_regs = (void *)CMMU_D3;

	/*
	 * Probe CMMU address to discover which CPU slots are populated.
	 * On the luna88k, badaddr() returns 'good' on unpopulated slots,
	 * so we check the CMMU type value for each CMMU register address.
	 */
	cmmu = m8820x_cmmu;

	for (num = 0; num < 8; num++) {
		volatile unsigned *cr = m8820x_cmmu[num].cmmu_regs;
		int type;

		type = CMMU_TYPE(cr[CMMU_IDR]);
		if (type != M88200_ID && type != M88204_ID)
			break;
	}

	max_cpus = num >> 1;
	max_cmmus = max_cpus << 1;
	cmmu_shift = 1;	/* fixed 2:1 configuration */

	/*
	 * Now that we know which CMMUs are there, report every association
	 */
	for (num = 0; num < max_cpus; num++) {
		volatile unsigned *cr;
		int type;

 		cr = m8820x_cmmu[num << cmmu_shift].cmmu_regs;

		m88k_cpus[num].ci_alive = 1;	/* This cpu installed... */

		type = CMMU_TYPE(cr[CMMU_IDR]);
		printf("CPU%d is associated to %d MC8820%c CMMUs\n",
		    num, 1 << cmmu_shift, type == M88204_ID ? '4' : '0');
	}
}

/*
 * Find out the CPU number from accessing CMMU
 * Better be at splhigh, or even better, with interrupts
 * disabled.
 */
#define ILLADDRESS	0x3ffffff0 	/* any faulty address for luna88k2 */

cpuid_t
m8820x_cpu_number()
{
	u_int cmmu;
	u_int i;

	CMMU_LOCK;

	for (i = 0; i < 10; i++) {
		/* clear CMMU P-bus status registers */
		for (cmmu = 0; cmmu < max_cmmus; cmmu++) {
			if (CMMU_MODE(cmmu) != INST_CMMU)
				m8820x_cmmu[cmmu].cmmu_regs[CMMU_PFSR] = 0;
		}

		/* access faulting address */
		badaddr((vaddr_t)ILLADDRESS, 4);

		/* check which CMMU is reporting the fault  */
		for (cmmu = 0; cmmu < max_cmmus; cmmu++) {
			if (CMMU_MODE(cmmu) != INST_CMMU &&
			    CMMU_PFSR_FAULT(m8820x_cmmu[cmmu].
			      cmmu_regs[CMMU_PFSR]) != CMMU_PFSR_SUCCESS) {
				/* clean register, just in case... */
				m8820x_cmmu[cmmu].cmmu_regs[CMMU_PFSR] = 0;
				CMMU_UNLOCK;
				return cmmu >> 1;
			}
		}
	}
	CMMU_UNLOCK;

	panic("m8820x_cpu_number: could not determine my cpu number");
}
