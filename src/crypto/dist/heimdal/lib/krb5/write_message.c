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

__RCSID("$Heimdal: write_message.c,v 1.8 2001/07/02 18:43:06 joda Exp $"
        "$NetBSD: write_message.c,v 1.1.1.5 2002/09/12 12:41:41 joda Exp $");

krb5_error_code
krb5_write_message (krb5_context context,
		    krb5_pointer p_fd,
		    krb5_data *data)
{
    u_int32_t len;
    u_int8_t buf[4];
    int ret;

    len = data->length;
    _krb5_put_int(buf, len, 4);
    if (krb5_net_write (context, p_fd, buf, 4) != 4
	|| krb5_net_write (context, p_fd, data->data, len) != len) {
	ret = errno;
	krb5_set_error_string (context, "write: %s", strerror(ret));
	return ret;
    }
    return 0;
}

krb5_error_code
krb5_write_priv_message(krb5_context context,
			krb5_auth_context ac,
			krb5_pointer p_fd,
			krb5_data *data)
{
    krb5_error_code ret;
    krb5_data packet;

    ret = krb5_mk_priv (context, ac, data, &packet, NULL);
    if(ret)
	return ret;
    ret = krb5_write_message(context, p_fd, &packet);
    krb5_data_free(&packet);
    return ret;
}

krb5_error_code
krb5_write_safe_message(krb5_context context,
			krb5_auth_context ac,
			krb5_pointer p_fd,
			krb5_data *data)
{
    krb5_error_code ret;
    krb5_data packet;
    ret = krb5_mk_safe (context, ac, data, &packet, NULL);
    if(ret)
	return ret;
    ret = krb5_write_message(context, p_fd, &packet);
    krb5_data_free(&packet);
    return ret;
}
