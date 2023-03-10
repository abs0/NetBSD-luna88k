# $KTH-KRB: Makefile.am,v 1.37 2002/08/19 15:08:37 joda Exp $
# $NetBSD: Makefile.am,v 1.1.1.4 2002/09/12 12:22:13 joda Exp $

include $(top_srcdir)/Makefile.am.common

INCLUDES += $(INCLUDE_krb4) $(AFS_EXTRA_DEFS) $(ROKEN_RENAME)

if KRB4
AFSLIBS = libkafs.la
DEPLIB_krb4 = $(LIB_krb4) $(LIB_des)

if AIX
AFSL_EXP = $(srcdir)/afsl.exp

if AIX4
AFS_EXTRA_LD = -bnoentry
else
AFS_EXTRA_LD = -e _nostart
endif

if AIX_DYNAMIC_AFS
if HAVE_DLOPEN
AIX_SRC = 
else
AIX_SRC = dlfcn.c
endif
AFS_EXTRA_LIBS = afslib.so
AFS_EXTRA_DEFS =
else
AIX_SRC = afslib.c
AFS_EXTRA_LIBS = 
AFS_EXTRA_DEFS = -DSTATIC_AFS
endif

else
AFSL_EXP =
AIX_SRC =
endif # AIX

else
AFSLIBS = 
DEPLIB_krb4  =
endif # KRB4

if KRB5
libkafs_la_LIBADD = ../krb5/libkrb5.la ../roken/libroken.la $(DEPLIB_krb4)
else
libkafs_la_LIBADD = ../roken/libroken.la $(DEPLIB_krb4)
endif # KRB5

lib_LTLIBRARIES = $(AFSLIBS)
libkafs_la_LDFLAGS = -version-info 3:4:3
foodir = $(libdir)
foo_DATA = $(AFS_EXTRA_LIBS)
# EXTRA_DATA = afslib.so

CLEANFILES= $(AFS_EXTRA_LIBS) $(ROKEN_SRCS)

include_HEADERS = kafs.h

if KRB5
afskrb5_c = afskrb5.c
endif

if do_roken_rename
ROKEN_SRCS = resolve.c strtok_r.c strlcpy.c strsep.c
endif

libkafs_la_SOURCES =				\
	afssys.c				\
	afskrb.c				\
	$(afskrb5_c)				\
	common.c				\
	$(AIX_SRC)				\
	kafs_locl.h				\
	afssysdefs.h				\
	$(ROKEN_SRCS)

#afslib_so_SOURCES = afslib.c

EXTRA_libkafs_la_SOURCES = afskrb5.c dlfcn.c afslib.c dlfcn.h

EXTRA_DIST = README.dlfcn afsl.exp afslib.exp

man_MANS = kafs.3

# AIX: this almost works with gcc, but somehow it fails to use the
# correct ld, use ld instead
afslib.so: afslib.o
	ld -o $@ -bM:SRE -bI:$(srcdir)/afsl.exp -bE:$(srcdir)/afslib.exp $(AFS_EXTRA_LD) afslib.o -lc

$(OBJECTS): ../../include/config.h

resolve.c:
	$(LN_S) $(srcdir)/../roken/resolve.c .

strtok_r.c:
	$(LN_S) $(srcdir)/../roken/strtok_r.c .

strlcpy.c:
	$(LN_S) $(srcdir)/../roken/strlcpy.c .

strsep.c:
	$(LN_S) $(srcdir)/../roken/strsep.c .
