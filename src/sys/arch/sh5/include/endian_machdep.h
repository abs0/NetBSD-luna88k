/*	$NetBSD: endian_machdep.h,v 1.4 2004/06/10 16:01:39 kleink Exp $	*/

#ifdef __LITTLE_ENDIAN__
#define _BYTE_ORDER     _LITTLE_ENDIAN
#else
#define _BYTE_ORDER     _BIG_ENDIAN
#endif

#ifdef	__GNUC__

#include <sh5/byte_swap.h>

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define	ntohl(x)	((uint32_t)_sh5_bswap32((uint32_t)(x)))
#define	ntohs(x)	((uint16_t)_sh5_bswap16((uint16_t)(x)))
#define	htonl(x)	((uint32_t)_sh5_bswap32((uint32_t)(x)))
#define	htons(x)	((uint16_t)_sh5_bswap16((uint16_t)(x)))
#endif

#endif	/* __GNUC__ */
