/*	$NetBSD: z8530tty.c,v 1.21 2003/12/04 13:05:16 keihan Exp $	*/

/*-
 * Copyright (c) 1993, 1994, 1995, 1996, 1997, 1998, 1999
 *	Charles M. Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles M. Hannum.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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
 *
 *	@(#)zs.c	8.1 (Berkeley) 7/19/93
 */

/*
 * Copyright (c) 1994 Gordon W. Ross
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 *	@(#)zs.c	8.1 (Berkeley) 7/19/93
 */

/*
 * Zilog Z8530 Dual UART driver (tty interface)
 *
 * This is the "slave" driver that will be attached to
 * the "zsc" driver for plain "tty" async. serial lines.
 *
 * Credits, history:
 *
 * The original version of this code was the sparc/dev/zs.c driver
 * as distributed with the Berkeley 4.4 Lite release.  Since then,
 * Gordon Ross reorganized the code into the current parent/child
 * driver scheme, separating the Sun keyboard and mouse support
 * into independent child drivers.
 *
 * RTS/CTS flow-control support was a collaboration of:
 *	Gordon Ross <gwr@NetBSD.org>,
 *	Bill Studenmund <wrstuden@loki.stanford.edu>
 *	Ian Dall <Ian.Dall@dsto.defence.gov.au>
 *
 * The driver was massively overhauled in November 1997 by Charles Hannum,
 * fixing *many* bugs, and substantially improving performance.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: z8530tty.c,v 1.21 2003/12/04 13:05:16 keihan Exp $");

#include "opt_kgdb.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/malloc.h>
#include <sys/tty.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/syslog.h>

#include <dev/ic/z8530reg.h>
#include <machine/z8530var.h>

#include <dev/cons.h>

#include "locators.h"

/*
 * How many input characters we can buffer.
 * The port-specific var.h may override this.
 * Note: must be a power of two!
 */
#ifndef	ZSTTY_RING_SIZE
#define	ZSTTY_RING_SIZE	2048
#endif

/*
 * Make this an option variable one can patch.
 * But be warned:  this must be a power of 2!
 */
u_int zstty_rbuf_size = ZSTTY_RING_SIZE;

/* Stop input when 3/4 of the ring is full; restart when only 1/4 is full. */
u_int zstty_rbuf_hiwat = (ZSTTY_RING_SIZE * 1) / 4;
u_int zstty_rbuf_lowat = (ZSTTY_RING_SIZE * 3) / 4;

struct zstty_softc {
	struct	device zst_dev;		/* required first: base device */
	struct  tty *zst_tty;
	struct	zs_chanstate *zst_cs;

	struct callout zst_diag_ch;

	u_int zst_overflows,
	      zst_floods,
	      zst_errors;

	int zst_hwflags,	/* see z8530var.h */
	    zst_swflags;	/* TIOCFLAG_SOFTCAR, ... <ttycom.h> */

	u_int zst_r_hiwat,
	      zst_r_lowat;
	u_char *volatile zst_rbget,
	       *volatile zst_rbput;
	volatile u_int zst_rbavail;
	u_char *zst_rbuf,
	       *zst_ebuf;

	/*
	 * The transmit byte count and address are used for pseudo-DMA
	 * output in the hardware interrupt code.  PDMA can be suspended
	 * to get pending changes done; heldtbc is used for this.  It can
	 * also be stopped for ^S; this sets TS_TTSTOP in tp->t_state.
	 */
	u_char *zst_tba;		/* transmit buffer address */
	u_int zst_tbc,			/* transmit byte count */
	      zst_heldtbc;		/* held tbc while xmission stopped */

	/* Flags to communicate with zstty_softint() */
	volatile u_char zst_rx_flags,	/* receiver blocked */
#define	RX_TTY_BLOCKED		0x01
#define	RX_TTY_OVERFLOWED	0x02
#define	RX_IBUF_BLOCKED		0x04
#define	RX_IBUF_OVERFLOWED	0x08
#define	RX_ANY_BLOCK		0x0f
			zst_tx_busy,	/* working on an output chunk */
			zst_tx_done,	/* done with one output chunk */
			zst_tx_stopped,	/* H/W level stop (lost CTS) */
			zst_st_check,	/* got a status interrupt */
			zst_rx_ready;
};

/* Macros to clear/set/test flags. */
#define SET(t, f)	(t) |= (f)
#define CLR(t, f)	(t) &= ~(f)
#define ISSET(t, f)	((t) & (f))

/* Definition of the driver for autoconfig. */
static int	zstty_match(struct device *, struct cfdata *, void *);
static void	zstty_attach(struct device *, struct device *, void *);

CFATTACH_DECL(zstty, sizeof(struct zstty_softc),
    zstty_match, zstty_attach, NULL, NULL);

extern struct cfdriver zstty_cd;

dev_type_open(zsopen);
dev_type_close(zsclose);
dev_type_read(zsread);
dev_type_write(zswrite);
dev_type_ioctl(zsioctl);
dev_type_stop(zsstop);
dev_type_tty(zstty);
dev_type_poll(zspoll);

const struct cdevsw zstty_cdevsw = {
	zsopen, zsclose, zsread, zswrite, zsioctl,
	zsstop, zstty, zspoll, nommap, ttykqfilter, D_TTY
};

struct zsops zsops_tty;

static void zs_shutdown __P((struct zstty_softc *));
static void	zsstart __P((struct tty *));
static int	zsparam __P((struct tty *, struct termios *));
static void zs_modem __P((struct zstty_softc *, int));
static void tiocm_to_zs __P((struct zstty_softc *, int, int));
static int  zs_to_tiocm __P((struct zstty_softc *));
static int    zshwiflow __P((struct tty *, int));
static void  zs_hwiflow __P((struct zstty_softc *));

