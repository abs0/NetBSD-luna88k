/*
 * Written by J.T. Conklin <jtc@NetBSD.org>.
 * Public domain.
 * Adapted for NetBSD/x86_64 by Frank van der Linden <fvdl@wasabisystems.com>
 */

#include <machine/asm.h>

#if defined(LIBC_SCCS)
	RCSID("$NetBSD: strcpy.S,v 1.2 2003/12/04 13:57:31 keihan Exp $")
#endif

/*
 * NOTE: I've unrolled the loop eight times: large enough to make a
 * significant difference, and small enough not to totally trash the
 * cache.
 */

ENTRY(strcpy)
	movq	%rdi,%r11

L1:	movb	(%rsi),%al		/* unroll loop, but not too much */
	movb	%al,(%rdi)
	testb	%al,%al
	jz	L2
	movb	1(%rsi),%al
	movb	%al,1(%rdi)
	testb	%al,%al
	jz	L2
	movb	2(%rsi),%al
	movb	%al,2(%rdi)
	testb	%al,%al
	jz	L2
	movb	3(%rsi),%al
	movb	%al,3(%rdi)
	testb	%al,%al
	jz	L2
	movb	4(%rsi),%al
	movb	%al,4(%rdi)
	testb	%al,%al
	jz	L2
	movb	5(%rsi),%al
	movb	%al,5(%rdi)
	testb	%al,%al
	jz	L2
	movb	6(%rsi),%al
	movb	%al,6(%rdi)
	testb	%al,%al
	jz	L2
	movb	7(%rsi),%al
	movb	%al,7(%rdi)
	addq	$8,%rsi
	addq	$8,%rdi
	testb	%al,%al
	jnz	L1
L2:	movq	%r11,%rax
	ret
