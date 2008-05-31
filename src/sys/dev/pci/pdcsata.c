/*	$NetBSD: pdcsata.c,v 1.3 2005/02/27 00:27:33 perry Exp $	*/

/*
 * Copyright (c) 2004, Manuel Bouyer.
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
 *	This product includes software developed by Manuel Bouyer.
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

#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/param.h>
#include <sys/systm.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>
#include <dev/pci/pciidereg.h>
#include <dev/pci/pciidevar.h>

#define PDC203xx_NCHANNELS 4

#define PDC203xx_BAR_IDEREGS 0x1c /* BAR where the IDE registers are mapped */

static void pdcsata_chip_map(struct pciide_softc *, struct pci_attach_args *);
static void pdc203xx_setup_channel(struct ata_channel *);
static int  pdc203xx_pci_intr(void *);
static void pdc203xx_irqack(struct ata_channel *);
static int  pdc203xx_dma_init(void *, int, int, void *, size_t, int);
static void pdc203xx_dma_start(void *,int ,int);
static int  pdc203xx_dma_finish(void *, int, int, int);

static int  pdcsata_match(struct device *, struct cfdata *, void *);
static void pdcsata_attach(struct device *, struct device *, void *);

CFATTACH_DECL(pdcsata, sizeof(struct pciide_softc),
    pdcsata_match, pdcsata_attach, NULL, NULL);

static const struct pciide_product_desc pciide_pdcsata_products[] =  {
	{ PCI_PRODUCT_PROMISE_PDC20318,
	  0,
	  "Promise PDC20318 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20319,
	  0,
	  "Promise PDC20319 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20371,
	  0,
	  "Promise PDC20371 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20375,
	  0,
	  "Promise PDC20375 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20376,
	  0,
	  "Promise PDC20376 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20377,
	  0,
	  "Promise PDC20377 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20378,
	  0,
	  "Promise PDC20378 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ PCI_PRODUCT_PROMISE_PDC20379,
	  0,
	  "Promise PDC20379 SATA150 controller",
	  pdcsata_chip_map,
	},
	{ 0,
	  0,
	  NULL,
	  NULL
	}
};

static int
pdcsata_match(struct device *parent, struct cfdata *match, void *aux)
{
	struct pci_attach_args *pa = aux;

	if (PCI_VENDOR(pa->pa_id) == PCI_VENDOR_PROMISE) {
		if (pciide_lookup_product(pa->pa_id, pciide_pdcsata_products))
			return (2);
	}
	return (0);
}

static void
pdcsata_attach(struct device *parent, struct device *self, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct pciide_softc *sc = (struct pciide_softc *)self;

	pciide_common_attach(sc, pa,
	    pciide_lookup_product(pa->pa_id, pciide_pdcsata_products));
}

