#	$NetBSD: Makefile,v 1.15 2004/05/23 02:24:50 lukem Exp $
#
# This is the Makefile for the Taylor UUCP uuconf library

MKPRIVATELIB=	yes

LIB=		uuconf
SRCS=		addblk.c addstr.c allblk.c alloc.c base.c bool.c callin.c \
		calout.c chatc.c cmdarg.c cmdfil.c cmdlin.c debfil.c deblev.c \
		diacod.c dial.c diasub.c dnams.c errno.c errstr.c filnam.c \
		freblk.c fredia.c free.c freprt.c fresys.c grdcmp.c hdial.c \
		hdnams.c hinit.c hlocnm.c hport.c hrmunk.c hsinfo.c hsnams.c \
		hsys.c hunk.c iniglb.c init.c int.c lckdir.c lineno.c llocnm.c \
		local.c locnm.c logfil.c maxuxq.c mrgblk.c paramc.c port.c \
		prtsub.c pubdir.c rdlocs.c rdperm.c reliab.c remunk.c sinfo.c \
		snams.c split.c spool.c stafil.c syssub.c tcalou.c tdial.c \
		tdialc.c tdnams.c tgcmp.c thread.c time.c tinit.c tlocnm.c \
		tport.c tportc.c tsinfo.c tsnams.c tsys.c tval.c ugtlin.c \
		unk.c val.c vinit.c vport.c vsinfo.c vsnams.c vsys.c runuxq.c \
		strip.c
CPPFLAGS+=	-I$(.CURDIR)/../common_sources \
		-DNEWCONFIGLIB=\"$(newconfigdir)\"\
		-DOLDCONFIGLIB=\"$(oldconfigdir)\"

.include <bsd.lib.mk>
