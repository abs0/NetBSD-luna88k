/*	$NetBSD: sfb.c,v 1.42 2003/08/07 16:29:10 agc Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell and Rick Macklem.
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
 *	from: @(#)sfb.c	8.1 (Berkeley) 6/10/93
 */

/*
 *  devGraphics.c --
 *
 *     	This file contains machine-dependent routines for the graphics device.
 *
 *	Copyright (C) 1989 Digital Equipment Corporation.
 *	Permission to use, copy, modify, and distribute this software and
 *	its documentation for any purpose and without fee is hereby granted,
 *	provided that the above copyright notice appears in all copies.
 *	Digital Equipment Corporation makes no representations about the
 *	suitability of this software for any purpose.  It is provided "as is"
 *	without express or implied warranty.
 *
 * from: Header: /sprite/src/kernel/dev/ds3100.md/RCS/devGraphics.c,
 *	v 9.2 90/02/13 22:16:24 shirriff Exp  SPRITE (DECWRL)";
 */
/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
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
__KERNEL_RCSID(0, "$NetBSD: sfb.c,v 1.42 2003/08/07 16:29:10 agc Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <dev/tc/tcvar.h>

#include <machine/autoconf.h>
#include <dev/sun/fbio.h>
#include <machine/fbvar.h>
#include <machine/pmioctl.h>

#include <pmax/dev/bt459.h>
#include <pmax/dev/fbreg.h>
#include <pmax/dev/sfbvar.h>		/* XXX dev/tc ? */
#include <pmax/dev/sfbreg.h>


/*  turn on SFB-driver debugging  */
/* #define SFBDEBUG */

/*
 * These need to be mapped into user space.
 */
static struct fbuaccess sfbu;
static struct pmax_fbtty sfbfb;
static struct fbinfo *sfb_fi;


/*
 * Forward references.
 */

static int	sfbmatch __P((struct device *, struct cfdata *, void *));
static void	sfbattach __P((struct device *, struct device *, void *));
static int	sfbinit __P((struct fbinfo *, caddr_t, int, int));
static int	sfb_intr __P((void *sc));

CFATTACH_DECL(sfb, sizeof(struct fbinfo),
    sfbmatch, sfbattach, NULL, NULL);

struct fbdriver sfb_driver = {
	bt459_video_on,
	bt459_video_off,
	bt459InitColorMap,
	bt459GetColorMap,
	bt459LoadColorMap,
	bt459PosCursor,
	bt459LoadCursor,
	bt459CursorColor
};

int
sfb_cnattach(addr)
paddr_t addr;
{
	struct fbinfo *fi;
	caddr_t base;

	base = (caddr_t)MIPS_PHYS_TO_KSEG1(addr);
	fbcnalloc(&fi);
	if (sfbinit(fi, base, 0, 1) < 0)
	      return (0);
	sfb_fi = fi;
	return (1);
}

/* match and attach routines cut-and-pasted from cfb */

static int
sfbmatch(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct tc_attach_args *ta = aux;

	/* make sure that we're looking for this type of device. */
	if (!TC_BUS_MATCHNAME(ta, "PMAGB-BA"))
		return (0);

	return (1);
}

/*
 * Attach a device.  Hand off all the work to sfbinit(),
 * so console-config code can attach sfbs early in boot.
 */
static void
sfbattach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct tc_attach_args *ta = aux;
	caddr_t sfbaddr = (caddr_t)ta->ta_addr;
	int unit = self->dv_unit;
	struct fbinfo *fi;

	if (sfb_fi)
		fi = sfb_fi;
	else {
		if (fballoc(&fi) < 0 || sfbinit(fi, sfbaddr, unit, 0) < 0)
		return; /* failed */
	}
	((struct fbsoftc *)self)->sc_fi = fi;

	printf(": %dx%dx%d%s",
		fi->fi_type.fb_width,
		fi->fi_type.fb_height,
		fi->fi_type.fb_depth,
		(sfb_fi) ? " console" : "");

#if 0 /*XXX*/

	/*
	 * Sean Davidson (davidson@sean.zk3.dec.com) reports this
	 *  isn't sufficient on a 3MIN, Use an interrupt handler instead.
	 */

	*(sfbaddr + SFB_INTERRUPT_ENABLE) = 0;

#endif
	/*
	 * 3MIN does not mask un-established TC option interrupts,
	 * so establish a handler.
	 * XXX Should store cmap updates in softc and apply in the
	 * interrupt handler, which interrupts during vertical-retrace.
	 */
	tc_intr_establish(parent, ta->ta_cookie, TC_IPL_NONE, sfb_intr, fi);
	printf("\n");
}


