#	$NetBSD: Makefile,v 1.10 2004/02/15 19:52:27 skrll Exp $

REQUIRETOOLS=	yes
NOLINT=		# defined

.include <bsd.own.mk>

LIB=		objc

.if ${MKGCC} != "no"

SHLIB_MAJOR=	2
SHLIB_MINOR=	0

# Machine-independent definitions (include file names).
.include "${.CURDIR}/defs.mk"

LIBGCCOBJ!=    cd ${.CURDIR}/../libgcc3/libgcc && ${PRINTOBJDIR}

DIST=		${NETBSDSRCDIR}/gnu/dist/gcc
GNUHOSTDIST=	${NETBSDSRCDIR}/gnu/dist/gcc
GCCARCH=	${NETBSDSRCDIR}/gnu/usr.bin/gcc3/arch/${MACHINE_ARCH}
GCCLIBXX=	${NETBSDSRCDIR}/gnu/lib/libstdc++-v3/arch/${MACHINE_ARCH}

SRCS=		${G_OBJS:N[A-Z]*:Nlinking.lo:.lo=.c} ${G_OBJS:M[A-Z]*:.lo=.m} linking.m

GCPPFLAGS=	${G_ALL_CFLAGS} ${G_INCLUDES}
CPPFLAGS+=	-I. -I${GCCARCH} -I${LIBGCCOBJ} ${GCPPFLAGS:M-D*} ${GCPPFLAGS:M-I*:N-I.*}
CPPFLAGS+=	-I${GCCLIBXX}

BUILDSYMLINKS=	${.CURDIR}/../libstdc++-v3/arch/${MACHINE_ARCH} bits
DPSRCS+=	bits

.for f in Protocol.m Object.m NXConstStr.m linking.m
COPTS.{$f}=	-fgnu-runtime
.endfor

INCS=		${G_OBJC_H}
INCSDIR=	/usr/include/objc

DPSRCS+=	runtime-info.h
CLEANFILES+=	rtscratch rtscratch.s runtime-info.h
runtime-info.h:
	${_MKTARGET_CREATE}
	touch rtscratch
	`${OBJC} --print-prog-name=cc1obj` -print-objc-runtime-info rtscratch >$@

.PATH: ${DIST}/libobjc ${DIST}/libobjc/objc

.include <bsd.lib.mk>

${OBJS} ${POBJS} ${SOBJS}: runtime-info.h

.else
.include <bsd.prog.mk>	# do nothing
.endif
