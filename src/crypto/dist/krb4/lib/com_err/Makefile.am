# $KTH-KRB: Makefile.am,v 1.27 2002/03/10 23:52:41 assar Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:13 joda Exp $

include $(top_srcdir)/Makefile.am.common

YFLAGS = -d

lib_LTLIBRARIES = libcom_err.la
libcom_err_la_LDFLAGS = -version-info 2:1:1

bin_PROGRAMS = compile_et

include_HEADERS = com_err.h com_right.h

compile_et_SOURCES = compile_et.c compile_et.h parse.y lex.l

libcom_err_la_SOURCES = error.c com_err.c roken_rename.h

CLEANFILES = lex.c parse.c parse.h

$(compile_et_OBJECTS): parse.h parse.c ## XXX broken automake 1.4s

compile_et_LDADD = \
	$(LIB_roken) \
	$(LEXLIB)
