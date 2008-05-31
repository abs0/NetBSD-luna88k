/*	$NetBSD: tc.c,v 1.40 2005/02/04 02:10:48 perry Exp $	*/

/*
 * Copyright (c) 1994, 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: tc.c,v 1.40 2005/02/04 02:10:48 perry Exp $");

#include "opt_tcverbose.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <dev/tc/tcreg.h>
#include <dev/tc/tcvar.h>
#include <dev/tc/tcdevs.h>

#include "locators.h"

/* Definition of the driver for autoconfig. */
int	tcmatch(struct device *, struct cfdata *, void *);

CFATTACH_DECL(tc, sizeof(struct tc_softc),
    tcmatch, tcattach, NULL, NULL);

extern struct cfdriver tc_cd;

int	tcprint(void *, const char *);
int	tcsubmatch(struct device *, struct cfdata *,
			const locdesc_t *, void *);
void	tc_devinfo(const char *, char *, size_t);

int
tcmatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct tcbus_attach_args *tba = aux;

	if (strcmp(tba->tba_busname, cf->cf_name))
		return (0);

	return (1);
}

void
tcattach(parent, self, aux)
	struct device *parent;
	struct device *self;
	void *aux;
{
	struct tc_softc *sc = (struct tc_softc *)self;
	struct tcbus_attach_args *tba = aux;
	struct tc_attach_args ta;
	const struct tc_builtin *builtin;
	struct tc_slotdesc *slot;
	tc_addr_t tcaddr;
	int i;
	int help[3];
	locdesc_t *ldesc = (void *)&help; /* XXX */

	printf(": %s MHz clock\n",
	    tba->tba_speed == TC_SPEED_25_MHZ ? "25" : "12.5");

	/*
	 * Save important CPU/chipset information.
	 */
	sc->sc_speed = tba->tba_speed;
	sc->sc_nslots = tba->tba_nslots;
	sc->sc_slots = tba->tba_slots;
	sc->sc_intr_evcnt = tba->tba_intr_evcnt;
	sc->sc_intr_establish = tba->tba_intr_establish;
	sc->sc_intr_disestablish = tba->tba_intr_disestablish;
	sc->sc_get_dma_tag = tba->tba_get_dma_tag;

	/*
	 * Try to configure each built-in device
	 */
	for (i = 0; i < tba->tba_nbuiltins; i++) {
		builtin = &tba->tba_builtins[i];

		/* sanity check! */
		if (builtin->tcb_slot > sc->sc_nslots)
			panic("tcattach: builtin %d slot > nslots", i);

		/*
		 * Make sure device is really there, because some
		 * built-in devices are really optional.
		 */
		tcaddr = sc->sc_slots[builtin->tcb_slot].tcs_addr +
		    builtin->tcb_offset;
		if (tc_badaddr(tcaddr))
			continue;

		/*
		 * Set up the device attachment information.
		 */
		strncpy(ta.ta_modname, builtin->tcb_modname, TC_ROM_LLEN);
		ta.ta_memt = tba->tba_memt;
		ta.ta_dmat = (*sc->sc_get_dma_tag)(builtin->tcb_slot);
		ta.ta_modname[TC_ROM_LLEN] = '\0';
		ta.ta_slot = builtin->tcb_slot;
		ta.ta_offset = builtin->tcb_offset;
		ta.ta_addr = tcaddr;
		ta.ta_cookie = builtin->tcb_cookie;
		ta.ta_busspeed = sc->sc_speed;

		/*
		 * Mark the slot as used, so we don't check it later.
		 */
		sc->sc_slots[builtin->tcb_slot].tcs_used = 1;

		ldesc->len = 2;
		ldesc->locs[TCCF_SLOT] = builtin->tcb_slot;
		ldesc->locs[TCCF_OFFSET] = builtin->tcb_offset;
		/*
		 * Attach the device.
		 */
		config_found_sm_loc(self, "tc", ldesc, &ta,
				    tcprint, tcsubmatch);
	}

	/*
	 * Try to configure each unused slot, last to first.
	 */
	for (i = sc->sc_nslots - 1; i >= 0; i--) {
		slot = &sc->sc_slots[i];

		/* If already checked above, don't look again now. */
		if (slot->tcs_used)
			continue;

		/*
		 * Make sure something is there, and find out what it is.
		 */
		tcaddr = slot->tcs_addr;
		if (tc_badaddr(tcaddr))
			continue;
		if (tc_checkslot(tcaddr, ta.ta_modname) == 0)
			continue;

		/*
		 * Set up the rest of the attachment information.
		 */
		ta.ta_memt = tba->tba_memt;
		ta.ta_dmat = (*sc->sc_get_dma_tag)(i);
		ta.ta_slot = i;
		ta.ta_offset = 0;
		ta.ta_addr = tcaddr;
		ta.ta_cookie = slot->tcs_cookie;

		/*
		 * Mark the slot as used.
		 */
		slot->tcs_used = 1;

		ldesc->len = 2;
		ldesc->locs[TCCF_SLOT] = i;
		ldesc->locs[TCCF_OFFSET] = 0;
		/*
		 * Attach the device.
		 */
		config_found_sm_loc(self, "tc", ldesc, &ta,
				    tcprint, tcsubmatch);
	}
}