/* Low-level routines. */
static void zstty_rxint   __P((struct zs_chanstate *));
static void zstty_stint   __P((struct zs_chanstate *, int));
static void zstty_txint   __P((struct zs_chanstate *));
static void zstty_softint __P((struct zs_chanstate *));

#define	ZSUNIT(x)	(minor(x) & 0x7ffff)
#define	ZSDIALOUT(x)	(minor(x) & 0x80000)

/*
 * zstty_match: how is this zs channel configured?
 */
int 
zstty_match(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void   *aux;
{
	struct zsc_attach_args *args = aux;

	/* Exact match is better than wildcard. */
	if (cf->cf_loc[ZSCCF_CHANNEL] == args->channel)
		return 2;

	/* This driver accepts wildcard. */
	if (cf->cf_loc[ZSCCF_CHANNEL] == ZSCCF_CHANNEL_DEFAULT)
		return 1;

	return 0;
}

void 
zstty_attach(parent, self, aux)
	struct device *parent, *self;
	void   *aux;

{
	struct zsc_softc *zsc = (void *) parent;
	struct zstty_softc *zst = (void *) self;
	struct cfdata *cf = self->dv_cfdata;
	struct zsc_attach_args *args = aux;
	struct zs_chanstate *cs;
	struct tty *tp;
	int channel, s, tty_unit;
	dev_t dev;

	callout_init(&zst->zst_diag_ch);

	tty_unit = zst->zst_dev.dv_unit;
	channel = args->channel;
	cs = zsc->zsc_cs[channel];
	cs->cs_private = zst;
	cs->cs_ops = &zsops_tty;

	zst->zst_cs = cs;
	zst->zst_swflags = cf->cf_flags;	/* softcar, etc. */
	zst->zst_hwflags = args->hwflags;
	dev = makedev(cdevsw_lookup_major(&zstty_cdevsw), tty_unit);

	if (zst->zst_swflags)
		printf(" flags 0x%x", zst->zst_swflags);

	if (ISSET(zst->zst_hwflags, ZS_HWFLAG_CONSOLE)) {
		printf(" (console)\n");
		DELAY(20000);
		cn_tab->cn_dev = dev;
	} else
#ifdef KGDB
	if (zs_check_kgdb(cs, dev)) {
		/*
		 * Allow kgdb to "take over" this port.  Returns true
		 * if this serial port is in-use by kgdb.
		 */
		printf(" (kgdb)\n");
		/*
		 * This is the kgdb port (exclusive use)
		 * so skip the normal attach code.
		 */
		return;
	} else
#endif
		printf("\n");

	tp = ttymalloc();
	tp->t_dev = dev;
	tp->t_oproc = zsstart;
	tp->t_param = zsparam;
	tp->t_hwiflow = zshwiflow;
	tty_attach(tp);

	zst->zst_tty = tp;
	zst->zst_rbuf = malloc(zstty_rbuf_size << 1, M_DEVBUF, M_WAITOK);
	zst->zst_ebuf = zst->zst_rbuf + (zstty_rbuf_size << 1);
	/* Disable the high water mark. */
	zst->zst_r_hiwat = 0;
	zst->zst_r_lowat = 0;
	zst->zst_rbget = zst->zst_rbput = zst->zst_rbuf;
	zst->zst_rbavail = zstty_rbuf_size;

	/* XXX - Do we need an MD hook here? */

	/*
	 * Hardware init
	 */
	if (ISSET(zst->zst_hwflags, ZS_HWFLAG_CONSOLE)) {
		/* Call zsparam similar to open. */
		struct termios t;

		/* Setup the "new" parameters in t. */
		t.c_ispeed = 0;
		t.c_ospeed = cs->cs_defspeed;
		t.c_cflag = cs->cs_defcflag;

		s = splzs();

		/*
		 * Turn on receiver and status interrupts.
		 * We defer the actual write of the register to zsparam(),
		 * but we must make sure status interrupts are turned on by
		 * the time zsparam() reads the initial rr0 state.
		 */
		SET(cs->cs_preg[1], ZSWR1_RIE | ZSWR1_SIE);

		splx(s);

		/* Make sure zsparam will see changes. */
		tp->t_ospeed = 0;
		(void) zsparam(tp, &t);

		s = splzs();

		/* Make sure DTR is on now. */
		zs_modem(zst, 1);

		splx(s);
	} else {
		/* Not the console; may need reset. */
		int reset;

		reset = (channel == 0) ? ZSWR9_A_RESET : ZSWR9_B_RESET;

		s = splzs();

		zs_write_reg(cs, 9, reset);

		/* Will raise DTR in open. */
		zs_modem(zst, 0);

		splx(s);
	}
}


/*
 * Return pointer to our tty.
 */
struct tty *
zstty(dev)
	dev_t dev;
{
	struct zstty_softc *zst;
	int unit = ZSUNIT(dev);

#ifdef	DIAGNOSTIC
	if (unit >= zstty_cd.cd_ndevs)
		panic("zstty");
#endif
	zst = zstty_cd.cd_devs[unit];
	return (zst->zst_tty);
}


void
zs_shutdown(zst)
	struct zstty_softc *zst;
{
	struct zs_chanstate *cs = zst->zst_cs;
	struct tty *tp = zst->zst_tty;
	int s;

	s = splzs();

	/* If we were asserting flow control, then deassert it. */
	SET(zst->zst_rx_flags, RX_IBUF_BLOCKED);
	zs_hwiflow(zst);

	/* Clear any break condition set with TIOCSBRK. */
	zs_break(cs, 0);

	/*
	 * Hang up if necessary.  Wait a bit, so the other side has time to
	 * notice even if we immediately open the port again.
	 */
	if (ISSET(tp->t_cflag, HUPCL)) {
		zs_modem(zst, 0);
		(void) tsleep(cs, TTIPRI, ttclos, hz);
	}

	/* Turn off interrupts if not the console. */
	if (!ISSET(zst->zst_hwflags, ZS_HWFLAG_CONSOLE)) {
		CLR(cs->cs_preg[1], ZSWR1_RIE | ZSWR1_SIE);
		cs->cs_creg[1] = cs->cs_preg[1];
		zs_write_reg(cs, 1, cs->cs_creg[1]);
	}

	splx(s);
}

/*
 * Open a zs serial (tty) port.
 */
int
zsopen(dev, flags, mode, p)
	dev_t dev;
	int flags;
	int mode;
	struct proc *p;
{
	int unit = ZSUNIT(dev);
	struct zstty_softc *zst;
	struct zs_chanstate *cs;
	struct tty *tp;
	int s, s2;
	int error;

	if (unit >= zstty_cd.cd_ndevs)
		return (ENXIO);
	zst = zstty_cd.cd_devs[unit];
	if (zst == 0)
		return (ENXIO);
	tp = zst->zst_tty;
	cs = zst->zst_cs;

	/* If KGDB took the line, then tp==NULL */
	if (tp == NULL)
		return (EBUSY);

	if (ISSET(tp->t_state, TS_ISOPEN) &&
	    ISSET(tp->t_state, TS_XCLUDE) &&
	    p->p_ucred->cr_uid != 0)
		return (EBUSY);

	s = spltty();

	/*
	 * Do the following iff this is a first open.
	 */
	if (!ISSET(tp->t_state, TS_ISOPEN) && tp->t_wopen == 0) {
		struct termios t;

		tp->t_dev = dev;

		/*
		 * Initialize the termios status to the defaults.  Add in the
		 * sticky bits from TIOCSFLAGS.
		 */
		t.c_ispeed = 0;
		t.c_ospeed = cs->cs_defspeed;
		t.c_cflag = cs->cs_defcflag;
		if (ISSET(zst->zst_swflags, TIOCFLAG_CLOCAL))
			SET(t.c_cflag, CLOCAL);
		if (ISSET(zst->zst_swflags, TIOCFLAG_CRTSCTS))
			SET(t.c_cflag, CRTSCTS);
		if (ISSET(zst->zst_swflags, TIOCFLAG_CDTRCTS))
			SET(t.c_cflag, CDTRCTS);
		if (ISSET(zst->zst_swflags, TIOCFLAG_MDMBUF))
			SET(t.c_cflag, MDMBUF);

		s2 = splzs();

		/*
		 * Turn on receiver and status interrupts.
		 * We defer the actual write of the register to zsparam(),
		 * but we must make sure status interrupts are turned on by
		 * the time zsparam() reads the initial rr0 state.
		 */
		SET(cs->cs_preg[1], ZSWR1_RIE | ZSWR1_SIE);

		splx(s2);

		/* Make sure zsparam will see changes. */
		tp->t_ospeed = 0;
		(void) zsparam(tp, &t);

		/*
		 * Note: zsparam has done: cflag, ispeed, ospeed
		 * so we just need to do: iflag, oflag, lflag, cc
		 * For "raw" mode, just leave all zeros.
		 */
		if (!ISSET(zst->zst_hwflags, ZS_HWFLAG_RAW)) {
			tp->t_iflag = TTYDEF_IFLAG;
			tp->t_oflag = TTYDEF_OFLAG;
			tp->t_lflag = TTYDEF_LFLAG;
		} else {
			tp->t_iflag = 0;
			tp->t_oflag = 0;
			tp->t_lflag = 0;
		}
		ttychars(tp);
		ttsetwater(tp);

		s2 = splzs();

		/*
		 * Turn on DTR.  We must always do this, even if carrier is not
		 * present, because otherwise we'd have to use TIOCSDTR
		 * immediately after setting CLOCAL, which applications do not
		 * expect.  We always assert DTR while the device is open
		 * unless explicitly requested to deassert it.
		 */
		zs_modem(zst, 1);

		/* Clear the input ring, and unblock. */
		zst->zst_rbget = zst->zst_rbput = zst->zst_rbuf;
		zst->zst_rbavail = zstty_rbuf_size;
		zs_iflush(cs);
		CLR(zst->zst_rx_flags, RX_ANY_BLOCK);
		zs_hwiflow(zst);

		splx(s2);
	}

	splx(s);

	error = ttyopen(tp, ZSDIALOUT(dev), ISSET(flags, O_NONBLOCK));
	if (error)
		goto bad;

	error = (*tp->t_linesw->l_open)(dev, tp);
	if (error)
		goto bad;

	return (0);

bad:
	if (!ISSET(tp->t_state, TS_ISOPEN) && tp->t_wopen == 0) {
		/*
		 * We failed to open the device, and nobody else had it opened.
		 * Clean up the state as appropriate.
		 */
		zs_shutdown(zst);
	}

	return (error);
}

/*
 * Close a zs serial port.
 */
int
zsclose(dev, flags, mode, p)
	dev_t dev;
	int flags;
	int mode;
	struct proc *p;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(dev)];
	struct tty *tp = zst->zst_tty;

	/* XXX This is for cons.c. */
	if (!ISSET(tp->t_state, TS_ISOPEN))
		return 0;

	(*tp->t_linesw->l_close)(tp, flags);
	ttyclose(tp);

	if (!ISSET(tp->t_state, TS_ISOPEN) && tp->t_wopen == 0) {
		/*
		 * Although we got a last close, the device may still be in
		 * use; e.g. if this was the dialout node, and there are still
		 * processes waiting for carrier on the non-dialout node.
		 */
		zs_shutdown(zst);
	}

	return (0);
}

