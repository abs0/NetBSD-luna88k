/*	$NetBSD: tcpflags.c,v 1.1.1.1 2004/03/28 08:56:21 martti Exp $	*/

/*
 * Copyright (C) 1993-2001 by Darren Reed.
 *
 * See the IPFILTER.LICENCE file for details on licencing.
 *
 * Id: tcpflags.c,v 1.3 2002/11/02 07:18:01 darrenr Exp
 */

#include "ipf.h"


/*
 * ECN is a new addition to TCP - RFC 2481
 */
#ifndef TH_ECN
# define	TH_ECN  0x40
#endif
#ifndef TH_CWR
# define	TH_CWR  0x80
#endif

extern	char	flagset[];
extern	u_char	flags[];


u_char tcpflags(flgs)
char *flgs;
{
	u_char tcpf = 0;
	char *s, *t;

	for (s = flgs; *s; s++) {
		if (*s == 'W')
			tcpf |= TH_CWR;
		else {
			if (!(t = strchr(flagset, *s))) {
				return 0;
			}
			tcpf |= flags[t - flagset];
		}
	}
	return tcpf;
}