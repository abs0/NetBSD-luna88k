/*	$NetBSD: bootxx.c,v 1.9 2003/01/24 21:55:13 fvdl Exp $ */
/*	$OpenBSD: bootxx.c,v 1.6 2006/05/16 22:52:09 miod Exp $ */

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Paul Kranenburg.
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

/*
 * This is a generic "first-stage" boot program.
 *
 * Note that this program has absolutely no filesystem knowledge!
 *
 * Instead, this uses a table of disk block numbers that are
 * filled in by the installboot program such that this program
 * can load the "second-stage" boot program.
 */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/exec.h>
#include <sys/exec_elf.h>
#include <machine/prom.h>

#include "stand.h"
#include "libsa.h"

int copyboot __P((struct open_file *, char **));

/*
 * Boot device is derived from ROM provided information.
 */

/* This determines the largest boot program we can load. */
#define MAXBLOCKNUM	64

/*
 * These three names are known by installboot.
 * The block_table contains starting block numbers,
 * in terms of 512-byte blocks.  Each non-zero value
 * will result in a read of block_size bytes.
 */
size_t     	block_size = 512;	/* default */
int     	block_count = MAXBLOCKNUM;	/* length of table */
daddr_t 	block_table[MAXBLOCKNUM] = { 0 };

extern		char *version;

int main(void);

int
main()
{
	struct open_file f;
	char *addr;
	char *fname;
	int error;

	printf("\nbootxx: first level bootstrap program [%s]\n", version);
#ifdef DEBUG
	printf("boot device: ctrl=%d, dev=%d\n",
		bugargs.ctrl_lun, bugargs.dev_lun);
#endif

	f.f_flags = F_RAW;
	if (devopen(&f, 0, &fname)) {
		printf("bootxx: open failed\n");
		_rtt();
	}

	addr = (char *)STAGE2_RELOC;
	error = copyboot(&f, &addr);
	f.f_dev->dv_close(&f);
	if (!error) {
		bugexec((mvmeprom_client_entry_point*)(addr + 8));
	}
	/* copyboot had a problem... */
	_rtt();
	return (0);
}

int
copyboot(fp, addr)
    struct open_file *fp;
    char **addr;
{
	size_t n;
	int i, blknum;
	char *laddr = (char *) *addr;
	union boothead {
		struct exec ah;
		Elf32_Ehdr eh;
	} *x;

	x = (union boothead *)laddr;

	if (!block_count) {
		printf("bootxx: no data!?!\n");
		return -1;
	}

	for (i = 0; i < block_count; i++) {

		if ((blknum = block_table[i]) == 0)
			break;

#ifdef DEBUG
		printf("bootxx: read block # %d = %d\n", i, blknum);
#endif
		if ((fp->f_dev->dv_strategy)(fp->f_devdata, F_READ,
		    blknum, block_size, laddr, &n)) {
			printf("bootxx: read failed\n");
			return -1;
		}
		if (n != block_size) {
			printf("bootxx: short read\n");
			return -1;
		}
		laddr += block_size;
	}

	if (N_GETMAGIC(x->ah) == OMAGIC)
		*addr += sizeof(x->ah);
	else
	if (memcmp(x->eh.e_ident, ELFMAG, SELFMAG) == 0) {
		Elf32_Phdr *ep = (Elf32_Phdr *)(*addr + x->eh.e_phoff);
		*addr += ep->p_offset;
	} else {
		printf("bootxx: secondary bootstrap isn't an executable\n");
		return(-1);
	}

	return (0);
}
