/* $NetBSD: v.c,v 1.2 2003/10/11 09:55:58 jdolecek Exp $ */

/* vinum.c: vinum interface program */
/*-
 * Copyright (c) 1997, 1998
 *	Nan Yang Computer Services Limited.  All rights reserved.
 *
 *  Written by Greg Lehey
 *
 *  This software is distributed under the so-called ``Berkeley
 *  License'':
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
 *	This product includes software developed by Nan Yang Computer
 *      Services Limited.
 * 4. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * This software is provided ``as is'', and any express or implied
 * warranties, including, but not limited to, the implied warranties of
 * merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the company or contributors be liable for any
 * direct, indirect, incidental, special, exemplary, or consequential
 * damages (including, but not limited to, procurement of substitute
 * goods or services; loss of use, data, or profits; or business
 * interruption) however caused and on any theory of liability, whether
 * in contract, strict liability, or tort (including negligence or
 * otherwise) arising in any way out of the use of this software, even if
 * advised of the possibility of such damage.
 *
 * $Id: v.c,v 1.2 2003/10/11 09:55:58 jdolecek Exp $
 * $FreeBSD$
 */

#include "vext.h"

__RCSID("$NetBSD: v.c,v 1.2 2003/10/11 09:55:58 jdolecek Exp $");

FILE *cf;						    /* config file handle */
FILE *History;						    /* history file */
char *historyfile;					    /* and its name */

char *dateformat;					    /* format in which to store date */

char buffer[BUFSIZE];					    /* buffer to read in to */

int line = 0;						    /* stdin line number for error messages */
int file_line = 0;					    /* and line in input file (yes, this is tacky) */
int inerror;						    /* set to 1 to exit after end of config file */

/* flags */

int debug = 0;						    /* debug flag, usage varies */
int force = 0;						    /* set to 1 to force some dangerous ops */
int interval = 0;					    /* interval in ms between init/revive */
int vflag = 0;						    /* set verbose operation or verify */
int Verbose = 0;					    /* set very verbose operation */
int recurse = 0;					    /* set recursion */
int sflag = 0;						    /* show statistics */
int SSize = 0;						    /* sector size for revive */
int dowait = 0;						    /* wait for completion */
char *objectname;					    /* name to be passed for -n flag */

/*
 * Structures to read kernel data into.  These are shortened versions
 * of the kernel data structures, without the bits and pieces we
 * shouldn't be using.
 */
struct __vinum_conf vinum_conf;				    /* configuration information */
struct _volume vol;
struct _plex plex;
struct _sd sd;
struct _drive drive;

jmp_buf command_fail;					    /* return on a failed command */
int superdev;						    /* vinum super device */
gid_t gid_operator;					    /* group operator for chown */
#define GROUP_OPERATOR "operator"
#define UID_ROOT 0					    /* no need to lookup... */

void start_daemon(void);

#define ofs(x) ((void *) (& ((struct confdata *) 0)->x))    /* offset of x in struct confdata */

char *token[MAXARGS];					    /* pointers to individual tokens */
int tokens;						    /* number of tokens */

