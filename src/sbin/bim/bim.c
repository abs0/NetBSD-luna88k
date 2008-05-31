/*	$NetBSD: bim.c,v 1.17 2005/02/05 14:23:24 xtraeme Exp $	*/

/*
 * Copyright (c) 1994 Philip A. Nelson.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Philip A. Nelson.
 * 4. The name of Philip A. Nelson may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PHILIP NELSON ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL PHILIP NELSON BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: bim.c,v 1.17 2005/02/05 14:23:24 xtraeme Exp $");
#endif /* not lint */

/*
 *  Boot Image Manager
 *
 *  (First copy called "hdsetup" and was written under Minix.)
 *
 *   Phil Nelson
 *   Sept 30, 1990
 */

#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <a.out.h>
#include <err.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#define FSTYPENAMES
#include <sys/disklabel.h>
#include "images.h"

#include "extern.h"

int	copy_bytes(int, int, int, int, int);
unsigned short dkcksum(struct disklabel *);
void	getlf(char);
void	getstr(char *, int);
void	init_images(char);
void	save_images(void);
void	usage(void);

#define TRUE 1
#define FALSE 0
#define MAXARGCMDS 20

#define BLOCK_SIZE 1024

#define DEFAULT_DEVICE "/dev/sd0c"

/*  Global data....  */
char    disk_info[BLOCK_SIZE];
int     disk_fd;		/* The file descriptor for the disk. */
int	yesmode;

struct disklabel *dk_label;
struct imageinfo *im_table;

int     label_changed = FALSE;
int     images_changed = FALSE;
int     secsize;


/* Utility routines... */
/***********************/

void 
usage(void)
{
	printf("usage: %s [-y] [-c command [-c command ...]] [device]\n",

	    getprogname());
	printf("  Maximum of %d commands\n", MAXARGCMDS);
	exit(1);
}

void 
getlf(char inchar)
{

	while (inchar != '\n')
		inchar = getchar();
}

void 
getstr(char *str, int size)
{
	char	inchar;
	int	count;

	count = 0;
	inchar = getchar();
	while (count < size - 1 && inchar != '\n') {
		*str++ = inchar;
		count++;
		inchar = getchar();
	}
	*str++ = 0;
	getlf(inchar);
}

/* Checksum a disk label */
unsigned short
dkcksum(struct disklabel *lp)
{
	unsigned short *start, *end, sum = 0;

	start = (unsigned short *) lp;
	end = (unsigned short *) &lp->d_partitions[lp->d_npartitions];
	while (start < end)
		sum ^= *start++;
	return sum;
}

void 
save_images(void)
{
	int     count;

	count = (int) lseek(disk_fd, (off_t) 0, SEEK_SET);
	if (count != 0)
		err(1, "lseek error in saving image info");
	count = write(disk_fd, disk_info, BLOCK_SIZE);
	if (count != BLOCK_SIZE)
		err(1, "write error in saving image info");
	sync();
}

/* This function will initialize the image information . */
void 
init_images(char badmagic)
{
	char    answer[80];
	int     idx;

	if (badmagic) {
		printf("Image information has improper magic number.\n");
		while (TRUE) {
			if (yesmode == 1)
				break;
			prompt(answer, 3,
			    "Do you want the images initialized? (y or n) ");
			if (answer[0] == 'y')
				break;
			if (answer[0] == 'n')
				errx(1, "images not initialized.");
		}
	}
	/* Initialize the image table. */
	im_table->ii_magic = IMAGE_MAGIC;
	im_table->ii_boot_partition = -1;
	for (idx = 0; idx < dk_label->d_npartitions; idx++)
		if (dk_label->d_partitions[idx].p_fstype == FS_BOOT) {
			im_table->ii_boot_partition = idx;
			break;
		}
	im_table->ii_boot_count = MAXIMAGES;
	im_table->ii_boot_used = 0;
	im_table->ii_boot_default = -1;
	images_changed = TRUE;
}

/* Print out the header and other information about the disk. */
int 
display_part(int num, char **args, char *syntax)
{
	int     count;

	printf("\nDisk: %s   Type: %s\n", dk_label->d_packname,
	    dk_label->d_typename);
	printf("Physical Sector Size = %d\n", dk_label->d_secsize);
	printf("Disk Size = %ld\n", (long)dk_label->d_secperunit);

	/* Disk Partitions. */
	printf(" partition         type  sector start  length in sectors\n");
	for (count = 0; count < dk_label->d_npartitions; count++) {
		if (dk_label->d_partitions[count].p_fstype != FS_UNUSED) {
			printf(" %5c  ", 'a' + count);
			printf("%14s",
			    fstypenames[dk_label->d_partitions[count].p_fstype]);
			printf("%14ld  %17ld\n",
			    (long)dk_label->d_partitions[count].p_offset,
			    (long)dk_label->d_partitions[count].p_size);
		}
	}
	printf("\n");

	return FALSE;
}

