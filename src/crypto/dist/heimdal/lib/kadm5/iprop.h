/*
 * Copyright (c) 1998-2002 Kungliga Tekniska H?gskolan
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

/* $Heimdal: iprop.h,v 1.7 2002/07/04 14:39:19 joda Exp $
   $NetBSD: iprop.h,v 1.1.1.3 2002/09/12 12:41:40 joda Exp $ */

#ifndef __IPROP_H__
#define __IPROP_H__

#include "kadm5_locl.h"
#include <krb5-private.h> /* _krb5_{get,put}_int */
#include <getarg.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_UTIL_H
#include <util.h>
#endif

#define IPROP_VERSION "iprop-0.0"

#define KADM5_SLAVE_ACL HDB_DB_DIR "/slaves"

#define KADM5_SLAVE_STATS HDB_DB_DIR "/slaves-stats"

#define IPROP_NAME "iprop"

#define IPROP_SERVICE "iprop"

#define IPROP_PORT 2121

enum iprop_cmd { I_HAVE = 1,
		 FOR_YOU = 2,
		 TELL_YOU_EVERYTHING = 3,
		 ONE_PRINC = 4,
		 NOW_YOU_HAVE = 5
};

#endif /* __IPROP_H__ */
