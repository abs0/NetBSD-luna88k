/*
 * Copyright (c) 2000 Kungliga Tekniska H?gskolan
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

#include "hprop.h"

__RCSID("$Heimdal: mit_dump.c,v 1.3 2000/08/09 09:57:37 joda Exp $"
        "$NetBSD: mit_dump.c,v 1.3 2002/09/12 13:19:02 joda Exp $");

/*
can have any number of princ stanzas.
format is as follows (only \n indicates newlines)
princ\t%d\t (%d is KRB5_KDB_V1_BASE_LENGTH, always 38)
%d\t (strlen of principal e.g. shadow/foo@ANDREW.CMU.EDU)
%d\t (number of tl_data)
%d\t (number of key data, e.g. how many keys for this user)
%d\t (extra data length) 
%s\t (principal name)
%d\t (attributes)
%d\t (max lifetime, seconds)
%d\t (max renewable life, seconds)
%d\t (expiration, seconds since epoch or 2145830400 for never)
%d\t (password expiration, seconds, 0 for never) 
%d\t (last successful auth, seconds since epoch)
%d\t (last failed auth, per above)
%d\t (failed auth count)
foreach tl_data 0 to number of tl_data - 1 as above
  %d\t%d\t (data type, data length)
  foreach tl_data 0 to length-1
    %02x (tl data contents[element n])
  except if tl_data length is 0
    %d (always -1)
  \t
foreach key 0 to number of keys - 1 as above
  %d\t%d\t (key data version, kvno)
  foreach version 0 to key data version - 1 (a key or a salt)
    %d\t%d\t(data type for this key, data length for this key)
    foreach key data length 0 to length-1
      %02x (key data contents[element n])
    except if key_data length is 0
      %d (always -1)
    \t    
foreach extra data length 0 to length - 1
  %02x (extra data part)
unless no extra data
  %d (always -1)
;\n

*/

static int
hex_to_octet_string(const char *ptr, krb5_data *data)
{
    int i;
    unsigned int v;
    for(i = 0; i < data->length; i++) {
	if(sscanf(ptr + 2 * i, "%02x", &v) != 1)
	    return -1;
	((unsigned char*)data->data)[i] = v;
    }
    return 2 * i;
}

static char *
nexttoken(char **p)
{
    char *q;
    do {
	q = strsep(p, " \t");
    } while(q && *q == '\0');
    return q;
}

static size_t
getdata(char **p, unsigned char *buf, size_t len)
{
    size_t i;
    int v;
    char *q = nexttoken(p);
    i = 0;
    while(*q && i < len) {
	if(sscanf(q, "%02x", &v) != 1)
	    break;
	buf[i++] = v;
	q += 2;
    }
    return i;
}

static int
getint(char **p)
{
    int val;
    char *q = nexttoken(p);
    sscanf(q, "%d", &val);
    return val;
}

#include <kadm5/admin.h>

static void
attr_to_flags(unsigned attr, HDBFlags *flags)
{
    flags->postdate =		!(attr & KRB5_KDB_DISALLOW_POSTDATED);
    flags->forwardable =	!(attr & KRB5_KDB_DISALLOW_FORWARDABLE);
    flags->initial =	       !!(attr & KRB5_KDB_DISALLOW_TGT_BASED);
    flags->renewable =		!(attr & KRB5_KDB_DISALLOW_RENEWABLE);
    flags->proxiable =		!(attr & KRB5_KDB_DISALLOW_PROXIABLE);
    /* DUP_SKEY */
    flags->invalid =	       !!(attr & KRB5_KDB_DISALLOW_ALL_TIX);
    flags->require_preauth =   !!(attr & KRB5_KDB_REQUIRES_PRE_AUTH);
    /* HW_AUTH */
    flags->server =		!(attr & KRB5_KDB_DISALLOW_SVR);
    flags->change_pw = 	       !!(attr & KRB5_KDB_PWCHANGE_SERVICE);
    flags->client =	        1; /* XXX */
}

