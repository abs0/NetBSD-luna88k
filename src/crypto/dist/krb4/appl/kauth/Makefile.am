# $KTH-KRB: Makefile.am,v 1.13 2002/04/26 12:22:23 joda Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:07 joda Exp $

include $(top_srcdir)/Makefile.am.common

INCLUDES += $(INCLUDE_krb4)

bin_PROGRAMS = kauth
bin_SCRIPTS = ksrvtgt
libexec_PROGRAMS = kauthd
CLEANFILES = $(bin_SCRIPTS)

man_MANS = kauth.1 ksrvtgt.1 kauthd.8 

EXTRA_DIST = zrefresh ksrvtgt.in

kauth_SOURCES = \
	kauth.c \
	kauth.h \
	rkinit.c \
	marshall.c \
	encdata.c

kauthd_SOURCES = \
	kauthd.c \
	kauth.h \
	marshall.c \
	encdata.c

ksrvtgt: ksrvtgt.in
	sed -e "s!%bindir%!$(bindir)!" $(srcdir)/ksrvtgt.in > $@
	chmod +x $@

install-exec-local:
	if test -f $(DESTDIR)$(bindir)/zrefresh -o -r $(DESTDIR)$(bindir)/zrefresh; then \
	  true; \
	else \
	  $(INSTALL_SCRIPT) $(srcdir)/zrefresh $(DESTDIR)$(bindir)/`echo zrefresh | sed '$(transform)'`; \
	fi

LDADD = \
	$(LIB_kafs) \
	$(LIB_krb4) \
	$(LIB_des) \
	$(LIB_roken)
