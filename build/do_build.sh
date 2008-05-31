#!/bin/sh
#
# Calls build.sh with the proper arguments.
# Not necessary to do any other setup before calling this.
# This is really only needed for building 'tools' and 'makewrapper'.
# After that point, nbmake-mvme88k may be used directly to call make.

MACHINE=mvme88k

root=`cd .. && pwd`

TOOLDIR=${root}/build/tools/`uname -s`
OBJDIR=${root}/build/${MACHINE}/obj
DESTDIR=${root}/build/${MACHINE}/root

(cd ${root}/src && ./build.sh -j 4 -N 1 -U -m ${MACHINE} -T "$TOOLDIR" -O "$OBJDIR" -D "$DESTDIR" $*)