int
main(int argc, char *argv[], char *envp[])
{
    struct stat histstat;
    struct group *g;

    dateformat = getenv("VINUM_DATEFORMAT");
    if (dateformat == NULL)
	dateformat = "%e %b %Y %H:%M:%S";
    historyfile = getenv("VINUM_HISTORY");
    if (historyfile == NULL)
	historyfile = DEFAULT_HISTORYFILE;
    if (stat(historyfile, &histstat) == 0) {		    /* history file exists */
	if ((histstat.st_mode & S_IFMT) != S_IFREG) {
	    fprintf(stderr,
		"Vinum history file %s must be a regular file\n",
		historyfile);
	    exit(1);
	}
    } else if ((errno != ENOENT)			    /* not "not there",  */
    &&(errno != EROFS)) {				    /* and not read-only file system */
	fprintf(stderr,
	    "Can't open %s: %s (%d)\n",
	    historyfile,
	    strerror(errno),
	    errno);
	exit(1);
    }
    History = fopen(historyfile, "a+");
    if (History != NULL) {
	timestamp();
	fprintf(History, "*** " VINUMMOD " started ***\n");
	fflush(History);				    /* before we start the daemon */
    }
    superdev = open(VINUM_SUPERDEV_NAME, O_RDWR);	    /* open vinum superdevice */
    if (superdev < 0) {					    /* no go */
	if (errno == ENOENT)				    /* we don't have our node, */
	    make_devices();				    /* create them first */
	if (superdev < 0) {
	    perror("Can't open " VINUM_SUPERDEV_NAME);
	    return 1;
	}
    }
    /*
     * Check that we match the kernel version.  There are a number of
     * possibilities here:
     *
     * 0: The versions are OK.
     * 1: The kernel module could be a pre-version 1 module, which
     *    doesn't include this check.  In that case, vinum_conf will be too
     *    short, and so we'll get an EINVAL back when trying to get it.  In
     *    this case we'll fake a 0 in the version.
     * 2: The module versions are different.  Print appropriate messages
     *    and die.
     */
    if (ioctl(superdev, VINUM_GETCONFIG, &vinum_conf) < 0) {
	if (errno == EINVAL)				    /* wrong length, */
	    vinum_conf.version = 0;			    /* must be the old version */
	else {
	    perror("Can't get vinum config");
	    return 1;
	}
    }
    if (vinum_conf.version != VINUMVERSION) {
	fprintf(stderr,
	    "Version mismatch.  The kernel module is version %d of Vinum,\n"
	    "but this program is designed for version %d\n",
	    vinum_conf.version,
	    VINUMVERSION);
	if (vinum_conf.version < VINUMVERSION)
	    fprintf(stderr, "Please upgrade your kernel module.\n");
	else
	    fprintf(stderr, "Please upgrade vinum(8).\n");
	return 1;
    }
    /* Check if the d?mon is running.  If not, start it in the
     * background */
    start_daemon();
    if ((g = getgrnam(GROUP_OPERATOR)) != NULL)
	gid_operator = g->gr_gid;
    endgrent();

    if (argc > 1) {					    /* we have a command on the line */
	if (setjmp(command_fail) != 0)			    /* long jumped out */
	    return -1;
	parseline(argc - 1, &argv[1]);			    /* do it */
    } else {
	/*
	 * Catch a possible race condition which could cause us to
	 * longjmp() into nowhere if we receive a SIGINT in the next few
	 * lines.
	 */
	if (setjmp(command_fail))			    /* come back here on catastrophic failure */
	    return 1;
	setsigs();					    /* set signal handler */
	for (;;) {					    /* ugh */
	    char *c;
	    int childstatus;				    /* from wait4 */

	    if (setjmp(command_fail) == 2)		    /* come back here on catastrophic failure */
		fprintf(stderr, "*** interrupted ***\n");   /* interrupted */

	    while (wait4(-1, &childstatus, WNOHANG, NULL) > 0);	/* wait for all dead children */
	    c = readline(VINUMMOD " -> ");		    /* get an input */
	    if (c == NULL) {				    /* EOF or error */
		if (ferror(stdin)) {
		    fprintf(stderr, "Can't read input: %s (%d)\n", strerror(errno), errno);
		    return 1;
		} else {				    /* EOF */
		    printf("\n");
		    return 0;
		}
	    } else if (*c) {				    /* got something there */
		add_history(c);				    /* save it in the history */
		strcpy(buffer, c);			    /* put it where we can munge it */
		free(c);
		line++;					    /* count the lines */
		tokens = tokenize(buffer, token, MAXARGS);
		/* got something potentially worth parsing */
		if (tokens)
		    parseline(tokens, token);		    /* and do what he says */
	    }
	    if (History)
		fflush(History);
	}
    }
    return 0;						    /* normal completion */
}

/* stop the hard way */
void
vinum_quit(int argc, char *argv[], char *argv0[])
{
    exit(0);
}

/* Set action on receiving a SIGINT */
void
setsigs()
{
    struct sigaction act;

    act.sa_handler = catchsig;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
}

void
catchsig(int ignore)
{
    longjmp(command_fail, 2);
}

#define FUNKEY(x) { kw_##x, &vinum_##x }		    /* create pair "kw_foo", vinum_foo */
#define vinum_move vinum_mv				    /* synonym for 'mv' */

