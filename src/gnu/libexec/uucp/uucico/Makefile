#	$NetBSD: Makefile,v 1.7 1997/10/25 09:31:48 lukem Exp $
#
# Makefile for uucico

BINDIR=		$(sbindir)
BINOWN=		$(owner)
BINMODE=	4555

PROG=		uucico
SRCS=		uucico.c trans.c send.c rec.c xcmd.c prot.c protg.c protf.c \
		prott.c prote.c proti.c protj.c proty.c protz.c time.c chat.c \
		conn.c copy.c log.c util.c
LDADD+=		$(LIBUNIX) $(LIBUUCONF) $(LIBUUCP)
DPADD+=		$(LIBUNIX) $(LIBUUCONF) $(LIBUUCP)
CPPFLAGS+=	-I$(.CURDIR)/../common_sources\
		-DVERSION=\"$(VERSION)\" -DOWNER=\"$(owner)\" 

MAN=		uucico.8

.include <bsd.prog.mk>
.PATH: $(.CURDIR)/../common_sources