static void
pdcsata_chip_map(struct pciide_softc *sc, struct pci_attach_args *pa)
{
	struct pciide_channel *cp;
	struct ata_channel *wdc_cp;
	struct wdc_regs *wdr;
	int channel, i;
	bus_size_t dmasize;
	pci_intr_handle_t intrhandle;
	const char *intrstr;

	/*
	 * Promise SATA controllers have 3 or 4 channels,
	 * the usual IDE registers are mapped in I/O space, with offsets.
	 */
	if (pci_intr_map(pa, &intrhandle) != 0) {
		aprint_error("%s: couldn't map interrupt\n",
		    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
		return;
	}
	intrstr = pci_intr_string(pa->pa_pc, intrhandle);
	sc->sc_pci_ih = pci_intr_establish(pa->pa_pc,
	    intrhandle, IPL_BIO, pdc203xx_pci_intr, sc);
	if (sc->sc_pci_ih == NULL) {
		aprint_error("%s: couldn't establish native-PCI interrupt",
		    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
		if (intrstr != NULL)
		    aprint_normal(" at %s", intrstr);
		aprint_normal("\n");
		return;
	}
	aprint_normal("%s: interrupting at %s\n",
		sc->sc_wdcdev.sc_atac.atac_dev.dv_xname,
		intrstr ? intrstr : "unknown interrupt");

	sc->sc_dma_ok = (pci_mapreg_map(pa, PCIIDE_REG_BUS_MASTER_DMA,
	    PCI_MAPREG_MEM_TYPE_32BIT, 0, &sc->sc_dma_iot,
	    &sc->sc_dma_ioh, NULL, &dmasize) == 0);
	if (!sc->sc_dma_ok) {
		aprint_error("%s: couldn't map bus-master DMA registers\n",
		    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
		pci_intr_disestablish(pa->pa_pc, sc->sc_pci_ih);
		return;
	}

	sc->sc_dmat = pa->pa_dmat;

	if (pci_mapreg_map(pa, PDC203xx_BAR_IDEREGS,
	    PCI_MAPREG_MEM_TYPE_32BIT, 0, &sc->sc_ba5_st,
	    &sc->sc_ba5_sh, NULL, NULL) != 0) {
		aprint_error("%s: couldn't map IDE registers\n",
		    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
		bus_space_unmap(sc->sc_dma_iot, sc->sc_dma_ioh, dmasize);
		pci_intr_disestablish(pa->pa_pc, sc->sc_pci_ih);
		return;
	}

	aprint_normal("%s: bus-master DMA support present\n",
	    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
	sc->sc_wdcdev.sc_atac.atac_cap = ATAC_CAP_DATA16;
	if (sc->sc_dma_ok) {
		sc->sc_wdcdev.sc_atac.atac_cap |= ATAC_CAP_DMA | ATAC_CAP_UDMA;
	}
	if (PCI_CLASS(pa->pa_class) == PCI_CLASS_MASS_STORAGE &&
	    PCI_SUBCLASS(pa->pa_class) == PCI_SUBCLASS_MASS_STORAGE_RAID)
		sc->sc_wdcdev.sc_atac.atac_cap |= ATAC_CAP_RAID;
	sc->sc_wdcdev.irqack = pdc203xx_irqack;
	sc->sc_wdcdev.sc_atac.atac_pio_cap = 4;
	sc->sc_wdcdev.sc_atac.atac_dma_cap = 2;
	sc->sc_wdcdev.sc_atac.atac_udma_cap = 6;
	sc->sc_wdcdev.sc_atac.atac_set_modes = pdc203xx_setup_channel;
	sc->sc_wdcdev.sc_atac.atac_channels = sc->wdc_chanarray;
	bus_space_write_4(sc->sc_ba5_st, sc->sc_ba5_sh, 0x06c, 0x00ff0033);
	sc->sc_wdcdev.sc_atac.atac_nchannels =
	    (bus_space_read_4(sc->sc_ba5_st, sc->sc_ba5_sh, 0x48) & 0x02) ?
	    PDC203xx_NCHANNELS : 3;
	wdc_allocate_regs(&sc->sc_wdcdev);

	sc->sc_wdcdev.dma_arg = sc;
	sc->sc_wdcdev.dma_init = pdc203xx_dma_init;
	sc->sc_wdcdev.dma_start = pdc203xx_dma_start;
	sc->sc_wdcdev.dma_finish = pdc203xx_dma_finish;

	for (channel = 0; channel < sc->sc_wdcdev.sc_atac.atac_nchannels;
	     channel++) {
		cp = &sc->pciide_channels[channel];
		sc->wdc_chanarray[channel] = &cp->ata_channel;

		cp->ih = sc->sc_pci_ih;
		cp->name = NULL;
		cp->ata_channel.ch_channel = channel;
		cp->ata_channel.ch_atac = &sc->sc_wdcdev.sc_atac;
		cp->ata_channel.ch_queue =
		    malloc(sizeof(struct ata_queue), M_DEVBUF, M_NOWAIT);
		if (cp->ata_channel.ch_queue == NULL) {
			aprint_error("%s channel %d: "
			    "can't allocate memory for command queue\n",
			sc->sc_wdcdev.sc_atac.atac_dev.dv_xname, channel);
			goto next_channel;
		}
		wdc_cp = &cp->ata_channel;
		wdr = CHAN_TO_WDC_REGS(wdc_cp);

		wdr->ctl_iot = sc->sc_ba5_st;
		wdr->cmd_iot = sc->sc_ba5_st;

		if (bus_space_subregion(sc->sc_ba5_st, sc->sc_ba5_sh,
		    0x0238 + (channel << 7), 1, &wdr->ctl_ioh) != 0) {
			aprint_error("%s: couldn't map channel %d ctl regs\n",
			    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname,
			    channel);
			goto next_channel;
		}
		for (i = 0; i < WDC_NREG; i++) {
			if (bus_space_subregion(sc->sc_ba5_st, sc->sc_ba5_sh,
			    0x0200 + (i << 2) + (channel << 7), i == 0 ? 4 : 1,
			    &wdr->cmd_iohs[i]) != 0) {
				aprint_error("%s: couldn't map channel %d cmd "
				    "regs\n",
				    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname,
				    channel);
				goto next_channel;
			}
		}
		wdc_init_shadow_regs(wdc_cp);

		/*
		 * subregion de busmaster registers. They're spread all over
		 * the controller's register space :(. They are also 4 bytes
		 * sized, with some specific extentions in the extra bits.
		 * It also seems that the IDEDMA_CTL register isn't available.
		 */
		if (bus_space_subregion(sc->sc_ba5_st, sc->sc_ba5_sh,
		    0x260 + (channel << 7), 1,
		    &cp->dma_iohs[IDEDMA_CMD]) != 0) {
			aprint_normal("%s channel %d: can't subregion DMA "
			    "registers\n",
			    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname, channel);
			goto next_channel;
		}
		if (bus_space_subregion(sc->sc_ba5_st, sc->sc_ba5_sh,
		    0x244 + (channel << 7), 4,
		    &cp->dma_iohs[IDEDMA_TBL]) != 0) {
			aprint_normal("%s channel %d: can't subregion DMA "
			    "registers\n",
			    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname, channel);
			goto next_channel;
		}

		wdcattach(wdc_cp);
		bus_space_write_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD], 0,
		    (bus_space_read_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD],
			0) & ~0x00003f9f) | (channel + 1));
		bus_space_write_4(sc->sc_ba5_st, sc->sc_ba5_sh,
		    (channel + 1) << 2, 0x00000001);
next_channel:
	continue;
	}
	return;
}

static void
pdc203xx_setup_channel(struct ata_channel *chp)
{
	struct ata_drive_datas *drvp;
	int drive, s;
	struct pciide_channel *cp = CHAN_TO_PCHAN(chp);

	pciide_channel_dma_setup(cp);

	for (drive = 0; drive < 2; drive++) {
		drvp = &chp->ch_drive[drive];
		if ((drvp->drive_flags & DRIVE) == 0)
			continue;
		if (drvp->drive_flags & DRIVE_UDMA) {
			s = splbio();
			drvp->drive_flags &= ~DRIVE_DMA;
			splx(s);
		}
	}
}

static int
pdc203xx_pci_intr(void *arg)
{
	struct pciide_softc *sc = arg;
	struct pciide_channel *cp;
	struct ata_channel *wdc_cp;
	int i, rv, crv;
	u_int32_t scr;

	rv = 0;
	scr = bus_space_read_4(sc->sc_ba5_st, sc->sc_ba5_sh, 0x00040);

	for (i = 0; i < sc->sc_wdcdev.sc_atac.atac_nchannels; i++) {
		cp = &sc->pciide_channels[i];
		wdc_cp = &cp->ata_channel;
		if (scr & (1 << (i + 1))) {
			crv = wdcintr(wdc_cp);
			if (crv == 0) {
				printf("%s:%d: bogus intr (reg 0x%x)\n",
				    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname,
				    i, scr);
			} else
				rv = 1;
		}
	}
	return rv;
}

static void
pdc203xx_irqack(struct ata_channel *chp)
{
	struct pciide_channel *cp = CHAN_TO_PCHAN(chp);
	struct pciide_softc *sc = CHAN_TO_PCIIDE(chp);

	bus_space_write_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD], 0,
	    (bus_space_read_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD],
		0) & ~0x00003f9f) | (cp->ata_channel.ch_channel + 1));
	bus_space_write_4(sc->sc_ba5_st, sc->sc_ba5_sh,
	    (cp->ata_channel.ch_channel + 1) << 2, 0x00000001);
}

