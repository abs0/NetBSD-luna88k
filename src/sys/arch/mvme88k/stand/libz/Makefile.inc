#	$OpenBSD: Makefile.inc,v 1.1 1998/08/22 06:39:14 smurph Exp $

.include <bsd.own.mk>

LIB_Z_DIR=${S}/arch/mvme88k/stand/libz

LIBZ_DIR!= cd ${LIB_Z_DIR} && ${PRINTOBJDIR}

LIBZ=${LIBZ_DIR}/libz.a

$(LIBZ):	.NOTMAIN __always_make_libz
	@echo making sure the libz is up to date...
	@(cd ${LIB_Z_DIR}; ${MAKE} "XCFLAGS=${CFLAGS}")

__always_make_libz:	.NOTMAIN
