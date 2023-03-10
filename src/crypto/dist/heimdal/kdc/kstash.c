/*
 * Copyright (c) 1997-2002 Kungliga Tekniska H?gskolan
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

#include "headers.h"
#include <openssl/ui.h>

__RCSID("$Heimdal: kstash.c,v 1.15 2002/04/18 09:47:25 joda Exp $"
        "$NetBSD: kstash.c,v 1.2 2003/07/24 14:16:55 itojun Exp $");

krb5_context context;

const char *keyfile = HDB_DB_DIR "/m-key";
int convert_flag;
int help_flag;
int version_flag;

int master_key_fd = -1;

const char *enctype_str = "des3-cbc-sha1";

struct getargs args[] = {
    { "enctype", 'e', arg_string, &enctype_str, "encryption type" },
    { "key-file", 'k', arg_string, &keyfile, "master key file", "file" },
    { "convert-file", 0, arg_flag, &convert_flag, 
      "just convert keyfile to new format" },
    { "master-key-fd", 0, arg_integer, &master_key_fd, 
      "filedescriptor to read passphrase from", "fd" },
    { "help", 'h', arg_flag, &help_flag },
    { "version", 0, arg_flag, &version_flag }
};

int num_args = sizeof(args) / sizeof(args[0]);

int
main(int argc, char **argv)
{
    char buf[1024];
    krb5_error_code ret;
    
    krb5_enctype enctype;

    hdb_master_key mkey;
    
    krb5_program_setup(&context, argc, argv, args, num_args, NULL);

    if(help_flag)
	krb5_std_usage(0, args, num_args);
    if(version_flag){
	print_version(NULL);
	exit(0);
    }

    ret = krb5_string_to_enctype(context, enctype_str, &enctype);
    if(ret)
	krb5_err(context, 1, ret, "krb5_string_to_enctype");

    ret = hdb_read_master_key(context, keyfile, &mkey);
    if(ret && ret != ENOENT)
	krb5_err(context, 1, ret, "reading master key from %s", keyfile);

    if (convert_flag) {
	if (ret)
	    krb5_err(context, 1, ret, "reading master key from %s", keyfile);
    } else {
	krb5_keyblock key;
	krb5_salt salt;
	salt.salttype = KRB5_PW_SALT;
	/* XXX better value? */
	salt.saltvalue.data = NULL;
	salt.saltvalue.length = 0;
	if(master_key_fd != -1) {
	    ssize_t n;
	    n = read(master_key_fd, buf, sizeof(buf));
	    if(n <= 0)
		krb5_err(context, 1, errno, "failed to read passphrase");
	    buf[n] = '\0';
	    buf[strcspn(buf, "\r\n")] = '\0';
	} else {
	    if(UI_UTIL_read_pw_string(buf, sizeof(buf), "Master key: ", 1))
		exit(1);
	}
	krb5_string_to_key_salt(context, enctype, buf, salt, &key);
	ret = hdb_add_master_key(context, &key, &mkey);
	
	krb5_free_keyblock_contents(context, &key);

    }
    
    {
	char *new, *old;
	asprintf(&old, "%s.old", keyfile);
	asprintf(&new, "%s.new", keyfile);
	if(unlink(new) < 0 && errno != ENOENT) {
	    ret = errno;
	    goto out;
	}
	krb5_warnx(context, "writing key to `%s'", keyfile);
	ret = hdb_write_master_key(context, new, mkey);
	if(ret)
	    unlink(new);
	else {
	    unlink(old);
	    if(link(keyfile, old) < 0 && errno != ENOENT) {
		ret = errno;
		unlink(new);
	    } else if(rename(new, keyfile) < 0) {
		ret = errno;
	    }
	}
    out:
	free(old);
	free(new);
	if(ret)
	    krb5_warn(context, errno, "writing master key file");
    }

    hdb_free_master_key(context, mkey);

    exit(ret != 0);
}
