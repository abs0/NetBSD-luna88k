/*
 * Copyright (c) 1998 - 2003 Kungliga Tekniska H?gskolan
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

__RCSID("$Heimdal: display_status.c,v 1.9 2003/03/16 17:45:36 lha Exp $"
        "$NetBSD: display_status.c,v 1.4 2003/05/15 21:36:41 lha Exp $");

static char *krb5_error_string;

static char *
calling_error(OM_uint32 v)
{
    static char *msgs[] = {
	NULL,			/* 0 */
	"A required input parameter could not be read.", /*  */
	"A required output parameter could not be written.", /*  */
	"A parameter was malformed"
    };

    v >>= GSS_C_CALLING_ERROR_OFFSET;

    if (v == 0)
	return "";
    else if (v >= sizeof(msgs)/sizeof(*msgs))
	return "unknown calling error";
    else
	return msgs[v];
}

static char *
routine_error(OM_uint32 v)
{
    static char *msgs[] = {
	NULL,			/* 0 */
	"An unsupported mechanism was requested",
	"An invalid name was supplied",
	"A supplied name was of an unsupported type",
	"Incorrect channel bindings were supplied",
	"An invalid status code was supplied",
	"A token had an invalid MIC",
	"No credentials were supplied, "
	"or the credentials were unavailable or inaccessible.",
	"No context has been established",
	"A token was invalid",
	"A credential was invalid",
	"The referenced credentials have expired",
	"The context has expired",
	"Miscellaneous failure (see text)",
	"The quality-of-protection requested could not be provide",
	"The operation is forbidden by local security policy",
	"The operation or option is not available",
	"The requested credential element already exists",
	"The provided name was not a mechanism name.",
    };

    v >>= GSS_C_ROUTINE_ERROR_OFFSET;

    if (v == 0)
	return "";
    else if (v >= sizeof(msgs)/sizeof(*msgs))
	return "unknown routine error";
    else
	return msgs[v];
}

static char *
supplementary_error(OM_uint32 v)
{
    static char *msgs[] = {
	"normal completion",
	"continuation call to routine required",
	"duplicate per-message token detected",
	"timed-out per-message token detected",
	"reordered (early) per-message token detected",
	"skipped predecessor token(s) detected"
    };

    v >>= GSS_C_SUPPLEMENTARY_OFFSET;

    if (v >= sizeof(msgs)/sizeof(*msgs))
	return "unknown routine error";
    else
	return msgs[v];
}

void
gssapi_krb5_set_error_string (void)
{
    krb5_error_string = krb5_get_error_string(gssapi_krb5_context);
}

char *
gssapi_krb5_get_error_string (void)
{
    char *ret = krb5_error_string;
    krb5_error_string = NULL;
    return ret;
}

OM_uint32 gss_display_status
           (OM_uint32		*minor_status,
	    OM_uint32		 status_value,
	    int			 status_type,
	    const gss_OID	 mech_type,
	    OM_uint32		*message_context,
	    gss_buffer_t	 status_string)
{
  char *buf;

  GSSAPI_KRB5_INIT ();

  status_string->length = 0;
  status_string->value = NULL;

  if (gss_oid_equal(mech_type, GSS_C_NO_OID) == 0 &&
      gss_oid_equal(mech_type, GSS_KRB5_MECHANISM) == 0) {
      *minor_status = 0;
      return GSS_C_GSS_CODE;
  }

  if (status_type == GSS_C_GSS_CODE) {
      if (GSS_SUPPLEMENTARY_INFO(status_value))
	  asprintf(&buf, "%s", 
		   supplementary_error(GSS_SUPPLEMENTARY_INFO(status_value)));
      else
	  asprintf (&buf, "%s %s",
		    calling_error(GSS_CALLING_ERROR(status_value)),
		    routine_error(GSS_ROUTINE_ERROR(status_value)));
  } else if (status_type == GSS_C_MECH_CODE) {
      buf = gssapi_krb5_get_error_string ();
      if (buf == NULL) {
	  const char *tmp = krb5_get_err_text (gssapi_krb5_context,
					       status_value);
	  if (tmp == NULL)
	      asprintf(&buf, "unknown mech error-code %u",
		       (unsigned)status_value);
	  else
	      buf = strdup(tmp);
      }
  } else {
      *minor_status = EINVAL;
      return GSS_S_BAD_STATUS;
  }

  if (buf == NULL) {
      *minor_status = ENOMEM;
      return GSS_S_FAILURE;
  }

  *message_context = 0;
  *minor_status = 0;

  status_string->length = strlen(buf);
  status_string->value  = buf;
  
  return GSS_S_COMPLETE;
}
