# $KTH-KRB: Makefile.am,v 1.6 1999/03/20 13:58:15 joda Exp $
# $NetBSD: Makefile.am,v 1.1.1.3 2002/09/12 12:22:05 joda Exp $

include $(top_srcdir)/Makefile.am.common

SUBDIRS = libtelnet telnet telnetd

dist-hook:
	$(mkinstalldirs) $(distdir)/arpa
	$(INSTALL_DATA) $(srcdir)/arpa/telnet.h $(distdir)/arpa

EXTRA_DIST = README.ORIG telnet.state