/*
 * Read/write zs serial port.
 */
int
zsread(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(dev)];
	struct tty *tp = zst->zst_tty;

	return ((*tp->t_linesw->l_read)(tp, uio, flags));
}

int
zswrite(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(dev)];
	struct tty *tp = zst->zst_tty;

	return ((*tp->t_linesw->l_write)(tp, uio, flags));
}

int
zspoll(dev, events, p)
	dev_t dev;
	int events;
	struct proc *p;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(dev)];
	struct tty *tp = zst->zst_tty;
 
	return ((*tp->t_linesw->l_poll)(tp, events, p));
}

int
zsioctl(dev, cmd, data, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(dev)];
	struct zs_chanstate *cs = zst->zst_cs;
	struct tty *tp = zst->zst_tty;
	int error;
	int s;

	error = (*tp->t_linesw->l_ioctl)(tp, cmd, data, flag, p);
	if (error != EPASSTHROUGH)
		return (error);

	error = ttioctl(tp, cmd, data, flag, p);
	if (error != EPASSTHROUGH)
		return (error);

#ifdef	ZS_MD_IOCTL
	error = ZS_MD_IOCTL(cs, cmd, data);
	if (error != EPASSTHROUGH)
		return (error);
#endif	/* ZS_MD_IOCTL */

	error = 0;

	s = splzs();

	switch (cmd) {
	case TIOCSBRK:
		zs_break(cs, 1);
		break;

	case TIOCCBRK:
		zs_break(cs, 0);
		break;

	case TIOCGFLAGS:
		*(int *)data = zst->zst_swflags;
		break;

	case TIOCSFLAGS:
		error = suser(p->p_ucred, &p->p_acflag);
		if (error)
			break;
		zst->zst_swflags = *(int *)data;
		break;

	case TIOCSDTR:
		zs_modem(zst, 1);
		break;

	case TIOCCDTR:
		zs_modem(zst, 0);
		break;

	case TIOCMSET:
	case TIOCMBIS:
	case TIOCMBIC:
		tiocm_to_zs(zst, cmd, *(int *)data);
		break;

	case TIOCMGET:
		*(int *)data = zs_to_tiocm(zst);
		break;

	default:
		error = EPASSTHROUGH;
		break;
	}

	splx(s);

	return (error);
}

