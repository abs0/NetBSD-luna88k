# $KTH-KRB: Makefile.am,v 1.11 2001/08/28 08:31:21 assar Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:05 joda Exp $

include $(top_srcdir)/Makefile.am.common

INCLUDES += $(INCLUDE_des)

bin_PROGRAMS = otp otpprint
bin_SUIDS = otp
otp_SOURCES = otp.c otp_locl.h
otpprint_SOURCES = otpprint.c otp_locl.h

man_MANS = otp.1  otpprint.1

LDADD = \
	$(top_builddir)/lib/otp/libotp.la
