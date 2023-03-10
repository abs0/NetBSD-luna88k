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

__RCSID("$Heimdal: get_default_realm.c,v 1.10 2001/07/19 16:55:27 assar Exp $"
        "$NetBSD: get_default_realm.c,v 1.2 2002/12/21 13:22:20 wiz Exp $");

/*
 * Return a NULL-terminated list of default realms in `realms'.
 * Free this memory with krb5_free_host_realm.
 */

krb5_error_code
krb5_get_default_realms (krb5_context context,
			 krb5_realm **realms)
{
    if (context->default_realms == NULL) {
	krb5_error_code ret = krb5_set_default_realm (context, NULL);
	if (ret)
	    return KRB5_CONFIG_NODEFREALM;
    }

    return krb5_copy_host_realm (context,
				 context->default_realms,
				 realms);
}

/*
 * Return the first default realm.  For compatibility.
 */

krb5_error_code
krb5_get_default_realm(krb5_context context,
		       krb5_realm *realm)
{
    char *res;

    if (context->default_realms == NULL
	|| context->default_realms[0] == NULL) {
	krb5_error_code ret = krb5_set_default_realm (context, NULL);
	if (ret) {
	    krb5_set_error_string(context, "no default realm configured");
	    return KRB5_CONFIG_NODEFREALM;
	}
    }

    res = strdup (context->default_realms[0]);
    if (res == NULL) {
	krb5_set_error_string(context, "malloc: out of memory");
	return ENOMEM;
    }
    *realm = res;
    return 0;
}