struct funkey {
    enum keyword kw;
    void (*fun) (int argc, char *argv[], char *arg0[]);
} funkeys[] = {

    FUNKEY(create),
	FUNKEY(read),
	FUNKEY(debug),
	FUNKEY(modify),
	FUNKEY(list),
	FUNKEY(ld),
	FUNKEY(ls),
	FUNKEY(lp),
	FUNKEY(lv),
	FUNKEY(info),
	FUNKEY(set),
	FUNKEY(init),
	FUNKEY(label),
	FUNKEY(resetconfig),
	FUNKEY(rm),
	FUNKEY(mv),
	FUNKEY(move),
	FUNKEY(attach),
	FUNKEY(detach),
	FUNKEY(rename),
	FUNKEY(replace),
	FUNKEY(printconfig),
	FUNKEY(saveconfig),
	FUNKEY(start),
	FUNKEY(stop),
	FUNKEY(makedev),
	FUNKEY(help),
	FUNKEY(quit),
	FUNKEY(concat),
	FUNKEY(stripe),
	FUNKEY(raid4),
	FUNKEY(raid5),
	FUNKEY(mirror),
	FUNKEY(setdaemon),
	FUNKEY(readpol),
	FUNKEY(resetstats),
	FUNKEY(setstate),
	FUNKEY(checkparity),
	FUNKEY(rebuildparity),
	FUNKEY(dumpconfig)
};

/* Take args arguments at argv and attempt to perform the operation specified */
void
parseline(int args, char *argv[])
{
    int i;
    int j;
    enum keyword command;				    /* command to execute */

    if (History != NULL) {				    /* save the command to history file */
	timestamp();
	for (i = 0; i < args; i++)			    /* all args */
	    fprintf(History, "%s ", argv[i]);
	fputs("\n", History);
    }
    if ((args == 0)					    /* empty line */
    ||(*argv[0] == '#'))				    /* or a comment, */
	return;
    if (args == MAXARGS) {				    /* too many arguments, */
	fprintf(stderr, "Too many arguments to %s, this can't be right\n", argv[0]);
	return;
    }
    command = get_keyword(argv[0], &keyword_set);
    dowait = 0;						    /* initialize flags */
    force = 0;						    /* initialize flags */
    vflag = 0;						    /* initialize flags */
    Verbose = 0;					    /* initialize flags */
    recurse = 0;					    /* initialize flags */
    sflag = 0;						    /* initialize flags */
    objectname = NULL;					    /* no name yet */

    /*
     * first handle generic options
     * We don't use getopt(3) because
     * getopt doesn't allow merging flags
     * (for example, -fr).
     */
    for (i = 1; (i < args) && (argv[i][0] == '-'); i++) {   /* while we have flags */
	for (j = 1; j < strlen(argv[i]); j++)
	    switch (argv[i][j]) {
	    case 'd':					    /* -d: debug */
		debug = 1;
		break;

	    case 'f':					    /* -f: force */
		force = 1;
		break;

	    case 'i':					    /* interval */
		interval = 0;
		if (argv[i][j + 1] != '\0')		    /* operand follows, */
		    interval = atoi(&argv[i][j + 1]);	    /* use it */
		else if (args > (i + 1))		    /* another following, */
		    interval = atoi(argv[++i]);		    /* use it */
		if (interval == 0)			    /* nothing valid, */
		    fprintf(stderr, "-i: no interval specified\n");
		break;

	    case 'n':					    /* -n: get name */
		if (i == args - 1) {			    /* last arg */
		    fprintf(stderr, "-n requires a name parameter\n");
		    return;
		}
		objectname = argv[++i];			    /* pick it up */
		j = strlen(argv[i]);			    /* skip the next parm */
		break;

	    case 'r':					    /* -r: recurse */
		recurse = 1;
		break;

	    case 's':					    /* -s: show statistics */
		sflag = 1;
		break;

	    case 'S':
		SSize = 0;
		if (argv[i][j + 1] != '\0')		    /* operand follows, */
		    SSize = atoi(&argv[i][j + 1]);	    /* use it */
		else if (args > (i + 1))		    /* another following, */
		    SSize = atoi(argv[++i]);		    /* use it */
		if (SSize == 0)				    /* nothing valid, */
		    fprintf(stderr, "-S: no size specified\n");
		break;

	    case 'v':					    /* -v: verbose */
		vflag++;
		break;

	    case 'V':					    /* -V: Very verbose */
		vflag++;
		Verbose++;
		break;

	    case 'w':					    /* -w: wait for completion */
		dowait = 1;
		break;

	    default:
		fprintf(stderr, "Invalid flag: %s\n", argv[i]);
	    }
    }

    /* Pass what we have left to the command to handle it */
    for (j = 0; j < (sizeof(funkeys) / sizeof(struct funkey)); j++) {
	if (funkeys[j].kw == command) {			    /* found the command */
	    funkeys[j].fun(args - i, &argv[i], &argv[0]);
	    return;
	}
    }
    fprintf(stderr, "Unknown command: %s\n", argv[0]);
}

