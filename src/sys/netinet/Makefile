#	$NetBSD: Makefile,v 1.11.2.2 2005/07/24 11:03:35 tron Exp $

INCSDIR= /usr/include/netinet

INCS=	icmp6.h icmp_var.h if_atm.h if_ether.h if_inarp.h igmp.h \
	igmp_var.h in.h in_gif.h in_pcb.h in_pcb_hdr.h in_systm.h \
	in_var.h ip.h ip6.h ip_ecn.h ip_encap.h ip_gre.h \
	ip_icmp.h ip_mroute.h ip_var.h pim.h pim_var.h \
	tcp.h tcp_debug.h tcp_fsm.h tcp_seq.h tcp_timer.h tcp_var.h \
	tcpip.h udp.h udp_var.h

# ipfilter headers
# XXX shouldn't be here
.include <bsd.own.mk>

INCS+=	ip_compat.h	# always needed by kdump(1)

.if (${MKIPFILTER} != "no")
INCS+=	ip_auth.h ip_fil.h ip_frag.h ip_htable.h ip_nat.h \
	ip_lookup.h ip_pool.h ip_proxy.h ip_scan.h ip_state.h ip_sync.h \
	ipl.h
.endif

.include <bsd.kinc.mk>

.PATH: ${NETBSDSRCDIR}/sys/dist/ipf/netinet
