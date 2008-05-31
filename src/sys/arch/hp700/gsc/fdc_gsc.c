/*	$NetBSD: fdc_gsc.c,v 1.5 2003/11/20 03:58:47 chs Exp $	*/

/*	$OpenBSD: fdc_gsc.c,v 1.1 1998/09/30 04:45:46 mickey Exp $	*/

/*
 * Copyright (c) 1998 Michael Shalayeff
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
 *	This product includes software developed by Michael Shalayeff.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: fdc_gsc.c,v 1.5 2003/11/20 03:58:47 chs Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <machine/iomod.h>
#include <machine/bus.h>
#include <machine/intr.h>
#include <machine/autoconf.h>

#include <dev/ic/fdreg.h>
#include <dev/ic/fdlink.h>

#include <hp700/dev/cpudevs.h>

/* controller driver configuration */
int fdc_gsc_probe(struct device *, void *, void *);
void fdc_gsc_attach(struct device *, struct device *, void *);

CFATTACH_DECL(fdc_gsc, sizeof(struct fdc_softc),
    fdc_gsc_probe, fdc_gsc_attach, NULL, NULL);

int
fdc_gsc_probe(struct device *parent, void *match, void *aux)
{
	struct confargs *ca = aux;
	bus_space_handle_t ioh;
	int rv;

	if (ca->ca_type.iodc_type != HPPA_TYPE_FIO ||
	    ca->ca_type.iodc_sv_model != HPPA_FIO_GPCFD)
		return 0;

	/* Map the i/o space. */
	if (bus_space_map(ca->ca_iot, ca->ca_hpa, IOMOD_HPASIZE, 0, &ioh))
		return 0;

	rv = fdcprobe1(ca->ca_iot, ioh | IOMOD_DEVOFFSET);
	bus_space_unmap(ca->ca_iot, ioh, IOMOD_HPASIZE);
	return rv;
}

void
fdc_gsc_attach(struct device *parent, struct device *self, void *aux)
{
	struct fdc_softc *sc = (void *)self;
	bus_space_handle_t ioh;
	struct confargs *ca = aux;

	/* Re-map the I/O space. */
	if (bus_space_map(ca->ca_iot, ca->ca_hpa, IOMOD_HPASIZE, 0, &ioh))
		panic("fdcattach: couldn't map I/O ports");

	ioh |= IOMOD_DEVOFFSET;
	sc->sc_iot = ca->ca_iot;
	sc->sc_ioh = ioh;
	sc->sc_ioh_ctl = ioh + FDCTL_OFFSET;

	fdc_attach_subr(sc);
}

