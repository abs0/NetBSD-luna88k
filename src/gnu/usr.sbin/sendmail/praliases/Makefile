#	$NetBSD: Makefile,v 1.6 2003/03/24 15:04:07 atatat Exp $

.include <bsd.own.mk>

DIST=		${NETBSDSRCDIR}/gnu/dist
.PATH: ${DIST}/sendmail/praliases

PROG=		praliases

LIBSMUTILDIR!=	cd ${.CURDIR}/../libsmutil && ${PRINTOBJDIR}
LIBSMDBDIR!=	cd ${.CURDIR}/../libsmdb && ${PRINTOBJDIR}
LIBSMDIR!=	cd ${.CURDIR}/../libsm && ${PRINTOBJDIR}

CPPFLAGS+=	-DNOT_SENDMAIL
LDADD+=		-L${LIBSMDBDIR} -lsmdb -L${LIBSMUTILDIR} -lsmutil \
		-L${LIBSMDIR} -lsm
DPADD+=		${LIBSMDBDIR}/libsmdb.a ${LIBSMUTILDIR}/libsmutil.a \
		${LIBSMDIR}/libsm.a

MAN=		praliases.8

.include <bsd.prog.mk>
