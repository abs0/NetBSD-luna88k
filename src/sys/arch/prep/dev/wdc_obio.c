/*	$NetBSD: wdc_obio.c,v 1.18 2004/08/20 06:39:38 thorpej Exp $	*/

/*-
 * Copyright (c) 1998, 2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum and by Onno van der Linden.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: wdc_obio.c,v 1.18 2004/08/20 06:39:38 thorpej Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/ata/atavar.h>
#include <dev/ic/wdcvar.h>

#include <prep/dev/obiovar.h>

#define	WDC_OBIO_REG_NPORTS	8
#define	WDC_OBIO_AUXREG_OFFSET	0x206
#define	WDC_OBIO_AUXREG_NPORTS	1 /* XXX "fdc" owns ports 0x3f7/0x377 */

/* options passed via the 'flags' config keyword */
#define WDC_OPTIONS_32	0x01 /* try to use 32bit data I/O */

struct wdc_obio_softc {
	struct	wdc_softc sc_wdcdev;
	struct	ata_channel *sc_chanlist[1];
	struct	ata_channel sc_channel;
	struct	ata_queue sc_chqueue;
	struct	wdc_regs sc_wdc_regs;
	void	*sc_ih;
};

static int	wdc_obio_probe(struct device *, struct cfdata *, void *);
static void	wdc_obio_attach(struct device *, struct device *, void *);

CFATTACH_DECL(wdc_obio, sizeof(struct wdc_obio_softc),
    wdc_obio_probe, wdc_obio_attach, NULL, NULL);

static int
wdc_obio_probe(struct device *parent, struct cfdata *match, void *aux)
{
	struct ata_channel ch;
	struct wdc_softc wdc;
	struct wdc_regs wdr;
	struct obio_attach_args *oa = aux;
	int result = 0;
	int i;

	memset(&wdc, 0, sizeof(wdc));
	memset(&ch, 0, sizeof(ch));
	ch.ch_atac = &wdc.sc_atac;
	wdc.regs = &wdr;

	wdr.cmd_iot = oa->oa_iot;
	if (bus_space_map(wdr.cmd_iot, oa->oa_iobase, WDC_OBIO_REG_NPORTS, 0,
	    &wdr.cmd_baseioh))
		goto out;

	for (i = 0; i < WDC_OBIO_REG_NPORTS; i++) {
		if (bus_space_subregion(wdr.cmd_iot, wdr.cmd_baseioh, i,
		    i == 0 ? 4 : 1, &wdr.cmd_iohs[i]) != 0)
			goto outunmap;
	}
	wdc_init_shadow_regs(&ch);

	wdr.ctl_iot = oa->oa_iot;
	if (bus_space_map(wdr.ctl_iot, oa->oa_iobase + WDC_OBIO_AUXREG_OFFSET,
	    WDC_OBIO_AUXREG_NPORTS, 0, &wdr.ctl_ioh))
		goto outunmap;

	result = wdcprobe(&ch);
	if (result) {
		oa->oa_iosize = WDC_OBIO_REG_NPORTS;
		oa->oa_msize = 0;
	}

	bus_space_unmap(wdr.ctl_iot, wdr.ctl_ioh, WDC_OBIO_AUXREG_NPORTS);
outunmap:
	bus_space_unmap(wdr.cmd_iot, wdr.cmd_baseioh, WDC_OBIO_REG_NPORTS);
out:
	return (result);
}

static void
wdc_obio_attach(struct device *parent, struct device *self, void *aux)
{
	struct wdc_obio_softc *sc = (void *)self;
	struct wdc_regs *wdr;
	struct obio_attach_args *oa = aux;
	int i;

	printf("\n");

	sc->sc_wdcdev.regs = wdr = &sc->sc_wdc_regs;

	wdr->cmd_iot = oa->oa_iot;
	wdr->ctl_iot = oa->oa_iot;
	if (bus_space_map(wdr->cmd_iot, oa->oa_iobase,
	    WDC_OBIO_REG_NPORTS, 0, &wdr->cmd_baseioh) ||
	    bus_space_map(wdr->ctl_iot,
	      oa->oa_iobase + WDC_OBIO_AUXREG_OFFSET, WDC_OBIO_AUXREG_NPORTS,
	      0, &wdr->ctl_ioh)) {
		printf("%s: couldn't map registers\n",
		    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
	}

	for (i = 0; i < WDC_OBIO_REG_NPORTS; i++) {
		if (bus_space_subregion(wdr->cmd_iot,
		      wdr->cmd_baseioh, i, i == 0 ? 4 : 1,
		      &wdr->cmd_iohs[i]) != 0) {
			printf(": couldn't subregion registers\n");
			return;
		}
	}
	wdc_init_shadow_regs(&sc->sc_channel);

	wdr->data32iot = wdr->cmd_iot;
	wdr->data32ioh = wdr->cmd_iohs[0];

	sc->sc_ih = obio_intr_establish(oa->oa_irq, IST_LEVEL,
	    IPL_BIO, wdcintr, &sc->sc_channel);

	sc->sc_wdcdev.cap |= WDC_CAPABILITY_PREATA;
	sc->sc_wdcdev.sc_atac.atac_cap |= ATAC_CAP_DATA16;
	if (sc->sc_wdcdev.sc_atac.atac_dev.dv_cfdata->cf_flags & WDC_OPTIONS_32)
		sc->sc_wdcdev.sc_atac.atac_cap |= ATAC_CAP_DATA32;
	sc->sc_wdcdev.sc_atac.atac_pio_cap = 0;
	sc->sc_chanlist[0] = &sc->sc_channel;
	sc->sc_wdcdev.sc_atac.atac_channels = sc->sc_chanlist;
	sc->sc_wdcdev.sc_atac.atac_nchannels = 1;
	sc->sc_channel.ch_channel = 0;
	sc->sc_channel.ch_atac = &sc->sc_wdcdev.sc_atac;
	sc->sc_channel.ch_queue = &sc->sc_chqueue;

	wdcattach(&sc->sc_channel);
}
