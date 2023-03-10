/*
 * Copyright (c) 1999 - 2003 Kungliga Tekniska H?gskolan
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
 * 3. Neither the name of KTH nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY KTH AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL KTH OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include <config.h>

__RCSID("$Heimdal: su.c,v 1.26.2.1 2003/05/06 12:06:44 joda Exp $"
        "$NetBSD: su.c,v 1.1.1.7 2003/05/15 20:28:41 lha Exp $");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <syslog.h>

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

#include <pwd.h>

#include "crypto-headers.h"
#ifdef KRB5
#include <krb5.h>
#endif
#ifdef KRB4
#include <krb.h>
#endif
#include <kafs.h>
#include <err.h>
#include <roken.h>
#include <getarg.h>

#ifndef _PATH_DEFPATH
#define _PATH_DEFPATH "/usr/bin:/bin"
#endif

#ifndef _PATH_BSHELL
#define _PATH_BSHELL "/bin/sh"
#endif

int kerberos_flag = 1;
int csh_f_flag;
int full_login;
int env_flag;
char *kerberos_instance = "root";
int help_flag;
int version_flag;
char *cmd;
char tkfile[256];

struct getargs args[] = {
    { "kerberos", 'K', arg_negative_flag, &kerberos_flag,
      "don't use kerberos" },
    { NULL,	  'f', arg_flag,	  &csh_f_flag,
      "don't read .cshrc" },
    { "full",	  'l', arg_flag,          &full_login,
      "simulate full login" },
    { NULL,	  'm', arg_flag,          &env_flag,
      "leave environment unmodified" },
    { "instance", 'i', arg_string,        &kerberos_instance,
      "root instance to use" },
    { "command",  'c', arg_string,        &cmd,
      "command to execute" },
    { "help", 	  'h', arg_flag,          &help_flag },
    { "version",  0,   arg_flag,          &version_flag },
};


static void
usage (int ret)
{
    arg_printusage (args,
		    sizeof(args)/sizeof(*args),
		    NULL,
		    "[login [shell arguments]]");
    exit (ret);
}

static void
free_info(struct passwd *p)
{
    free (p->pw_name);
    free (p->pw_passwd);
    free (p->pw_dir);
    free (p->pw_shell);
    free (p);
}

static struct passwd*
dup_info(const struct passwd *pwd)
{
    struct passwd *info;

    info = malloc(sizeof(*info));
    if(info == NULL)
	return NULL;
    info->pw_name = strdup(pwd->pw_name);
    info->pw_passwd = strdup(pwd->pw_passwd);
    info->pw_uid = pwd->pw_uid;
    info->pw_gid = pwd->pw_gid;
    info->pw_dir = strdup(pwd->pw_dir);
    info->pw_shell = strdup(pwd->pw_shell);
    if(info->pw_name == NULL || info->pw_passwd == NULL ||
       info->pw_dir == NULL || info->pw_shell == NULL) {
	free_info (info);
	return NULL;
    }
    return info;
}

#if defined(KRB4) || defined(KRB5)
static void
set_tkfile()
{
#ifndef TKT_ROOT
#define TKT_ROOT "/tmp/tkt"
#endif
    int fd;
    if(*tkfile != '\0')
	return;
    snprintf(tkfile, sizeof(tkfile), "%s_XXXXXX", TKT_ROOT);
    fd = mkstemp(tkfile);
    if(fd >= 0)
	close(fd);
#ifdef KRB4
    krb_set_tkt_string(tkfile);
#endif
}
#endif

#ifdef KRB5
static krb5_context context;
static krb5_ccache ccache;

static int
krb5_verify(const struct passwd *login_info,
	    const struct passwd *su_info,
	    const char *kerberos_instance)
{
    krb5_error_code ret;
    krb5_principal p;
    char *login_name = NULL;
	
#if defined(HAVE_GETLOGIN) && !defined(POSIX_GETLOGIN)
    login_name = getlogin();
#endif
    ret = krb5_init_context (&context);
    if (ret) {
#if 0
	warnx("krb5_init_context failed: %d", ret);
#endif
	return 1;
    }
	
    if (login_name == NULL || strcmp (login_name, "root") == 0) 
	login_name = login_info->pw_name;
    if (strcmp (su_info->pw_name, "root") == 0)
	ret = krb5_make_principal(context, &p, NULL, 
				  login_name,
				  kerberos_instance,
				  NULL);
    else
	ret = krb5_make_principal(context, &p, NULL, 
				  su_info->pw_name,
				  NULL);
    if(ret)
	return 1;
	
    if(su_info->pw_uid != 0 || krb5_kuserok(context, p, su_info->pw_name)) {
	ret = krb5_cc_gen_new(context, &krb5_mcc_ops, &ccache);
	if(ret) {
#if 1
	    krb5_warn(context, ret, "krb5_cc_gen_new");
#endif
	    krb5_free_principal (context, p);
	    return 1;
	}
	ret = krb5_verify_user_lrealm(context, p, ccache, NULL, TRUE, NULL);
	krb5_free_principal (context, p);
	if(ret) {
	    krb5_cc_destroy(context, ccache);
	    switch (ret) {
	    case KRB5_LIBOS_PWDINTR :
		break;
	    case KRB5KRB_AP_ERR_BAD_INTEGRITY:
	    case KRB5KRB_AP_ERR_MODIFIED:
		krb5_warnx(context, "Password incorrect");
		break;
	    default :
		krb5_warn(context, ret, "krb5_verify_user");
		break;
	    }
	    return 1;
	}
	return 0;
    }
    krb5_free_principal (context, p);
    return 1;
}

static int
krb5_start_session(void)
{
    krb5_ccache ccache2;
    char *cc_name;
    int ret;

    ret = krb5_cc_gen_new(context, &krb5_fcc_ops, &ccache2);
    if (ret) {
	krb5_cc_destroy(context, ccache);
	return 1;
    }

    ret = krb5_cc_copy_cache(context, ccache, ccache2);

    asprintf(&cc_name, "%s:%s", krb5_cc_get_type(context, ccache2),
	     krb5_cc_get_name(context, ccache2));
    esetenv("KRB5CCNAME", cc_name, 1);

    /* we want to export this even if we don't directly support KRB4 */
    set_tkfile();
    esetenv("KRBTKFILE", tkfile, 1);
            
    /* convert creds? */
    if(k_hasafs()) {
	if (k_setpag() == 0)
	    krb5_afslog(context, ccache2, NULL, NULL);
    }
            
    krb5_cc_close(context, ccache2);
    krb5_cc_destroy(context, ccache);
    return 0;
}
#endif

