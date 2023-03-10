#	Id: NEWS-OS.6.x,v 8.14 2002/03/21 23:59:25 gshapiro Exp
#	$NetBSD: NEWS-OS.6.x,v 1.4 2003/06/01 14:06:55 atatat Exp $

dnl	DO NOT EDIT THIS FILE.
dnl	Place personal settings in devtools/Site/site.config.m4

define(`confCC', `/bin/cc')
define(`confBEFORE', `sysexits.h ndbm.o')
define(`confMAPDEF', `-DNDBM -DNIS')
define(`confLIBS', `ndbm.o -lelf -lsocket -lnsl')
define(`confMBINDIR', `/usr/lib')
define(`confSBINDIR', `/usr/etc')
define(`confUBINDIR', `/usr/ucb')
define(`confEBINDIR', `/usr/lib')
define(`confSBINGRP', `sys')
define(`confINSTALL', `/usr/ucb/install')
PUSHDIVERT(3)
sysexits.h:
	ln -s /usr/ucbinclude/sysexits.h .

ndbm.o:
	if [ ! -f /usr/include/ndbm.h ]; then \
		ln -s /usr/ucbinclude/ndbm.h .; \
	fi; \
	if [ -f /usr/lib/libndbm.a ]; then \
		ar x /usr/lib/libndbm.a ndbm.o; \
	else \
		ar x /usr/ucblib/libucb.a ndbm.o; \
	fi;
POPDIVERT
