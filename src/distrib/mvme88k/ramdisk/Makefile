#	$NetBSD: Makefile,v 1.24 2004/05/21 15:25:18 he Exp $

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

IMAGE=		ramdisk.fs
IMAGESIZE=	1024k

WARNS=		1
DBG=		-Os

CRUNCHBIN=	ramdiskbin
LISTS=		${.CURDIR}/list
MTREECONF=	${DISTRIBDIR}/common/mtree.common
IMAGEENDIAN=	be
MAKEDEVTARGETS=	std md0 sd0 sd1 sd2 st0 cd0
IMAGEDEPENDS=	${CRUNCHBIN} \
		dot.profile rd.fstab rd.welcome \
		${NETBSDSRCDIR}/etc/master.passwd \
		${NETBSDSRCDIR}/etc/netconfig ${NETBSDSRCDIR}/etc/protocols \
		${NETBSDSRCDIR}/etc/services
#XXX TKM		${DESTDIR}/usr/mdec/bootsd ${DESTDIR}/usr/mdec/bootst \
#XXX TKM		${DESTDIR}/usr/mdec/bootxx ${DESTDIR}/usr/mdec/stboot \

# Use stubs to eliminate some large stuff from libc
HACKSRC=	${DISTRIBDIR}/utils/libhack
.include	"${HACKSRC}/Makefile.inc"
${CRUNCHBIN}:	libhack.o

.include "${DISTRIBDIR}/common/Makefile.crunch"
.include "${DISTRIBDIR}/common/Makefile.makedev"
.include "${DISTRIBDIR}/common/Makefile.image"

release:

.include <bsd.prog.mk>
