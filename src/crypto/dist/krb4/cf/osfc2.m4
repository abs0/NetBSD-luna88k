dnl $KTH-KRB: osfc2.m4,v 1.3 2002/04/30 16:46:18 joda Exp $
dnl $NetBSD: osfc2.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl
dnl enable OSF C2 stuff

AC_DEFUN(AC_CHECK_OSFC2,[
AC_ARG_ENABLE(osfc2,
	AC_HELP_STRING([--enable-osfc2],[enable some OSF C2 support]))
LIB_security=
if test "$enable_osfc2" = yes; then
	AC_DEFINE(HAVE_OSFC2, 1, [Define to enable basic OSF C2 support.])
	LIB_security=-lsecurity
fi
AC_SUBST(LIB_security)
])
