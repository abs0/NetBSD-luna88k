dnl Id: Makefile.m4,v 8.17 2002/06/21 22:01:34 ca Exp
dnl $NetBSD: Makefile.m4,v 1.1.1.4 2005/03/15 02:05:35 atatat Exp $
include(confBUILDTOOLSDIR`/M4/switch.m4')

define(`confREQUIRE_LIBSM', `true')
# sendmail dir
SMSRCDIR=	ifdef(`confSMSRCDIR', `confSMSRCDIR', `${SRCDIR}/sendmail')
PREPENDDEF(`confENVDEF', `confMAPDEF')
PREPENDDEF(`confINCDIRS', `-I${SMSRCDIR} ')

bldPRODUCT_START(`library', `libsmutil')
define(`bldSOURCES', `debug.c err.c lockfile.c safefile.c snprintf.c cf.c ')
APPENDDEF(`confENVDEF', `-DNOT_SENDMAIL')
bldPRODUCT_END

bldFINISH
