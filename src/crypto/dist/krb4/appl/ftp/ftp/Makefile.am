# $KTH-KRB: Makefile.am,v 1.15 2001/08/28 08:31:21 assar Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:07 joda Exp $

include $(top_srcdir)/Makefile.am.common

INCLUDES += -I$(srcdir)/../common $(INCLUDE_readline) $(INCLUDE_krb4) $(INCLUDE_des)

bin_PROGRAMS = ftp

CHECK_LOCAL = 

if KRB4
krb4_sources = krb4.c kauth.c
endif
if KRB5
krb5_sources = gssapi.c
endif

ftp_SOURCES = \
	cmds.c \
	cmdtab.c \
	extern.h \
	ftp.c \
	ftp_locl.h \
	ftp_var.h \
	main.c \
	pathnames.h \
	ruserpass.c \
	domacro.c \
	globals.c \
	security.c \
	security.h \
	$(krb4_sources) \
	$(krb5_sources)

EXTRA_ftp_SOURCES = krb4.c kauth.c gssapi.c

man_MANS = ftp.1

LDADD = \
	../common/libcommon.a \
	$(LIB_gssapi) \
	$(LIB_krb5) \
	$(LIB_krb4) \
	$(LIB_des) \
	$(LIB_roken) \
	$(LIB_readline)