#ifdef KRB4

static int
krb_verify(const struct passwd *login_info,
	   const struct passwd *su_info,
	   const char *kerberos_instance)
{
    int ret;
    char *login_name = NULL;
    char *name, *instance, realm[REALM_SZ];
	
#if defined(HAVE_GETLOGIN) && !defined(POSIX_GETLOGIN)
    login_name = getlogin();
#endif

    ret = krb_get_lrealm(realm, 1);
	
    if (login_name == NULL || strcmp (login_name, "root") == 0) 
	login_name = login_info->pw_name;
    if (strcmp (su_info->pw_name, "root") == 0) {
	name = login_name;
	instance = (char*)kerberos_instance;
    } else {
	name = su_info->pw_name;
	instance = "";
    }

    if(su_info->pw_uid != 0 || 
       krb_kuserok(name, instance, realm, su_info->pw_name) == 0) {
	char password[128];
	char *prompt;
	asprintf (&prompt, 
		  "%s's Password: ",
		  krb_unparse_name_long (name, instance, realm));
	if (des_read_pw_string (password, sizeof (password), prompt, 0)) {
	    memset (password, 0, sizeof (password));
	    free(prompt);
	    return (1);
	}
	free(prompt);
	if (strlen(password) == 0)
	    return (1);		/* Empty passwords are not allowed */
	set_tkfile();
	setuid(geteuid()); /* need to run as root here */
	ret = krb_verify_user(name, instance, realm, password, 
			      KRB_VERIFY_SECURE, NULL);
	memset(password, 0, sizeof(password));
	
	if(ret) {
	    warnx("%s", krb_get_err_text(ret));
	    return 1;
	}
	chown (tkt_string(), su_info->pw_uid, su_info->pw_gid);
	return 0;
    }
    return 1;
}


static int
krb_start_session(void)
{
    esetenv("KRBTKFILE", tkfile, 1);
            
    /* convert creds? */
    if(k_hasafs() && k_setpag() == 0)
	krb_afslog(NULL, NULL);
            
    return 0;
}
#endif

static int
verify_unix(struct passwd *su)
{
    char prompt[128];
    char pw_buf[1024];
    char *pw;
    int r;
    if(su->pw_passwd != NULL && *su->pw_passwd != '\0') {
	snprintf(prompt, sizeof(prompt), "%s's password: ", su->pw_name);
	r = des_read_pw_string(pw_buf, sizeof(pw_buf), prompt, 0);
	if(r != 0)
	    exit(0);
	pw = crypt(pw_buf, su->pw_passwd);
	memset(pw_buf, 0, sizeof(pw_buf));
	if(strcmp(pw, su->pw_passwd) != 0)
	    return 1;
    }
    return 0;
}