#define KRB5_KDB_SALTTYPE_NORMAL	0
#define KRB5_KDB_SALTTYPE_V4		1
#define KRB5_KDB_SALTTYPE_NOREALM	2
#define KRB5_KDB_SALTTYPE_ONLYREALM	3
#define KRB5_KDB_SALTTYPE_SPECIAL	4
#define KRB5_KDB_SALTTYPE_AFS3		5

static krb5_error_code
fix_salt(krb5_context context, hdb_entry *ent, int key_num)
{
    krb5_error_code ret;
    Salt *salt = ent->keys.val[key_num].salt;
    /* fix salt type */
    switch((int)salt->type) {
    case KRB5_KDB_SALTTYPE_NORMAL:
	salt->type = KRB5_PADATA_PW_SALT;
	break;
    case KRB5_KDB_SALTTYPE_V4:
	krb5_data_free(&salt->salt);
	salt->type = KRB5_PADATA_PW_SALT;
	break;
    case KRB5_KDB_SALTTYPE_NOREALM:
    {
	size_t len;
	int i;
	krb5_error_code ret;
	char *p;
	    
	len = 0;
	for (i = 0; i < ent->principal->name.name_string.len; ++i)
	    len += strlen(ent->principal->name.name_string.val[i]);
	ret = krb5_data_alloc (&salt->salt, len);
	if (ret)
	    return ret;
	p = salt->salt.data;
	for (i = 0; i < ent->principal->name.name_string.len; ++i) {
	    memcpy (p,
		    ent->principal->name.name_string.val[i],
		    strlen(ent->principal->name.name_string.val[i]));
	    p += strlen(ent->principal->name.name_string.val[i]);
	}

	salt->type = KRB5_PADATA_PW_SALT;
	break;
    }
    case KRB5_KDB_SALTTYPE_ONLYREALM:
	krb5_data_free(&salt->salt);
	ret = krb5_data_copy(&salt->salt, 
			     ent->principal->realm, 
			     strlen(ent->principal->realm));
	if(ret)
	    return ret;
	salt->type = KRB5_PADATA_PW_SALT;
	break;
    case KRB5_KDB_SALTTYPE_SPECIAL:
	salt->type = KRB5_PADATA_PW_SALT;
	break;
    case KRB5_KDB_SALTTYPE_AFS3:
	krb5_data_free(&salt->salt);
	ret = krb5_data_copy(&salt->salt, 
		       ent->principal->realm, 
		       strlen(ent->principal->realm));
	if(ret)
	    return ret;
	salt->type = KRB5_PADATA_AFS3_SALT;
	break;
    default:
	abort();
    }
    return 0;
}

