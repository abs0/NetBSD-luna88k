dnl $KTH-KRB: check-type-extra.m4,v 1.2 1999/03/01 09:52:23 joda Exp $
dnl $NetBSD: check-type-extra.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl
dnl ac_check_type + extra headers

dnl AC_CHECK_TYPE_EXTRA(TYPE, DEFAULT, HEADERS)
AC_DEFUN(AC_CHECK_TYPE_EXTRA,
[AC_REQUIRE([AC_HEADER_STDC])dnl
AC_MSG_CHECKING(for $1)
AC_CACHE_VAL(ac_cv_type_$1,
[AC_EGREP_CPP(dnl
changequote(<<,>>)dnl
<<$1[^a-zA-Z_0-9]>>dnl
changequote([,]), [#include <sys/types.h>
#if STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#endif
$3], ac_cv_type_$1=yes, ac_cv_type_$1=no)])dnl
AC_MSG_RESULT($ac_cv_type_$1)
if test $ac_cv_type_$1 = no; then
  AC_DEFINE($1, $2, [Define this to what the type $1 should be.])
fi
])
