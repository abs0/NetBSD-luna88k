# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: toolchain2netbsd,v 1.14 2002/01/22 13:13:00 mrg Exp 
#
G_CXX_EXTRA_HEADERS=${DIST}/gcc/cp/inc/typeinfo ${DIST}/gcc/cp/inc/exception  ${DIST}/gcc/cp/inc/new ${DIST}/gcc/cp/inc/new.h
G_CXX_LIB2FUNCS=tinfo.o tinfo2.o new.o opnew.o opnewnt.o opvnew.o opvnewnt.o  opdel.o opdelnt.o opvdel.o opvdelnt.o exception.o
G_CXX_LIB2SRCS=${DIST}/gcc/cp/new.cc ${DIST}/gcc/cp/new1.cc ${DIST}/gcc/cp/new2.cc  ${DIST}/gcc/cp/exception.cc ${DIST}/gcc/cp/tinfo.cc  ${DIST}/gcc/cp/tinfo2.cc ${DIST}/gcc/cp/tinfo.h
G_INCLUDES=-I. -I${DIST}/gcc -I${DIST}/gcc/config -I${DIST}/gcc/../include
G_LIB2ADD=${DIST}/gcc/frame.c  cplib2.txt
G_LIB2FUNCS=_muldi3 _divdi3 _moddi3 _udivdi3 _umoddi3 _negdi2  _lshrdi3 _ashldi3 _ashrdi3 _ffsdi2  _udiv_w_sdiv _udivmoddi4 _cmpdi2 _ucmpdi2 _floatdidf _floatdisf  _fixunsdfsi _fixunssfsi _fixunsdfdi _fixdfdi _fixunssfdi _fixsfdi  _fixxfdi _fixunsxfdi _floatdixf _fixunsxfsi  _fixtfdi _fixunstfdi _floatditf  __gcc_bcmp _varargs __dummy _eprintf  _bb _shtab _clear_cache _trampoline __main _exit  _ctors _pure
G_LIB2FUNCS_EH=_eh
G_LIBGCC2_CFLAGS=-O2   -DIN_GCC    -g -I./include   -g1  -DIN_LIBGCC2 -D__GCC_FLOAT_NOT_NEEDED
G_MAYBE_USE_COLLECT2=