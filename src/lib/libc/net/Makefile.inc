#	$NetBSD: Makefile.inc,v 1.68 2005/01/11 06:01:41 itojun Exp $
#	@(#)Makefile.inc	8.2 (Berkeley) 9/5/93

# net sources
.PATH: ${ARCHDIR}/net ${.CURDIR}/net

SRCS+=	__cmsg_alignbytes.c base64.c ethers.c gethnamaddr.c getifaddrs.c \
	getnetnamadr.c getnetent.c \
	getprotobyname.c getprotobynumber.c getprotoent.c \
	getprotobyname_r.c getprotobynumber_r.c getprotoent_r.c \
	getservbyname.c getservbyport.c getservent.c \
	getservbyname_r.c getservbyport_r.c getservent_r.c \
	iso_addr.c linkaddr.c \
	nsdispatch.c nslexer.l nsparser.y ns_addr.c ns_ntoa.c nsap_addr.c \
	rcmd.c recv.c send.c sethostent.c \
	sockatmark.c

.if (${MKHESIOD} != "no")
SRCS+=	hesiod.c
.endif

SRCS+=	getaddrinfo.c getnameinfo.c
.if (${USE_INET6} != "no")
SRCS+=	ip6opt.c rthdr.c vars6.c
.endif
SRCS+=	if_indextoname.c if_nameindex.c if_nametoindex.c

LPREFIX=_nsyy
YPREFIX=_nsyy
YHEADER=1

CLEANFILES+=nsparser.c nslexer.c nsparser.h

nslexer.c: nslexer.l nsparser.h
	${_MKTARGET_LEX}
	${LEX.l} -t ${.IMPSRC} | sed -e '/YY_BUF_SIZE/s/16384/1024/' >${.TARGET}

# machine-dependent net sources
# m-d Makefile.inc must include sources for:
#	htonl() htons() ntohl() ntohs()

.include "${ARCHDIR}/net/Makefile.inc"

MAN+=	byteorder.3 ethers.3 gethostbyname.3 getifaddrs.3 \
	getnetent.3 getprotoent.3 \
	getservent.3 inet.3 inet_net.3 iso_addr.3 linkaddr.3 ns.3 \
	nsdispatch.3 rcmd.3 resolver.3 sockatmark.3

.if (${MKHESIOD} != "no")
MAN+=	hesiod.3
.endif

MLINKS+=byteorder.3 htonl.3 byteorder.3 htons.3 byteorder.3 ntohl.3 \
	byteorder.3 ntohs.3
MLINKS+=ethers.3 ether_aton.3 ethers.3 ether_hostton.3 ethers.3 ether_line.3 \
	ethers.3 ether_ntoa.3 ethers.3 ether_ntohost.3
MLINKS+=gethostbyname.3 endhostent.3 gethostbyname.3 gethostbyaddr.3 \
	gethostbyname.3 gethostent.3 gethostbyname.3 sethostent.3 \
	gethostbyname.3 herror.3 gethostbyname.3 hstrerror.3 \
	gethostbyname.3 gethostbyname2.3
MLINKS+=getnetent.3 endnetent.3 getnetent.3 getnetbyaddr.3 \
	getnetent.3 getnetbyname.3 getnetent.3 setnetent.3
MLINKS+=getprotoent.3 endprotoent.3 getprotoent.3 getprotobyname.3 \
	getprotoent.3 getprotobynumber.3 getprotoent.3 setprotoent.3
MLINKS+=getservent.3 endservent.3 getservent.3 getservbyname.3 \
	getservent.3 getservbyport.3 getservent.3 setservent.3
.if (${MKHESIOD} != "no")
MLINKS+=hesiod.3 hesiod_end.3 hesiod.3 hesiod_free_list.3 \
	hesiod.3 hesiod_init.3 hesiod.3 hesiod_resolve.3 \
	hesiod.3 hesiod_to_bind.3
.endif
MLINKS+=inet.3 addr.3 inet.3 inet_addr.3 inet.3 inet_aton.3 \
	inet.3 inet_lnaof.3 inet.3 inet_makeaddr.3 inet.3 inet_netof.3 \
	inet.3 inet_network.3 inet.3 inet_ntoa.3 inet.3 network.3 \
	inet.3 ntoa.3 inet.3 inet_ntop.3 inet.3 inet_pton.3
MLINKS+=inet_net.3 inet_net_ntop.3 inet_net.3 inet_net_pton.3
MLINKS+=linkaddr.3 link_addr.3 linkaddr.3 link_ntoa.3 linkaddr.3 linkntoa.3
MLINKS+=ns.3 ns_addr.3 ns.3 ns_ntoa.3
MLINKS+=rcmd.3 iruserok.3 rcmd.3 rresvport.3 rcmd.3 ruserok.3 rcmd.3 orcmd.3
MLINKS+=resolver.3 dn_comp.3 resolver.3 dn_expand.3 resolver.3 res_init.3 \
	resolver.3 res_mkquery.3 resolver.3 res_send.3 resolver.3 res_query.3 \
	resolver.3 res_search.3

# IPv6
MAN+=	gai_strerror.3 getaddrinfo.3 getnameinfo.3 if_indextoname.3 \
	inet6_option_space.3 inet6_rthdr_space.3
MLINKS+=getaddrinfo.3 freeaddrinfo.3 \
	getifaddrs.3 freeifaddrs.3 \
	if_indextoname.3 if_nametoindex.3 if_indextoname.3 if_nameindex.3 \
	if_indextoname.3 if_freenameindex.3 \
	inet6_option_space.3 inet6_option_init.3 \
	inet6_option_space.3 inet6_option_append.3 \
	inet6_option_space.3 inet6_option_alloc.3 \
	inet6_option_space.3 inet6_option_next.3 \
	inet6_option_space.3 inet6_option_find.3 \
	inet6_rthdr_space.3 inet6_rthdr_init.3 \
	inet6_rthdr_space.3 inet6_rthdr_add.3 \
	inet6_rthdr_space.3 inet6_rthdr_lasthop.3 \
	inet6_rthdr_space.3 inet6_rthdr_reverse.3 \
	inet6_rthdr_space.3 inet6_rthdr_segments.3 \
	inet6_rthdr_space.3 inet6_rthdr_getaddr.3 \
	inet6_rthdr_space.3 inet6_rthdr_getflags.3 \
	rcmd.3 rcmd_af.3 rcmd.3 iruserok_sa.3 rcmd.3 rresvport_af.3 \
	rcmd.3 orcmd_af.3