/*
 * Initialization
 */
static int
sfbinit(fi, base, unit, silent)
	struct fbinfo *fi;
	char *base;
	int unit;
	int silent;
{

	int h_setup, v_setup;
	int x_pixels, y_pixels;	/* visible pixel dimensions */

	/* check for no frame buffer */
	if (badaddr(base + SFB_OFFSET_VRAM, 4))
		return (0);

	/* fetch sfb h_setup and v_setup for pixel dimensions */
	h_setup = * (u_int32_t*) ( ((caddr_t)base) + SFB_VHORIZONTAL);
	v_setup = * (u_int32_t*) ( ((caddr_t)base) + SFB_VVERTICAL);
	x_pixels = (h_setup & 0x01ff) << 2;
	y_pixels = (v_setup & 0x07ff);
#if defined(DEBUG) || defined(SFBDEBUG)
	printf(" (%d x %d pixels) ", x_pixels, y_pixels);
#endif

	/* Fill in main frame buffer info struct. */
	fi->fi_unit = unit;
	fi->fi_pixels = (caddr_t)(base + SFB_OFFSET_VRAM);
	fi->fi_pixelsize = x_pixels * y_pixels;
	fi->fi_base = (caddr_t)(base + SFB_ASIC_OFFSET);
	fi->fi_vdac = (caddr_t)(base + SFB_OFFSET_BT459);
	fi->fi_size = (fi->fi_pixels + SFB_FB_SIZE) - fi->fi_base;
	fi->fi_linebytes = x_pixels;
	fi->fi_driver = &sfb_driver;
	fi->fi_blanked = 0;

	/* Fill in Frame Buffer Type struct. */
	fi->fi_type.fb_type = PMAX_FBTYPE_SFB;
	fi->fi_type.fb_width = 	x_pixels;
	fi->fi_type.fb_height = y_pixels;
	fi->fi_type.fb_depth = 8;
	fi->fi_type.fb_cmsize = 256;
	fi->fi_type.fb_size = SFB_FB_SIZE;

	/* Initialize the RAMDAC. */
	bt459init (fi);

	/* Initialize the SFB ASIC... */
	*((int *)(base + SFB_MODE)) = 0;
	*((int *)(base + SFB_PLANEMASK)) = 0xFFFFFFFF;

/* XXX below, up to fbconnect(), cut-and-pasted from cfb */
	/*
	 * qvss/pm-style mmap()ed event queue compatibility glue
	 */

	/*
	 * Must be in Uncached space since the fbuaccess structure is
	 * mapped into the user's address space uncached.
	 */
	fi->fi_fbu = (struct fbuaccess *)
		MIPS_PHYS_TO_KSEG1(MIPS_KSEG0_TO_PHYS(&sfbu));

	/* This is glass-tty state but it's in the shared structure. Ick. */
	fi->fi_fbu->scrInfo.max_row = fi->fi_type.fb_height / 15 - 1;
	fi->fi_fbu->scrInfo.max_col = 80;

#if defined(DEBUG) || defined(SFBDEBUG)
	printf(" (tty %d rows by %d cols) ",
	       fi->fi_fbu->scrInfo.max_row, fi->fi_fbu->scrInfo.max_col);
#endif
	init_pmaxfbu(fi);

	/*
	 * Initialize old-style pmax glass-tty screen info.
	 */
	fi->fi_glasstty = &sfbfb;


	/*
	 * Initialize the color map, the screen, and the mouse.
	 */
	if (tb_kbdmouseconfig(fi)) {
		return (-1);
	}
	fbconnect(fi);
	return (0);
}


/*
 * The TURBOChannel sfb interrupts by default on every vertical retrace,
 * and we don't know to disable those interrupt requests.
 * The 4.4BSD/pmax kernel never enabled delivery of those interrupts
 * from the TC bus, but there's a kernel design bug on the 3MIN, where
 * disabling (or enabling) TC option interrupts has no effect; each slot
 * interrupt is mapped directly to a separate R3000 interrupt and they
 * always seem to be taken.
 *
 * This function simply dismisses SFB interrupts, or the interrupt
 * request from the card will still be active.
 */
static int
sfb_intr(sc)
	void *sc;
{
	struct fbinfo *fi;
	char *slot_addr;

	fi = (struct fbinfo *)sc;
	slot_addr = (((char *)fi->fi_base) - SFB_ASIC_OFFSET);
	
	/* reset vertical-retrace interrupt by writing a dont-care */
	*(int*) (slot_addr + SFB_CLEAR) = 0;

	return (0);
}

/* old bt459 code used to be here */
