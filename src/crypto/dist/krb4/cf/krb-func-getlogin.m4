dnl
dnl $KTH-KRB: krb-func-getlogin.m4,v 1.1 1999/07/13 17:45:30 assar Exp $
dnl $NetBSD: krb-func-getlogin.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl
dnl test for POSIX (broken) getlogin
dnl


AC_DEFUN(AC_FUNC_GETLOGIN, [
AC_CHECK_FUNCS(getlogin setlogin)
if test "$ac_cv_func_getlogin" = yes; then
AC_CACHE_CHECK(if getlogin is posix, ac_cv_func_getlogin_posix, [
if test "$ac_cv_func_getlogin" = yes -a "$ac_cv_func_setlogin" = yes; then
	ac_cv_func_getlogin_posix=no
else
	ac_cv_func_getlogin_posix=yes
fi
])
if test "$ac_cv_func_getlogin_posix" = yes; then
	AC_DEFINE(POSIX_GETLOGIN, 1, [Define if getlogin has POSIX flavour (and not BSD).])
fi
fi
])
