# $NetBSD: Makefile,v 1.8 2005/01/10 03:11:17 lukem Exp $

NOLINT=		# defined

.include <bsd.own.mk>

DIST=		${NETBSDSRCDIR}/crypto/dist
.PATH:		${DIST}/krb4/lib/kdb

WARNS?=		1

LIB=		kdb

SRCS=		krb_cache.c krb_kdb_utils.c copykey.c \
		krb_lib.c krb_dbm.c print_princ.c

INCS=		krb_db.h
INCSDIR=	/usr/include/kerberosIV

LIBKRBINC!=	cd ${NETBSDSRCDIR}/lib/libkrb && ${PRINTOBJDIR}

CPPFLAGS+= -I.					\
	-I${DIST}/krb4/lib/kdb			\
	-I${LIBKRBINC}				\
	-I${DIST}/krb4/lib/krb			\
	-I${NETBSDSRCDIR}/include/heimdal	\
	-I${NETBSDSRCDIR}/lib/libroken		\
	-I${DIST}/heimdal/lib/roken		\
	-DHAVE_CONFIG_H

.if (${USE_INET6} != "no")
CPPFLAGS+=-DHAVE_IPV6
.endif

.include <bsd.lib.mk>
