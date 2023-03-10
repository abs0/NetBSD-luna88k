dnl
dnl $KTH-KRB: broken-realloc.m4,v 1.1 2000/07/15 18:05:36 joda Exp $
dnl $NetBSD: broken-realloc.m4,v 1.1.1.2 2002/09/12 12:22:14 joda Exp $
dnl
dnl Test for realloc that doesn't handle NULL as first parameter
dnl
AC_DEFUN(rk_BROKEN_REALLOC, [
AC_CACHE_CHECK(if realloc if broken, ac_cv_func_realloc_broken, [
ac_cv_func_realloc_broken=no
AC_TRY_RUN([
#include <stddef.h>
#include <stdlib.h>

int main()
{
	return realloc(NULL, 17) == NULL;
}
],:, ac_cv_func_realloc_broken=yes, :)
])
if test "$ac_cv_func_realloc_broken" = yes ; then
	AC_DEFINE(BROKEN_REALLOC, 1, [Define if realloc(NULL) doesn't work.])
fi
AH_BOTTOM([#ifdef BROKEN_REALLOC
#define realloc(X, Y) isoc_realloc((X), (Y))
#define isoc_realloc(X, Y) ((X) ? realloc((X), (Y)) : malloc(Y))
#endif])
])
