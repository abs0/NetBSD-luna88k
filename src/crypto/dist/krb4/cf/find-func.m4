dnl $KTH-KRB: find-func.m4,v 1.1 1997/12/14 15:58:58 joda Exp $
dnl $NetBSD: find-func.m4,v 1.1.1.3 2002/09/12 12:22:14 joda Exp $
dnl
dnl AC_FIND_FUNC(func, libraries, includes, arguments)
AC_DEFUN(AC_FIND_FUNC, [
AC_FIND_FUNC_NO_LIBS([$1], [$2], [$3], [$4])
if test -n "$LIB_$1"; then
	LIBS="$LIB_$1 $LIBS"
fi
])