/*
 * Start or restart transmission.
 */
static void
zsstart(tp)
	struct tty *tp;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(tp->t_dev)];
	struct zs_chanstate *cs = zst->zst_cs;
	int s;

	s = spltty();
	if (ISSET(tp->t_state, TS_BUSY | TS_TIMEOUT | TS_TTSTOP))
		goto out;
	if (zst->zst_tx_stopped)
		goto out;

	if (tp->t_outq.c_cc <= tp->t_lowat) {
		if (ISSET(tp->t_state, TS_ASLEEP)) {
			CLR(tp->t_state, TS_ASLEEP);
			wakeup((caddr_t)&tp->t_outq);
		}
		selwakeup(&tp->t_wsel);
		if (tp->t_outq.c_cc == 0)
			goto out;
	}

	/* Grab the first contiguous region of buffer space. */
	{
		u_char *tba;
		int tbc;

		tba = tp->t_outq.c_cf;
		tbc = ndqb(&tp->t_outq, 0);
	
		(void) splzs();

		zst->zst_tba = tba;
		zst->zst_tbc = tbc;
	}

	SET(tp->t_state, TS_BUSY);
	zst->zst_tx_busy = 1;

#ifdef ZS_TXDMA
	if (zst->zst_tbc > 1) {
		zs_dma_setup(cs, zst->zst_tba, zst->zst_tbc);
		goto out;
	}
#endif
	/* Enable transmit completion interrupts if necessary. */
	if (!ISSET(cs->cs_preg[1], ZSWR1_TIE)) {
		SET(cs->cs_preg[1], ZSWR1_TIE);
		cs->cs_creg[1] = cs->cs_preg[1];
		zs_write_reg(cs, 1, cs->cs_creg[1]);
	}

	/* Output the first character of the contiguous buffer. */
	{
		zs_write_data(cs, *zst->zst_tba);
		zst->zst_tbc--;
		zst->zst_tba++;
	}
out:
	splx(s);
	return;
}

/*
 * Stop output, e.g., for ^S or output flush.
 */
void
zsstop(tp, flag)
	struct tty *tp;
	int flag;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(tp->t_dev)];
	int s;

	s = splzs();
	if (ISSET(tp->t_state, TS_BUSY)) {
		/* Stop transmitting at the next chunk. */
		zst->zst_tbc = 0;
		zst->zst_heldtbc = 0;
		if (!ISSET(tp->t_state, TS_TTSTOP))
			SET(tp->t_state, TS_FLUSH);
	}
	splx(s);
}

/*
 * Set ZS tty parameters from termios.
 * XXX - Should just copy the whole termios after
 * making sure all the changes could be done.
 */
