dnl
dnl $KTH-KRB: capabilities.m4,v 1.2 1999/09/01 11:02:26 joda Exp $
dnl $NetBSD: capabilities.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl

dnl
dnl Test SGI capabilities
dnl

AC_DEFUN(KRB_CAPABILITIES,[

AC_CHECK_HEADERS(capability.h sys/capability.h)

AC_CHECK_FUNCS(sgi_getcapabilitybyname cap_set_proc)
])
