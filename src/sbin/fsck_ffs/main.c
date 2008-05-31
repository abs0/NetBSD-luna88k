/*	$NetBSD: main.c,v 1.57 2005/01/20 15:29:40 xtraeme Exp $	*/

/*
 * Copyright (c) 1980, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
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

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT("@(#) Copyright (c) 1980, 1986, 1993\n\
	The Regents of the University of California.  All rights reserved.\n");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)main.c	8.6 (Berkeley) 5/14/95";
#else
__RCSID("$NetBSD: main.c,v 1.57 2005/01/20 15:29:40 xtraeme Exp $");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/resource.h>

#include <ufs/ufs/dinode.h>
#include <ufs/ufs/ufsmount.h>
#include <ufs/ffs/fs.h>
#include <ufs/ffs/ffs_extern.h>

#include <ctype.h>
#include <err.h>
#include <fstab.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fsck.h"
#include "extern.h"
#include "fsutil.h"

int	returntosingle;
int	progress = 0;

static int	argtoi(int, char *, char *, int);
static int	checkfilesys(const char *, char *, long, int);
static void	usage(void);

int
main(int argc, char *argv[])
{
	struct rlimit r;
	int ch;
	int ret = 0;

	if (getrlimit(RLIMIT_DATA, &r) == 0) {
		r.rlim_cur = r.rlim_max;
		(void) setrlimit(RLIMIT_DATA, &r);
	}
	sync();
	skipclean = 1;
	markclean = 1;
	forceimage = 0;
	endian = 0;
	isappleufs = 0;
	while ((ch = getopt(argc, argv, "aB:b:c:dFfm:npPqy")) != -1) {
		switch (ch) {
		case 'a':
			isappleufs = 1;
			break;

		case 'B':
			if (strcmp(optarg, "be") == 0)
				endian = BIG_ENDIAN;
			else if (strcmp(optarg, "le") == 0)
				endian = LITTLE_ENDIAN;
			else usage();
			break;

		case 'b':
			skipclean = 0;
			bflag = argtoi('b', "number", optarg, 10);
			printf("Alternate super block location: %d\n", bflag);
			break;

		case 'c':
			skipclean = 0;
			cvtlevel = argtoi('c', "conversion level", optarg, 10);
			if (cvtlevel > 4) {
				cvtlevel = 4;
				warnx("Using maximum conversion level of %d\n",cvtlevel);
			}
			break;
		
		case 'd':
			debug++;
			break;

		case 'F':
			forceimage = 1;
			break;

		case 'f':
			skipclean = 0;
			break;

		case 'm':
			lfmode = argtoi('m', "mode", optarg, 8);
			if (lfmode &~ 07777)
				errx(EEXIT, "bad mode to -m: %o", lfmode);
			printf("** lost+found creation mode %o\n", lfmode);
			break;

		case 'n':
			nflag++;
			yflag = 0;
			break;

		case 'p':
			preen++;
			break;

		case 'P':
			progress = 1;
			break;

		case 'q':
			quiet++;
			break;

		case 'y':
			yflag++;
			nflag = 0;
			break;

		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;

	if (!argc)
		usage();

	if (debug)
		progress = 0;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void)signal(SIGINT, catch);
	if (preen)
		(void)signal(SIGQUIT, catchquit);
#ifdef PROGRESS
	if (progress) {
		progress_ttywidth(0);
		(void)signal(SIGWINCH, progress_ttywidth);
	}
#endif /* ! PROGRESS */
	signal(SIGINFO, infohandler);

	while (argc-- > 0) {
		const char *path = blockcheck(*argv);

		if (path == NULL)
			pfatal("Can't check %s\n", *argv);
		else
			(void)checkfilesys(blockcheck(*argv), 0, 0L, 0);
		argv++;
	}

	if (returntosingle)
		ret = 2;

	exit(ret);
}

static int
argtoi(int flag, char *req, char *str, int base)
{
	char *cp;
	int ret;

	ret = (int)strtol(str, &cp, base);
	if (cp == str || *cp)
		errx(EEXIT, "-%c flag requires a %s", flag, req);
	return (ret);
}

/*
 * Check the specified filesystem.
 */