static int
zsparam(tp, t)
	struct tty *tp;
	struct termios *t;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(tp->t_dev)];
	struct zs_chanstate *cs = zst->zst_cs;
	int ospeed, cflag;
	u_char tmp3, tmp4, tmp5, tmp15;
	int s, error;

	ospeed = t->c_ospeed;
	cflag = t->c_cflag;

	/* Check requested parameters. */
	if (ospeed < 0)
		return (EINVAL);
	if (t->c_ispeed && t->c_ispeed != ospeed)
		return (EINVAL);

	/*
	 * For the console, always force CLOCAL and !HUPCL, so that the port
	 * is always active.
	 */
	if (ISSET(zst->zst_swflags, TIOCFLAG_SOFTCAR) ||
	    ISSET(zst->zst_hwflags, ZS_HWFLAG_CONSOLE)) {
		SET(cflag, CLOCAL);
		CLR(cflag, HUPCL);
	}

	/*
	 * Only whack the UART when params change.
	 * Some callers need to clear tp->t_ospeed
	 * to make sure initialization gets done.
	 */
	if (tp->t_ospeed == ospeed &&
	    tp->t_cflag == cflag)
		return (0);

	/*
	 * Call MD functions to deal with changed
	 * clock modes or H/W flow control modes.
	 * The BRG divisor is set now. (reg 12,13)
	 */
	error = zs_set_speed(cs, ospeed);
	if (error)
		return (error);
	error = zs_set_modes(cs, cflag);
	if (error)
		return (error);

	/*
	 * Block interrupts so that state will not
	 * be altered until we are done setting it up.
	 *
	 * Initial values in cs_preg are set before
	 * our attach routine is called.  The master
	 * interrupt enable is handled by zsc.c
	 *
	 */
	s = splzs();

	cs->cs_rr0_mask = cs->cs_rr0_cts | cs->cs_rr0_dcd;
	tmp15 = cs->cs_preg[15];
	if (ISSET(cs->cs_rr0_mask, ZSRR0_DCD))
		SET(tmp15, ZSWR15_DCD_IE);
	else
		CLR(tmp15, ZSWR15_DCD_IE);
	if (ISSET(cs->cs_rr0_mask, ZSRR0_CTS))
		SET(tmp15, ZSWR15_CTS_IE);
	else
		CLR(tmp15, ZSWR15_CTS_IE);
	cs->cs_preg[15] = tmp15;

	/* Recompute character size bits. */
	tmp3 = cs->cs_preg[3];
	tmp5 = cs->cs_preg[5];
	CLR(tmp3, ZSWR3_RXSIZE);
	CLR(tmp5, ZSWR5_TXSIZE);
	switch (ISSET(cflag, CSIZE)) {
	case CS5:
		SET(tmp3, ZSWR3_RX_5);
		SET(tmp5, ZSWR5_TX_5);
		break;
	case CS6:
		SET(tmp3, ZSWR3_RX_6);
		SET(tmp5, ZSWR5_TX_6);
		break;
	case CS7:
		SET(tmp3, ZSWR3_RX_7);
		SET(tmp5, ZSWR5_TX_7);
		break;
	case CS8:
		SET(tmp3, ZSWR3_RX_8);
		SET(tmp5, ZSWR5_TX_8);
		break;
	}
	cs->cs_preg[3] = tmp3;
	cs->cs_preg[5] = tmp5;

	/*
	 * Recompute the stop bits and parity bits.  Note that
	 * zs_set_speed() may have set clock selection bits etc.
	 * in wr4, so those must preserved.
	 */
	tmp4 = cs->cs_preg[4];
	CLR(tmp4, ZSWR4_SBMASK | ZSWR4_PARMASK);
	if (ISSET(cflag, CSTOPB))
		SET(tmp4, ZSWR4_TWOSB);
	else
		SET(tmp4, ZSWR4_ONESB);
	if (!ISSET(cflag, PARODD))
		SET(tmp4, ZSWR4_EVENP);
	if (ISSET(cflag, PARENB))
		SET(tmp4, ZSWR4_PARENB);
	cs->cs_preg[4] = tmp4;

	/* And copy to tty. */
	tp->t_ispeed = 0;
	tp->t_ospeed = ospeed;
	tp->t_cflag = cflag;

	/*
	 * If nothing is being transmitted, set up new current values,
	 * else mark them as pending.
	 */
	if (!cs->cs_heldchange) {
		if (zst->zst_tx_busy) {
			zst->zst_heldtbc = zst->zst_tbc;
			zst->zst_tbc = 0;
			cs->cs_heldchange = 1;
		} else
			zs_loadchannelregs(cs);
	}

	/*
	 * If hardware flow control is disabled, turn off the buffer water
	 * marks and unblock any soft flow control state.  Otherwise, enable
	 * the water marks.
	 */
	if (!ISSET(cflag, CHWFLOW)) {
		zst->zst_r_hiwat = 0;
		zst->zst_r_lowat = 0;
		if (ISSET(zst->zst_rx_flags, RX_TTY_OVERFLOWED)) {
			CLR(zst->zst_rx_flags, RX_TTY_OVERFLOWED);
			zst->zst_rx_ready = 1;
			cs->cs_softreq = 1;
		}
		if (ISSET(zst->zst_rx_flags, RX_TTY_BLOCKED|RX_IBUF_BLOCKED)) {
			CLR(zst->zst_rx_flags, RX_TTY_BLOCKED|RX_IBUF_BLOCKED);
			zs_hwiflow(zst);
		}
	} else {
		zst->zst_r_hiwat = zstty_rbuf_hiwat;
		zst->zst_r_lowat = zstty_rbuf_lowat;
	}

	/*
	 * Force a recheck of the hardware carrier and flow control status,
	 * since we may have changed which bits we're looking at.
	 */
	zstty_stint(cs, 1);

	splx(s);

	/*
	 * If hardware flow control is disabled, unblock any hard flow control
	 * state.
	 */
	if (!ISSET(cflag, CHWFLOW)) {
		if (zst->zst_tx_stopped) {
			zst->zst_tx_stopped = 0;
			zsstart(tp);
		}
	}

	zstty_softint(cs);

	return (0);
}

/*
 * Raise or lower modem control (DTR/RTS) signals.  If a character is
 * in transmission, the change is deferred.
 */
