/*
 * Copyright (c) 1998 - 2002 Kungliga Tekniska H?gskolan
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

/* $KTH-KRB: roken_rename.h,v 1.13 2002/08/21 13:32:06 joda Exp $
   $NetBSD: roken_rename.h,v 1.1.1.4 2002/09/12 12:22:10 joda Exp $ */

#ifndef __roken_rename_h__
#define __roken_rename_h__

/*
 * Libroken routines that are added libkrb
 */

#define base64_decode _krb_base64_decode
#define base64_encode _krb_base64_encode

#define net_write _krb_net_write
#define net_read  _krb_net_read
#define _resolve_debug _krb_resolve_debug

#ifndef HAVE_FLOCK
#define flock _krb_flock
#endif
#ifndef HAVE_GETHOSTNAME
#define gethostname _krb_gethostname
#endif
#ifndef HAVE_GETTIMEOFDAY
#define gettimeofday _krb_gettimeofday
#endif
#ifndef HAVE_GETUID
#define getuid _krb_getuid
#endif
#ifndef HAVE_SNPRINTF
#define snprintf _krb_snprintf
#endif
#ifndef HAVE_ASPRINTF
#define asprintf _krb_asprintf
#endif
#ifndef HAVE_ASNPRINTF
#define asnprintf _krb_asnprintf
#endif
#ifndef HAVE_VASPRINTF
#define vasprintf _krb_vasprintf
#endif
#ifndef HAVE_VASNPRINTF
#define vasnprintf _krb_vasnprintf
#endif
#ifndef HAVE_VSNPRINTF
#define vsnprintf _krb_vsnprintf
#endif
#ifndef HAVE_STRCASECMP
#define strcasecmp _krb_strcasecmp
#endif
#ifndef HAVE_STRNCASECMP
#define strncasecmp _krb_strncasecmp
#endif
#ifndef HAVE_STRDUP
#define strdup _krb_strdup
#endif
#ifndef HAVE_STRLCAT
#define strlcat _krb_strlcat
#endif
#ifndef HAVE_STRLCPY
#define strlcpy _krb_strlcpy
#endif
#ifndef HAVE_STRNLEN
#define strnlen _krb_strnlen
#endif
#ifndef HAVE_SWAB
#define swab _krb_swab
#endif
#ifndef HAVE_STRTOK_R
#define strtok_r _krb_strtok_r
#endif

#define rk_dns_free_data _krb_dns_free_data
#define rk_dns_lookup _krb_dns_lookup
#define rk_dns_string_to_type _krb_dns_string_to_type
#define rk_dns_type_to_string _krb_dns_type_to_string
#define rk_dns_srv_order _krb_dns_srv_order

#endif /* __roken_rename_h__ */
