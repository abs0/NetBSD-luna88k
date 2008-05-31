/*	$NetBSD: inkernel.c,v 1.4 2003/04/20 18:17:07 bjh21 Exp $	*/

/*-
 * Copyright (c) 1996, 1997 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Kazuki Sakamoto.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stand.h>
#include <sys/param.h>
#include "boot.h"
#include "magic.h"

#define	KERNENTRY	(RELOC - 0x200000)

void
init_in()
{
	int p;

	for (p = endaddr(); p < KERNENTRY; p += sizeof (int)) {
		if (*(int *)p != ENTRY ||
		    *(int *)(p + sizeof (int)) != 0 ||
		    *(int *)(p + sizeof (int) * 2) != 0)
			continue;

		p += sizeof (int) * 3;
		if (memcmp((char *)p, magic, MAGICSIZE) == 0) {
			kern_len = *(int *)(p + MAGICSIZE);
			memcpy((char *)KERNENTRY,
				(char *)(p + MAGICSIZE + KERNLENSIZE),
				kern_len);
			break;
		}
	}
}

int
inopen(p)
	struct open_file *p;
{
	if (kern_len)
		return (0);
	return (EINVAL);
}

int
inclose(p)
	struct open_file *p;
{
	return (0);
}

int
instrategy(devdata, func, blk, size, buf, rsize)
	void *devdata;	/* device uniq data */
	int func;	/* function (read or write) */
	daddr_t blk;	/* block number */
	size_t size;	/* request size in bytes */
	void *buf;	/* buffer */
	size_t *rsize;	/* bytes transferred */
{

	memcpy(buf, (char *)KERNENTRY + blk * DEV_BSIZE, size);
	*rsize = size;
	return (0);
}