static void
zs_modem(zst, onoff)
	struct zstty_softc *zst;
	int onoff;
{
	struct zs_chanstate *cs = zst->zst_cs;

	if (cs->cs_wr5_dtr == 0)
		return;

	if (onoff)
		SET(cs->cs_preg[5], cs->cs_wr5_dtr);
	else
		CLR(cs->cs_preg[5], cs->cs_wr5_dtr);

	if (!cs->cs_heldchange) {
		if (zst->zst_tx_busy) {
			zst->zst_heldtbc = zst->zst_tbc;
			zst->zst_tbc = 0;
			cs->cs_heldchange = 1;
		} else
			zs_loadchannelregs(cs);
	}
}

static void
tiocm_to_zs(zst, how, ttybits)
	struct zstty_softc *zst;
	int how, ttybits;
{
	struct zs_chanstate *cs = zst->zst_cs;
	u_char zsbits;

	zsbits = 0;
	if (ISSET(ttybits, TIOCM_DTR))
		SET(zsbits, ZSWR5_DTR);
	if (ISSET(ttybits, TIOCM_RTS))
		SET(zsbits, ZSWR5_RTS);

	switch (how) {
	case TIOCMBIC:
		CLR(cs->cs_preg[5], zsbits);
		break;

	case TIOCMBIS:
		SET(cs->cs_preg[5], zsbits);
		break;

	case TIOCMSET:
		CLR(cs->cs_preg[5], ZSWR5_RTS | ZSWR5_DTR);
		SET(cs->cs_preg[5], zsbits);
		break;
	}

	if (!cs->cs_heldchange) {
		if (zst->zst_tx_busy) {
			zst->zst_heldtbc = zst->zst_tbc;
			zst->zst_tbc = 0;
			cs->cs_heldchange = 1;
		} else
			zs_loadchannelregs(cs);
	}
}

static int
zs_to_tiocm(zst)
	struct zstty_softc *zst;
{
	struct zs_chanstate *cs = zst->zst_cs;
	u_char zsbits;
	int ttybits = 0;

	zsbits = cs->cs_preg[5];
	if (ISSET(zsbits, ZSWR5_DTR))
		SET(ttybits, TIOCM_DTR);
	if (ISSET(zsbits, ZSWR5_RTS))
		SET(ttybits, TIOCM_RTS);

	zsbits = cs->cs_rr0;
	if (ISSET(zsbits, ZSRR0_DCD))
		SET(ttybits, TIOCM_CD);
	if (ISSET(zsbits, ZSRR0_CTS))
		SET(ttybits, TIOCM_CTS);

	return (ttybits);
}

/*
 * Try to block or unblock input using hardware flow-control.
 * This is called by kern/tty.c if MDMBUF|CRTSCTS is set, and
 * if this function returns non-zero, the TS_TBLOCK flag will
 * be set or cleared according to the "block" arg passed.
 */
int
zshwiflow(tp, block)
	struct tty *tp;
	int block;
{
	struct zstty_softc *zst = zstty_cd.cd_devs[ZSUNIT(tp->t_dev)];
	struct zs_chanstate *cs = zst->zst_cs;
	int s;

	if (cs->cs_wr5_rts == 0)
		return (0);

	s = splzs();
	if (block) {
		if (!ISSET(zst->zst_rx_flags, RX_TTY_BLOCKED)) {
			SET(zst->zst_rx_flags, RX_TTY_BLOCKED);
			zs_hwiflow(zst);
		}
	} else {
		if (ISSET(zst->zst_rx_flags, RX_TTY_OVERFLOWED)) {
			CLR(zst->zst_rx_flags, RX_TTY_OVERFLOWED);
			zst->zst_rx_ready = 1;
			cs->cs_softreq = 1;
		}
		if (ISSET(zst->zst_rx_flags, RX_TTY_BLOCKED)) {
			CLR(zst->zst_rx_flags, RX_TTY_BLOCKED);
			zs_hwiflow(zst);
		}
	}
	splx(s);
	return (1);
}

/*
 * Internal version of zshwiflow
 * called at splzs
 */
static void
zs_hwiflow(zst)
	struct zstty_softc *zst;
{
	struct zs_chanstate *cs = zst->zst_cs;

	if (cs->cs_wr5_rts == 0)
		return;

	if (ISSET(zst->zst_rx_flags, RX_ANY_BLOCK)) {
		CLR(cs->cs_preg[5], cs->cs_wr5_rts);
		CLR(cs->cs_creg[5], cs->cs_wr5_rts);
	} else {
		SET(cs->cs_preg[5], cs->cs_wr5_rts);
		SET(cs->cs_creg[5], cs->cs_wr5_rts);
	}
	zs_write_reg(cs, 5, cs->cs_creg[5]);
}


/****************************************************************
 * Interface to the lower layer (zscc)
 ****************************************************************/

#define	integrate	static inline
integrate void zstty_rxsoft __P((struct zstty_softc *, struct tty *));
integrate void zstty_txsoft __P((struct zstty_softc *, struct tty *));
integrate void zstty_stsoft __P((struct zstty_softc *, struct tty *));
static void zstty_diag __P((void *));

/*
 * receiver ready interrupt.
 * called at splzs
 */
static void
zstty_rxint(cs)
	struct zs_chanstate *cs;
{
	struct zstty_softc *zst = cs->cs_private;
	u_char *put, *end;
	u_int cc;
	u_char rr0, rr1, c;

	end = zst->zst_ebuf;
	put = zst->zst_rbput;
	cc = zst->zst_rbavail;

	while (cc > 0) {
		/*
		 * First read the status, because reading the received char
		 * destroys the status of this char.
		 */
		rr1 = zs_read_reg(cs, 1);
		c = zs_read_data(cs);

		if (ISSET(rr1, ZSRR1_FE | ZSRR1_DO | ZSRR1_PE)) {
			/* Clear the receive error. */
			zs_write_csr(cs, ZSWR0_RESET_ERRORS);
		}

		put[0] = c;
		put[1] = rr1;
		put += 2;
		if (put >= end)
			put = zst->zst_rbuf;
		cc--;

		rr0 = zs_read_csr(cs);
		if (!ISSET(rr0, ZSRR0_RX_READY))
			break;
	}

