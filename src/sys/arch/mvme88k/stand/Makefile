#	$OpenBSD: Makefile,v 1.7 2004/01/28 08:46:29 miod Exp $

.if ${MACHINE} == "mvme88k"
SUBDIR=  bugcrt libbug libsa libz tftpboot bootxx bootsd # XXX TKM - not building currently: netboot bootst
# not really written yet...
# SUBDIR+=	sboot
.endif

SUBDIR+=installboot

.ifmake !(install)
SUBDIR+= wrtvid prtvid
.endif

.include <bsd.subdir.mk>
