#	$NetBSD: Makefile,v 1.26 2004/05/16 09:53:10 lukem Exp $
#	install these sources in /usr/share/sendmail/cf

.include <bsd.own.mk>

DIST=		${NETBSDSRCDIR}/gnu/dist
.PATH: ${DIST}/sendmail/cf/cf

FILESDIR=	/usr/share/sendmail/cf
FILES=		boat-anchor.mc chimera.mc ihack.net.mc minbar.mc \
		netbsd-proto.mc netbsd-msp.mc submit.mc sun-lamp.mc zhadum.mc

FILES+=		Makefile.sendmail
FILESNAME_Makefile.sendmail=Makefile

.if (${.CURDIR} == ${FILESDIR})
CFDIR?=		${.CURDIR}/..
.else
CFDIR?=		${DIST}/sendmail/cf
.endif

ALL=		netbsd-proto.cf netbsd-msp.cf
FILES+=		${ALL}

OTHERS=		${FILES:S/Makefile//:C/netbsd-(proto|msp).*//:S/.mc/.cf/g}

.include <Makefile.sendmail>

realall: ${ALL}

# XXX not friendly with "make obj"
others: ${OTHERS}

clean cleandir distclean:
	${RM} ${ALL} core

realdepend:

BUILDSYMLINKS+=		netbsd-proto.cf sendmail.cf \
			netbsd-msp.cf etcsubmit.cf

CONFIGFILES=		sendmail.cf
FILESDIR_sendmail.cf=	/etc/mail
.if !defined(SENDMAIL_SUIDROOT) || ${SENDMAIL_SUIDROOT} != "yes"
CONFIGFILES+=		etcsubmit.cf
FILESDIR_etcsubmit.cf=	/etc/mail
FILESNAME_etcsubmit.cf=	submit.cf
.endif

# this is overkill, but....
.include <../domain/Makefile.inc>
.include <../feature/Makefile.inc>
.include <../hack/Makefile.inc>
.include <../m4/Makefile.inc>
.include <../mailer/Makefile.inc>
.include <../ostype/Makefile.inc>
.include <../siteconfig/Makefile.inc>
M4FILES=	${CF_DOMAIN_FILES:@.cf.@${CFDIR}/domain/${.cf.}@} \
		${.CURDIR}/../domain/NetBSD.ORG.m4 \
		${.CURDIR}/../domain/ihack.net.m4 \
		${CF_FEATURE_FILES:@.cf.@${CFDIR}/feature/${.cf.}@} \
		${CF_HACK_FILES:@.cf.@${CFDIR}/hack/${.cf.}@} \
		${CF_M4_FILES:@.cf.@${CFDIR}/m4/${.cf.}@} \
		${CF_MAILER_FILES:@.cf.@${CFDIR}/mailer/${.cf.}@} \
		${CF_OSTYPE_FILES:@.cf.@${CFDIR}/ostype/${.cf.}@} \
		${CF_SITECONFIG_FILES:@.cf.@${CFDIR}/siteconfig/${.cf.}@}

${ALL}: ${M4FILES}

.include <bsd.prog.mk>
