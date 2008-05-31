dnl $KTH-KRB: krb-prog-yacc.m4,v 1.3 2000/03/28 12:12:23 assar Exp $
dnl $NetBSD: krb-prog-yacc.m4,v 1.1.1.4 2002/09/12 12:22:14 joda Exp $
dnl
dnl
dnl We prefer byacc or yacc because they do not use `alloca'
dnl

AC_DEFUN(AC_KRB_PROG_YACC,
[AC_CHECK_PROGS(YACC, byacc yacc 'bison -y')
if test "$YACC" = ""; then
  AC_MSG_WARN([yacc not found - some stuff will not build])
fi
])
