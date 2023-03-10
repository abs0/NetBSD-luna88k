dnl Id: Makefile.m4,v 1.5 2002/06/21 22:01:30 ca Exp
dnl $NetBSD: Makefile.m4,v 1.1.1.3 2005/03/15 02:06:05 atatat Exp $
include(confBUILDTOOLSDIR`/M4/switch.m4')

define(`confREQUIRE_LIBSM', `true')
# sendmail dir
SMSRCDIR=	ifdef(`confSMSRCDIR', `confSMSRCDIR', `${SRCDIR}/sendmail')
PREPENDDEF(`confENVDEF', `confMAPDEF')
PREPENDDEF(`confINCDIRS', `-I${SMSRCDIR} ')

bldPRODUCT_START(`executable', `editmap')
define(`bldSOURCES', `editmap.c ')
define(`bldINSTALL_DIR', `S')
bldPUSH_SMLIB(`sm')
bldPUSH_SMLIB(`smutil')
bldPUSH_SMLIB(`smdb')
APPENDDEF(`confENVDEF', `-DNOT_SENDMAIL')
bldPRODUCT_END

bldPRODUCT_START(`manpage', `editmap')
define(`bldSOURCES', `editmap.8')
bldPRODUCT_END

bldFINISH