int
main(int argc, char **argv)
{
    int i, optind = 0;
    char *su_user;
    struct passwd *su_info;
    struct passwd *login_info;

    struct passwd *pwd;

    char *shell;

    int ok = 0;
    int kerberos_error=1;

    setprogname (argv[0]);

    if(getarg(args, sizeof(args) / sizeof(args[0]), argc, argv, &optind))
	usage(1);

    for (i=0; i < optind; i++)
      if (strcmp(argv[i], "-") == 0) {
	 full_login = 1;
	 break;
      }
	
    if(help_flag)
	usage(0);
    if(version_flag) {
	print_version(NULL);
	exit(0);
    }
    if(optind >= argc)
	su_user = "root";
    else
	su_user = argv[optind++];

    pwd = k_getpwnam(su_user);
    if(pwd == NULL)
	errx (1, "unknown login %s", su_user);
    if (pwd->pw_uid == 0 && strcmp ("root", su_user) != 0) {
	syslog (LOG_ALERT, "NIS attack, user %s has uid 0", su_user);
	errx (1, "unknown login %s", su_user);
    }
    su_info = dup_info(pwd);
    if (su_info == NULL)
	errx (1, "malloc: out of memory");
    
	pwd = getpwuid(getuid());
    if(pwd == NULL)
	errx(1, "who are you?");
    login_info = dup_info(pwd);
    if (login_info == NULL)
	errx (1, "malloc: out of memory");
    if(env_flag)
	shell = login_info->pw_shell;
    else
	shell = su_info->pw_shell;
    if(shell == NULL || *shell == '\0')
	shell = _PATH_BSHELL;
    

#ifdef KRB5
    if(kerberos_flag && ok == 0 &&
      (kerberos_error=krb5_verify(login_info, su_info, kerberos_instance)) == 0)
	ok = 5;
#endif
#ifdef KRB4
    if(kerberos_flag && ok == 0 &&
       (kerberos_error = krb_verify(login_info, su_info, kerberos_instance)) == 0)
	ok = 4;
#endif

    if(ok == 0 && login_info->pw_uid && verify_unix(su_info) != 0) {
	printf("Sorry!\n");
	exit(1);
    }

#ifdef HAVE_GETSPNAM
   {  struct spwd *sp;
      long    today;
    
    sp = getspnam(su_info->pw_name);
    if (sp != NULL) {
	today = time(0)/(24L * 60 * 60);
	if (sp->sp_expire > 0) {
	    if (today >= sp->sp_expire) {
		if (login_info->pw_uid) 
		    errx(1,"Your account has expired.");
		else
		    printf("Your account has expired.");
            }
            else if (sp->sp_expire - today < 14) 
                printf("Your account will expire in %d days.\n",
		       (int)(sp->sp_expire - today));
	} 
	if (sp->sp_max > 0) {
	    if (today >= sp->sp_lstchg + sp->sp_max) {
		if (login_info->pw_uid)    
		    errx(1,"Your password has expired. Choose a new one.");
		else
		    printf("Your password has expired. Choose a new one.");
	    }
	    else if (today >= sp->sp_lstchg + sp->sp_max - sp->sp_warn)
		printf("Your account will expire in %d days.\n",
		       (int)(sp->sp_lstchg + sp->sp_max -today));
	}
    }
    }
#endif
    {
	char *tty = ttyname (STDERR_FILENO);
	syslog (LOG_NOTICE | LOG_AUTH, tty ? "%s to %s" : "%s to %s on %s",
		login_info->pw_name, su_info->pw_name, tty);
    }


    if(!env_flag) {
	if(full_login) {
	    char *t = getenv ("TERM");
	    
	    environ = malloc (10 * sizeof (char *));
	    if (environ == NULL)
		err (1, "malloc");
	    environ[0] = NULL;
	    esetenv ("PATH", _PATH_DEFPATH, 1);
	    if (t)
		esetenv ("TERM", t, 1);
	    if (chdir (su_info->pw_dir) < 0)
		errx (1, "no directory");
	}
	if (full_login || su_info->pw_uid)
	    esetenv ("USER", su_info->pw_name, 1);
	esetenv("HOME", su_info->pw_dir, 1);
	esetenv("SHELL", shell, 1);
    }

    {
	int i;
	char **args;
	char *p;

	p = strrchr(shell, '/');
	if(p)
	    p++;
	else
	    p = shell;

	if (strcmp(p, "csh") != 0)
	    csh_f_flag = 0;

        args = malloc(((cmd ? 2 : 0) + 1 + argc - optind + 1 + csh_f_flag) * sizeof(*args));
	if (args == NULL)
	    err (1, "malloc");
	i = 0;
	if(full_login)
	    asprintf(&args[i++], "-%s", p);
	else
	    args[i++] = p;
	if (cmd) {
	   args[i++] = "-c";
	   args[i++] = cmd;
	}  
	   
	if (csh_f_flag)
	    args[i++] = "-f";

	for (argv += optind; *argv; ++argv)
	   args[i++] = *argv;
	args[i] = NULL;
	
	if(setgid(su_info->pw_gid) < 0)
	    err(1, "setgid");
	if (initgroups (su_info->pw_name, su_info->pw_gid) < 0)
	    err (1, "initgroups");
	if(setuid(su_info->pw_uid) < 0
	   || (su_info->pw_uid != 0 && setuid(0) == 0))
	    err(1, "setuid");

#ifdef KRB5
        if (ok == 5)
           krb5_start_session();
#endif
#ifdef KRB4
	if (ok == 4)
	    krb_start_session();
#endif
	execv(shell, args);
    }
    
    exit(1);
}
