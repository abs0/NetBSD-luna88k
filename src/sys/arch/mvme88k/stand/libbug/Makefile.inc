#	$OpenBSD: Makefile.inc,v 1.2 1998/08/22 07:39:52 smurph Exp $

.include <bsd.own.mk>

LIB_BUG_DIR=${S}/arch/mvme88k/stand/libbug

LIBBUG_DIR!= cd ${LIB_BUG_DIR} && ${PRINTOBJDIR}

LIBBUG=${LIBBUG_DIR}/libbug.a

$(LIBBUG):	.NOTMAIN __always_make_libbug
	@echo making sure the libbug is up to date...
	@(cd ${LIB_BUG_DIR}; ${MAKE})

__always_make_libbug:	.NOTMAIN
