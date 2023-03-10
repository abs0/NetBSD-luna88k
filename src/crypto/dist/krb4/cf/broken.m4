dnl $KTH-KRB: broken.m4,v 1.6 2002/05/19 19:36:52 joda Exp $
dnl $NetBSD: broken.m4,v 1.1.1.4 2002/09/12 12:22:14 joda Exp $
dnl
dnl
dnl Same as AC _REPLACE_FUNCS, just define HAVE_func if found in normal
dnl libraries 

AC_DEFUN([AC_BROKEN],
[AC_FOREACH([rk_func], [$1],
	[AC_CHECK_FUNC(rk_func,
		[AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_[]rk_func), 1, 
			[Define if you have the function `]rk_func['.])],
		[rk_LIBOBJ(rk_func)])])])
