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

#include "kadmin_locl.h"
#include <kadm5/private.h>

__RCSID("$Heimdal: load.c,v 1.44 2002/09/04 20:44:35 joda Exp $"
        "$NetBSD: load.c,v 1.1.1.6 2002/09/12 12:41:39 joda Exp $");

struct entry {
    char *principal;
    char *key;
    char *max_life;
    char *max_renew;
    char *created;
    char *modified;
    char *valid_start;
    char *valid_end;
    char *pw_end;
    char *flags;
    char *generation;
};

static char *
skip_next(char *p)
{
    while(*p && !isspace((unsigned char)*p)) 
	p++;
    *p++ = 0;
    while(*p && isspace((unsigned char)*p))
	p++;
    return p;
}

/*
 * Parse the time in `s', returning:
 * -1 if error parsing
 * 0  if none  present
 * 1  if parsed ok
 */

static int
parse_time_string(time_t *t, const char *s)
{
    int year, month, date, hour, minute, second;
    struct tm tm;

    if(strcmp(s, "-") == 0)
	return 0;
    if(sscanf(s, "%04d%02d%02d%02d%02d%02d", 
	      &year, &month, &date, &hour, &minute, &second) != 6)
	return -1;
    tm.tm_year  = year - 1900;
    tm.tm_mon   = month - 1;
    tm.tm_mday  = date;
    tm.tm_hour  = hour;
    tm.tm_min   = minute;
    tm.tm_sec   = second;
    tm.tm_isdst = 0;
    *t = timegm(&tm);
    return 1;
}

/*
 * parse time, allocating space in *t if it's there
 */

static int
parse_time_string_alloc (time_t **t, const char *s)
{
    time_t tmp;
    int ret;

    *t = NULL;
    ret = parse_time_string (&tmp, s);
    if (ret == 1) {
	*t = malloc (sizeof (**t));
	if (*t == NULL)
	    krb5_errx (context, 1, "malloc: out of memory");
	**t = tmp;
    }
    return ret;
}

/*
 * see parse_time_string for calling convention
 */

static int
parse_integer(unsigned *u, const char *s)
{
    if(strcmp(s, "-") == 0)
	return 0;
    if (sscanf(s, "%u", u) != 1)
	return -1;
    return 1;
}

static int
parse_integer_alloc (int **u, const char *s)
{
    unsigned tmp;
    int ret;

    *u = NULL;
    ret = parse_integer (&tmp, s);
    if (ret == 1) {
	*u = malloc (sizeof (**u));
	if (*u == NULL)
	    krb5_errx (context, 1, "malloc: out of memory");
	**u = tmp;
    }
    return ret;
}

/*
 * Parse dumped keys in `str' and store them in `ent'
 * return -1 if parsing failed
 */

static int
parse_keys(hdb_entry *ent, char *str)
{
    krb5_error_code ret;
    int tmp;
    char *p;
    int i;
    
    p = strsep(&str, ":");
    if (sscanf(p, "%d", &tmp) != 1)
	return 1;
    ent->kvno = tmp;
    p = strsep(&str, ":");
    while(p){
	Key *key;
	key = realloc(ent->keys.val, 
		      (ent->keys.len + 1) * sizeof(*ent->keys.val));
	if(key == NULL)
	    krb5_errx (context, 1, "realloc: out of memory");
	ent->keys.val = key;
	key = ent->keys.val + ent->keys.len;
	ent->keys.len++;
	memset(key, 0, sizeof(*key));
	if(sscanf(p, "%d", &tmp) == 1) {
	    key->mkvno = malloc(sizeof(*key->mkvno));
	    *key->mkvno = tmp;
	} else
	    key->mkvno = NULL;
	p = strsep(&str, ":");
	if (sscanf(p, "%d", &tmp) != 1)
	    return 1;
	key->key.keytype = tmp;
	p = strsep(&str, ":");
	ret = krb5_data_alloc(&key->key.keyvalue, (strlen(p) - 1) / 2 + 1);
	if (ret)
	    krb5_err (context, 1, ret, "krb5_data_alloc");
	for(i = 0; i < strlen(p); i += 2) {
	    if(sscanf(p + i, "%02x", &tmp) != 1)
		return 1;
	    ((u_char*)key->key.keyvalue.data)[i / 2] = tmp;
	}
	p = strsep(&str, ":");
	if(strcmp(p, "-") != 0){
	    unsigned type;
	    size_t p_len;

	    if(sscanf(p, "%u/", &type) != 1)
		return 1;
	    p = strchr(p, '/');
	    if(p == NULL)
		return 1;
	    p++;
	    p_len = strlen(p);

	    key->salt = malloc(sizeof(*key->salt));
	    if (key->salt == NULL)
		krb5_errx (context, 1, "malloc: out of memory");
	    key->salt->type = type;
		
	    if (p_len) {
		if(*p == '\"') {
		    ret = krb5_data_copy(&key->salt->salt, p + 1, p_len - 2);
		    if (ret)
			krb5_err (context, 1, ret, "krb5_data_copy");
		} else {
		    ret = krb5_data_alloc(&key->salt->salt,
					  (p_len - 1) / 2 + 1);
		    if (ret)
			krb5_err (context, 1, ret, "krb5_data_alloc");
		    for(i = 0; i < p_len; i += 2){
			if (sscanf(p + i, "%02x", &tmp) != 1)
			    return 1;
			((u_char*)key->salt->salt.data)[i / 2] = tmp;
		    }
		}
	    } else
		krb5_data_zero (&key->salt->salt);
	}
	p = strsep(&str, ":");
    }
    return 0;
}

