/*	$NetBSD: main.c,v 1.27.2.1 2005/11/06 13:43:18 tron Exp $	*/

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <nbcompat.h>
#if HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif
#ifndef lint
#if 0
static const char *rcsid = "from FreeBSD Id: main.c,v 1.17 1997/10/08 07:46:23 charnier Exp";
#else
__RCSID("$NetBSD: main.c,v 1.27.2.1 2005/11/06 13:43:18 tron Exp $");
#endif
#endif

/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
 *
 * Jordan K. Hubbard
 * 18 July 1993
 *
 * This is the create module.
 *
 */

#if HAVE_ERR_H
#include <err.h>
#endif
#include "lib.h"
#include "create.h"

static const char Options[] = "B:C:D:EFI:K:L:OP:RS:UVX:b:c:d:f:hi:k:lm:n:p:r:s:t:v";

char   *Prefix = NULL;
char   *Comment = NULL;
char   *Desc = NULL;
char   *Display = NULL;
char   *Install = NULL;
char   *DeInstall = NULL;
char   *Contents = NULL;
char   *Require = NULL;
char   *ExcludeFrom = NULL;
char   *Mtree = NULL;
char   *Pkgdeps = NULL;
char   *Pkgcfl = NULL;
char   *BuildVersion = NULL;
char   *BuildInfo = NULL;
char   *SizePkg = NULL;
char   *SizeAll = NULL;
char   *Preserve = NULL;
char   *SrcDir = NULL;
char   *realprefix = NULL;
char    PlayPen[MaxPathSize];
size_t  PlayPenSize = sizeof(PlayPen);
int	update_pkgdb = 1;
int	create_views = 0;
int     Dereference = 0;
int     PlistOnly = 0;
int     RelativeLinks = 0;
int     ReorderDirs = 0;
Boolean File2Pkg = FALSE;

static void
usage(void)
{
	fprintf(stderr, "%s\n%s\n%s\n%s\n%s\n%s\n",
	    "usage: pkg_create [-ORUEhlVv] [-P dpkgs] [-C cpkgs] [-p prefix] [-f contents]",
	    "                  [-i iscript] [-k dscript] [-r rscript] [-t template]",
	    "                  [-X excludefile] [-D displayfile] [-m mtreefile]",
	    "                  [-b build-version-file] [-B build-info-file]",
	    "                  [-I realprefix]",
	    "                  -c comment -d description -f packlist pkg-name");
	exit(1);
}

int
main(int argc, char **argv)
{
	int     ch;
	lpkg_head_t pkgs;
	lpkg_t *lpp;

	setprogname(argv[0]);
	while ((ch = getopt(argc, argv, Options)) != -1)
		switch (ch) {
		case 'v':
			Verbose = TRUE;
			break;

		case 'E':
			create_views = 1;
			break;

		case 'I':
			realprefix = optarg;
			break;

		case 'O':
			PlistOnly = 1;
			break;

		case 'R':
			ReorderDirs = 1;
			break;

		case 'U':
			update_pkgdb = 0;
			break;

		case 'p':
			Prefix = optarg;
			break;

		case 's':
			SizePkg = optarg;
			break;

		case 'S':
			SizeAll = optarg;
			break;

		case 'f':
			Contents = optarg;
			break;

		case 'c':
			Comment = optarg;
			break;

		case 'd':
			Desc = optarg;
			break;

		case 'i':
			Install = optarg;
			break;

		case 'K':
			_pkgdb_setPKGDB_DIR(optarg);
			break;

		case 'k':
			DeInstall = optarg;
			break;

		case 'l':
			RelativeLinks = 1;
			break;

		case 'L':
			SrcDir = optarg;
			break;

		case 'r':
			Require = optarg;
			break;

		case 't':
			strlcpy(PlayPen, optarg, sizeof(PlayPen));
			break;

		case 'X':
			ExcludeFrom = optarg;
			break;

		case 'h':
			Dereference = 1;
			break;

		case 'D':
			Display = optarg;
			break;

		case 'm':
			Mtree = optarg;
			break;

		case 'n':
			Preserve = optarg;
			break;

		case 'P':
			Pkgdeps = optarg;
			break;

		case 'C':
			Pkgcfl = optarg;
			break;

		case 'b':
			BuildVersion = optarg;
			break;

		case 'B':
			BuildInfo = optarg;
			break;

		case 'V':
			show_version();
			/* NOTREACHED */

		case '?':
		default:
			usage();
			break;
		}

	argc -= optind;
	argv += optind;

	TAILQ_INIT(&pkgs);

	/* Get all the remaining package names, if any */
	while (*argv) {
		lpp = alloc_lpkg(*argv);
		TAILQ_INSERT_TAIL(&pkgs, lpp, lp_link);
		argv++;
	}

	/* If no packages, yelp */
	lpp = TAILQ_FIRST(&pkgs);
	if (lpp == NULL)
		warnx("missing package name"), usage();
	lpp = TAILQ_NEXT(lpp, lp_link);
	if (lpp != NULL)
		warnx("only one package name allowed ('%s' extraneous)",
		    lpp->lp_name),
		    usage();
	if (!pkg_perform(&pkgs)) {
		if (Verbose)
			warnx("package creation failed");
		return 1;
	} else
		return 0;
}
