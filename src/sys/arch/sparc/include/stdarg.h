/*	$NetBSD: stdarg.h,v 1.21.10.1 2006/04/30 23:31:32 riz Exp $ */

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)stdarg.h	8.2 (Berkeley) 9/27/93
 */

#ifndef _SPARC_STDARG_H_
#define	_SPARC_STDARG_H_

#include <machine/ansi.h>
#include <sys/featuretest.h>

typedef _BSD_VA_LIST_	va_list;

#ifdef __lint__

# define va_start(ap, last)	((ap) = *(va_list *)0)
# define va_arg(ap, type)	(*(type *)(void *)&(ap))
# define va_end(ap)
# define __va_copy(dest, src)	((dest) = (src))

#elif __GNUC_PREREQ__(3,0)

# define va_start(ap, last)	__builtin_va_start(ap, last)
# define va_end(ap)		__builtin_va_end(ap)
# define va_arg(ap, type)	__builtin_va_arg(ap, type)
# define __va_copy(dst, src)	__builtin_va_copy(dst, src)

#else

# define va_start(ap, last) \
	(void)(__builtin_next_arg(last), (ap) = (va_list)__builtin_saveregs())


# define va_end(ap)	
# define __va_copy(dest, src) \
	((dest) = (src))

# ifdef __arch64__
/*
 * For sparcv9 code.
 */
#  define __va_arg8(ap, type) \
	(*(type *)(void *)((ap) += 8, (ap) - 8))
#  define __va_arg16(ap, type) \
	(*(type *)(void *)((ap) = (va_list)(((unsigned long)(ap) + 31) & -16),\
			   (ap) - 16))
#  define __va_int(ap, type) \
	(*(type *)(void *)((ap) += 8, (ap) - sizeof(type)))

#  define __REAL_TYPE_CLASS	8
#  define __RECORD_TYPE_CLASS	12
#  define va_arg(ap, type) \
	(__builtin_classify_type(*(type *)0) == __REAL_TYPE_CLASS ?	\
	 (__alignof__(type) == 16 ? __va_arg16(ap, type) :		\
	  __va_arg8(ap, type)) :					\
	 (__builtin_classify_type(*(type *)0) < __RECORD_TYPE_CLASS ?	\
	  __va_int(ap, type) :						\
	  (sizeof(type) <= 8 ? __va_arg8(ap, type) :			\
	   (sizeof(type) <= 16 ? __va_arg16(ap, type) :			\
	    *__va_arg8(ap, type *)))))
# else /* __arch64__ */
/* 
 * For sparcv8 code.
 */
#  define __va_size(type) \
	(((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

/*
 * va_arg picks up the next argument of type `type'.  Appending an
 * asterisk to `type' must produce a pointer to `type' (i.e., `type'
 * may not be, e.g., `int (*)()').
 *
 * Gcc-2.x tries to use ldd/std for double and quad_t values, but Sun's
 * brain-damaged calling convention does not quad-align these.  Thus, for
 * 8-byte arguments, we have to pick up the actual value four bytes at a
 * time, and use type punning (i.e., a union) to produce the result.
 * (We could also do this with a libc function, actually, by returning
 * 8 byte integers in %o0+%o1 and the same 8 bytes as a double in %f0+%f1.)
 *
 * Note: We don't declare __d with type `type', since in C++ the type might
 * have a constructor.
 */

#  define __va_8byte(ap, type) \
	__extension__ ({						\
		union { char __d[sizeof(type)]; int __i[2]; } __va_u;	\
		__va_u.__i[0] = ((int *)(void *)(ap))[0];		\
		__va_u.__i[1] = ((int *)(void *)(ap))[1];		\
		(ap) += 8; *(type *)(va_list)__va_u.__d;		\
	})

#  define __va_arg(ap, type) \
	(*(type *)((ap) += __va_size(type),			\
		   (ap) - (sizeof(type) < sizeof(long) &&	\
			   sizeof(type) != __va_size(type) ?	\
			   sizeof(type) : __va_size(type))))

#  define __RECORD_TYPE_CLASS	12
#  define va_arg(ap, type) \
	(__builtin_classify_type(*(type *)0) >= __RECORD_TYPE_CLASS ?	\
	 *__va_arg(ap, type *) : __va_size(type) == 8 ?			\
	 __va_8byte(ap, type) : __va_arg(ap, type))

# endif /* __arch64__ */
#endif /* !__GNUC_PREREQ(3, 0) */

#if !defined(_ANSI_SOURCE) &&						\
    (defined(_ISOC99_SOURCE) || (__STDC_VERSION__ - 0) >= 199901L ||	\
     defined(_NETBSD_SOURCE))
# define va_copy(dst, src)	__va_copy(dst, src)
#endif

#endif /* !_SPARC_STDARG_H_ */