/* ARGSUSED */
static int
checkfilesys(const char *filesys, char *mntpt, long auxdata, int child)
{
	daddr_t n_ffree, n_bfree;
	struct dups *dp;
	struct zlncnt *zlnp;
	int cylno;
#ifdef LITE2BORKEN
	int flags;
#endif
#ifdef PROGRESS
	off_t progress_total = 0;
#endif /* PROGRESS */

	if (preen && child)
		(void)signal(SIGQUIT, voidquit);
	setcdevname(filesys, preen);
	if (debug && preen)
		pwarn("starting\n");
	switch (setup(filesys)) {
	case 0:
		if (preen)
			pfatal("CAN'T CHECK FILE SYSTEM.");
		/* fall through */
	case -1:
		return (0);
	}
	/*
	 * Cleared if any questions answered no. Used to decide if
	 * the superblock should be marked clean.
	 */
	resolved = 1;

#ifdef PROGRESS
	/*
	 * Pass 1, Pass 4, and Pass 5 all iterate over cylinder
	 * groups.  Account for those now.  We'll never need to
	 * add in Pass 1b, since that pass is never executed when
	 * preening.
	 *
	 * Pass 2 and Pass 3 iterate over directory inodes, but we
	 * don't know how many of those exist until after Pass 1.
	 * We'll add those in after Pass 1 has completed.
	 */
	if (preen)
		progress_total += sblock->fs_ncg * 3;
#endif /* PROGRESS */

	/*
	 * 1: scan inodes tallying blocks used
	 */
	if (preen == 0) {
		pwarn("** Last Mounted on %s\n", sblock->fs_fsmnt);
		if (hotroot())
			pwarn("** Root file system\n");
		pwarn("** Phase 1 - Check Blocks and Sizes\n");
	}
	pass1();

#ifdef PROGRESS
	/* Account for number of directory inodes (used twice). */
	if (preen)
		progress_total += inplast * 2;
	progress_switch(progress);
	progress_init(progress_total);
#endif /* PROGRESS */


	/*
	 * 1b: locate first references to duplicates, if any
	 */
	if (duplist) {
		if (preen)
			pfatal("INTERNAL ERROR: dups with -p\n");
		if (usedsoftdep)
			pfatal("INTERNAL ERROR: dups with softdep\n");
		pwarn("** Phase 1b - Rescan For More DUPS\n");
		pass1b();
	}

	/*
	 * 2: traverse directories from root to mark all connected directories
	 */
	if (preen == 0)
		pwarn("** Phase 2 - Check Pathnames\n");
	pass2();

	/*
	 * 3: scan inodes looking for disconnected directories
	 */
	if (preen == 0)
		pwarn("** Phase 3 - Check Connectivity\n");
	pass3();

	/*
	 * 4: scan inodes looking for disconnected files; check reference counts
	 */
	if (preen == 0)
		pwarn("** Phase 4 - Check Reference Counts\n");
	pass4();

	/*
	 * 5: check and repair resource counts in cylinder groups
	 */
	if (preen == 0)
		pwarn("** Phase 5 - Check Cyl groups\n");
	pass5();

	/*
	 * print out summary statistics
	 */
	n_ffree = sblock->fs_cstotal.cs_nffree;
	n_bfree = sblock->fs_cstotal.cs_nbfree;
	pwarn("%d files, %lld used, %lld free ",
	    n_files, (long long)n_blks,
	    (long long)(n_ffree + sblock->fs_frag * n_bfree));
	printf("(%lld frags, %lld blocks, %lld.%lld%% fragmentation)\n",
	    (long long)n_ffree, (long long)n_bfree,
	    (long long)(n_ffree * 100 / (daddr_t)sblock->fs_dsize),
	    (long long)(((n_ffree * 1000 + (daddr_t)sblock->fs_dsize / 2)
		/ (daddr_t)sblock->fs_dsize) % 10));
	if (debug &&
	    (n_files -= maxino - ROOTINO - sblock->fs_cstotal.cs_nifree))
		printf("%d files missing\n", n_files);
	if (debug) {
		n_blks += sblock->fs_ncg *
			(cgdmin(sblock, 0) - cgsblock(sblock, 0));
		n_blks += cgsblock(sblock, 0) - cgbase(sblock, 0);
		n_blks += howmany(sblock->fs_cssize, sblock->fs_fsize);
		if (n_blks -= maxfsblock - (n_ffree + sblock->fs_frag * n_bfree))
			printf("%lld blocks missing\n", (long long)n_blks);
		if (duplist != NULL) {
			printf("The following duplicate blocks remain:");
			for (dp = duplist; dp; dp = dp->next)
				printf(" %lld,", (long long)dp->dup);
			printf("\n");
		}
		if (zlnhead != NULL) {
			printf("The following zero link count inodes remain:");
			for (zlnp = zlnhead; zlnp; zlnp = zlnp->next)
				printf(" %u,", zlnp->zlncnt);
			printf("\n");
		}
	}
	zlnhead = (struct zlncnt *)0;
	duplist = (struct dups *)0;
	muldup = (struct dups *)0;
	inocleanup();
	if (fsmodified) {
		sblock->fs_time = time(NULL);
		sbdirty();
	}
	if (rerun)
		markclean = 0;
#if LITE2BORKEN
	if (!hotroot()) {
		ckfini();
	} else {
		struct statvfs stfs_buf;
		/*
		 * Check to see if root is mounted read-write.
		 */
		if (statvfs("/", &stfs_buf) == 0)
			flags = stfs_buf.f_flag;
		else
			flags = 0;
		if (markclean)
			markclean = flags & MNT_RDONLY;
		ckfini();
	}
#else
	ckfini();
#endif
	for (cylno = 0; cylno < sblock->fs_ncg; cylno++)
		if (inostathead[cylno].il_stat != NULL)
			free(inostathead[cylno].il_stat);
	free(inostathead);
	inostathead = NULL;

	if (!resolved || rerun) {
		pwarn("\n***** UNRESOLVED INCONSISTENCIES REMAIN *****\n");
		returntosingle = 1;
	}
	if (!fsmodified)
		return (0);
	if (!preen)
		pwarn("\n***** FILE SYSTEM WAS MODIFIED *****\n");
	if (rerun)
		pwarn("\n***** PLEASE RERUN FSCK *****\n");
	if (hotroot()) {
		struct statvfs stfs_buf;
		/*
		 * We modified the root.  Do a mount update on
		 * it, unless it is read-write, so we can continue.
		 */
		if (statvfs("/", &stfs_buf) == 0) {
			long flags = stfs_buf.f_flag;
			struct ufs_args args;
			int ret;

			if (flags & MNT_RDONLY) {
				args.fspec = 0;
				args.export.ex_flags = 0;
				args.export.ex_root = 0;
				flags |= MNT_UPDATE | MNT_RELOAD;
				ret = mount(MOUNT_FFS, "/", flags, &args);
				if (ret == 0)
					return(0);
			}
		}
		if (!preen)
			pwarn("\n***** REBOOT NOW *****\n");
		sync();
		return (4);
	}
	return (0);
}

static void
usage(void)
{

	(void) fprintf(stderr,
	    "usage: %s [-adFfnPpqy] [-B be|le] [-b block] [-c level] [-m mode]"
	    " filesystem ...\n",
	    getprogname());
	exit(1);
}

