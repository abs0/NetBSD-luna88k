/*
 * Copyright (c) 2001 Kungliga Tekniska H?gskolan
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

#include "ktutil_locl.h"

__RCSID("$Heimdal: rename.c,v 1.1 2001/07/23 10:17:32 joda Exp $"
        "$NetBSD: rename.c,v 1.1.1.2 2002/09/12 12:41:33 joda Exp $");

int
kt_rename(int argc, char **argv)
{
    krb5_error_code ret = 0;
    krb5_keytab_entry entry;
    krb5_keytab keytab;
    krb5_kt_cursor cursor;
    krb5_principal from_princ, to_princ;
    int help_flag = 0;

    struct getargs args[] = {
	{ "help", 'h', arg_flag, NULL }
    };
    int num_args = sizeof(args) / sizeof(args[0]);
    int optind = 0;
    int i = 0;

    args[i++].value = &help_flag;
    if(getarg(args, num_args, argc, argv, &optind)) {
	arg_printusage(args, num_args, "ktutil rename", "from to");
	return 1;
    }
    if(help_flag) {
	arg_printusage(args, num_args, "ktutil rename", "from to");
	return 0;
    }
    argv += optind;
    argc -= optind;
    if(argc != 2) {
	arg_printusage(args, num_args, "ktutil rename", "from to");
	return 0;
    }

    ret = krb5_parse_name(context, argv[0], &from_princ);
    if(ret != 0) {
	krb5_warn(context, ret, "%s", argv[0]);
	return 0;
    }

    ret = krb5_parse_name(context, argv[1], &to_princ);
    if(ret != 0) {
	krb5_free_principal(context, from_princ);
	krb5_warn(context, ret, "%s", argv[1]);
	return 0;
    }

    if((keytab = ktutil_open_keytab()) == NULL) {
	krb5_free_principal(context, from_princ);
	krb5_free_principal(context, to_princ);
	return 1;
    }

    ret = krb5_kt_start_seq_get(context, keytab, &cursor);
    if(ret) {
	krb5_kt_close(context, keytab);
	krb5_free_principal(context, from_princ);
	krb5_free_principal(context, to_princ);
	return 1;
    }
    while(1) {
	ret = krb5_kt_next_entry(context, keytab, &entry, &cursor);
	if(ret != 0) {
	    if(ret != KRB5_CC_END && ret != KRB5_KT_END)
		krb5_warn(context, ret, "getting entry from keytab");
	    break;
	}
	if(krb5_principal_compare(context, entry.principal, from_princ)) {
	    krb5_free_principal(context, entry.principal);
	    entry.principal = to_princ;
	    ret = krb5_kt_add_entry(context, keytab, &entry);
	    if(ret) {
		entry.principal = NULL;
		krb5_kt_free_entry(context, &entry);
		krb5_warn(context, ret, "adding entry");
		break;
	    }
	    entry.principal = from_princ;
	    ret = krb5_kt_remove_entry(context, keytab, &entry);
	    if(ret) {
		entry.principal = NULL;
		krb5_kt_free_entry(context, &entry);
		krb5_warn(context, ret, "removing entry");
		break;
	    }
	    entry.principal = NULL;
	}
	krb5_kt_free_entry(context, &entry);
    }
    krb5_kt_end_seq_get(context, keytab, &cursor);

    krb5_free_principal(context, from_princ);
    krb5_free_principal(context, to_princ);

    return 0;
}

