/*
 * Copyright 1985, 1986, 1987, 1988 by the Massachusetts Institute
 * of Technology 
 *
 * For copying and distribution information, please see the file
 * <mit-copyright.h>.
 *
 * Description.
 */

#include "adm_locl.h"

__RCSID("$KTH-KRB: kstash.c,v 1.11 2001/02/20 23:07:49 assar Exp $"
      "$NetBSD: kstash.c,v 1.3 2002/09/12 12:33:10 joda Exp $");

/* change this later, but krblib_dbm needs it for now */

static des_cblock master_key;
static des_key_schedule master_key_schedule;

static void 
clear_secrets(void)
{
    memset(master_key_schedule, 0, sizeof(master_key_schedule));
    memset(master_key, 0, sizeof(master_key));
}

int
main(int argc, char **argv)
{
    long    n;
    int ret = 0;
    setprogname (argv[0]);

    if ((n = kerb_init()))
        errx(1, "Kerberos db and cache init failed = %ld", n);

    if (kdb_get_master_key (KDB_GET_PROMPT, &master_key,
			    master_key_schedule) != 0) {
	clear_secrets();
	errx(1, "Couldn't read master key.");
    }

    if (kdb_verify_master_key (&master_key, master_key_schedule, stderr) < 0) {
	clear_secrets();
	return 1;
    }

    ret = kdb_kstash(&master_key, MKEYFILE);
    if(ret < 0)
        warn("writing master key");
    else
	fprintf(stderr, "Wrote master key to %s\n", MKEYFILE);
    
    clear_secrets();
    return ret;
}