static int
pdc203xx_dma_init(void *v, int channel, int drive, void *databuf,
    size_t datalen, int flags)
{
	struct pciide_softc *sc = v;

	return pciide_dma_dmamap_setup(sc, channel, drive,
	    databuf, datalen, flags);
}

static void
pdc203xx_dma_start(void *v, int channel, int drive)
{
	struct pciide_softc *sc = v;
	struct pciide_channel *cp = &sc->pciide_channels[channel];
	struct pciide_dma_maps *dma_maps = &cp->dma_maps[drive];

	/* Write table addr */
	bus_space_write_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_TBL], 0,
	    dma_maps->dmamap_table->dm_segs[0].ds_addr);
	/* start DMA engine */
	bus_space_write_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD], 0,
	    (bus_space_read_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD],
	    0) & ~0xc0) | ((dma_maps->dma_flags & WDC_DMA_READ) ? 0x80 : 0xc0));
}

static int
pdc203xx_dma_finish(void *v, int channel, int drive, int force)
{
	struct pciide_softc *sc = v;
	struct pciide_channel *cp = &sc->pciide_channels[channel];
	struct pciide_dma_maps *dma_maps = &cp->dma_maps[drive];

	/* stop DMA channel */
	bus_space_write_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD], 0,
	    (bus_space_read_4(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CMD],
	    0) & ~0x80));

	/* Unload the map of the data buffer */
	bus_dmamap_sync(sc->sc_dmat, dma_maps->dmamap_xfer, 0,
	    dma_maps->dmamap_xfer->dm_mapsize,
	    (dma_maps->dma_flags & WDC_DMA_READ) ?
	    BUS_DMASYNC_POSTREAD : BUS_DMASYNC_POSTWRITE);
	bus_dmamap_unload(sc->sc_dmat, dma_maps->dmamap_xfer);

	return 0;
}
