#	$NetBSD: Makefile,v 1.9 2003/11/28 21:53:32 provos Exp $
#	$OpenBSD: Makefile,v 1.4 2002/06/05 17:34:56 mickey Exp $

.include <bsd.own.mk>

PROG=	systrace
DPADD+=		${LIBEVENT}
LDADD+=		-levent

SRCS=	cradle.c filter.c intercept-translate.c intercept.c \
	netbsd-syscalls.c util.c \
	policy.c systrace-errno.h systrace-error.c \
	systrace-translate.c systrace.c alias.c register.c \
	parse.y lex.l

CPPFLAGS+= -I. -I${.CURDIR} -I${NETBSDSRCDIR}/sys

YHEADER=yes

.include <bsd.prog.mk>
