# $KTH-KRB: Makefile.am,v 1.5 2002/08/28 22:57:42 assar Exp $
# $NetBSD: Makefile.am,v 1.1.1.2 2002/09/12 12:22:11 joda Exp $

include $(top_srcdir)/Makefile.am.common

CLEANFILES		= print_version.h

noinst_LTLIBRARIES	= libvers.la

build_HEADERZ		= vers.h

noinst_PROGRAMS		= make-print-version

if KRB4
if KRB5
## need to link with des here; otherwise, if krb4 is shared the link
## will fail with unresolved references
make_print_version_LDADD = $(LIB_krb4) $(LIB_des)
endif
endif

libvers_la_SOURCES	= print_version.c

print_version.lo: print_version.h

print_version.h: make-print-version$(EXEEXT)
	./make-print-version$(EXEEXT) print_version.h

make-print-version.o: $(top_builddir)/include/version.h
