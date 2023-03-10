#	Id: Darwin.7.x,v 1.2 2004/01/19 21:21:22 ca Exp
#	$NetBSD: Darwin.7.x,v 1.1.1.1 2005/03/15 02:05:33 atatat Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

#
define(`confCC', `cc -pipe ${Extra_CC_Flags}')
define(`confMAPDEF', `-DNEWDB -DNIS -DMAP_REGEX')
define(`confENVDEF', `-DDARWIN -DBIND_8_COMPAT')
define(`confLDOPTS', `${Extra_LD_Flags}')
define(`confMILTER_STATIC', `')
define(`confDEPEND_TYPE', `CC-M')
define(`confOPTIMIZE', `-O3')
define(`confRANLIBOPTS', `-c')
define(`confHFDIR', `/usr/share/sendmail')
define(`confINSTALL_RAWMAN')
define(`confMANOWN', `root')
define(`confMANGRP', `wheel')
define(`confUBINOWN', `root')
define(`confUBINGRP', `wheel')
define(`confSBINOWN', `root')
define(`confSBINGRP', `wheel')
