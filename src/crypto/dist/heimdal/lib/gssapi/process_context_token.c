/*
 * Copyright (c) 2003 Kungliga Tekniska H?gskolan
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

#include "gssapi_locl.h"

__RCSID("$Heimdal: process_context_token.c,v 1.1 2003/03/16 18:19:05 lha Exp $"
        "$NetBSD: process_context_token.c,v 1.1.1.1 2003/05/15 20:28:46 lha Exp $");

OM_uint32 gss_process_context_token (
	OM_uint32          *minor_status,
	const gss_ctx_id_t context_handle,
	const gss_buffer_t token_buffer
    )
{
    OM_uint32 ret = GSS_S_FAILURE;
    gss_buffer_desc empty_buffer;
    gss_qop_t qop_state;

    empty_buffer.length = 0;
    empty_buffer.value = NULL;

    qop_state = GSS_C_QOP_DEFAULT;

    ret = gss_verify_mic_internal(minor_status, context_handle,
				  token_buffer, &empty_buffer,
				  GSS_C_QOP_DEFAULT, "\x01\x02");

    if (ret == GSS_S_COMPLETE)
	ret = gss_delete_sec_context(minor_status,
				     (gss_ctx_id_t *)&context_handle,
				     GSS_C_NO_BUFFER);
    if (ret == GSS_S_COMPLETE)
	*minor_status = 0;

    return ret;
}
