/*	$OpenBSD: nvram.c,v 1.29 2006/06/19 15:13:35 deraadt Exp $ */

/*
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
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include <machine/autoconf.h>
#include <machine/bugio.h>
#include <machine/conf.h>
#include <machine/cpu.h>
#include <machine/mioctl.h>
#include <machine/psl.h>
#include <machine/vmparam.h>

#include <uvm/uvm_param.h>

#include <mvme88k/dev/memdevs.h>
#include <mvme88k/dev/nvramreg.h>
#include <mvme88k/dev/pcctworeg.h>

struct nvramsoftc {
	struct device           sc_dev;
	paddr_t			sc_base;
	bus_space_tag_t		sc_iot;
	bus_space_handle_t	sc_ioh;
	bus_addr_t		sc_regs;
	size_t			sc_len;

#ifdef MVME188
	u_int8_t		*sc_nvram;
#endif
};

void    nvramattach(struct device *, struct device *, void *);
int     nvrammatch(struct device *, void *, void *);

struct cfattach nvram_ca = {
	sizeof(struct nvramsoftc), nvrammatch, nvramattach
};

struct cfdriver nvram_cd = {
	NULL, "nvram", DV_DULL
};

u_long	chiptotime(int, int, int, int, int, int);
int	nvram188read(struct nvramsoftc *, struct uio *, int);
int	nvram188write(struct nvramsoftc *, struct uio *, int);

int
nvrammatch(parent, vcf, args)
	struct device *parent;
	void *vcf, *args;
{
	struct confargs *ca = args;
	bus_space_handle_t ioh;
	int rc;

	if (bus_space_map(ca->ca_iot, ca->ca_paddr, PAGE_SIZE, 0, &ioh) != 0)
		return (0);
	rc = badaddr((vaddr_t)bus_space_vaddr(ca->ca_iot, ioh), 1) == 0;
	bus_space_unmap(ca->ca_iot, ioh, PAGE_SIZE);
	return (rc);
}

void
nvramattach(parent, self, args)
	struct device *parent, *self;
	void *args;
{
	struct confargs *ca = args;
	struct nvramsoftc *sc = (struct nvramsoftc *)self;
	bus_space_handle_t ioh;
	vsize_t maplen;

	switch (brdtyp) {
#ifdef MVME188
	case BRD_188:
		sc->sc_len = MK48T02_SIZE;
		maplen = sc->sc_len * 4;
		sc->sc_regs = M188_NVRAM_TOD_OFF;
		break;
#endif
	default:
		sc->sc_len = MK48T08_SIZE;
		maplen = sc->sc_len;
		sc->sc_regs = SBC_NVRAM_TOD_OFF;
		break;
	}

	sc->sc_iot = ca->ca_iot;
	sc->sc_base = ca->ca_paddr;

	if (bus_space_map(sc->sc_iot, sc->sc_base, round_page(maplen),
	    BUS_SPACE_MAP_LINEAR, &ioh) != 0) {
		printf(": can't map memory!\n");
		return;
	}

	sc->sc_ioh = ioh;

	printf(": MK48T0%d\n", sc->sc_len / 1024);
}

/*
 * Return the best possible estimate of the time in the timeval
 * to which tvp points.  We do this by returning the current time
 * plus the amount of time since the last clock interrupt (clock.c:clkread).
 *
 * Check that this time is no less than any previously-reported time,
 * which could happen around the time of a clock adjustment.  Just for fun,
 * we guarantee that the time will be greater than the value obtained by a
 * previous call.
 */
void
microtime(tvp)
	struct timeval *tvp;
{
	int s = splhigh();
	static struct timeval lasttime;