int
mit_prop_dump(void *arg, const char *file)
{
    krb5_error_code ret;
    char buf [1024];
    FILE *f;
    int lineno = 0;
    struct hdb_entry ent;

    struct prop_data *pd = arg;

    f = fopen(file, "r");
    if(f == NULL)
	return errno;
    
    while(fgets(buf, sizeof(buf), f)) {
	char *p = buf, *q;

	int i;

	int num_tl_data;
	int num_key_data;
	int extra_data_length;
	int attributes;

	int tmp;

	lineno++;

	memset(&ent, 0, sizeof(ent));

	q = nexttoken(&p);
	if(strcmp(q, "kdb5_util") == 0) {
	    int major;
	    q = nexttoken(&p); /* load_dump */
	    if(strcmp(q, "load_dump"))
		errx(1, "line %d: unknown version", lineno);
	    q = nexttoken(&p); /* load_dump */
	    if(strcmp(q, "version"))
		errx(1, "line %d: unknown version", lineno);
	    q = nexttoken(&p); /* x.0 */
	    if(sscanf(q, "%d", &major) != 1)
		errx(1, "line %d: unknown version", lineno);
	    if(major != 4)
		errx(1, "unknown dump file format, got %d, expected 4", major);
	    continue;
	} else if(strcmp(q, "princ") != 0) {
	    warnx("line %d: not a principal", lineno);
	    continue;
	}
	tmp = getint(&p);
	if(tmp != 38) {
	    warnx("line %d: bad base length %d != 38", lineno, tmp);
	    continue;
	}
	q = nexttoken(&p); /* length of principal */
	num_tl_data = getint(&p); /* number of tl-data */
	num_key_data = getint(&p); /* number of key-data */
	extra_data_length = getint(&p);  /* length of extra data */
	q = nexttoken(&p); /* principal name */
	krb5_parse_name(pd->context, q, &ent.principal);
	attributes = getint(&p); /* attributes */
	attr_to_flags(attributes, &ent.flags);
	tmp = getint(&p); /* max life */
	if(tmp != 0) {
	    ALLOC(ent.max_life);
	    *ent.max_life = tmp;
	}
	tmp = getint(&p); /* max renewable life */
	if(tmp != 0) {
	    ALLOC(ent.max_renew);
	    *ent.max_renew = tmp;
	}
	tmp = getint(&p); /* expiration */
	if(tmp != 0 && tmp != 2145830400) {
	    ALLOC(ent.valid_end);
	    *ent.valid_end = tmp;
	}
	tmp = getint(&p); /* pw expiration */
	if(tmp != 0) {
	    ALLOC(ent.pw_end);
	    *ent.pw_end = tmp;
	}
	q = nexttoken(&p); /* last auth */
	q = nexttoken(&p); /* last failed auth */
	q = nexttoken(&p); /* fail auth count */
	for(i = 0; i < num_tl_data; i++) {
	    unsigned long val;
	    int tl_type, tl_length;
	    unsigned char *buf;
	    krb5_principal princ;

	    tl_type = getint(&p); /* data type */
	    tl_length = getint(&p); /* data length */

#define KRB5_TL_LAST_PWD_CHANGE	1
#define KRB5_TL_MOD_PRINC	2
	    switch(tl_type) {
	    case KRB5_TL_MOD_PRINC:
		buf = malloc(tl_length);
		getdata(&p, buf, tl_length); /* data itself */
		val = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		ret = krb5_parse_name(pd->context, buf + 4, &princ);
		free(buf);
		ALLOC(ent.modified_by);
		ent.modified_by->time = val;
		ent.modified_by->principal = princ;
		break;
	    default:
		nexttoken(&p);
		break;
	    }
	}
	ALLOC_SEQ(&ent.keys, num_key_data);
	for(i = 0; i < num_key_data; i++) {
	    int key_versions;
	    key_versions = getint(&p); /* key data version */
	    ent.kvno = getint(&p); /* XXX kvno */
	    
	    ALLOC(ent.keys.val[i].mkvno);
	    *ent.keys.val[i].mkvno = 0;
	    
	    /* key version 0 -- actual key */
	    ent.keys.val[i].key.keytype = getint(&p); /* key type */
	    tmp = getint(&p); /* key length */
	    /* the first two bytes of the key is the key length --
	       skip it */
	    krb5_data_alloc(&ent.keys.val[i].key.keyvalue, tmp - 2);
	    q = nexttoken(&p); /* key itself */
	    hex_to_octet_string(q + 4, &ent.keys.val[i].key.keyvalue);

	    if(key_versions > 1) {
		/* key version 1 -- optional salt */
		ALLOC(ent.keys.val[i].salt);
		ent.keys.val[i].salt->type = getint(&p); /* salt type */
		tmp = getint(&p); /* salt length */
		if(tmp > 0) {
		    krb5_data_alloc(&ent.keys.val[i].salt->salt, tmp - 2);
		    q = nexttoken(&p); /* salt itself */
		    hex_to_octet_string(q + 4, &ent.keys.val[i].salt->salt);
		} else {
		    ent.keys.val[i].salt->salt.length = 0;
		    ent.keys.val[i].salt->salt.data = NULL;
		    tmp = getint(&p);	/* -1, if no data. */
		}
		fix_salt(pd->context, &ent, i);
	    }
	}
	q = nexttoken(&p); /* extra data */
	v5_prop(pd->context, NULL, &ent, arg);
    }
    return 0;
}