void
get_drive_info(struct _drive *drive, int index)
{
    *(int *) drive = index;				    /* put in drive to hand to driver */
    if (ioctl(superdev, VINUM_DRIVECONFIG, drive) < 0) {
	fprintf(stderr,
	    "Can't get config for drive %d: %s\n",
	    index,
	    strerror(errno));
	longjmp(command_fail, -1);
    }
}

void
get_sd_info(struct _sd *sd, int index)
{
    *(int *) sd = index;				    /* put in sd to hand to driver */
    if (ioctl(superdev, VINUM_SDCONFIG, sd) < 0) {
	fprintf(stderr,
	    "Can't get config for subdisk %d: %s\n",
	    index,
	    strerror(errno));
	longjmp(command_fail, -1);
    }
}

/* Get the contents of the sd entry for subdisk <sdno>
 * of the specified plex. */
void
get_plex_sd_info(struct _sd *sd, int plexno, int sdno)
{
    ((int *) sd)[0] = plexno;
    ((int *) sd)[1] = sdno;				    /* pass parameters */
    if (ioctl(superdev, VINUM_PLEXSDCONFIG, sd) < 0) {
	fprintf(stderr,
	    "Can't get config for subdisk %d (part of plex %d): %s\n",
	    sdno,
	    plexno,
	    strerror(errno));
	longjmp(command_fail, -1);
    }
}

void
get_plex_info(struct _plex *plex, int index)
{
    *(int *) plex = index;				    /* put in plex to hand to driver */
    if (ioctl(superdev, VINUM_PLEXCONFIG, plex) < 0) {
	fprintf(stderr,
	    "Can't get config for plex %d: %s\n",
	    index,
	    strerror(errno));
	longjmp(command_fail, -1);
    }
}

void
get_volume_info(struct _volume *volume, int index)
{
    *(int *) volume = index;				    /* put in volume to hand to driver */
    if (ioctl(superdev, VINUM_VOLCONFIG, volume) < 0) {
	fprintf(stderr,
	    "Can't get config for volume %d: %s\n",
	    index,
	    strerror(errno));
	longjmp(command_fail, -1);
    }
}

struct _drive *
find_drive_by_devname(char *name)
{
    int driveno;

    if (ioctl(superdev, VINUM_GETCONFIG, &vinum_conf) < 0) {
	perror("Can't get vinum config");
	return NULL;
    }
    for (driveno = 0; driveno < vinum_conf.drives_allocated; driveno++) {
	get_drive_info(&drive, driveno);
	if ((drive.state != drive_unallocated)		    /* real drive */
	&&(!strcmp(drive.devicename, name)))		    /* and the name's right, */
	    return &drive;				    /* found it */
    }
    return NULL;					    /* no drive of that name */
}