	/*
	 * Current string of incoming characters ended because
	 * no more data was available or we ran out of space.
	 * Schedule a receive event if any data was received.
	 * If we're out of space, turn off receive interrupts.
	 */
	zst->zst_rbput = put;
	zst->zst_rbavail = cc;
	if (!ISSET(zst->zst_rx_flags, RX_TTY_OVERFLOWED)) {
		zst->zst_rx_ready = 1;
		cs->cs_softreq = 1;
	}

	/*
	 * See if we are in danger of overflowing a buffer. If
	 * so, use hardware flow control to ease the pressure.
	 */
	if (!ISSET(zst->zst_rx_flags, RX_IBUF_BLOCKED) &&
	    cc < zst->zst_r_hiwat) {
		SET(zst->zst_rx_flags, RX_IBUF_BLOCKED);
		zs_hwiflow(zst);
	}

	/*
	 * If we're out of space, disable receive interrupts
	 * until the queue has drained a bit.
	 */
	if (!cc) {
		SET(zst->zst_rx_flags, RX_IBUF_OVERFLOWED);
		CLR(cs->cs_preg[1], ZSWR1_RIE);
		cs->cs_creg[1] = cs->cs_preg[1];
		zs_write_reg(cs, 1, cs->cs_creg[1]);
	}

#if 0
	printf("%xH%04d\n", zst->zst_rx_flags, zst->zst_rbavail);
#endif
}

/*
 * transmitter ready interrupt.  (splzs)
 */
static void
zstty_txint(cs)
	struct zs_chanstate *cs;
{
	struct zstty_softc *zst = cs->cs_private;

	/*
	 * If we've delayed a parameter change, do it now, and restart
	 * output.
	 */
	if (cs->cs_heldchange) {
		zs_loadchannelregs(cs);
		cs->cs_heldchange = 0;
		zst->zst_tbc = zst->zst_heldtbc;
		zst->zst_heldtbc = 0;
	}

	/* Output the next character in the buffer, if any. */
	if (zst->zst_tbc > 0) {
		zs_write_data(cs, *zst->zst_tba);
		zst->zst_tbc--;
		zst->zst_tba++;
	} else {
		/* Disable transmit completion interrupts if necessary. */
		if (ISSET(cs->cs_preg[1], ZSWR1_TIE)) {
			CLR(cs->cs_preg[1], ZSWR1_TIE);
			cs->cs_creg[1] = cs->cs_preg[1];
			zs_write_reg(cs, 1, cs->cs_creg[1]);
		}
		if (zst->zst_tx_busy) {
			zst->zst_tx_busy = 0;
			zst->zst_tx_done = 1;
			cs->cs_softreq = 1;
		}
	}
}

/*
 * status change interrupt.  (splzs)
 */
static void
zstty_stint(cs, force)
	struct zs_chanstate *cs;
	int force;
{
	struct zstty_softc *zst = cs->cs_private;
	u_char rr0, delta;

	rr0 = zs_read_csr(cs);
	zs_write_csr(cs, ZSWR0_RESET_STATUS);

	/*
	 * Check here for console break, so that we can abort
	 * even when interrupts are locking up the machine.
	 */
	if (ISSET(rr0, ZSRR0_BREAK) &&
	    ISSET(zst->zst_hwflags, ZS_HWFLAG_CONSOLE)) {
		zs_abort(cs);
		return;
	}

	if (!force)
		delta = rr0 ^ cs->cs_rr0;
	else
		delta = cs->cs_rr0_mask;
	cs->cs_rr0 = rr0;

	if (ISSET(delta, cs->cs_rr0_mask)) {
		SET(cs->cs_rr0_delta, delta);

		/*
		 * Stop output immediately if we lose the output
		 * flow control signal or carrier detect.
		 */
		if (ISSET(~rr0, cs->cs_rr0_mask)) {
			zst->zst_tbc = 0;
			zst->zst_heldtbc = 0;
		}

		zst->zst_st_check = 1;
		cs->cs_softreq = 1;
	}
}

void
zstty_diag(arg)
	void *arg;
{
	struct zstty_softc *zst = arg;
	int overflows, floods;
	int s;

	s = splzs();
	overflows = zst->zst_overflows;
	zst->zst_overflows = 0;
	floods = zst->zst_floods;
	zst->zst_floods = 0;
	zst->zst_errors = 0;
	splx(s);

	log(LOG_WARNING, "%s: %d silo overflow%s, %d ibuf flood%s\n",
	    zst->zst_dev.dv_xname,
	    overflows, overflows == 1 ? "" : "s",
	    floods, floods == 1 ? "" : "s");
}

integrate void
zstty_rxsoft(zst, tp)
	struct zstty_softc *zst;
	struct tty *tp;
{
	struct zs_chanstate *cs = zst->zst_cs;
	int (*rint) __P((int c, struct tty *tp)) = tp->t_linesw->l_rint;
	u_char *get, *end;
	u_int cc, scc;
	u_char rr1;
	int code;
	int s;

	end = zst->zst_ebuf;
	get = zst->zst_rbget;
	scc = cc = zstty_rbuf_size - zst->zst_rbavail;

	if (cc == zstty_rbuf_size) {
		zst->zst_floods++;
		if (zst->zst_errors++ == 0)
			callout_reset(&zst->zst_diag_ch, 60 * hz,
			    zstty_diag, zst);
	}

