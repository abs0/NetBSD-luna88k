#	$NetBSD: Makefile,v 1.6 2003/12/21 07:05:25 mrg Exp $

REQUIRETOOLS=	yes
NOLINT=		# defined

.include <bsd.own.mk>

LIB=		stdc++

.if exists(${.CURDIR}/arch/${MACHINE_ARCH}/defs.mk) && ${MKGCC} != "no"
.include "${.CURDIR}/arch/${MACHINE_ARCH}/defs.mk"

SHLIB_MAJOR=	5
SHLIB_MINOR=	0

SUBDIR=		include

INCS=		c++config.h
INCSDIR=	/usr/include/g++

.include "${.CURDIR}/../libsupc++/Makefile.common"

SRCS=		${LIBMATHSRCS} ${LIBSUPCXXSRCS} ${LIBSTDCXXSRCS}
# affects profiling; can't switch it off just for profiling easily.
#CXXFLAGS+=	${G_SECTION_FLAGS}
#CXXFLAGS+=	-ffunction-sections -fdata-sections 
CXXFLAGS+=	-fno-implicit-templates -fdiagnostics-show-location=once

CONF=		${DIST}/libstdc++-v3/config

BUILDSYMLINKS+=	${CONF}/io/basic_file_stdio.cc		basic_file.cc \
		${CONF}/locale/generic/c_locale.cc	c++locale.cc

GLIBCPP_INCLUDE_DIR=	${DESTDIR}/usr/include/g++

COPTS.concept-inst.cc=	-D_GLIBCPP_CONCEPT_CHECKS -fimplicit-templates
CPPFLAGS.strstream.cc=	-I$(GLIBCPP_INCLUDE_DIR)/backward -Wno-deprecated

# XXX
MKDEPFLAGS+=	-I$(GLIBCPP_INCLUDE_DIR)/backward

.include <bsd.lib.mk>
.include <bsd.subdir.mk>
.include <bsd.info.mk>

.PATH:	${DIST}/libstdc++-v3/src \
	${DIST}/libstdc++-v3/libmath \
	${DIST}/libstdc++-v3/libsupc++ \
	${DIST}/libiberty \
	${CONF}/locale/generic \
	${.CURDIR}/arch/${MACHINE_ARCH}

${OBJS}: ${.CURDIR}/arch/${MACHINE_ARCH}/defs.mk
.else
.include <bsd.prog.mk> # do nothing
.endif

.if (${OBJECT_FMT} == "a.out")
LDADD=	${DESTDIR}/usr/lib/c++rt0.o
DPADD=	${DESTDIR}/usr/lib/c++rt0.o
.endif