/* Create the device nodes for vinum objects */
void
make_devices(void)
{
    int volno;
    int plexno;
    int sdno;
    int driveno;

    if (access(_PATH_DEV, W_OK) < 0) {			    /* can't access /dev to write? */
	if (errno == EROFS)				    /* because it's read-only, */
	    fprintf(stderr, VINUMMOD ": " _PATH_DEV
		" is mounted read-only, not rebuilding " VINUM_DIR "\n");
	else
	    perror(VINUMMOD ": Can't write to " _PATH_DEV);
	return;
    }
    if (History) {
	timestamp();
	fprintf(History, "*** Created devices ***\n");
    }
    if (superdev >= 0)					    /* super device open */
	close(superdev);

    system("rm -rf " VINUM_DIR);			    /* remove the old directories */
    system("rm -rf " VINUM_DIR);			    /* remove the old directories */
    system("mkdir -p " VINUM_DIR "/drive "		    /* and make them again */
	VINUM_DIR "/rplex "
	VINUM_DIR "/rsd "
	VINUM_DIR "/rvol "
	VINUM_DIR "/plex "
	VINUM_DIR "/sd "
	VINUM_DIR "/vol");

    if (mknod(VINUM_SUPERDEV_NAME,
	    S_IRUSR | S_IWUSR | S_IFCHR,		    /* user only */
	    makedev(VINUM_CDEV_MAJOR, VINUM_SUPERDEV_VOL)) < 0)
	fprintf(stderr, "Can't create %s: %s\n", VINUM_SUPERDEV_NAME, strerror(errno));

    superdev = open(VINUM_SUPERDEV_NAME, O_RDWR);	    /* open the super device */
    if (superdev < 0) {
	perror(VINUM_SUPERDEV_NAME);
	return;
    }
    if (mknod(VINUM_DAEMON_DEV_NAME,			    /* daemon super device */
	    S_IRUSR | S_IWUSR | S_IFCHR,		    /* user only */
	    makedev(VINUM_CDEV_MAJOR, VINUM_DAEMON_VOL)) < 0)
	fprintf(stderr, "Can't create %s: %s\n", VINUM_DAEMON_DEV_NAME, strerror(errno));

    if (ioctl(superdev, VINUM_GETCONFIG, &vinum_conf) < 0) {
	perror("Can't get vinum config");
	return;
    }
    for (volno = 0; volno < vinum_conf.volumes_allocated; volno++)
	make_vol_dev(volno, 0);

    for (plexno = 0; plexno < vinum_conf.plexes_allocated; plexno++)
	make_plex_dev(plexno, 0);

    for (sdno = 0; sdno < vinum_conf.subdisks_allocated; sdno++)
	make_sd_dev(sdno);

    /* Drives.  Do this later (both logical and physical names) XXX */
    for (driveno = 0; driveno < vinum_conf.drives_allocated; driveno++) {
	char filename[PATH_MAX];			    /* for forming file names */

	get_drive_info(&drive, driveno);
	if (drive.state > drive_referenced) {
	    sprintf(filename, "ln -s %s " VINUM_DIR "/drive/%s", drive.devicename, drive.label.name);
	    system(filename);
	}
    }
}

/* make the devices for a volume */
void
make_vol_dev(int volno, int recurse)
{
    dev_t voldev;
    dev_t rvoldev;
    char filename[PATH_MAX];				    /* for forming file names */
    int plexno;

    get_volume_info(&vol, volno);
    if (vol.state != volume_unallocated) {		    /* we could have holes in our lists */
	voldev = VINUMBDEV(volno, VINUM_VOLUME_TYPE);	    /* create a block device number */
	rvoldev = VINUMDEV(volno, VINUM_VOLUME_TYPE);	    /* and a character device */

	/* Create /dev/vinum/<myvol> */
	sprintf(filename, VINUM_DIR "/%s", vol.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFBLK, voldev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	/* Create /dev/vinum/r<myvol> XXX until we fix fsck and friends */
	sprintf(filename, VINUM_DIR "/r%s", vol.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFCHR, rvoldev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	/* Create /dev/vinum/vol/<myvol> */
	sprintf(filename, VINUM_DIR "/vol/%s", vol.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFBLK, voldev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	/* Create /dev/vinum/rvol/<myvol> */
	sprintf(filename, VINUM_DIR "/rvol/%s", vol.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFCHR, rvoldev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	if (vol.plexes > 0) {
	    /* Create /dev/vinum/vol/<myvol>.plex/ */
	    sprintf(filename, VINUM_DIR "/vol/%s.plex", vol.name);
	    if (mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0)
		fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));
	}
	if (recurse)
	    for (plexno = 0; plexno < vol.plexes; plexno++)
		make_plex_dev(plex.plexno, recurse);
    }
}

/*
 * Create device entries for the plexes in
 * /dev/vinum/<vol>.plex/ and /dev/vinum/plex.
 */
void
make_plex_dev(int plexno, int recurse)
{
    dev_t plexdev;					    /* block device */
    dev_t plexcdev;					    /* and character device device */
    char filename[PATH_MAX];				    /* for forming file names */
    int sdno;

    get_plex_info(&plex, plexno);
    if (plex.state != plex_unallocated) {
	plexdev = VINUM_BLOCK_PLEX(plexno);
	plexcdev = VINUM_PLEX(plexno);

	/* /dev/vinum/plex/<plex> */
	sprintf(filename, VINUM_DIR "/plex/%s", plex.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFBLK, plexdev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	/* /dev/vinum/rplex/<plex> */
	sprintf(filename, VINUM_DIR "/rplex/%s", plex.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFCHR, plexcdev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	if (plex.volno >= 0) {
	    get_volume_info(&vol, plex.volno);
	    plexdev = VINUMBDEV(plex.volno, VINUM_PLEX_TYPE);

	    /* Create device /dev/vinum/vol/<vol>.plex/<plex> */
	    sprintf(filename, VINUM_DIR "/vol/%s.plex/%s", vol.name, plex.name);
	    if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFBLK, plexdev) < 0)
		fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	    /* Create directory /dev/vinum/vol/<vol>.plex/<plex>.sd */
	    sprintf(filename, VINUM_DIR "/vol/%s.plex/%s.sd", vol.name, plex.name);
	    if (mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0)
		fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));
	}
	if (recurse) {
	    for (sdno = 0; sdno < plex.subdisks; sdno++) {
		get_plex_sd_info(&sd, plex.plexno, sdno);
		make_sd_dev(sd.sdno);
	    }
	}
    }
}

