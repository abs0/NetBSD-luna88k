/*	$NetBSD: __setjmp14.S,v 1.10 2004/03/23 02:21:49 simonb Exp $	*/

/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell.
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
 */

#include <machine/cdefs.h>
#include <sys/syscall.h>
#include <mips/regnum.h>
#include <mips/asm.h>
#include <machine/signal.h>
#include <machine/setjmp.h>

#if defined(LIBC_SCCS) && !defined(lint)
	ASMSTR("from: @(#)setjmp.s	8.1 (Berkeley) 6/4/93")
	ASMSTR("$NetBSD: __setjmp14.S,v 1.10 2004/03/23 02:21:49 simonb Exp $")
#endif /* LIBC_SCCS and not lint */

#ifdef __ABICALLS__
	.abicalls
#endif

/*
 * C library -- setjmp, longjmp
 *
 *	longjmp(a,v)
 * will generate a "return(v)" from
 * the last call to
 *	setjmp(a)
 * by restoring registers from the stack,
 * and a struct sigcontext, see <signal.h>
 */

#define SETJMP_FRAME_SIZE	(CALLFRAME_SIZ + 12)

NON_LEAF(__setjmp14, SETJMP_FRAME_SIZE, ra)
	.mask	0x80000000, (CALLFRAME_RA - CALLFRAME_SIZ)
#ifdef __ABICALLS__
	.set	noreorder
	.cpload	t9
	.set	reorder
#endif
	subu	sp, sp, SETJMP_FRAME_SIZE	# allocate stack frame
#ifdef __ABICALLS__
	.cprestore 16
#endif
	sw	ra, CALLFRAME_RA(sp)		# save RA
	sw	a0, SETJMP_FRAME_SIZE(sp)	# squirrel away sc

	/* Get the signal mask. */
	addu	a2, a0, _OFFSETOF_SC_MASK	# &sc.sc_mask
	move	a1, zero
	jal	_C_LABEL(__sigprocmask14)	# get current signal mask
	move	a0, zero			# (in delay slot)

	/* Get the signal stack. */
	move	a0, zero
	addu	a1, sp, CALLFRAME_SIZ	# pointer to struct sigaltstack
	jal	_C_LABEL(__sigaltstack14)
	nop
	lw	v1, CALLFRAME_SIZ+8(sp)	# get old ss_onstack
	lw	a0, SETJMP_FRAME_SIZE(sp)	# restore sc pointer
	and	v1, v1, 1			# extract onstack flag
	sw	v1, 0(a0)			# save it in sc_onstack

	lw	ra, CALLFRAME_RA(sp)		# restore RA
	addu	sp, sp, SETJMP_FRAME_SIZE	# pop stack frame

	blt	v0, zero, botch			# check for sigaltstack() error

	REG_PROLOGUE
	REG_S	ra, (2 * 4)(a0)			# sc_pc = return address
	REG_LI	v0, 0xACEDBADE			# sigcontext magic number
	REG_S	v0, (_OFFSETOF_SC_REGS + _R_ZERO * SZREG)(a0) #   saved in sc_regs[0]
	REG_S	s0, (_OFFSETOF_SC_REGS + _R_S0 * SZREG)(a0)
	REG_S	s1, (_OFFSETOF_SC_REGS + _R_S1 * SZREG)(a0)
	REG_S	s2, (_OFFSETOF_SC_REGS + _R_S2 * SZREG)(a0)
	REG_S	s3, (_OFFSETOF_SC_REGS + _R_S3 * SZREG)(a0)
	REG_S	s4, (_OFFSETOF_SC_REGS + _R_S4 * SZREG)(a0)
	REG_S	s5, (_OFFSETOF_SC_REGS + _R_S5 * SZREG)(a0)
	REG_S	s6, (_OFFSETOF_SC_REGS + _R_S6 * SZREG)(a0)
	REG_S	s7, (_OFFSETOF_SC_REGS + _R_S7 * SZREG)(a0)
	REG_S	gp, (_OFFSETOF_SC_REGS + _R_GP * SZREG)(a0)
	REG_S	sp, (_OFFSETOF_SC_REGS + _R_SP * SZREG)(a0)
	REG_S	s8, (_OFFSETOF_SC_REGS + _R_S8 * SZREG)(a0)
	li	v0, 1				# be nice if we could tell
	sw	v0, (_OFFSETOF_SC_REGS + 34 * SZREG)(a0)	# sc_fpused = 1
	cfc1	v0, $31
	swc1	$f20, (20 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f21, (21 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f22, (22 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f23, (23 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f24, (24 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f25, (25 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f26, (26 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f27, (27 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f28, (28 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f29, (29 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f30, (30 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	swc1	$f31, (31 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	sw	v0, (32 * 4 + _OFFSETOF_SC_FPREGS)(a0)
	move	v0, zero
	j	ra
	REG_EPILOGUE
botch:
	jal	_C_LABEL(abort)
END(__setjmp14)
