#	$NetBSD: Makefile,v 1.5 2004/03/18 19:48:15 skrll Exp $

NOLINKLIB=	# defined
NOLINT=		# defined
NOPROFILE=	# defined

# For ../Makefile.inc and bsd.own.mk
.include <bsd.init.mk>

LIB=		frontend

SRCS=		gcc.c version.c intl.c prefix.c

CPPFLAGS+=	-I${.CURDIR} -I${GCCARCH} -I${BACKENDOBJ} -I. \
		${G_ALL_CFLAGS:M-D*} ${G_INCLUDES:M-I*:N-I.*} \
		-DPREFIX=\"/usr\" \
		-DDEFAULT_TARGET_VERSION=\"${G_version}\" \
		-DDEFAULT_TARGET_MACHINE=\"${MACHINE_GNU_PLATFORM}\"
HOST_CPPFLAGS+=	-I${GCCARCH} ${G_ALL_CFLAGS:M-D*} ${G_INCLUDES:M-I*:N-I.*}

DPSRCS+=	multilib.h specs.h

MKPIC:=		no
MKPICLIB:=	no

.include <bsd.lib.mk>

.PATH: ${DIST}/gcc

#
# Build the specs.h file
#
CLEANFILES+=	specs.h
specs.h: Makefile
	for f in cp f objc; do \
		echo "#include \"$$f/lang-specs.h\""; \
	done > specs.h
