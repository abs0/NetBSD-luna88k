/*	$NetBSD: disksubr.c,v 1.9 2003/10/08 04:25:45 lukem Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
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
 *	@(#)ufs_disksubr.c	7.16 (Berkeley) 5/4/91
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: disksubr.c,v 1.9 2003/10/08 04:25:45 lukem Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/disklabel.h>
#include <sys/disk.h>
#include <sys/syslog.h>

#include "opt_mbr.h"


int fat_types[] = { MBR_PTYPE_FAT12, MBR_PTYPE_FAT16S,
		    MBR_PTYPE_FAT16B, MBR_PTYPE_FAT32,
		    MBR_PTYPE_FAT32L, MBR_PTYPE_FAT16L,
		    -1 };

#define NO_MBR_SIGNATURE ((struct mbr_partition *) -1)

static struct mbr_partition *
mbr_findslice __P((struct mbr_partition* dp, struct buf *bp));


/* 
 * Scan MBR for  NetBSD partittion.  Return NO_MBR_SIGNATURE if no MBR found
 * Otherwise, copy valid MBR partition-table into dp, and if a NetBSD
 * partition is found, return a pointer to it; else return  NULL.
 */
static
struct mbr_partition *
mbr_findslice(dp, bp)
	struct mbr_partition *dp;
	struct buf *bp;
{
	struct mbr_partition *ourdp = NULL;
	u_int16_t *mbrmagicp;
	int i;

	/* Note: Magic number is little-endian. */
	mbrmagicp = (u_int16_t *)(bp->b_data + MBR_MAGIC_OFFSET);
	if (*mbrmagicp != MBR_MAGIC)
		return (NO_MBR_SIGNATURE);

	/* XXX how do we check veracity/bounds of this? */
	memcpy(dp, bp->b_data + MBR_PART_OFFSET, MBR_PART_COUNT * sizeof(*dp));

	/* look for NetBSD partition */
	for (i = 0; i < MBR_PART_COUNT; i++) {
		if (dp[i].mbrp_type == MBR_PTYPE_NETBSD) {
			ourdp = &dp[i];
			break;
		}
	}

#ifdef COMPAT_386BSD_MBRPART
	/* didn't find it -- look for 386BSD partition */
	if (!ourdp) {
		for (i = 0; i < MBR_PART_COUNT; i++) {
			if (dp[i].mbrp_type == MBR_PTYPE_386BSD) {
				printf("WARNING: old BSD partition ID!\n");
				ourdp = &dp[i];
 				/*
				 * If more than one matches, take last,
				 * as NetBSD install tool does.
				 */
#if 0
				break;
#endif
			}
		}
	}
#endif	/* COMPAT_386BSD_MBRPART */

		return (ourdp);
}


/*
 * Attempt to read a disk label from a device
 * using the indicated strategy routine.
 * The label must be partly set up before this:
 * secpercyl, secsize and anything required for a block i/o read
 * operation in the driver's strategy/start routines
 * must be filled in before calling us.
 *
 * If dos partition table requested, attempt to load it and
 * find disklabel inside a DOS partition. Also, if bad block
 * table needed, attempt to extract it as well. Return buffer
 * for use in signalling errors if requested.
 *
 * Returns null on success and an error string on failure.
 */
const char *
readdisklabel(dev, strat, lp, osdep)
	dev_t dev;
	void (*strat) __P((struct buf *));
	struct disklabel *lp;
	struct cpu_disklabel *osdep;
{
	struct mbr_partition *dp;
	struct partition *pp;
	struct dkbad *bdp;
	struct buf *bp;
	struct disklabel *dlp;
	char *msg = NULL;
	int dospartoff, cyl, i, *ip;

