/* Base configuration file for all NetBSD targets.
   Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002, 2003
   Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* TARGET_OS_CPP_BUILTINS() common to all NetBSD targets.  */
#define NETBSD_OS_CPP_BUILTINS_COMMON()		\
  do						\
    {						\
      builtin_define ("__NetBSD__");		\
      builtin_assert ("system=unix");		\
      builtin_assert ("system=NetBSD");		\
      if (flag_pic)				\
        {					\
          builtin_define ("__PIC__");		\
          builtin_define ("__pic__");		\
        }					\
    }						\
  while (0)

/* TARGET_OS_CPP_BUILTINS() common to all LP64 NetBSD targets.  */
#define NETBSD_OS_CPP_BUILTINS_LP64()		\
  do						\
    {						\
      builtin_define ("_LP64");			\
    }						\
  while (0)

/* CPP_SPEC parts common to all NetBSD targets.  */
#define NETBSD_CPP_SPEC				\
  "%{posix:-D_POSIX_SOURCE} \
   %{pthread:-D_REENTRANT -D_PTHREADS}"

/* NETBSD_NATIVE is defined when gcc is integrated into the NetBSD
   source tree so it can be configured appropriately without using
   the GNU configure/build mechanism.  */

#ifdef NETBSD_NATIVE

/* Look for the include files in the system-defined places.  */

#undef GPLUSPLUS_INCLUDE_DIR
#define GPLUSPLUS_INCLUDE_DIR "/usr/include/g++"

#undef GPLUSPLUS_BACKWARD_INCLUDE_DIR
#define GPLUSPLUS_BACKWARD_INCLUDE_DIR "/usr/include/g++/backward"

#undef GCC_INCLUDE_DIR
#define GCC_INCLUDE_DIR "/usr/include"

#undef INCLUDE_DEFAULTS
#define INCLUDE_DEFAULTS				\
  {							\
    { GPLUSPLUS_INCLUDE_DIR, "G++", 1, 1 },		\
    { GPLUSPLUS_BACKWARD_INCLUDE_DIR, "G++", 1, 1 },	\
    { GCC_INCLUDE_DIR, "GCC", 0, 0 },			\
    { 0, 0, 0, 0 }					\
  }

/* Under NetBSD, the normal location of the compiler back ends is the
   /usr/libexec directory.  */

#undef STANDARD_EXEC_PREFIX
#define STANDARD_EXEC_PREFIX		"/usr/libexec/"

/* Under NetBSD, the normal location of the various *crt*.o files is the
   /usr/lib directory.  */

#undef STANDARD_STARTFILE_PREFIX
#define STANDARD_STARTFILE_PREFIX	"/usr/lib/"

#undef TOOLDIR_BASE_PREFIX
#define TOOLDIR_BASE_PREFIX "/usr/"

#undef STANDARD_BINDIR_PREFIX
#define STANDARD_BINDIR_PREFIX "/usr/bin"

#endif /* NETBSD_NATIVE */


/* Provide a LIB_SPEC appropriate for NetBSD.  Here we:

   1. Select the appropriate set of libs, depending on whether we're
      profiling.

   2. Include the pthread library if -pthread is specified (only
      if threads are enabled).

   3. Include the posix library if -posix is specified.

   FIXME: Could eliminate the duplication here if we were allowed to
   use string concatenation.  */

#ifdef NETBSD_ENABLE_PTHREADS
#define NETBSD_LIB_SPEC		\
  "%{pthread:			\
     %{!p:			\
       %{!pg:-lpthread}}	\
     %{p:-lpthread_p}		\
     %{pg:-lpthread_p}}		\
   %{posix:			\
     %{!p:			\
       %{!pg:-lposix}}		\
     %{p:-lposix_p}		\
     %{pg:-lposix_p}}		\
   %{!shared:			\
     %{!symbolic:		\
       %{!p:			\
	 %{!pg:-lc}}		\
       %{p:-lc_p}		\
       %{pg:-lc_p}}}"