int 
display_image(int num, char **args, char *syntax)
{
	int     count;

	/* Boot Images. */
	if (im_table->ii_boot_partition != -1)
		printf("Boot partition = %c\n",
		    'a' + im_table->ii_boot_partition);
	if (im_table->ii_boot_default != -1)
		printf("Default boot image  = %d\n", im_table->ii_boot_default);
	printf("Boot Images: total of %d\n", im_table->ii_boot_count);
	printf("  (image address and size in sectors.)\n");
	printf("Image  address   size  load addr  run addr   name\n");
	for (count = 0; count < im_table->ii_boot_used; count++) {
		printf("%5d %8ld %6ld  %#9lx %#9lx   %s\n", count,
		    im_table->ii_images[count].boot_address / secsize,
		    im_table->ii_images[count].boot_size / secsize,
		    im_table->ii_images[count].boot_load_adr,
		    im_table->ii_images[count].boot_run_adr,
		    im_table->ii_images[count].boot_name);
	}
	printf("\n");
	return FALSE;
}

int 
display_head(int num, char **args, char *syntax)
{

	printf("\nDisk: %s   Type: %s\n", dk_label->d_packname,
	    dk_label->d_typename);
	printf("Physical Sector Size = %d\n", dk_label->d_secsize);
	printf("Disk Size = %ld\n", (long)dk_label->d_secperunit);

	return FALSE;
}


/*
 * Utility routine for moving boot images.  These are byte addresses
 *  relative to the start of the files.
 */
int 
copy_bytes(int from_fd, int from_adr, int to_fd, int to_adr, int number)
{
	int     count;
	int     idx;
	char    buffer[BLOCK_SIZE];

	/* Check the parameters. */
	if (to_adr > from_adr && from_fd == to_fd) {
		printf("There is a system error. (copy_bytes)\n");
		return 0;
	}
	/* Do the copy. */
	for (idx = 0; idx < number; idx += BLOCK_SIZE) {
		count = lseek(from_fd, (off_t) (from_adr + idx), SEEK_SET);
		if (count != from_adr + idx) {
			printf("Error in copying (seek from)\n");
			return 0;
		}
		count = read(from_fd, buffer, BLOCK_SIZE);
		if (count != BLOCK_SIZE) {
			if (idx != number - 1 || count < 0) {
				printf("Error in copying (read from)\n");
				return 0;
			} else {
				while (count < BLOCK_SIZE)
					buffer[count++] = 0;
			}
		}
		count = lseek(to_fd, (off_t) (to_adr + idx), SEEK_SET);
		if (count != to_adr + idx) {
			printf("Error in copying (seek to)\n");
			return 0;
		}
		count = write(to_fd, buffer, BLOCK_SIZE);
		if (count != BLOCK_SIZE) {
			printf("Error in copying (write to)\n");
			return 0;
		}
	}

	/* Success. */
	return 1;
}

