# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-gcc,v 1.15 2004/12/10 13:58:11 mrg Exp 
#
G_INCLUDES=-I. -I. -I${GNUHOSTDIST}/gcc -I${GNUHOSTDIST}/gcc/.  -I${GNUHOSTDIST}/gcc/config -I${GNUHOSTDIST}/gcc/../include
G_LIB2ADD=fpgnulib.c xfgnulib.c
G_LIB2ADDEH=${GNUHOSTDIST}/gcc/unwind-dw2.c ${GNUHOSTDIST}/gcc/unwind-dw2-fde.c  ${GNUHOSTDIST}/gcc/unwind-sjlj.c ${GNUHOSTDIST}/gcc/unwind-c.c
G_LIB2FUNCS_1=_muldi3 _negdi2 _lshrdi3 _ashldi3 _ashrdi3 _ffsdi2 _clz  _cmpdi2 _ucmpdi2 _floatdidf _floatdisf _fixunsdfsi _fixunssfsi  _fixunsdfdi _fixdfdi _fixunssfdi _fixsfdi _fixxfdi _fixunsxfdi
G_LIB2FUNCS_2=_floatdixf _fixunsxfsi _fixtfdi _fixunstfdi _floatditf  _clear_cache _trampoline __main _exit _absvsi2 _absvdi2 _addvsi3  _addvdi3 _subvsi3 _subvdi3 _mulvsi3 _mulvdi3 _negvsi2 _negvdi2 _ctors
G_LIB1ASMFUNCS=_mulsi3 _udivsi3 _divsi3 _umodsi3 _modsi3  _double _float _floatex  _eqdf2 _nedf2 _gtdf2 _gedf2 _ltdf2 _ledf2  _eqsf2 _nesf2 _gtsf2 _gesf2 _ltsf2 _lesf2
G_LIB1ASMSRC=m68k/lb1sf68.asm
G_LIB2_DIVMOD_FUNCS=_divdi3 _moddi3 _udivdi3 _umoddi3 _udiv_w_sdiv _udivmoddi4
G_LIB2FUNCS_ST=_eprintf _bb __gcc_bcmp
G_LIBGCC2_CFLAGS=-O2  -DIN_GCC   -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -isystem ./include   -g -DHAVE_GTHR_DEFAULT -DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED
G_USE_COLLECT2=collect2
G_SHLIB_MKMAP=
G_SHLIB_MKMAP_OPTS=
G_SHLIB_MAPFILES=
G_SHLIB_NM_FLAGS=-pg
G_EXTRA_HEADERS=${GNUHOSTDIST}/gcc/config/m68k/math-68881.h
G_tm_defines=NETBSD_ENABLE_PTHREADS
G_xm_file=ansidecl.h  m68k/m68k.h dbxelf.h elfos.h netbsd.h netbsd-elf.h m68k/netbsd-elf.h defaults.h
G_xm_defines=POSIX