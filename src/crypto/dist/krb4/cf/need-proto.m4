dnl $KTH-KRB: need-proto.m4,v 1.4 2002/08/23 15:07:41 joda Exp $
dnl $NetBSD: need-proto.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl
dnl
dnl Check if we need the prototype for a function
dnl

dnl AC_NEED_PROTO(includes, function)

AC_DEFUN(AC_NEED_PROTO, [
if test "$ac_cv_func_$2+set" != set -o "$ac_cv_func_$2" = yes; then
AC_CACHE_CHECK([if $2 needs a prototype], ac_cv_func_$2_noproto,
AC_TRY_COMPILE([$1],
[struct foo { int foo; } xx;
extern int $2 (struct foo*);
$2(&xx);
],
eval "ac_cv_func_$2_noproto=yes",
eval "ac_cv_func_$2_noproto=no"))
if test "$ac_cv_func_$2_noproto" = yes; then
	AC_DEFINE(AS_TR_CPP(NEED_[]$2[]_PROTO), 1,
		[define if the system is missing a prototype for $2()])
fi
fi
])