/* Add a boot image. */
int
add_image(int num, char **args, char *syntax)
{
	struct exec im_exec;	/* Information about a new image. */
	int     im_file;
	int     im_size;	/* Size of text and data in bytes. Rounded up
				 * to a full block in both text and data. */
	int     which_image;	/* Which image is to be operated upon. */
	int     count;		/* read/write counts. */
	int     idx;		/* temporary variable for loops. */

	int     part_size;	/* The total size of the boot partition (in
				 * bytes). */
	int     total_size;	/* The total size of all images (in bytes). */
	int     boot_start;	/* Byte address of start of boot partition. */
	int     new_size;	/* The new total size of all images. */
	int     im_addr;	/* Byte address of the new boot image. */
	unsigned int im_load_adr;	/* The memory load address. */
	unsigned int im_run_adr;/* The memory run address. */
	char   *nptr;		/* Pointer for making name lower case. */

	/* Check argument numbers. */
	if (num != 2 && num != 3) {
		printf("Syntax: %s\n", syntax);
		return FALSE;
	}
	/* Check for a boot partition. */
	if (im_table->ii_boot_partition == -1) {
		printf("There is no boot partition.\n");
		return FALSE;
	}
	/* Any free images? */
	which_image = im_table->ii_boot_used;
	if (which_image == im_table->ii_boot_count) {
		printf("No more boot image slots available.\n");
		return FALSE;
	}
	/* Open the file. */
	im_file = open(args[1], O_RDONLY);
	if (im_file < 0) {
		printf("Could not open %s\n", args[1]);
		return FALSE;
	}
	/* check the exec header. */
	count = read(im_file, (char *) &im_exec, sizeof(struct exec));
	if (count != sizeof(struct exec)) {
		printf("Read problems for file %s\n", args[1]);
		close(im_file);
		return FALSE;
	}
	if (N_GETMAGIC(im_exec) != ZMAGIC || N_GETMID(im_exec) != MID_MACHINE) {
		printf("%s is not a a pc532 executable file.\n", args[1]);
		close(im_file);
		return FALSE;
	}
	if (im_exec.a_entry < 0x2000) {
		printf("%s has a load address less than 0x2000.\n", args[1]);
		close(im_file);
		return FALSE;
	}
	im_load_adr = im_exec.a_entry - sizeof(im_exec); /* & ~(AOUT_LDPGSZ-1); */
	im_run_adr = im_exec.a_entry;

	if (im_load_adr > 0xFFFFFF) {
		im_load_adr = im_load_adr & 0xFFFFFF;
		im_run_adr = im_run_adr & 0xFFFFFF;
		printf("%s has a load address greater than 0xFFFFFF.\n",
		    args[1]);
		printf("using the address:\n\tload address = 0x%x\n"
		    "\trun address = 0x%x\n", im_load_adr, im_run_adr);
	}
	/* Check the sizes.  */
	boot_start = 
	    dk_label->d_partitions[im_table->ii_boot_partition].p_offset
	    * secsize;
	part_size = dk_label->d_partitions[im_table->ii_boot_partition].p_size
	    * secsize;
	total_size = 0;
	for (idx = 0; idx < im_table->ii_boot_used; idx++)
		total_size = total_size + im_table->ii_images[idx].boot_size;

	/* Calculate other things. */
	im_size = im_exec.a_text + im_exec.a_data;

	/* Final check. */
	new_size = total_size + im_size;
	if (new_size > part_size) {
		printf("Image too big to fit in boot partition.\n");
		close(im_file);
	}
	/* Add the image. */
	im_addr = (total_size + secsize - 1) / secsize * secsize;
	im_table->ii_images[which_image].boot_address = im_addr;
	im_table->ii_images[which_image].boot_size = im_size;
	im_table->ii_images[which_image].boot_load_adr = im_load_adr;
	im_table->ii_images[which_image].boot_run_adr = im_run_adr;
	if (num == 3)
		strncpy(im_table->ii_images[which_image].boot_name, args[2],
		    15);
	else
		strncpy(im_table->ii_images[which_image].boot_name, args[1],
		    15);
	if (copy_bytes(im_file, 0, disk_fd, boot_start + im_addr, im_size)) {
		im_table->ii_boot_used++;
		/* Make name lowercase and report on image. */
		for (nptr = im_table->ii_images[which_image].boot_name;
		    *nptr != 0;
		    nptr++)
			if (isupper((unsigned char)*nptr))
				*nptr = tolower((unsigned char)*nptr);
		printf("added image %d (%s).\n", which_image,
		    im_table->ii_images[which_image].boot_name);
		close(im_file);
	} else {
		printf("Problems in installing image.\n");
		close(im_file);
		return FALSE;
	}

	/* Save the changes. */
	save_images();
	return FALSE;
}

