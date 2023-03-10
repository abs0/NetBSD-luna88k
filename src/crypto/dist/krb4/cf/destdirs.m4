dnl
dnl $KTH-KRB: destdirs.m4,v 1.2 2002/08/12 15:12:50 joda Exp $
dnl $NetBSD: destdirs.m4,v 1.1.1.1 2002/09/12 12:22:14 joda Exp $
dnl

AC_DEFUN([rk_DESTDIRS], [
# This is done by AC_OUTPUT but we need the result here.
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

AC_FOREACH([rk_dir], [bin lib libexec localstate sbin sysconf], [
	x="${rk_dir[]dir}"
	eval y="$x"
	while test "x$y" != "x$x"; do
		x="$y"
		eval y="$x"
	done
	AC_DEFINE_UNQUOTED(AS_TR_CPP(rk_dir[]dir), "$x", [path to ]rk_dir[])])
])
