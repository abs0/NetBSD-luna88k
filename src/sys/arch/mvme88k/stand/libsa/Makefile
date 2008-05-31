#	$OpenBSD: Makefile,v 1.7 2006/05/16 22:51:30 miod Exp $

LIB=sa

NOPIC= # defined
NOLINT= # defined
NOPROFILE= # defined

# Logically src/sys
S=${.CURDIR}/../../../..
DIR_SA=$S/lib/libsa
DIR_KERN=$S/lib/libkern

SRC_net= arp.c ether.c in_cksum.c net.c netif.c rpc.c nfs.c \
	 rarp.c bootparam.c

SRC_sa=	alloc.c memcpy.c exit.c getfile.c gets.c globals.c \
	printf.c strerror.c memset.c memcmp.c strncpy.c strcmp.c strlen.c \
	strlcpy.c strlcat.c snprintf.c subr_prf.c panic.c \
	close.c closeall.c dev.c dkcksum.c \
	lseek.c open.c nullfs.c read.c files.c fstat.c \
	twiddle.c ufs.c cread.c loadfile.c loadfile_elf32.c

SRC_kern= ashrdi3.c bzero.c

SRC_mvme= exec_mvme.c

SRC_here= bugdev.c clock.c parse_args.c

SRCS= ${SRC_net} ${SRC_sa} ${SRC_mvme} ${SRC_here} ${SRC_kern}

# DBG= -DDEBUG -DNETIF_DEBUG -DNFS_DEBUG -DRPC_DEBUG \
# 	-DNET_DEBUG -DRARP_DEBUG -DETHER_DEBUG

#DEFS= -DCOMPAT_UFS
DEFS= -D__INTERNAL_LIBSA_CREAD
#DEFS+=-DNETIF_DEBUG
INCL= -I${.CURDIR} -I${.CURDIR}/../libbug -I${S}/lib/libsa -I${S}
CFLAGS+= ${XCFLAGS} ${COPTS} ${DEFS} ${DBG} ${INCL}

.PATH:  ${DIR_SA} ${DIR_KERN}

all: libsa.a

install:

.include <bsd.lib.mk>