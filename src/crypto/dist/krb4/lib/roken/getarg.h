/*
 * Copyright (c) 1997 - 2002 Kungliga Tekniska H?gskolan
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

/* $KTH-KRB: getarg.h,v 1.12 2002/04/18 08:50:08 joda Exp $
   $NetBSD: getarg.h,v 1.1.1.4 2002/09/12 12:22:10 joda Exp $ */

#ifndef __GETARG_H__
#define __GETARG_H__

#include <stddef.h>

struct getargs{
    const char *long_name;
    char short_name;
    enum { arg_integer, 
	   arg_string, 
	   arg_flag, 
	   arg_negative_flag, 
	   arg_strings,
	   arg_double,
	   arg_collect,
	   arg_counter
    } type;
    void *value;
    const char *help;
    const char *arg_help;
};

enum {
    ARG_ERR_NO_MATCH  = 1,
    ARG_ERR_BAD_ARG,
    ARG_ERR_NO_ARG
};

typedef struct getarg_strings {
    int num_strings;
    char **strings;
} getarg_strings;

typedef int (*getarg_collect_func)(int short_opt,
				   int argc,
				   char **argv,
				   int *goptind,
				   int *goptarg,
				   void *data);

typedef struct getarg_collect_info {
    getarg_collect_func func;
    void *data;
} getarg_collect_info;

int getarg(struct getargs *args, size_t num_args, 
	   int argc, char **argv, int *goptind);

void arg_printusage (struct getargs *args,
		     size_t num_args,
		     const char *progname,
		     const char *extra_string);

void free_getarg_strings (getarg_strings *);

#endif /* __GETARG_H__ */
