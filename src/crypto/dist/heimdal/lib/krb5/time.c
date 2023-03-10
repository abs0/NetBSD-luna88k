/*
 * Copyright (c) 1997 - 2001 Kungliga Tekniska H?gskolan
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

#include "krb5_locl.h"

__RCSID("$Heimdal: time.c,v 1.5 2001/05/02 10:06:11 joda Exp $"
        "$NetBSD: time.c,v 1.1.1.4 2002/09/12 12:41:41 joda Exp $");

/*
 * return ``corrected'' time in `timeret'.
 */

krb5_error_code
krb5_timeofday (krb5_context context,
		krb5_timestamp *timeret)
{
    *timeret = time(NULL) + context->kdc_sec_offset;
    return 0;
}

/*
 * like gettimeofday but with time correction to the KDC
 */

krb5_error_code
krb5_us_timeofday (krb5_context context,
		   int32_t *sec,
		   int32_t *usec)
{
    struct timeval tv;

    gettimeofday (&tv, NULL);

    *sec  = tv.tv_sec + context->kdc_sec_offset;
    *usec = tv.tv_usec;		/* XXX */
    return 0;
}

krb5_error_code
krb5_format_time(krb5_context context, time_t t, 
		 char *s, size_t len, krb5_boolean include_time)
{
    struct tm *tm;
    if(context->log_utc)
	tm = gmtime (&t);
    else
	tm = localtime(&t);
    strftime(s, len, include_time ? context->time_fmt : context->date_fmt, tm);
    return 0;
}

krb5_error_code
krb5_string_to_deltat(const char *string, krb5_deltat *deltat)
{
    if((*deltat = parse_time(string, "s")) == -1)
	return EINVAL;
    return 0;
}
