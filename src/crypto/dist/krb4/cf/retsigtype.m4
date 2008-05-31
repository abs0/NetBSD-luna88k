dnl
dnl $KTH-KRB: retsigtype.m4,v 1.1 2000/07/15 18:05:56 joda Exp $
dnl $NetBSD: retsigtype.m4,v 1.1.1.2 2002/09/12 12:22:14 joda Exp $
dnl
dnl Figure out return type of signal handlers, and define SIGRETURN macro
dnl that can be used to return from one
dnl
AC_DEFUN(rk_RETSIGTYPE,[
AC_TYPE_SIGNAL
if test "$ac_cv_type_signal" = "void" ; then
	AC_DEFINE(VOID_RETSIGTYPE, 1, [Define if signal handlers return void.])
fi
AC_SUBST(VOID_RETSIGTYPE)
AH_BOTTOM([#ifdef VOID_RETSIGTYPE
#define SIGRETURN(x) return
#else
#define SIGRETURN(x) return (RETSIGTYPE)(x)
#endif])
])