int
tcprint(aux, pnp)
	void *aux;
	const char *pnp;
{
	struct tc_attach_args *ta = aux;
	char devinfo[256];

	if (pnp) {
		tc_devinfo(ta->ta_modname, devinfo, sizeof(devinfo));
		aprint_normal("%s at %s", devinfo, pnp);
	}
	aprint_normal(" slot %d offset 0x%x", ta->ta_slot, ta->ta_offset);
	return (UNCONF);
}

int
tcsubmatch(parent, cf, ldesc, aux)
	struct device *parent;
	struct cfdata *cf;
	const locdesc_t *ldesc;
	void *aux;
{

	if ((cf->cf_loc[TCCF_SLOT] != TCCF_SLOT_DEFAULT) &&
	    (cf->cf_loc[TCCF_SLOT] != ldesc->locs[TCCF_SLOT]))
		return 0;
	if ((cf->cf_loc[TCCF_OFFSET] != TCCF_OFFSET_DEFAULT) &&
	    (cf->cf_loc[TCCF_OFFSET] != ldesc->locs[TCCF_OFFSET]))
		return 0;

	return (config_match(parent, cf, aux));
}


#define	NTC_ROMOFFS	2
static tc_offset_t tc_slot_romoffs[NTC_ROMOFFS] = {
	TC_SLOT_ROM,
	TC_SLOT_PROTOROM,
};

int
tc_checkslot(slotbase, namep)
	tc_addr_t slotbase;
	char *namep;
{
	struct tc_rommap *romp;
	int i, j;

	for (i = 0; i < NTC_ROMOFFS; i++) {
		romp = (struct tc_rommap *)
		    (slotbase + tc_slot_romoffs[i]);

		switch (romp->tcr_width.v) {
		case 1:
		case 2:
		case 4:
			break;

		default:
			continue;
		}

		if (romp->tcr_stride.v != 4)
			continue;

		for (j = 0; j < 4; j++)
			if (romp->tcr_test[j+0*romp->tcr_stride.v] != 0x55 ||
			    romp->tcr_test[j+1*romp->tcr_stride.v] != 0x00 ||
			    romp->tcr_test[j+2*romp->tcr_stride.v] != 0xaa ||
			    romp->tcr_test[j+3*romp->tcr_stride.v] != 0xff)
				continue;

		for (j = 0; j < TC_ROM_LLEN; j++)
			namep[j] = romp->tcr_modname[j].v;
		namep[j] = '\0';
		return (1);
	}
	return (0);
}

const struct evcnt *
tc_intr_evcnt(struct device *dev, void *cookie)
{
	struct tc_softc *sc = tc_cd.cd_devs[0];

	return ((*sc->sc_intr_evcnt)(dev, cookie));
}

void
tc_intr_establish(dev, cookie, level, handler, arg)
	struct device *dev;
	void *cookie, *arg;
	int level;
	int (*handler)(void *);
{
	struct tc_softc *sc = tc_cd.cd_devs[0];

	(*sc->sc_intr_establish)(dev, cookie, level, handler, arg);
}

void
tc_intr_disestablish(dev, cookie)
	struct device *dev;
	void *cookie;
{
	struct tc_softc *sc = tc_cd.cd_devs[0];

	(*sc->sc_intr_disestablish)(dev, cookie);
}

#ifdef TCVERBOSE
/*
 * Descriptions of of known devices.
 */
struct tc_knowndev {
	const char *id, *driver, *description;
};

#include <dev/tc/tcdevs_data.h>
#endif /* TCVERBOSE */

void
tc_devinfo(id, cp, l)
	const char *id;
	char *cp;
	size_t l;
{
	const char *driver, *description;
#ifdef TCVERBOSE
	struct tc_knowndev *tdp;
	int match;
	const char *unmatched = "unknown ";
#else
	const char *unmatched = "";
#endif

	driver = NULL;
	description = id;

#ifdef TCVERBOSE
	/* find the device in the table, if possible. */
	tdp = tc_knowndevs;
	while (tdp->id != NULL) {
		/* check this entry for a match */
		match = !strcmp(tdp->id, id);
		if (match) {
			driver = tdp->driver;
			description = tdp->description;
			break;
		}
		tdp++;
	}
#endif

	if (driver == NULL)
		snprintf(cp, l, "%sdevice %s", unmatched, id);
	else
		snprintf(cp, l, "%s (%s)", driver, description);
}