/* Create the contents of /dev/vinum/sd and /dev/vinum/rsd */
void
make_sd_dev(int sdno)
{
    dev_t sddev;					    /* block device */
    dev_t sdcdev;					    /* and character device */
    char filename[PATH_MAX];				    /* for forming file names */

    get_sd_info(&sd, sdno);
    if (sd.state != sd_unallocated) {
	sddev = VINUM_BLOCK_SD(sdno);
	sdcdev = VINUM_SD(sdno);

	/* /dev/vinum/sd/<sd> */
	sprintf(filename, VINUM_DIR "/sd/%s", sd.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFBLK, sddev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));

	/* And /dev/vinum/rsd/<sd> */
	sprintf(filename, VINUM_DIR "/rsd/%s", sd.name);
	if (mknod(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IFCHR, sdcdev) < 0)
	    fprintf(stderr, "Can't create %s: %s\n", filename, strerror(errno));
    }
}

/* command line interface for the 'makedev' command */
void
vinum_makedev(int argc, char *argv[], char *arg0[])
{
    make_devices();
}

/*
 * Find the object "name".  Return object type at type,
 * and the index as the return value.
 * If not found, return -1 and invalid_object.
 */
int
find_object(const char *name, enum objecttype *type)
{
    int object;

    if (ioctl(superdev, VINUM_GETCONFIG, &vinum_conf) < 0) {
	perror("Can't get vinum config");
	*type = invalid_object;
	return -1;
    }
    /* Search the drive table */
    for (object = 0; object < vinum_conf.drives_allocated; object++) {
	get_drive_info(&drive, object);
	if (strcmp(name, drive.label.name) == 0) {
	    *type = drive_object;
	    return object;
	}
    }

    /* Search the subdisk table */
    for (object = 0; object < vinum_conf.subdisks_allocated; object++) {
	get_sd_info(&sd, object);
	if (strcmp(name, sd.name) == 0) {
	    *type = sd_object;
	    return object;
	}
    }

    /* Search the plex table */
    for (object = 0; object < vinum_conf.plexes_allocated; object++) {
	get_plex_info(&plex, object);
	if (strcmp(name, plex.name) == 0) {
	    *type = plex_object;
	    return object;
	}
    }

    /* Search the volume table */
    for (object = 0; object < vinum_conf.volumes_allocated; object++) {
	get_volume_info(&vol, object);
	if (strcmp(name, vol.name) == 0) {
	    *type = volume_object;
	    return object;
	}
    }

    /* Didn't find the name: invalid */
    *type = invalid_object;
    return -1;
}

