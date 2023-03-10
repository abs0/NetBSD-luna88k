/*
 * Copyright (c) 1997 - 2000 Kungliga Tekniska H?gskolan
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

#include "test_locl.h"
#include <gssapi.h>
#include "gss_common.h"
__RCSID("$Heimdal: gssapi_client.c,v 1.16 2000/08/09 20:53:06 assar Exp $"
        "$NetBSD: gssapi_client.c,v 1.1.1.4 2002/09/12 12:41:33 joda Exp $");

static int
do_trans (int sock, gss_ctx_id_t context_hdl)
{
    OM_uint32 maj_stat, min_stat;
    gss_buffer_desc real_input_token, real_output_token;
    gss_buffer_t input_token = &real_input_token,
	output_token = &real_output_token;

    /* get_mic */

    input_token->length = 3;
    input_token->value  = strdup("hej");

    maj_stat = gss_get_mic(&min_stat,
			   context_hdl,
			   GSS_C_QOP_DEFAULT,
			   input_token,
			   output_token);
    if (GSS_ERROR(maj_stat))
	gss_err (1, min_stat, "gss_get_mic");

    write_token (sock, input_token);
    write_token (sock, output_token);

    /* wrap */

    input_token->length = 7;
    input_token->value  = "hemligt";


    maj_stat = gss_wrap (&min_stat,
			 context_hdl,
			 1,
			 GSS_C_QOP_DEFAULT,
			 input_token,
			 NULL,
			 output_token);
    if (GSS_ERROR(maj_stat))
	gss_err (1, min_stat, "gss_wrap");

    write_token (sock, output_token);

    return 0;
}

static int
proto (int sock, const char *hostname, const char *service)
{
    struct sockaddr_in remote, local;
    socklen_t addrlen;

    int context_established = 0;
    gss_ctx_id_t context_hdl = GSS_C_NO_CONTEXT;
    gss_buffer_desc real_input_token, real_output_token;
    gss_buffer_t input_token = &real_input_token,
	output_token = &real_output_token;
    OM_uint32 maj_stat, min_stat;
    gss_name_t server;
    gss_buffer_desc name_token;
    struct gss_channel_bindings_struct input_chan_bindings;
    u_char init_buf[4];
    u_char acct_buf[4];

    name_token.length = asprintf ((char **)&name_token.value,
				  "%s@%s", service, hostname);

    maj_stat = gss_import_name (&min_stat,
				&name_token,
				GSS_C_NT_HOSTBASED_SERVICE,
				&server);
    if (GSS_ERROR(maj_stat))
	gss_err (1, min_stat,
		 "Error importing name `%s@%s':\n", service, hostname);

    addrlen = sizeof(local);
    if (getsockname (sock, (struct sockaddr *)&local, &addrlen) < 0
	|| addrlen != sizeof(local))
	err (1, "getsockname(%s)", hostname);

    addrlen = sizeof(remote);
    if (getpeername (sock, (struct sockaddr *)&remote, &addrlen) < 0
	|| addrlen != sizeof(remote))
	err (1, "getpeername(%s)", hostname);

    input_token->length = 0;
    output_token->length = 0;

    input_chan_bindings.initiator_addrtype = GSS_C_AF_INET;
    input_chan_bindings.initiator_address.length = 4;
    init_buf[0] = (local.sin_addr.s_addr >> 24) & 0xFF;
    init_buf[1] = (local.sin_addr.s_addr >> 16) & 0xFF;
    init_buf[2] = (local.sin_addr.s_addr >>  8) & 0xFF;
    init_buf[3] = (local.sin_addr.s_addr >>  0) & 0xFF;
    input_chan_bindings.initiator_address.value = init_buf;

    input_chan_bindings.acceptor_addrtype = GSS_C_AF_INET;
    input_chan_bindings.acceptor_address.length = 4;
    acct_buf[0] = (remote.sin_addr.s_addr >> 24) & 0xFF;
    acct_buf[1] = (remote.sin_addr.s_addr >> 16) & 0xFF;
    acct_buf[2] = (remote.sin_addr.s_addr >>  8) & 0xFF;
    acct_buf[3] = (remote.sin_addr.s_addr >>  0) & 0xFF;
    input_chan_bindings.acceptor_address.value = acct_buf;
    
#if 0
    input_chan_bindings.application_data.value = emalloc(4);
    * (unsigned short*)input_chan_bindings.application_data.value = local.sin_port;
    * ((unsigned short *)input_chan_bindings.application_data.value + 1) = remote.sin_port;
    input_chan_bindings.application_data.length = 4;
#else
    input_chan_bindings.application_data.length = 0;
    input_chan_bindings.application_data.value = NULL;
#endif

    while(!context_established) {
	maj_stat =
	    gss_init_sec_context(&min_stat,
				 GSS_C_NO_CREDENTIAL,
				 &context_hdl,
				 server,
				 GSS_C_NO_OID,
				 GSS_C_MUTUAL_FLAG | GSS_C_SEQUENCE_FLAG
				 | GSS_C_DELEG_FLAG,
				 0,
				 &input_chan_bindings,
				 input_token,
				 NULL,
				 output_token,
				 NULL,
				 NULL);
	if (GSS_ERROR(maj_stat))
	    gss_err (1, min_stat, "gss_init_sec_context");
	if (output_token->length != 0)
	    write_token (sock, output_token);
	if (GSS_ERROR(maj_stat)) {
	    if (context_hdl != GSS_C_NO_CONTEXT)
		gss_delete_sec_context (&min_stat,
					&context_hdl,
					GSS_C_NO_BUFFER);
	    break;
	}
	if (maj_stat & GSS_S_CONTINUE_NEEDED) {
	    read_token (sock, input_token);
	} else {
	    context_established = 1;
	}

    }
    if (fork_flag) {
	pid_t pid;
	int pipefd[2];

	if (pipe (pipefd) < 0)
	    err (1, "pipe");

	pid = fork ();
	if (pid < 0)
	    err (1, "fork");
	if (pid != 0) {
	    gss_buffer_desc buf;

	    maj_stat = gss_export_sec_context (&min_stat,
					       &context_hdl,
					       &buf);
	    if (GSS_ERROR(maj_stat))
		gss_err (1, min_stat, "gss_export_sec_context");
	    write_token (pipefd[1], &buf);
	    exit (0);
	} else {
	    gss_ctx_id_t context_hdl;
	    gss_buffer_desc buf;

	    close (pipefd[1]);
	    read_token (pipefd[0], &buf);
	    close (pipefd[0]);
	    maj_stat = gss_import_sec_context (&min_stat, &buf, &context_hdl);
	    if (GSS_ERROR(maj_stat))
		gss_err (1, min_stat, "gss_import_sec_context");
	    gss_release_buffer (&min_stat, &buf);
	    return do_trans (sock, context_hdl);
	}
    } else {
	return do_trans (sock, context_hdl);
    }
}

int
main(int argc, char **argv)
{
    krb5_context context; /* XXX */
    int port = client_setup(&context, &argc, argv);
    return client_doit (argv[argc], port, service, proto);
}