#else
#define NETBSD_LIB_SPEC		\
  "%{posix:			\
     %{!p:			\
       %{!pg:-lposix}}		\
     %{p:-lposix_p}		\
     %{pg:-lposix_p}}		\
   %{!shared:			\
     %{!symbolic:		\
       %{!p:			\
	 %{!pg:-lc}}		\
       %{p:-lc_p}		\
       %{pg:-lc_p}}}"
#endif

#undef LIB_SPEC
#define LIB_SPEC NETBSD_LIB_SPEC

/* Don't provide a LIBGCC_SPEC for NetBSD as the default
   is correct. In the --disabled-shared case -lgcc is perfect.  */

#if defined(NETBSD_TOOLS) || defined(NETBSD_NATIVE)
#define	LIBGCC_PICSUFFIX	"_pic"
#endif

/* When building shared libraries, the initialization and finalization 
   functions for the library are .init and .fini respectively.  */

#define COLLECT_SHARED_INIT_FUNC(STREAM,FUNC)				\
  do {									\
    fprintf ((STREAM), "void __init() __asm__ (\".init\");");		\
    fprintf ((STREAM), "void __init() {\n\t%s();\n}\n", (FUNC));	\
  } while (0)

#define COLLECT_SHARED_FINI_FUNC(STREAM,FUNC)				\
  do {									\
    fprintf ((STREAM), "void __fini() __asm__ (\".fini\");");		\
    fprintf ((STREAM), "void __fini() {\n\t%s();\n}\n", (FUNC));	\
  } while (0)

#undef TARGET_HAS_F_SETLKW
#define TARGET_HAS_F_SETLKW

/* Implicit library calls should use memcpy, not bcopy, etc.  */

#undef TARGET_MEM_FUNCTIONS
#define TARGET_MEM_FUNCTIONS 1

/* Handle #pragma weak and #pragma pack.  */

#define HANDLE_SYSV_PRAGMA 1


/* Define some types that are the same on all NetBSD platforms,
   making them agree with <machine/ansi.h>.  */

#undef WCHAR_TYPE
#define WCHAR_TYPE "int"

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE 32

#undef WINT_TYPE
#define WINT_TYPE "int"


/* Attempt to turn on execute permission for the stack.  This may be
   used by TRANSFER_FROM_TRAMPOLINE of the target needs it (that is,
   if the target machine can change execute permissions on a page).

   There is no way to query the execute permission of the stack, so
   we always issue the mprotect() call.

   Note that we go out of our way to use namespace-non-invasive calls
   here.  Unfortunately, there is no libc-internal name for mprotect().

   Also note that no errors should be emitted by this code; it is considered
   dangerous for library calls to send messages to stdout/stderr.  */

#define NETBSD_ENABLE_EXECUTE_STACK					\
extern void __enable_execute_stack (void *);				\
void									\
__enable_execute_stack (addr)						\
     void *addr;							\
{									\
  extern int mprotect (void *, size_t, int);				\
  extern int __sysctl (int *, unsigned int, void *, size_t *,		\
		       void *, size_t);					\
									\
  static int size;							\
  static long mask;							\
									\
  char *page, *end;							\
									\
  if (size == 0)							\
    {									\
      int mib[2];							\
      size_t len;							\
									\
      mib[0] = 6; /* CTL_HW */						\
      mib[1] = 7; /* HW_PAGESIZE */					\
      len = sizeof (size);						\
      (void) __sysctl (mib, 2, &size, &len, NULL, 0);			\
      mask = ~((long) size - 1);					\
    }									\
									\
  page = (char *) (((long) addr) & mask);				\
  end  = (char *) ((((long) (addr + TRAMPOLINE_SIZE)) & mask) + size);	\
									\
  /* 7 == PROT_READ | PROT_WRITE | PROT_EXEC */				\
  (void) mprotect (page, end - page, 7);				\
}

/* NetBSD depends on "cpp" having GNUC semantics, and must default as if
   "cpp -gcc" was called. */
#define DEFAULT_CPP_NEED_NO_GCC 0