	while (cc) {
		code = get[0];
		rr1 = get[1];
		if (ISSET(rr1, ZSRR1_DO | ZSRR1_FE | ZSRR1_PE)) {
			if (ISSET(rr1, ZSRR1_DO)) {
				zst->zst_overflows++;
				if (zst->zst_errors++ == 0)
					callout_reset(&zst->zst_diag_ch,
					    60 * hz, zstty_diag, zst);
			}
			if (ISSET(rr1, ZSRR1_FE))
				SET(code, TTY_FE);
			if (ISSET(rr1, ZSRR1_PE))
				SET(code, TTY_PE);
		}
		if ((*rint)(code, tp) == -1) {
			/*
			 * The line discipline's buffer is out of space.
			 */
			if (!ISSET(zst->zst_rx_flags, RX_TTY_BLOCKED)) {
				/*
				 * We're either not using flow control, or the
				 * line discipline didn't tell us to block for
				 * some reason.  Either way, we have no way to
				 * know when there's more space available, so
				 * just drop the rest of the data.
				 */
				get += cc << 1;
				if (get >= end)
					get -= zstty_rbuf_size << 1;
				cc = 0;
			} else {
				/*
				 * Don't schedule any more receive processing
				 * until the line discipline tells us there's
				 * space available (through comhwiflow()).
				 * Leave the rest of the data in the input
				 * buffer.
				 */
				SET(zst->zst_rx_flags, RX_TTY_OVERFLOWED);
			}
			break;
		}
		get += 2;
		if (get >= end)
			get = zst->zst_rbuf;
		cc--;
	}

	if (cc != scc) {
		zst->zst_rbget = get;
		s = splzs();
		cc = zst->zst_rbavail += scc - cc;
		/* Buffers should be ok again, release possible block. */
		if (cc >= zst->zst_r_lowat) {
			if (ISSET(zst->zst_rx_flags, RX_IBUF_OVERFLOWED)) {
				CLR(zst->zst_rx_flags, RX_IBUF_OVERFLOWED);
				SET(cs->cs_preg[1], ZSWR1_RIE);
				cs->cs_creg[1] = cs->cs_preg[1];
				zs_write_reg(cs, 1, cs->cs_creg[1]);
			}
			if (ISSET(zst->zst_rx_flags, RX_IBUF_BLOCKED)) {
				CLR(zst->zst_rx_flags, RX_IBUF_BLOCKED);
				zs_hwiflow(zst);
			}
		}
		splx(s);
	}

#if 0
	printf("%xS%04d\n", zst->zst_rx_flags, zst->zst_rbavail);
#endif
}

integrate void
zstty_txsoft(zst, tp)
	struct zstty_softc *zst;
	struct tty *tp;
{

	CLR(tp->t_state, TS_BUSY);
	if (ISSET(tp->t_state, TS_FLUSH))
		CLR(tp->t_state, TS_FLUSH);
	else
		ndflush(&tp->t_outq, (int)(zst->zst_tba - tp->t_outq.c_cf));
	(*tp->t_linesw->l_start)(tp);
}

integrate void
zstty_stsoft(zst, tp)
	struct zstty_softc *zst;
	struct tty *tp;
{
	struct zs_chanstate *cs = zst->zst_cs;
	u_char rr0, delta;
	int s;

	s = splzs();
	rr0 = cs->cs_rr0;
	delta = cs->cs_rr0_delta;
	cs->cs_rr0_delta = 0;
	splx(s);

	if (ISSET(delta, cs->cs_rr0_dcd)) {
		/*
		 * Inform the tty layer that carrier detect changed.
		 */
		(void) (*tp->t_linesw->l_modem)(tp, ISSET(rr0, ZSRR0_DCD));
	}

	if (ISSET(delta, cs->cs_rr0_cts)) {
		/* Block or unblock output according to flow control. */
		if (ISSET(rr0, cs->cs_rr0_cts)) {
			zst->zst_tx_stopped = 0;
			(*tp->t_linesw->l_start)(tp);
		} else {
			zst->zst_tx_stopped = 1;
		}
	}
}

/*
 * Software interrupt.  Called at zssoft
 *
 * The main job to be done here is to empty the input ring
 * by passing its contents up to the tty layer.  The ring is
 * always emptied during this operation, therefore the ring
 * must not be larger than the space after "high water" in
 * the tty layer, or the tty layer might drop our input.
 *
 * Note: an "input blockage" condition is assumed to exist if
 * EITHER the TS_TBLOCK flag or zst_rx_blocked flag is set.
 */
static void
zstty_softint(cs)
	struct zs_chanstate *cs;
{
	struct zstty_softc *zst = cs->cs_private;
	struct tty *tp = zst->zst_tty;
	int s;

	s = spltty();

	if (zst->zst_rx_ready) {
		zst->zst_rx_ready = 0;
		zstty_rxsoft(zst, tp);
	}

	if (zst->zst_st_check) {
		zst->zst_st_check = 0;
		zstty_stsoft(zst, tp);
	}

	if (zst->zst_tx_done) {
		zst->zst_tx_done = 0;
		zstty_txsoft(zst, tp);
	}

	splx(s);
}

struct zsops zsops_tty = {
	zstty_rxint,	/* receive char available */
	zstty_stint,	/* external/status */
	zstty_txint,	/* xmit buffer empty */
	zstty_softint,	/* process software interrupt */
};

#ifdef ZS_TXDMA
void
zstty_txdma_int(arg)
	void *arg;
{
	struct zs_chanstate *cs = arg;
	struct zstty_softc *zst = cs->cs_private;

	zst->zst_tba += zst->zst_tbc;
	zst->zst_tbc = 0;

	if (zst->zst_tx_busy) {
		zst->zst_tx_busy = 0;
		zst->zst_tx_done = 1;
		cs->cs_softreq = 1;
	}
}
#endif
