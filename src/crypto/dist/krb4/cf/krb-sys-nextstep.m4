dnl $KTH-KRB: krb-sys-nextstep.m4,v 1.4 2002/08/28 18:44:32 joda Exp $
dnl $NetBSD: krb-sys-nextstep.m4,v 1.1.1.4 2002/09/12 12:22:14 joda Exp $
dnl
dnl NEXTSTEP is not posix compliant by default,
dnl you need a switch -posix to the compiler
dnl

AC_DEFUN(rk_SYS_NEXTSTEP, [
AC_CACHE_CHECK(for NeXTSTEP, rk_cv_sys_nextstep, [
AC_EGREP_CPP(yes, 
[#if defined(NeXT) && !defined(__APPLE__)
	yes
#endif 
], rk_cv_sys_nextstep=yes, rk_cv_sys_nextstep=no)])
if test "$rk_cv_sys_nextstep" = "yes"; then
  CFLAGS="$CFLAGS -posix"
  LIBS="$LIBS -posix"
fi
])
