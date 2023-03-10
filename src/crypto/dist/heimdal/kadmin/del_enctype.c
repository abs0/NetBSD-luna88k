/*
 * Copyright (c) 1999-2001 Kungliga Tekniska H?gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#include "kadmin_locl.h"

__RCSID("$Heimdal: del_enctype.c,v 1.7 2001/04/19 07:26:52 joda Exp $"
        "$NetBSD: del_enctype.c,v 1.3 2002/09/12 13:18:59 joda Exp $");

/*
 * del_enctype principal enctypes...
 */

static struct getargs args[] = {
    { "help", 'h', arg_flag, NULL }
};

static int num_args = sizeof(args) / sizeof(args[0]);

static void
usage(void)
{
    arg_printusage (args, num_args, "del_enctype", "principal enctypes...");
}


int
del_enctype(int argc, char **argv)
{
    int optind = 0;
    int help_flag = 0;

    kadm5_principal_ent_rec princ;
    krb5_principal princ_ent = NULL;
    krb5_error_code ret;
    const char *princ_name;
    int i, j, k;
    krb5_key_data *new_key_data;
    int n_etypes;
    krb5_enctype *etypes;

    args[0].value = &help_flag;

    if(getarg(args, num_args, argc, argv, &optind)) {
	usage ();
	return 0;
    }
    if(argc - optind < 2 || help_flag) {
	usage ();
	return 0;
    }

    memset (&princ, 0, sizeof(princ));
    princ_name = argv[1];
    n_etypes   = argc - 2;
    etypes     = malloc (n_etypes * sizeof(*etypes));
    if (etypes == NULL) {
	krb5_warnx (context, "out of memory");
	return 0;
    }
    for (i = 0; i < n_etypes; ++i) {
	ret = krb5_string_to_enctype (context, argv[i + 2], &etypes[i]);
	if (ret) {
	    krb5_warnx (context, "bad enctype `%s'", argv[i + 2]);
	    goto out2;
	}
    }

    ret = krb5_parse_name(context, princ_name, &princ_ent);
    if (ret) {
	krb5_warn (context, ret, "krb5_parse_name %s", princ_name);
	goto out2;
    }

    ret = kadm5_get_principal(kadm_handle, princ_ent, &princ,
			      KADM5_PRINCIPAL | KADM5_KEY_DATA);
    if (ret) {
	krb5_free_principal (context, princ_ent);
	krb5_warnx (context, "no such principal: %s", princ_name);
	goto out2;
    }

    new_key_data   = malloc(princ.n_key_data * sizeof(*new_key_data));
    if (new_key_data == NULL) {
	krb5_warnx (context, "out of memory");
	goto out;
    }

    for (i = 0, j = 0; i < princ.n_key_data; ++i) {
	krb5_key_data *key = &princ.key_data[i];
	int docopy = 1;

	for (k = 0; k < n_etypes; ++k)
	    if (etypes[k] == key->key_data_type[0]) {
		docopy = 0;
		break;
	    }
	if (docopy) {
	    new_key_data[j++] = *key;
	} else {
	    int16_t ignore = 1;

	    kadm5_free_key_data (kadm_handle, &ignore, key);
	}
    }

    free (princ.key_data);
    princ.n_key_data = j;
    princ.key_data   = new_key_data;

    ret = kadm5_modify_principal (kadm_handle, &princ, KADM5_KEY_DATA);
    if (ret)
	krb5_warn(context, ret, "kadm5_modify_principal");
out:
    krb5_free_principal (context, princ_ent);
    kadm5_free_principal_ent(kadm_handle, &princ);
out2:
    free (etypes);
    return 0;
}