	*tvp = time;
	while (tvp->tv_usec >= 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
	if (tvp->tv_sec == lasttime.tv_sec &&
	    tvp->tv_usec <= lasttime.tv_usec &&
	    (tvp->tv_usec = lasttime.tv_usec + 1) >= 1000000) {
		tvp->tv_sec++;
		tvp->tv_usec -= 1000000;
	}
	lasttime = *tvp;
	splx(s);
}

/*
 * BCD to decimal and decimal to BCD.
 */
#define	FROMBCD(x)	(((x) >> 4) * 10 + ((x) & 0xf))
#define	TOBCD(x)	(((x) / 10 * 16) + ((x) % 10))

#define	SECYR		(SECDAY * 365)
#define	LEAPYEAR(y)	(((y) & 3) == 0)

/*
 * This code is defunct after 2068.
 * Will Unix still be here then??
 */
const int dayyr[12] =
{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

u_long
chiptotime(sec, min, hour, day, mon, year)
	int sec, min, hour, day, mon, year;
{
	int days, yr;

	sec = FROMBCD(sec);
	min = FROMBCD(min);
	hour = FROMBCD(hour);
	day = FROMBCD(day);
	mon = FROMBCD(mon);
	year = FROMBCD(year) + YEAR0;

	/* simple sanity checks */
	if (year>164 || mon<1 || mon>12 || day<1 || day>31)
		return (0);
	yr = 70;
	days = 0;

	if (year < 70) {		/* 2000 <= year */
		for (; yr < 100; yr++)	/* deal with first 30 years */
			days += LEAPYEAR(yr) ? 366 : 365;
		yr = 0;
	}

	for (; yr < year; yr++)		/* deal with years left */
		days += LEAPYEAR(yr) ? 366 : 365;

	days += dayyr[mon - 1] + day - 1;

	if (LEAPYEAR(yr) && mon > 2)
		days++;

	/* now have days since Jan 1, 1970; the rest is easy... */
	return (days * SECDAY + hour * 3600 + min * 60 + sec);
}

struct chiptime {
	int     sec;
	int     min;
	int     hour;
	int     wday;
	int     day;
	int     mon;
	int     year;
};

void timetochip(struct chiptime *c);

void
timetochip(c)
	struct chiptime *c;
{
	int t, t2, t3, now = time.tv_sec;

	/* January 1 1970 was a Thursday (4 in unix wdays) */
	/* compute the days since the epoch */
	t2 = now / SECDAY;

	t3 = (t2 + 4) % 7;	/* day of week */
	c->wday = TOBCD(t3 + 1);

	/* compute the year */
	t = 69;
	while (t2 >= 0) {	/* whittle off years */
		t3 = t2;
		t++;
		t2 -= LEAPYEAR(t) ? 366 : 365;
	}
	c->year = t;

	/* t3 = month + day; separate */
	t = LEAPYEAR(t);
	for (t2 = 1; t2 < 12; t2++)
		if (t3 < (dayyr[t2] + ((t && (t2 > 1)) ? 1:0)))
			break;

	/* t2 is month */
	c->mon = t2;
	c->day = t3 - dayyr[t2 - 1] + 1;
	if (t && t2 > 2)
		c->day--;

	/* the rest is easy */
	t = now % SECDAY;
	c->hour = t / 3600;
	t %= 3600;
	c->min = t / 60;
	c->sec = t % 60;

	c->sec = TOBCD(c->sec);
	c->min = TOBCD(c->min);
	c->hour = TOBCD(c->hour);
	c->day = TOBCD(c->day);
	c->mon = TOBCD(c->mon);
	c->year = TOBCD((c->year - YEAR0) % 100);
}

/*
 * Set up the system's time, given a `reasonable' time value.
 */

void
inittodr(base)
	time_t base;
{
	struct nvramsoftc *sc = (struct nvramsoftc *) nvram_cd.cd_devs[0];
	int sec, min, hour, day, mon, year;
	int badbase = 0, waszero = base == 0;

	if (base < 35 * SECYR) {
		/*
		 * If base is 0, assume filesystem time is just unknown
		 * in stead of preposterous. Don't bark.
		 */
		if (base != 0)
			printf("WARNING: preposterous time in file system\n");
		/* not going to use it anyway, if the chip is readable */
		base = 36 * SECYR + 109 * SECDAY + 22 * 3600;
		badbase = 1;
	}

	if (brdtyp == BRD_188) {
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_CSR << 2), CLK_READ |
		    bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + (CLK_CSR << 2)));
		sec = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_SEC << 2)) & 0xff;
		min = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_MIN << 2)) & 0xff;
		hour = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_HOUR << 2)) & 0xff;
		day = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_DAY << 2)) & 0xff;
		mon = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_MONTH << 2)) & 0xff;
		year = bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_YEAR << 2)) & 0xff;
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_CSR << 2),
		    bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + (CLK_CSR << 2)) & ~CLK_READ);
	} else {
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_CSR, CLK_READ |
		    bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + CLK_CSR));
		sec = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_SEC);
		min = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_MIN);
		hour = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_HOUR);
		day = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_DAY);
		mon = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_MONTH);
		year = bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_YEAR);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_CSR,
		    bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + CLK_CSR) & ~CLK_READ);
	}

	if ((time.tv_sec = chiptotime(sec, min, hour, day, mon, year)) == 0) {
		printf("WARNING: bad date in nvram");
#ifdef DEBUG
		printf("\nday = %d, mon = %d, year = %d, hour = %d, min = %d, sec = %d",
		       FROMBCD(day), FROMBCD(mon), FROMBCD(year) + YEAR0,
		       FROMBCD(hour), FROMBCD(min), FROMBCD(sec));
#endif
		/*
		 * Believe the time in the file system for lack of
		 * anything better, resetting the clock.
		 */
		time.tv_sec = base;
		if (!badbase)
			resettodr();
	} else {
		int deltat = time.tv_sec - base;

		if (deltat < 0)
			deltat = -deltat;
		if (waszero || deltat < 2 * SECDAY)
			return;
		printf("WARNING: clock %s %d days",
		       time.tv_sec < base ? "lost" : "gained", deltat / SECDAY);
	}
	printf(" -- CHECK AND RESET THE DATE!\n");
}