	/* minimal requirements for archtypal disk label */
	if (lp->d_secsize == 0)
		lp->d_secsize = DEV_BSIZE;
	if (lp->d_secperunit == 0)
		lp->d_secperunit = 0x1fffffff;
#if 0
	if (lp->d_ncylinders == 16383) {
		printf("disklabel: Disk > 8G ... readjusting chs %d/%d/%d to ",
			lp->d_ncylinders, lp->d_ntracks, lp->d_nsectors);
		lp->d_ncylinders = lp->d_secperunit /  lp->d_ntracks / lp->d_nsectors;
		printf("%d/%d/%d\n",
			lp->d_ncylinders, lp->d_ntracks, lp->d_nsectors);
	}
#endif
	lp->d_npartitions = RAW_PART + 1;
	for (i = 0; i < RAW_PART; i++) {
		lp->d_partitions[i].p_size = 0;
		lp->d_partitions[i].p_offset = 0;
	}
	if (lp->d_partitions[i].p_size == 0)
		lp->d_partitions[i].p_size = 0x1fffffff;
	lp->d_partitions[i].p_offset = 0;

	/* get a buffer and initialize it */
	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;

	/* do dos partitions in the process of getting disklabel? */
	dospartoff = 0;
	cyl = LABELSECTOR / lp->d_secpercyl;
	if (!osdep)
		goto nombrpart;
	dp = osdep->dosparts;

	/* read master boot record */
	bp->b_blkno = MBR_BBSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	bp->b_cylinder = MBR_BBSECTOR / lp->d_secpercyl;
	(*strat)(bp);

	/* if successful, wander through dos partition table */
	if (biowait(bp)) {
		msg = "dos partition I/O error";
		goto done;
	} else {
		struct mbr_partition *ourdp = NULL;

		ourdp = mbr_findslice(dp, bp);
		if (ourdp ==  NO_MBR_SIGNATURE)
			goto nombrpart;

		for (i = 0; i < MBR_PART_COUNT; i++, dp++) {
			/* Install in partition e, f, g, or h. */
			pp = &lp->d_partitions[RAW_PART + 1 + i];
			pp->p_offset = dp->mbrp_start;
			pp->p_size = dp->mbrp_size;
			for (ip = fat_types; *ip != -1; ip++) {
				if (dp->mbrp_type == *ip)
					pp->p_fstype = FS_MSDOS;
			}
			if (dp->mbrp_type == MBR_PTYPE_LNXEXT2)
				pp->p_fstype = FS_EX2FS;

			if (dp->mbrp_type == MBR_PTYPE_NTFS)
				pp->p_fstype = FS_NTFS;

			/* is this ours? */
			if (dp == ourdp) {
				/* need sector address for SCSI/IDE,
				 cylinder for ESDI/ST506/RLL */
				dospartoff = dp->mbrp_start;
				cyl = MBR_PCYL(dp->mbrp_scyl, dp->mbrp_ssect);

				/* update disklabel with details */
				lp->d_partitions[2].p_size =
				    dp->mbrp_size;
				lp->d_partitions[2].p_offset = 
				    dp->mbrp_start;
#if 0
				if (lp->d_ntracks != dp->mbrp_ehd + 1 ||
				    lp->d_nsectors != DPSECT(dp->mbrp_esect)) {
					printf("disklabel: BIOS sees chs %d/%d/%d as ",
						lp->d_ncylinders, lp->d_ntracks,
						lp->d_nsectors);
					lp->d_ntracks = dp->mbrp_ehd + 1;
					lp->d_nsectors = DPSECT(dp->mbrp_esect);
					lp->d_secpercyl = lp->d_ntracks * lp->d_nsectors;
					lp->d_ncylinders = lp->d_secperunit / lp->d_secpercyl;
					if (! lp->d_ncylinders)
						lp->d_ncylinders = 1;
					printf("%d/%d/%d\n",
						lp->d_ncylinders, lp->d_ntracks,
						lp->d_nsectors);
				    }
#endif
			}
		}
		lp->d_npartitions = RAW_PART + 1 + i;
	}

nombrpart:
	/* next, dig out disk label */
	bp->b_blkno = dospartoff + LABELSECTOR;
	bp->b_cylinder = cyl;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	(*strat)(bp);

