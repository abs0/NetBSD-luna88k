/* This file is automatically generated.  DO NOT EDIT! */
/* Generated from: 	NetBSD: mknative-gcc,v 1.5 2003/07/30 05:10:06 mrg Exp  */

#ifndef NETBSD_ENABLE_PTHREADS
# define NETBSD_ENABLE_PTHREADS
#endif
#include "auto-build.h"
#ifdef IN_GCC
/* Provide three core typedefs used by everything, if we are compiling
   GCC.  These used to be found in rtl.h and tree.h, but this is no
   longer practical.  Providing these here rather that system.h allows
   the typedefs to be used everywhere within GCC. */
struct rtx_def;
typedef struct rtx_def *rtx;
struct rtvec_def;
typedef struct rtvec_def *rtvec;
union tree_node;
typedef union tree_node *tree;
#endif
#define GTY(x)
#ifdef IN_GCC
# include "ansidecl.h"
# include "rs6000/rs6000.h"
# include "dbxelf.h"
# include "elfos.h"
# include "netbsd.h"
# include "netbsd-elf.h"
# include "freebsd-spec.h"
# include "rs6000/sysv4.h"
# include "rs6000/netbsd.h"
# include "defaults.h"
#endif
#ifndef POSIX
# define POSIX
#endif