/* Continue reviving a subdisk in the background */
void
continue_revive(int sdno)
{
    struct _sd sd;
    pid_t pid;
    get_sd_info(&sd, sdno);

    if (dowait == 0)
	pid = fork();					    /* do this in the background */
    else
	pid = 0;
    if (pid == 0) {					    /* we're the child */
	struct _ioctl_reply reply;
	struct vinum_ioctl_msg *message = (struct vinum_ioctl_msg *) &reply;

	openlog(VINUMMOD, LOG_CONS | LOG_PERROR | LOG_PID, LOG_KERN);
	syslog(LOG_INFO | LOG_KERN, "reviving %s", sd.name);
	setproctitle("reviving %s", sd.name);

	for (reply.error = EAGAIN; reply.error == EAGAIN;) { /* revive the subdisk */
	    if (interval)
		usleep(interval * 1000);		    /* pause between each copy */
	    message->index = sdno;			    /* pass sd number */
	    message->type = sd_object;			    /* and type of object */
	    message->state = object_up;
	    if (SSize != 0) {				    /* specified a size for init */
		if (SSize < 512)
		    SSize <<= DEV_BSHIFT;
		message->blocksize = SSize;
	    } else
		message->blocksize = DEFAULT_REVIVE_BLOCKSIZE;
	    ioctl(superdev, VINUM_SETSTATE, message);
	}
	if (reply.error) {
	    syslog(LOG_ERR | LOG_KERN,
		"can't revive %s: %s",
		sd.name,
		reply.msg[0] ? reply.msg : strerror(reply.error));
	    if (dowait == 0)
		exit(1);
	} else {
	    get_sd_info(&sd, sdno);			    /* update the info */
	    syslog(LOG_INFO | LOG_KERN, "%s is %s", sd.name, sd_state(sd.state));
	    if (dowait == 0)
		exit(0);
	}
    } else if (pid < 0)					    /* couldn't fork? */
	fprintf(stderr, "Can't continue reviving %s: %s\n", sd.name, strerror(errno));
    else						    /* parent */
	printf("Reviving %s in the background\n", sd.name);
}

/*
 * Check if the daemon is running,
 * start it if it isn't.  The check itself
 * could take a while, so we do it as a separate
 * process, which will become the daemon if one isn't
 * running already
 */
void
start_daemon(void)
{
    int pid;
    int status;
    int error;

    pid = (int) fork();

    if (pid == 0) {					    /* We're the child, do the work */
	/*
	 * We have a problem when stopping the subsystem:
	 * The only way to know that we're idle is when
	 * all open superdevs close.  But we want the
	 * daemon to clean up for us, and since we can't
	 * count the opens, we need to have the main device
	 * closed when we stop.  We solve this conundrum
	 * by getting the daemon to open a separate device.
	 */
	close(superdev);				    /* this is the wrong device */
	superdev = open(VINUM_DAEMON_DEV_NAME, O_RDWR);	    /* open deamon superdevice */
	if (superdev < 0) {
	    perror("Can't open " VINUM_DAEMON_DEV_NAME);
	    exit(1);
	}
	error = daemon(0, 0);				    /* this will fork again, but who's counting? */
	if (error != 0) {
	    fprintf(stderr, "Can't start daemon: %s (%d)\n", strerror(errno), errno);
	    exit(1);
	}
	setproctitle(VINUMMOD " daemon");		    /* show what we're doing */
	status = ioctl(superdev, VINUM_FINDDAEMON, NULL);
	if (status != 0) {				    /* no daemon, */
	    ioctl(superdev, VINUM_DAEMON, &vflag);	    /* we should hang here */
	    syslog(LOG_ERR | LOG_KERN, "%s", strerror(errno));
	    exit(1);
	}
	exit(0);					    /* when told to die */
    } else if (pid < 0)					    /* couldn't fork */
	printf("Can't fork to check daemon\n");
}

void
timestamp()
{
    struct timeval now;
    struct tm *date;
    char datetext[MAXDATETEXT];
    time_t sec;

    if (History != NULL) {
	if (gettimeofday(&now, NULL) != 0) {
	    fprintf(stderr, "Can't get time: %s\n", strerror(errno));
	    return;
	}
	sec = now.tv_sec;
	date = localtime(&sec);
	strftime(datetext, MAXDATETEXT, dateformat, date),
	    fprintf(History,
	    "%s.%06ld ",
	    datetext,
	    now.tv_usec);
    }
}
