/*
 * Copyright (c) 2001-2002 Kungliga Tekniska H?gskolan
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

/* $KTH-KRB: roken_rename.h,v 1.6 2002/08/19 15:08:24 joda Exp $
   $NetBSD: roken_rename.h,v 1.1.1.2 2002/09/12 12:22:13 joda Exp $ */

#ifndef __roken_rename_h__
#define __roken_rename_h__

/*
 * Libroken routines that are added libkafs
 */

#define _resolve_debug _kafs_resolve_debug

#define rk_dns_free_data _kafs_dns_free_data
#define rk_dns_lookup _kafs_dns_lookup
#define rk_dns_string_to_type _kafs_dns_string_to_type
#define rk_dns_type_to_string _kafs_dns_type_to_string
#define rk_dns_srv_order _kafs_dns_srv_order

#ifndef HAVE_STRTOK_R
#define strtok_r _kafs_strtok_r
#endif
#ifndef HAVE_STRLCPY
#define strlcpy _kafs_strlcpy
#endif
#ifndef HAVE_STRSEP
#define strsep _kafs_strsep
#endif

#endif /* __roken_rename_h__ */
