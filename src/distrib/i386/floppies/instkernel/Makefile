#	$NetBSD: Makefile,v 1.4 2005/03/11 20:55:10 bouyer Exp $

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

#	create ${RAMDISK_*} variables
#
RAMDISKS=	RAMDISK_B	ramdisk-big		\
		RAMDISK_S	ramdisk-small		\
		RAMDISK_T	ramdisk-tiny		\
		RESCUE_T	ramdisk-rescuetiny

.for V F in ${RAMDISKS}
${V}DIR!=	cd ${.CURDIR}/../${F} && ${PRINTOBJDIR}
${V}=		${${V}DIR}/${F}.fs
.endfor

MDSETTARGETS=		INSTALL		${RAMDISK_B}	-	\
			INSTALL_LAPTOP	${RAMDISK_B}	-	\
			INSTALL_XENU	${RAMDISK_B}	-	\
			INSTALL_SMALL	${RAMDISK_S}	-	\
			INSTALL_TINY	${RAMDISK_T}	-	\
			INSTALL_PS2	${RAMDISK_S}	-	\
			INSTALL_TINY	${RESCUE_T}	netbsd-RESCUE_TINY

MDSET_RELEASEDIR=	binary/kernel

.include "${DISTRIBDIR}/common/Makefile.mdset"

.include <bsd.prog.mk>
