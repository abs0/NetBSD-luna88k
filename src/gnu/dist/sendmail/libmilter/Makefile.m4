dnl Id: Makefile.m4,v 8.31 2002/06/21 22:01:31 ca Exp
dnl $NetBSD: Makefile.m4,v 1.1.1.6 2005/03/15 02:06:05 atatat Exp $
include(confBUILDTOOLSDIR`/M4/switch.m4')

dnl only required for compilation of EXTRAS
define(`confREQUIRE_LIBSM', `true')
define(`confMT', `true')

# sendmail dir
SMSRCDIR=	ifdef(`confSMSRCDIR', `confSMSRCDIR', `${SRCDIR}/sendmail')
PREPENDDEF(`confINCDIRS', `-I${SMSRCDIR} ')

bldPRODUCT_START(`library', `libmilter')
define(`bldINSTALLABLE', `true')
define(`LIBMILTER_EXTRAS', `errstring.c strl.c')
APPENDDEF(`confENVDEF', `-DNOT_SENDMAIL -Dsm_snprintf=snprintf')
define(`bldSOURCES', `main.c engine.c listener.c handler.c comm.c smfi.c signal.c sm_gethost.c LIBMILTER_EXTRAS ')
define(`confBEFORE', `LIBMILTER_EXTRAS')
bldPUSH_INSTALL_TARGET(`install-mfapi')
bldPRODUCT_END

PUSHDIVERT(3)
errstring.c:
	${LN} ${LNOPTS} ${SRCDIR}/libsm/errstring.c .

strl.c:
	${LN} ${LNOPTS} ${SRCDIR}/libsm/strl.c .
POPDIVERT


divert(bldTARGETS_SECTION)
# Install the API header files
MFAPI=	${SRCDIR}/inc`'lude/libmilter/mfapi.h
MFDEF=	${SRCDIR}/inc`'lude/libmilter/mfdef.h
install-mfapi: ${MFAPI}
	if [ ! -d ${DESTDIR}${INCLUDEDIR}/libmilter ]; then mkdir -p ${DESTDIR}${INCLUDEDIR}/libmilter; else :; fi
	${INSTALL} -c -o ${INCOWN} -g ${INCGRP} -m ${INCMODE} ${MFAPI} ${DESTDIR}${INCLUDEDIR}/libmilter/mfapi.h
	${INSTALL} -c -o ${INCOWN} -g ${INCGRP} -m ${INCMODE} ${MFDEF} ${DESTDIR}${INCLUDEDIR}/libmilter/mfdef.h
divert(0)

bldFINISH