/*
 * Reset the clock based on the current time.
 * Used when the current clock is preposterous, when the time is changed,
 * and when rebooting.  Do nothing if the time is not yet known, e.g.,
 * when crashing during autoconfig.
 */
void
resettodr()
{
	struct nvramsoftc *sc = (struct nvramsoftc *) nvram_cd.cd_devs[0];
	struct chiptime c;

	if (!time.tv_sec || sc == NULL)
		return;
	timetochip(&c);

	if (brdtyp == BRD_188) {
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_CSR << 2), CLK_WRITE |
		    bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + (CLK_CSR << 2)));
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_SEC << 2), c.sec);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_MIN << 2), c.min);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_HOUR << 2), c.hour);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_WDAY << 2), c.wday);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_DAY << 2), c.day);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_MONTH << 2), c.mon);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_YEAR << 2), c.year);
		bus_space_write_4(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + (CLK_CSR << 2),
		    bus_space_read_4(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + (CLK_CSR << 2)) & ~CLK_WRITE);
	} else {
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_CSR, CLK_WRITE |
		    bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + CLK_CSR));
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_SEC, c.sec);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_MIN, c.min);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_HOUR, c.hour);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_WDAY, c.wday);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_DAY, c.day);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_MONTH, c.mon);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_YEAR, c.year);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh,
		    sc->sc_regs + CLK_CSR,
		    bus_space_read_1(sc->sc_iot, sc->sc_ioh,
		      sc->sc_regs + CLK_CSR) & ~CLK_WRITE);
	}
}

/*ARGSUSED*/
int
nvramopen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	if (minor(dev) >= nvram_cd.cd_ndevs ||
	    nvram_cd.cd_devs[minor(dev)] == NULL)
		return (ENODEV);

	return (0);
}

/*ARGSUSED*/
int
nvramclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
	/*
	 * On MVME188, it might be worth free()ing the NVRAM copy here.
	 */
	return (0);
}

