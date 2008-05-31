/*-
 * Copyright (c) 2002-2004 Sam Leffler, Errno Consulting, Atheros
 * Communications, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the following conditions are met:
 * 1. The materials contained herein are unmodified and are used
 *    unmodified.
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following NO
 *    ''WARRANTY'' disclaimer below (''Disclaimer''), without
 *    modification.
 * 3. Redistributions in binary form must reproduce at minimum a
 *    disclaimer similar to the Disclaimer below and any redistribution
 *    must be conditioned upon including a substantially similar
 *    Disclaimer requirement for further binary redistribution.
 * 4. Neither the names of the above-listed copyright holders nor the
 *    names of any contributors may be used to endorse or promote
 *    product derived from this software without specific prior written
 *    permission.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT,
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
 * FOR SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES.
 *
 * $NetBSD$
 * $Id: ah_osdep.c,v 1.3 2004/06/09 16:33:48 samleffler Exp $
 */
#ifdef __FreeBSD__
#include "opt_ah.h"
#endif
#ifdef __NetBSD__
#include <../contrib/sys/arch/i386/dev/athhal_opt.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <machine/bus.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <machine/stdarg.h>
#include <machine/param.h>

#include <net/if.h>
#include <net/if_ether.h>		/* XXX for ether_sprintf */

#include <../contrib/sys/dev/ic/athhal.h>

#define	AH_TIMEOUT	1000

#ifdef bcopy
#undef bcopy
#endif

#ifdef bzero
#undef bzero
#endif

#ifdef __NetBSD__
#define __va_list va_list
#define __printflike(x, y)
#endif

extern	void ath_hal_delay(int);
extern	u_int32_t ath_hal_getuptime(struct ath_hal *);
void	bcopy(const void *, void *, size_t);
void	bzero(void *, size_t);

extern	HAL_BOOL ath_hal_wait(struct ath_hal *, u_int reg,
		u_int32_t mask, u_int32_t val);
extern	void ath_hal_printf(struct ath_hal *, const char*, ...)
		__printflike(2,3);
extern	void ath_hal_vprintf(struct ath_hal *, const char*, __va_list)
		__printflike(2, 0);
extern	const char* ath_hal_ether_sprintf(const u_int8_t *mac);
extern	void *ath_hal_malloc(size_t);
extern	void ath_hal_free(void *);
#ifdef AH_ASSERT
extern	void ath_hal_assert_failed(const char* filename,
		int lineno, const char* msg);
#endif

#ifdef AH_DEBUG
static	int ath_hal_debug = 0;
#endif /* AH_DEBUG */

#ifdef __FreeBSD__
SYSCTL_STRING(_hw_ath_hal, OID_AUTO, version, CTLFLAG_RD, ath_hal_version, 0,
	"Atheros HAL version");
#endif /* __FreeBSD__ */

int	ath_hal_dma_beacon_response_time = 2;	/* in TU's */
int	ath_hal_sw_beacon_response_time = 10;	/* in TU's */
int	ath_hal_additional_swba_backoff = 0;	/* in TU's */

#ifdef __NetBSD__
/*
 * Setup sysctl(3) MIB, ath.hal.*.
 *
 * TBD condition SYSCTL_PERMANENT on being an LKM or not
 */
SYSCTL_SETUP(sysctl_ath_hal, "sysctl ath.hal subtree setup")
{
	int rc;
#ifdef AH_DEBUG
	int debug_nodenum;
#endif /* AH_DEBUG */
	int ath_nodenum, hal_nodenum, version_nodenum;
	struct sysctlnode *node;

	if ((rc = sysctl_createv(clog, 0, NULL, NULL,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE, "hw", NULL,
	    NULL, 0, NULL, 0, CTL_HW, CTL_EOL)) != 0)
		goto err;

	if ((rc = sysctl_createv(clog, 0, NULL, &node,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE, "ath", NULL,
	    NULL, 0, NULL, 0, CTL_HW, CTL_CREATE, CTL_EOL)) != 0)
		goto err;

	ath_nodenum = node->sysctl_num;

	if ((rc = sysctl_createv(clog, 0, NULL, &node,
	    CTLFLAG_PERMANENT, CTLTYPE_NODE, "hal", NULL,
	    NULL, 0, NULL, 0, CTL_HW, ath_nodenum, CTL_CREATE,
	    CTL_EOL)) != 0)
		goto err;

	hal_nodenum = node->sysctl_num;

	/* HAL version */
	if ((rc = sysctl_createv(clog, 0, NULL, &node,
	    CTLFLAG_PERMANENT|CTLFLAG_READONLY,
	    CTLTYPE_STRING, "version", NULL, NULL, 0, &ath_hal_version, 0,
	    CTL_HW, ath_nodenum, hal_nodenum, CTL_CREATE, CTL_EOL)) != 0)
		goto err;

	version_nodenum = node->sysctl_num;

#ifdef AH_DEBUG

	/* control debugging printfs */
	if ((rc = sysctl_createv(clog, 0, NULL, &node,
	    CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
	    CTLTYPE_INT, "debug", NULL, NULL, 0, &ath_hal_debug, 0,
	    CTL_HW, ath_nodenum, hal_nodenum, CTL_CREATE, CTL_EOL)) != 0)
		goto err;

	debug_nodenum = node->sysctl_num;

#endif /* AH_DEBUG */
	return;
err:
	printf("%s: sysctl_createv failed (rc = %d)\n", __func__, rc);
}
#endif /* __NetBSD__ */

