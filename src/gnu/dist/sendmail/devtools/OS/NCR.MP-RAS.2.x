#	Id: NCR.MP-RAS.2.x,v 8.14 2002/03/21 23:59:25 gshapiro Exp
#	$NetBSD: NCR.MP-RAS.2.x,v 1.4 2003/06/01 14:06:55 atatat Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

define(`confMAPDEF', `-DNDBM')
define(`confENVDEF', `-DNCR_MP_RAS2')
define(`confOPTIMIZE', `-O2')
APPENDDEF(`confINCDIRS', `-I/usr/include -I/usr/ucbinclude')
define(`confLIBDIRS', `-L/usr/ucblib')
define(`confLIBS', `-lnsl -lnet -lsocket -lelf -lc -lucb')
define(`confMBINDIR', `/usr/ucblib')
define(`confSBINDIR', `/usr/ucbetc')
define(`confUBINDIR', `/usr/ucb')
define(`confEBINDIR', `/usr/ucblib')
define(`confSTDIR', `/var/ucblib')
define(`confINSTALL', `/usr/ucb/install')
define(`confDEPEND_TYPE', `NCR')
