#	$NetBSD: Makefile,v 1.1 2003/07/25 16:32:40 mrg Exp $

NOMAN=		# defined

.include <bsd.own.mk>

PROG=           protoize
SRCS=		protoize.c ${G_PROTO_OBJS:.o=.c}

CPPFLAGS+=	-I${BACKENDOBJ} \
		-DDEFAULT_TARGET_MACHINE=\"\" \
		-DDEFAULT_TARGET_VERSION=\"\"


.include "../Makefile.frontend"

.PATH: ${DIST}/gcc

.include <bsd.info.mk>