/*
 * Print/log message support.
 */

void
ath_hal_vprintf(struct ath_hal *ah, const char* fmt, va_list ap)
{
	vprintf(fmt, ap);
}

void
ath_hal_printf(struct ath_hal *ah, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	ath_hal_vprintf(ah, fmt, ap);
	va_end(ap);
}

/*
 * Format an Ethernet MAC for printing.
 */
const char*
ath_hal_ether_sprintf(const u_int8_t *mac)
{
	return ether_sprintf(mac);
}

#ifdef AH_ASSERT
void
ath_hal_assert_failed(const char* filename, int lineno, const char *msg)
{
	printf("Atheros HAL assertion failure: %s: line %u: %s\n",
		filename, lineno, msg);
	panic("ath_hal_assert");
}
#endif /* AH_ASSERT */

/*
 * Memory-mapped device register read/write.  These are here
 * as routines when debugging support is enabled and/or when
 * explicitly configured to use function calls.  The latter is
 * for architectures that might need to do something before
 * referencing memory (e.g. remap an i/o window).
 *
 * NB: see the comments in ah_osdep.h about byte-swapping register
 *     reads and writes to understand what's going on below.
 */
#if defined(AH_DEBUG) || defined(AH_REGOPS_FUNC)
void
ath_hal_reg_write(struct ath_hal *ah, u_int32_t reg, u_int32_t val)
{
#if _BYTE_ORDER == _BIG_ENDIAN
	if (reg >= 0x4000 && reg < 0x5000)
		bus_space_write_4(ah->ah_st, ah->ah_sh, reg, htole32(val));
	else
#endif
		bus_space_write_4(ah->ah_st, ah->ah_sh, reg, val);
}

u_int32_t
ath_hal_reg_read(struct ath_hal *ah, u_int32_t reg)
{
	u_int32_t val;

	val = bus_space_read_4(ah->ah_st, ah->ah_sh, reg);
#if _BYTE_ORDER == _BIG_ENDIAN
	if (reg >= 0x4000 && reg < 0x5000)
		val = le32toh(val);
#endif
	return val;
}
#endif /* AH_DEBUG || AH_REGOPS_FUNC */

#ifdef AH_DEBUG
void
HALDEBUG(struct ath_hal *ah, const char* fmt, ...)
{
	if (ath_hal_debug) {
		va_list ap;
		va_start(ap, fmt);
		ath_hal_vprintf(ah, fmt, ap);
		va_end(ap);
	}
}


void
HALDEBUGn(struct ath_hal *ah, u_int level, const char* fmt, ...)
{
	if (ath_hal_debug >= level) {
		va_list ap;
		va_start(ap, fmt);
		ath_hal_vprintf(ah, fmt, ap);
		va_end(ap);
	}
}
#endif /* AH_DEBUG */

/*
 * Delay n microseconds.
 */
void
ath_hal_delay(int n)
{
	DELAY(n);
}

u_int32_t
ath_hal_getuptime(struct ath_hal *ah)
{
	struct timeval tv;
	int s;
	s = splclock();
	tv = mono_time;
	splx(s);

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/*
 * Allocate/free memory.
 */

void *
ath_hal_malloc(size_t size)
{
	return malloc(size, M_DEVBUF, M_NOWAIT | M_ZERO);
}

void
ath_hal_free(void* p)
{
	free(p, M_DEVBUF);
}

void *
ath_hal_memcpy(void *dst, const void *src, size_t n)
{
	return memcpy(dst, src, n);
}

void
bcopy(const void *src, void *dst, size_t len)
{
	(void)memmove(dst, src, len);
}

void
bzero(void *dst, size_t len)
{
	(void)memset(dst, 0, len);
}
