/*	$NetBSD: netbios.h,v 1.1.1.2 2004/09/27 17:06:51 dyoung Exp $	*/

/*
 * NETBIOS protocol formats
 *
 * @(#) Header: /tcpdump/master/tcpdump/netbios.h,v 1.3 2002/12/11 07:13:55 guy Exp
 */

struct p8022Hdr {
    u_char	dsap;
    u_char	ssap;
    u_char	flags;
};

#define	p8022Size	3		/* min 802.2 header size */

#define UI		0x03		/* 802.2 flags */

