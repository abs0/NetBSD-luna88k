/*
 * Written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: strchr.S,v 1.3 2003/12/04 13:57:31 keihan Exp $")
#endif

#ifdef INDEX
ENTRY(index)
#else
ENTRY(strchr)
#endif
	pushl	%ebx
	movl	8(%esp),%eax
	movb	12(%esp),%cl
	_ALIGN_TEXT,0x90
L1:
	movb	(%eax),%bl
	cmpb	%bl,%cl			/* found char??? */
	je 	L2
	incl	%eax
	testb	%bl,%bl			/* null terminator??? */
	jnz	L1
	xorl	%eax,%eax
L2:
	popl	%ebx
	ret
