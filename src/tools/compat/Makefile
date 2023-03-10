#	$NetBSD: Makefile,v 1.32.2.1 2005/09/08 19:15:44 tron Exp $

HOSTLIB=	nbcompat

SRCS=		atoll.c basename.c bswap16.c bswap32.c bswap64.c \
		dirname.c fgetln.c flock.c fparseln.c \
		getmode.c getopt_long.c gettemp.c heapsort.c issetugid.c \
		lchflags.c lchmod.c lchown.c libyywrap.c \
		md2c.c md2hl.c md4c.c md4hl.c md5c.c md5hl.c mkdtemp.c \
		mkstemp.c pread.c putc_unlocked.c pwcache.c pwrite.c \
		pw_scan.c rmd160.c rmd160hl.c setenv.c setgroupent.c \
		setpassent.c setprogname.c sha1.c sha1hl.c sha2.c \
		sha256hl.c sha384hl.c sha512hl.c snprintf.c \
		strlcat.c strlcpy.c strmode.c strsep.c strsuftoll.c \
		strtoll.c unvis.c vis.c _err.c _errx.c _verr.c _verrx.c \
		_vwarn.c _vwarnx.c _warn.c _warnx.c __fts13.c __glob13.c

BUILD_OSTYPE!=  uname -s

# Disable use of pre-compiled headers on Darwin.
.if ${BUILD_OSTYPE} == "Darwin"
CPPFLAGS+=	-no-cpp-precomp
.endif

# -D_FILE_OFFSET_BITS=64 produces a much more amenable `struct stat', and
# other file ops, on many systems, without changing function names.

CPPFLAGS+=	-I. -I./include -I${.CURDIR} -DHAVE_NBTOOL_CONFIG_H=1 \
		-D_FILE_OFFSET_BITS=64

.PATH:		${.CURDIR}/../../lib/libc/gen \
		${.CURDIR}/../../lib/libc/hash \
		${.CURDIR}/../../lib/libc/hash/sha2 \
		${.CURDIR}/../../lib/libc/md \
		${.CURDIR}/../../lib/libc/stdio \
		${.CURDIR}/../../lib/libc/stdlib \
		${.CURDIR}/../../lib/libc/string \
		${.CURDIR}/../../usr.bin/lex

DPSRCS+=	defs.mk
CLEANFILES+=	defs.mk
CLEANFILES+=	config.log config.status configure.lineno confdefs.h *.stamp

# Get components of Berkeley DB.
_CURDIR:=	${.CURDIR}
.CURDIR:=	${_CURDIR}/../../lib/libc
.include "${.CURDIR}/db/Makefile.inc"
.CURDIR:=	${_CURDIR}

SRCS:=		${SRCS:Nndbm.c:N__ndbm13.c}

config.cache: include/.stamp configure nbtool_config.h.in defs.mk.in
	rm -f ${.TARGET}
	CC=${HOST_CC:Q} CFLAGS=${HOST_CFLAGS:Q} LDFLAGS=${HOST_LDFLAGS:Q} \
		${HOST_SH} ${.CURDIR}/configure --cache-file=config.cache

defs.mk: config.cache
	@touch ${.TARGET}

# Run by hand, then "configure" script committed:
regen:
	cd ${.CURDIR} && ${TOOLDIR}/bin/${_TOOL_PREFIX}autoconf

include/.stamp:
	mkdir -p include/sys include/machine include/rpc
	@touch ${.TARGET}

cleandir: compat.clean
compat.clean:
	-rm -r -f include
	-rm -f config.cache nbtool_config.h

HOST_CPPFLAGS:=	${CPPFLAGS}
CPPFLAGS:=	# empty

.include <bsd.hostlib.mk>
