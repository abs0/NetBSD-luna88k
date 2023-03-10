/*	$NetBSD: movstrSI.S,v 1.6 2004/03/27 03:48:23 uwe Exp $	*/

/*
 * Copyright (c) 2000 SHIMIZU Ryo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <machine/asm.h>

#ifdef PIC	/* XXX: uwe */
	.hidden __movstrSI4
	.hidden __movstrSI8
	.hidden __movstrSI12
	.hidden __movstrSI16
	.hidden __movstrSI20
	.hidden __movstrSI24
	.hidden __movstrSI28
	.hidden __movstrSI32
	.hidden __movstrSI36
	.hidden __movstrSI40
	.hidden __movstrSI44
	.hidden __movstrSI48
	.hidden __movstrSI52
	.hidden __movstrSI56
	.hidden __movstrSI60
	.hidden __movstrSI64
#endif /* PIC */

NENTRY(__movstrSI64)
	mov.l	@(60, r5), r0
	mov.l	r0, @(60, r4)
ALTENTRY(__movstrSI60)
	mov.l	@(56, r5), r0
	mov.l	r0, @(56, r4)
ALTENTRY(__movstrSI56)
	mov.l	@(52, r5), r0
	mov.l	r0, @(52, r4)
ALTENTRY(__movstrSI52)
	mov.l	@(48, r5), r0
	mov.l	r0, @(48, r4)
ALTENTRY(__movstrSI48)
	mov.l	@(44, r5), r0
	mov.l	r0, @(44, r4)
ALTENTRY(__movstrSI44)
	mov.l	@(40, r5), r0
	mov.l	r0, @(40, r4)
ALTENTRY(__movstrSI40)
	mov.l	@(36, r5), r0
	mov.l	r0, @(36, r4)
ALTENTRY(__movstrSI36)
	mov.l	@(32, r5), r0
	mov.l	r0, @(32, r4)
ALTENTRY(__movstrSI32)
	mov.l	@(28, r5), r0
	mov.l	r0, @(28, r4)
ALTENTRY(__movstrSI28)
	mov.l	@(24, r5), r0
	mov.l	r0, @(24, r4)
ALTENTRY(__movstrSI24)
	mov.l	@(20, r5), r0
	mov.l	r0, @(20, r4)
ALTENTRY(__movstrSI20)
	mov.l	@(16, r5), r0
	mov.l	r0, @(16, r4)
ALTENTRY(__movstrSI16)
	mov.l	@(12, r5), r0
	mov.l	r0, @(12, r4)
ALTENTRY(__movstrSI12)
	mov.l	@(8, r5), r0
	mov.l	r0, @(8, r4)
ALTENTRY(__movstrSI8)
	mov.l	@(4, r5), r0
	mov.l	r0, @(4, r4)
ALTENTRY(__movstrSI4)
	mov.l	@r5, r0
	rts
	 mov.l	r0, @r4