/*
 * see parse_time_string for calling convention
 */

static int
parse_event(Event *ev, char *s)
{
    krb5_error_code ret;
    char *p;

    if(strcmp(s, "-") == 0)
	return 0;
    memset(ev, 0, sizeof(*ev));
    p = strsep(&s, ":");
    if(parse_time_string(&ev->time, p) != 1)
	return -1;
    p = strsep(&s, ":");
    ret = krb5_parse_name(context, p, &ev->principal);
    if (ret)
	return -1;
    return 1;
}

static int
parse_event_alloc (Event **ev, char *s)
{
    Event tmp;
    int ret;

    *ev = NULL;
    ret = parse_event (&tmp, s);
    if (ret == 1) {
	*ev = malloc (sizeof (**ev));
	if (*ev == NULL)
	    krb5_errx (context, 1, "malloc: out of memory");
	**ev = tmp;
    }
    return ret;
}

static int
parse_hdbflags2int(HDBFlags *f, const char *s)
{
    int ret;
    unsigned tmp;

    ret = parse_integer (&tmp, s);
    if (ret == 1)
	*f = int2HDBFlags (tmp);
    return ret;
}

static int
parse_generation(char *str, GENERATION **gen)
{
    char *p;
    int v;

    if(strcmp(str, "-") == 0 || *str == '\0') {
	*gen = NULL;
	return 0;
    }
    *gen = calloc(1, sizeof(**gen));

    p = strsep(&str, ":");
    if(parse_time_string(&(*gen)->time, p) != 1)
	return -1;
    p = strsep(&str, ":");
    if(sscanf(p, "%d", &v) != 1)
	return -1;
    (*gen)->usec = v;
    p = strsep(&str, ":");
    if(sscanf(p, "%d", &v) != 1)
	return -1;
    (*gen)->gen = v - 1; /* XXX gets bumped in _hdb_store */
    return 0;
}


/*
 * Parse the dump file in `filename' and create the database (merging
 * iff merge)
 */