/* Delete a boot image. */
int
delete_image(int num, char **args, char *syntax)
{
	int     which_image;	/* Which image is to be operated upon. */
	int     idx;		/* temporary variable for loops. */
	int     boot_start;	/* Zone number of start of boot partition. */
	int     del_size;	/* Size of the deleted image. */

	/* Check arguments. */
	if (num != 2) {
		printf("Syntax: %s\n", syntax);
		return FALSE;
	}
	/* Find the image. */
	which_image = -1;
	for (idx = which_image; idx < im_table->ii_boot_used; idx++)
		if (strcmp(im_table->ii_images[idx].boot_name,
		    args[1]) == 0) {
			which_image = idx;
			break;
		}
	if (which_image == -1)
		if (!Str2Int(args[1], &which_image)) {
			printf("Syntax: %s\n", syntax);
			return FALSE;
		}
	if (which_image < 0 || which_image >= im_table->ii_boot_used) {
		printf("Delete: No such image (%s)\n", args[1]);
		return FALSE;
	}
	/* Report on image we are deleteing. */
	printf("deleting image %d (%s).\n", which_image,
	    im_table->ii_images[which_image].boot_name);

	/* Do the delete. */
	boot_start =
	    dk_label->d_partitions[im_table->ii_boot_partition].p_offset
	    * dk_label->d_secsize;
	del_size = im_table->ii_images[which_image].boot_size;
	for (idx = which_image; idx < im_table->ii_boot_used - 1; idx++) {
		copy_bytes(
		    disk_fd,
		    boot_start + im_table->ii_images[idx + 1].boot_address,
		    disk_fd,
		    boot_start + im_table->ii_images[idx + 1].boot_address
			- del_size,
		    im_table->ii_images[idx + 1].boot_size);
		im_table->ii_images[idx] = im_table->ii_images[idx + 1];
		im_table->ii_images[idx].boot_address -= del_size;
	}
	im_table->ii_boot_used--;
	if (which_image == im_table->ii_boot_default)
		im_table->ii_boot_default = -1;
	else
		if (which_image < im_table->ii_boot_default)
			im_table->ii_boot_default--;

	/* Save the changes. */
	save_images();
	return FALSE;
}

/* Set the default boot image. */
int
set_default_image(int num, char **args, char *syntax)
{
	int     which_image;

	if (num != 2 || !Str2Int(args[1], &which_image)) {
		printf("Syntax: %s\n", syntax);
		return FALSE;
	}
	if (which_image >= im_table->ii_boot_used) {
		printf("No such image.\n");
		return FALSE;
	}
	im_table->ii_boot_default = which_image;
	images_changed = TRUE;
	return FALSE;
}

/* Initialize the disk or just the image portion. */
int
initialize(int num, char **args, char *syntax)
{

	/* Check the args */
	if (num > 1) {
		printf("Syntax: %s\n", syntax);
		return FALSE;
	}
	init_images(FALSE);
	return FALSE;
}

/* Write the disk header and exit. */
int 
write_exit(int num, char **args, char *syntax)
{

	if (images_changed)
		save_images();

	return TRUE;
}

/* The main program! */
/*********************/

int
main(int argc, char *argv[])
{
	int     count;		/* Used by reads. */
	const char   *fname;
	int     cmdscnt;	/* Number of argument line commands. */
	char   *argcmds[MAXARGCMDS];
	char    optchar;
	int     idx;
	off_t	loff;

	/* Check the parameters.  */
	yesmode = 0;
	cmdscnt = 0;
	opterr = TRUE;
	fname = DEFAULT_DEVICE;
	while ((optchar = getopt(argc, argv, "c:")) != -1)
		switch (optchar) {
		case 'y':
			yesmode = 1;
			break;
		case 'c':
			if (cmdscnt == MAXARGCMDS)
				usage();
			argcmds[cmdscnt++] = optarg;
			break;
		case '?':
			usage();
		}

	if (argc - optind > 1)
		usage();
	if (optind < argc)
		fname = argv[optind];

	loff = getlabeloffset();
	dk_label = (struct disklabel *) &disk_info[loff];
	im_table = (struct imageinfo *) (&disk_info[loff] +
	    sizeof(struct disklabel));

	disk_fd = open(fname, O_RDWR);
	if (disk_fd < 0)
		err(1, "Could not open %s", fname);

	/* Read the disk information block. */
	count = read(disk_fd, disk_info, BLOCK_SIZE);
	if (count != BLOCK_SIZE)
		err(1, "Could not read info block on %s", fname);

	/* Check for correct information and set up pointers. */
	if (dk_label->d_magic != DISKMAGIC)
		errx(1, "Could not find a disk label on %s", fname);
	if (im_table->ii_magic != IMAGE_MAGIC)
		init_images(TRUE);
	if (dkcksum(dk_label) != 0)
		printf("Warning: bad checksum in disk label.\n");

	/* initialize secsize. */
	secsize = dk_label->d_secsize;

	/* do the commands.... */
	if (cmdscnt > 0) {
		/* Process the argv commands. */
		for (idx = 0; idx < cmdscnt; idx++)
			one_command(argcmds[idx]);
	} else {
		/* Interactive command loop.  */
		display_part(0, NULL, NULL);
		display_image(0, NULL, NULL);
		command_loop();
	}
	exit(0);
}
