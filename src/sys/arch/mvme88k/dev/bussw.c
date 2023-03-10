/*	$OpenBSD: bussw.c,v 1.17 2006/05/08 14:36:10 miod Exp $ */
/*
 * Copyright (c) 1999 Steve Murphree, Jr.
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
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/systm.h>

#include <machine/autoconf.h>
#include <machine/cpu.h>

#include <mvme88k/dev/busswreg.h>

struct bussw_softc {
	struct device		sc_dev;
	struct intrhand 	sc_abih;	/* `abort' switch */
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
};

static void	bussw_attach(struct device *, struct device *, void *);
static int	bussw_match(struct device *, struct cfdata *, void *);
static int	bussw_print(void *, const char *);
static int	bussw_scan(struct device *, struct cfdata *, void *);
static int	busswabort(void *);
static int	busswintr_establish(int, struct intrhand *, const char *);

extern struct cfdriver bussw_cd;

CFATTACH_DECL(bussw,
	      sizeof (struct bussw_softc),
	      bussw_match,
	      bussw_attach,
	      NULL,
	      NULL);

int
bussw_match(struct device *parent, struct cfdata *vcf, void *args)
{
	struct confargs *ca = args;
	bus_space_handle_t ioh;
	int rc;
	u_int8_t chipid;

	/* Don't match if wrong cpu */
	if (brdtyp != BRD_197)
		return 0;

	if (bus_space_map(ca->ca_iot, ca->ca_paddr, BS_SIZE, 0, &ioh) != 0)
		return 0;
	rc = bus_space_peek_1(ca->ca_iot, ioh, BS_CHIPID, &chipid);
	bus_space_unmap(ca->ca_iot, ioh, BS_SIZE);

	if (rc == 0 && chipid != BUSSWITCH_ID) {
#ifdef DEBUG
		printf("==> busswitch: wrong chip id %x\n", chipid);
#endif
		return -1;
	}

	return 1;
}

void
bussw_attach(struct device *parent, struct device *self, void *args)
{
	struct confargs *ca = args;
	struct bussw_softc *sc = (struct bussw_softc *)self;
	bus_space_handle_t ioh;

	if (bus_space_map(ca->ca_iot, ca->ca_paddr, BS_SIZE, 0, &ioh) != 0) {
		printf(": can't map registers!\n");
		return;
	}

	sc->sc_iot = ca->ca_iot;
	sc->sc_ioh = ioh;

	bus_space_write_1(sc->sc_iot, ioh, BS_VBASE,
	    bus_space_read_1(sc->sc_iot, ioh, BS_VBASE) | BS_VECBASE);
	bus_space_write_2(sc->sc_iot, ioh, BS_GCSR,
	    bus_space_read_2(sc->sc_iot, ioh, BS_GCSR) | BS_GCSR_XIPL);

	printf(": BusSwitch ASIC, Rev %x\n",
	    bus_space_read_1(sc->sc_iot, ioh, BS_CHIPREV));

	/*
	 * pseudo driver, abort interrupt handler
	 */
	sc->sc_abih.ih_fn = busswabort;
	sc->sc_abih.ih_arg = 0;
	sc->sc_abih.ih_wantframe = 1;
	sc->sc_abih.ih_ipl = IPL_NMI;

	busswintr_establish(BS_ABORTIRQ, &sc->sc_abih, "abort");
	bus_space_write_1(sc->sc_iot, ioh, BS_ABORT,
	    bus_space_read_4(sc->sc_iot, ioh, BS_ABORT) | BS_ABORT_IEN);

	config_search(bussw_scan, self, args);
}

int
bussw_print(void *args, const char *bus)
{
	struct confargs *ca = args;

	if (ca->ca_offset != -1)
		printf(" offset %#x", ca->ca_offset);
	if (ca->ca_ipl > 0)
		printf(" ipl %d", ca->ca_ipl);
	return (UNCONF);
}

int
bussw_scan(struct device *parent, struct cfdata *cf, void *args)
{
	struct confargs oca, *ca = args;

	bzero(&oca, sizeof oca);
	oca.ca_iot    = ca->ca_iot;
	oca.ca_dmat   = ca->ca_dmat;
	oca.ca_offset = cf->cf_loc[0];
	oca.ca_ipl    = cf->cf_loc[1];
	if (oca.ca_offset != -1) {
		oca.ca_paddr = ca->ca_paddr + oca.ca_offset;
	} else {
		oca.ca_paddr = -1;
	}
	oca.ca_bustype = BUS_BUSSWITCH;
	oca.ca_name = cf->cf_name;
	if (config_match(parent, cf, &oca) == 0)
		return (0);
	config_attach(parent, cf, &oca, bussw_print);
	return (1);
}

int
busswintr_establish(int vec, struct intrhand *ih, const char *name)
{
#ifdef DIAGNOSTIC
	if (vec < 0 || vec >= BS_NVEC)
		panic("busswintr_establish: illegal vector 0x%x", vec);
#endif

	return intr_establish(BS_VECBASE + vec, ih, name);
}

int
busswabort(void *eframe)
{
	struct frame *frame = eframe;

	struct bussw_softc *sc = (struct bussw_softc *)bussw_cd.cd_devs[0];
	u_int8_t abort;

	abort = bus_space_read_1(sc->sc_iot, sc->sc_ioh, BS_ABORT);
	if (abort & BS_ABORT_INT) {
		bus_space_write_1(sc->sc_iot, sc->sc_ioh, BS_ABORT,
		    abort | BS_ABORT_ICLR);
		nmihand(frame);
		return 1;
	}
	return 0;
}