	/* if successful, locate disk label within block and validate */
	if (biowait(bp)) {
		msg = "disk label I/O error";
		goto done;
	}
	for (dlp = (struct disklabel *)bp->b_data;
	    dlp <= (struct disklabel *)(bp->b_data + lp->d_secsize - sizeof(*dlp));
	    dlp = (struct disklabel *)((char *)dlp + sizeof(long))) {
		if (dlp->d_magic != DISKMAGIC || dlp->d_magic2 != DISKMAGIC) {
			if (msg == NULL)
				msg = "no disk label";
		} else if (dlp->d_npartitions > MAXPARTITIONS ||
			   dkcksum(dlp) != 0)
			msg = "disk label corrupted";
		else {
			*lp = *dlp;
			msg = NULL;
			break;
		}
	}

	if (msg)
		goto done;

	/* obtain bad sector table if requested and present */
	if (osdep && (lp->d_flags & D_BADSECT)) {
		struct dkbad *db;

		bdp = &osdep->bad;
		i = 0;
		do {
			/* read a bad sector table */
			bp->b_flags = B_BUSY | B_READ;
			bp->b_blkno = lp->d_secperunit - lp->d_nsectors + i;
			if (lp->d_secsize > DEV_BSIZE)
				bp->b_blkno *= lp->d_secsize / DEV_BSIZE;
			else
				bp->b_blkno /= DEV_BSIZE / lp->d_secsize;
			bp->b_bcount = lp->d_secsize;
			bp->b_cylinder = lp->d_ncylinders - 1;
			(*strat)(bp);

			/* if successful, validate, otherwise try another */
			if (biowait(bp)) {
				msg = "bad sector table I/O error";
			} else {
				db = (struct dkbad *)(bp->b_data);
#define DKBAD_MAGIC 0x4321
				if (db->bt_mbz == 0
					&& db->bt_flag == DKBAD_MAGIC) {
					msg = NULL;
					*bdp = *db;
					break;
				} else
					msg = "bad sector table corrupted";
			}
		} while ((bp->b_flags & B_ERROR) && (i += 2) < 10 &&
			i < lp->d_nsectors);
	}

done:
	bp->b_flags |= B_INVAL;
	brelse(bp);
	return (msg);
}

/*
 * Check new disk label for sensibility
 * before setting it.
 */
int
setdisklabel(olp, nlp, openmask, osdep)
	struct disklabel *olp, *nlp;
	u_long openmask;
	struct cpu_disklabel *osdep;
{
	int i;
	struct partition *opp, *npp;

	/* sanity clause */
	if (nlp->d_secpercyl == 0 || nlp->d_secsize == 0
		|| (nlp->d_secsize % DEV_BSIZE) != 0)
			return(EINVAL);

	/* special case to allow disklabel to be invalidated */
	if (nlp->d_magic == 0xffffffff) {
		*olp = *nlp;
		return (0);
	}

	if (nlp->d_magic != DISKMAGIC || nlp->d_magic2 != DISKMAGIC ||
	    dkcksum(nlp) != 0)
		return (EINVAL);

	/* XXX missing check if other dos partitions will be overwritten */

	while (openmask != 0) {
		i = ffs(openmask) - 1;
		openmask &= ~(1 << i);
		if (nlp->d_npartitions <= i)
			return (EBUSY);
		opp = &olp->d_partitions[i];
		npp = &nlp->d_partitions[i];
		if (npp->p_offset != opp->p_offset || npp->p_size < opp->p_size)
			return (EBUSY);
		/*
		 * Copy internally-set partition information
		 * if new label doesn't include it.		XXX
		 */
		if (npp->p_fstype == FS_UNUSED && opp->p_fstype != FS_UNUSED) {
			npp->p_fstype = opp->p_fstype;
			npp->p_fsize = opp->p_fsize;
			npp->p_frag = opp->p_frag;
			npp->p_cpg = opp->p_cpg;
		}
	}
 	nlp->d_checksum = 0;
 	nlp->d_checksum = dkcksum(nlp);
	*olp = *nlp;
	return (0);
}


/*
 * Write disk label back to device after modification.
 */
int
writedisklabel(dev, strat, lp, osdep)
	dev_t dev;
	void (*strat) __P((struct buf *));
	struct disklabel *lp;
	struct cpu_disklabel *osdep;
{
	struct mbr_partition *dp;
	struct buf *bp;
	struct disklabel *dlp;
	int error, dospartoff, cyl;

