dnl
dnl $KTH-KRB: c-function.m4,v 1.2 1999/03/01 09:52:23 joda Exp $
dnl $NetBSD: c-function.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl

dnl
dnl Test for __FUNCTION__
dnl

AC_DEFUN(AC_C___FUNCTION__, [
AC_MSG_CHECKING(for __FUNCTION__)
AC_CACHE_VAL(ac_cv___function__, [
AC_TRY_RUN([
#include <string.h>

static char *foo()
{
  return __FUNCTION__;
}

int main()
{
  return strcmp(foo(), "foo") != 0;
}
],
ac_cv___function__=yes,
ac_cv___function__=no,
ac_cv___function__=no)])
if test "$ac_cv___function__" = "yes"; then
  AC_DEFINE(HAVE___FUNCTION__, 1, [define if your compiler has __FUNCTION__])
fi
AC_MSG_RESULT($ac_cv___function__)
])