static int
doit(const char *filename, int merge)
{
    krb5_error_code ret;
    FILE *f;
    char s[8192]; /* XXX should fix this properly */
    char *p;
    int line;
    int flags = O_RDWR;
    struct entry e;
    hdb_entry ent;
    HDB *db = _kadm5_s_get_db(kadm_handle);

    f = fopen(filename, "r");
    if(f == NULL){
	krb5_warn(context, errno, "fopen(%s)", filename);
	return 1;
    }
    ret = kadm5_log_truncate (kadm_handle);
    if (ret) {
	fclose (f);
	krb5_warn(context, ret, "kadm5_log_truncate");
	return 1;
    }

    if(!merge)
	flags |= O_CREAT | O_TRUNC;
    ret = db->open(context, db, flags, 0600);
    if(ret){
	krb5_warn(context, ret, "hdb_open");
	fclose(f);
	return 1;
    }
    line = 0;
    ret = 0;
    while(fgets(s, sizeof(s), f) != NULL) {
	ret = 0;
	line++;
	e.principal = s;
	for(p = s; *p; p++){
	    if(*p == '\\')
		p++;
	    else if(isspace((unsigned char)*p)) {
		*p = 0;
		break;
	    }
	}
	p = skip_next(p);
	
	e.key = p;
	p = skip_next(p);

	e.created = p;
	p = skip_next(p);

	e.modified = p;
	p = skip_next(p);

	e.valid_start = p;
	p = skip_next(p);

	e.valid_end = p;
	p = skip_next(p);

	e.pw_end = p;
	p = skip_next(p);

	e.max_life = p;
	p = skip_next(p);

	e.max_renew = p;
	p = skip_next(p);

	e.flags = p;
	p = skip_next(p);

	e.generation = p;
	p = skip_next(p);

	memset(&ent, 0, sizeof(ent));
	ret = krb5_parse_name(context, e.principal, &ent.principal);
	if(ret) {
	    fprintf(stderr, "%s:%d:%s (%s)\n", 
		    filename, 
		    line,
		    krb5_get_err_text(context, ret),
		    e.principal);
	    continue;
	}
	
	if (parse_keys(&ent, e.key)) {
	    fprintf (stderr, "%s:%d:error parsing keys (%s)\n",
		     filename, line, e.key);
	    hdb_free_entry (context, &ent);
	    continue;
	}
	
	if (parse_event(&ent.created_by, e.created) == -1) {
	    fprintf (stderr, "%s:%d:error parsing created event (%s)\n",
		     filename, line, e.created);
	    hdb_free_entry (context, &ent);
	    continue;
	}
	if (parse_event_alloc (&ent.modified_by, e.modified) == -1) {
	    fprintf (stderr, "%s:%d:error parsing event (%s)\n",
		     filename, line, e.modified);
	    hdb_free_entry (context, &ent);
	    continue;
	}
	if (parse_time_string_alloc (&ent.valid_start, e.valid_start) == -1) {
	    fprintf (stderr, "%s:%d:error parsing time (%s)\n",
		     filename, line, e.valid_start);
	    hdb_free_entry (context, &ent);
	    continue;
	}
	if (parse_time_string_alloc (&ent.valid_end,   e.valid_end) == -1) {
	    fprintf (stderr, "%s:%d:error parsing time (%s)\n",
		     filename, line, e.valid_end);
	    hdb_free_entry (context, &ent);
	    continue;
	}
	if (parse_time_string_alloc (&ent.pw_end,      e.pw_end) == -1) {
	    fprintf (stderr, "%s:%d:error parsing time (%s)\n",
		     filename, line, e.pw_end);
	    hdb_free_entry (context, &ent);
	    continue;
	}

	if (parse_integer_alloc (&ent.max_life,  e.max_life) == -1) {
	    fprintf (stderr, "%s:%d:error parsing lifetime (%s)\n",
		     filename, line, e.max_life);
	    hdb_free_entry (context, &ent);
	    continue;

	}
	if (parse_integer_alloc (&ent.max_renew, e.max_renew) == -1) {
	    fprintf (stderr, "%s:%d:error parsing lifetime (%s)\n",
		     filename, line, e.max_renew);
	    hdb_free_entry (context, &ent);
	    continue;
	}

	if (parse_hdbflags2int (&ent.flags, e.flags) != 1) {
	    fprintf (stderr, "%s:%d:error parsing flags (%s)\n",
		     filename, line, e.flags);
	    hdb_free_entry (context, &ent);
	    continue;
	}

	if(parse_generation(e.generation, &ent.generation) == -1) {
	    fprintf (stderr, "%s:%d:error parsing generation (%s)\n",
		     filename, line, e.generation);
	    hdb_free_entry (context, &ent);
	    continue;
	}

	ret = db->store(context, db, HDB_F_REPLACE, &ent);
	hdb_free_entry (context, &ent);
	if (ret) {
	    krb5_warn(context, ret, "db_store");
	    break;
	}
    }
    db->close(context, db);
    fclose(f);
    return ret != 0;
}


static struct getargs args[] = {
    { "help", 'h', arg_flag, NULL }
};

static int num_args = sizeof(args) / sizeof(args[0]);

static void
usage(const char *name)
{
    arg_printusage (args, num_args, name, "file");
}



int
load(int argc, char **argv)
{
    int optind = 0;
    int help_flag = 0;

    args[0].value = &help_flag;

    if(getarg(args, num_args, argc, argv, &optind)) {
	usage ("load");
	return 0;
    }
    if(argc - optind != 1 || help_flag) {
	usage ("load");
	return 0;
    }

    doit(argv[optind], 0);
    return 0;
}

int
merge(int argc, char **argv)
{
    int optind = 0;
    int help_flag = 0;

    args[0].value = &help_flag;

    if(getarg(args, num_args, argc, argv, &optind)) {
	usage ("merge");
	return 0;
    }
    if(argc - optind != 1 || help_flag) {
	usage ("merge");
	return 0;
    }

    doit(argv[optind], 1);
    return 0;
}