/*ARGSUSED*/
int
nvramioctl(dev, cmd, data, flag, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flag;
	struct proc *p;
{
	int unit = minor(dev);
	struct nvramsoftc *sc = (struct nvramsoftc *) nvram_cd.cd_devs[unit];
	int error = 0;

	switch (cmd) {
	case MIOCGSIZ:
		*(int *)data = sc->sc_len;
		break;
	default:
		error = ENOTTY;
		break;
	}
	return (error);
}

/*ARGSUSED*/
int
nvramread(dev_t dev, struct uio *uio, int flags)
{
	int unit = minor(dev);
	struct nvramsoftc *sc = (struct nvramsoftc *)nvram_cd.cd_devs[unit];

#ifdef MVME188
	if (brdtyp == BRD_188)
		return (nvram188read(sc, uio, flags));
#endif

	return (memdevrw(bus_space_vaddr(sc->sc_iot, sc->sc_ioh),
	    sc->sc_len, uio, flags));
}

/*ARGSUSED*/
int
nvramwrite(dev_t dev, struct uio *uio, int flags)
{
	int unit = minor(dev);
	struct nvramsoftc *sc = (struct nvramsoftc *) nvram_cd.cd_devs[unit];

#ifdef MVME188
	if (brdtyp == BRD_188)
		return (nvram188write(sc, uio, flags));
#endif

	return (memdevrw(bus_space_vaddr(sc->sc_iot, sc->sc_ioh),
	    sc->sc_len, uio, flags));
}

paddr_t
nvrammmap(dev, off, prot)
	dev_t dev;
	off_t off;
	int prot;
{
	int unit = minor(dev);
	struct nvramsoftc *sc = (struct nvramsoftc *) nvram_cd.cd_devs[unit];

	if (minor(dev) != 0)
		return (-1);

#ifdef MVME188
	/* disallow mmap on MVME188 due to non-linear layout */
	if (brdtyp == BRD_188)
		return (-1);
#endif

	/* allow access only in RAM */
	if (off < 0 || off > sc->sc_len)
		return (-1);
	return (atop(sc->sc_base + off));
}

#ifdef MVME188

int	read_nvram(struct nvramsoftc *);

/*
 * Build a local copy of the NVRAM contents.
 */
int
read_nvram(struct nvramsoftc *sc)
{
	u_int cnt;
	u_int8_t *dest;
	u_int32_t *src;

	if (sc->sc_nvram == NULL) {
		sc->sc_nvram = (u_int8_t *)malloc(sc->sc_len, M_DEVBUF, 
		    M_WAITOK | M_CANFAIL);
		if (sc->sc_nvram == NULL)
			return (EAGAIN);
	}

	dest = sc->sc_nvram;
	src = (u_int32_t *)bus_space_vaddr(sc->sc_iot, sc->sc_ioh);
	cnt = sc->sc_len;
	while (cnt-- != 0)
		*dest++ = (u_int8_t)*src++;

	return (0);
}

/*
 * Specific memdevrw wrappers to cope with the 188 design.
 */

int
nvram188read(struct nvramsoftc *sc, struct uio *uio, int flags)
{
	int rc;

	/*
	 * Get a copy of the NVRAM contents.
	 */
	rc = read_nvram(sc);
	if (rc != 0)
		return (rc);

	/*
	 * Move data from our NVRAM copy to the user.
	 */
	return (memdevrw(sc->sc_nvram, sc->sc_len, uio, flags));
}

int
nvram188write(struct nvramsoftc *sc, struct uio *uio, int flags)
{
	u_int cnt;
	u_int8_t *src;
	u_int32_t *dest;
	int rc;

	/*
	 * Get a copy of the NVRAM contents.
	 */
	rc = read_nvram(sc);
	if (rc != 0)
		return (rc);

	/*
	 * Move data from the user to our NVRAM copy.
	 */
	rc = memdevrw(sc->sc_nvram, sc->sc_len, uio, flags);
	if (rc != 0) {
		/* reset NVRAM copy contents */
		read_nvram(sc);
		return (rc);
	}

	/*
	 * Update the NVRAM. This could be optimized by only working on
	 * the areas which have been modified by the user.
	 */
	src = sc->sc_nvram;
	dest = (u_int32_t *)bus_space_vaddr(sc->sc_iot, sc->sc_ioh);
	cnt = sc->sc_len;
	while (cnt-- != 0) {
		if ((*dest & 0xff) != *src)
			*dest = (u_int32_t)*src;
		dest++;
		src++;
	}

	return (0);
}
#endif