	/* get a buffer and initialize it */
	bp = geteblk((int)lp->d_secsize);
	bp->b_dev = dev;

	/* do dos partitions in the process of getting disklabel? */
	dospartoff = 0;
	cyl = LABELSECTOR / lp->d_secpercyl;
	if (!osdep)
		goto nombrpart;
	dp = osdep->dosparts;

	/* read master boot record */
	bp->b_blkno = MBR_BBSECTOR;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	bp->b_cylinder = MBR_BBSECTOR / lp->d_secpercyl;
	(*strat)(bp);

	if ((error = biowait(bp)) == 0) {
		struct mbr_partition *ourdp = NULL;

		ourdp = mbr_findslice(dp, bp);
		if (ourdp ==  NO_MBR_SIGNATURE)
			goto nombrpart;

		if (ourdp) {
			/* need sector address for SCSI/IDE,
			 cylinder for ESDI/ST506/RLL */
			dospartoff = ourdp->mbrp_start;
			cyl = MBR_PCYL(ourdp->mbrp_scyl, ourdp->mbrp_ssect);
		}
	}

nombrpart:
#ifdef maybe
	/* disklabel in appropriate location? */
	if (lp->d_partitions[2].p_offset != 0
		&& lp->d_partitions[2].p_offset != dospartoff) {
		error = EXDEV;		
		goto done;
	}
#endif

	/* next, dig out disk label */
	bp->b_blkno = dospartoff + LABELSECTOR;
	bp->b_cylinder = cyl;
	bp->b_bcount = lp->d_secsize;
	bp->b_flags = B_BUSY | B_READ;
	(*strat)(bp);

	/* if successful, locate disk label within block and validate */
	if ((error = biowait(bp)) != 0)
		goto done;
	for (dlp = (struct disklabel *)bp->b_data;
	    dlp <= (struct disklabel *)(bp->b_data + lp->d_secsize - sizeof(*dlp));
	    dlp = (struct disklabel *)((char *)dlp + sizeof(long))) {
		if (dlp->d_magic == DISKMAGIC && dlp->d_magic2 == DISKMAGIC &&
		    dkcksum(dlp) == 0) {
			*dlp = *lp;
			bp->b_flags = B_BUSY | B_WRITE;
			(*strat)(bp);
			error = biowait(bp);
			goto done;
		}
	}
	error = ESRCH;

done:
	bp->b_flags |= B_INVAL;
	brelse(bp);
	return (error);
}

/*
 * Determine the size of the transfer, and make sure it is
 * within the boundaries of the partition. Adjust transfer
 * if needed, and signal errors or early completion.
 */
int
bounds_check_with_label(dk, bp, wlabel)
	struct disk *dk;
	struct buf *bp;
	int wlabel;
{
	struct disklabel *lp = dk->dk_label;
	struct partition *p = lp->d_partitions + DISKPART(bp->b_dev);
	int labelsector = lp->d_partitions[2].p_offset + LABELSECTOR;
	int sz;

	sz = howmany(bp->b_bcount, lp->d_secsize);

	if (bp->b_blkno + sz > p->p_size) {
		sz = p->p_size - bp->b_blkno;
		if (sz == 0) {
			/* If exactly at end of disk, return EOF. */
			bp->b_resid = bp->b_bcount;
			return (0);
		}
		if (sz < 0) {
			/* If past end of disk, return EINVAL. */
			bp->b_error = EINVAL;
			goto bad;
		}
		/* Otherwise, truncate request. */
		bp->b_bcount = sz << DEV_BSHIFT;
	}

	/* Overwriting disk label? */
	if (bp->b_blkno + p->p_offset <= labelsector &&
#if LABELSECTOR != 0
	    bp->b_blkno + p->p_offset + sz > labelsector &&
#endif
	    (bp->b_flags & B_READ) == 0 && !wlabel) {
		bp->b_error = EROFS;
		goto bad;
	}

	/* calculate cylinder for disksort to order transfers with */
	bp->b_cylinder = (bp->b_blkno + p->p_offset) /
	    (lp->d_secsize / DEV_BSIZE) / lp->d_secpercyl;
	return (1);

bad:
	bp->b_flags |= B_ERROR;
	return (-1);
}
