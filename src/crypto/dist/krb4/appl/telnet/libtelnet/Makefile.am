# $KTH-KRB: Makefile.am,v 1.9 2001/08/28 08:31:23 assar Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:05 joda Exp $

include $(top_srcdir)/Makefile.am.common

INCLUDES += -I$(srcdir)/.. $(INCLUDE_krb4) $(INCLUDE_des)

noinst_LIBRARIES = libtelnet.a

libtelnet_a_SOURCES = \
	auth-proto.h	\
	auth.c		\
	auth.h		\
	enc-proto.h	\
	enc_des.c	\
	encrypt.c	\
	encrypt.h	\
	genget.c	\
	kerberos.c	\
	kerberos5.c	\
	misc-proto.h	\
	misc.c		\
	misc.h

EXTRA_DIST = krb4encpwd.c rsaencpwd.c spx.c
