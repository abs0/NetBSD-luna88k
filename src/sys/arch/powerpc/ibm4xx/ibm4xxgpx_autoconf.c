/*	$NetBSD: ibm4xxgpx_autoconf.c,v 1.3 2005/01/13 17:14:36 shige Exp $	*/

/*
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ibm4xxgpx_autoconf.c,v 1.3 2005/01/13 17:14:36 shige Exp $");

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/device.h>
#include <sys/systm.h>

#include <dev/ic/comreg.h>	/* For COM_FREQ */

#include <powerpc/ibm4xx/dcr405gp.h>
#include <powerpc/ibm4xx/dev/plbvar.h>

/*
 * List of port-specific devices to attach to the processor local bus.
 */
static const struct plb_dev local_plb_devs [] = {
	{ "pbus", },
	{ NULL }
};

/*
 * Determine device configuration for a machine.
 */
void
cpu_configure(void)
{

	intr_init();
	calc_delayconst();

	/* Make sure that timers run at CPU frequency */
	mtdcr(DCR_CPC0_CR1, mfdcr(DCR_CPC0_CR1) & ~CPC0_CR1_CETE);

	if (config_rootfound("plb", &local_plb_devs) == NULL)
		panic("configure: plb not configured");

	printf("biomask %x netmask %x ttymask %x\n", (u_short)imask[IPL_BIO],
	    (u_short)imask[IPL_NET], (u_short)imask[IPL_TTY]);
	
	(void)spl0();

	/*
	 * Now allow hardware interrupts.
	 */
	asm volatile ("wrteei 1");
}
