/*
 * Copyright (c) 2000 - 2001 Kungliga Tekniska H?gskolan
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

__RCSID("$Heimdal: appdefault.c,v 1.7 2001/09/16 04:48:55 assar Exp $"
        "$NetBSD: appdefault.c,v 1.5 2002/09/12 13:19:13 joda Exp $");

void
krb5_appdefault_boolean(krb5_context context, const char *appname, 
			krb5_const_realm realm, const char *option,
			krb5_boolean def_val, krb5_boolean *ret_val)
{
    
    if(appname == NULL)
	appname = getprogname();

    def_val = krb5_config_get_bool_default(context, NULL, def_val, 
					   "libdefaults", option, NULL);
    if(realm != NULL)
	def_val = krb5_config_get_bool_default(context, NULL, def_val, 
					       "realms", realm, option, NULL);
	
    def_val = krb5_config_get_bool_default(context, NULL, def_val, 
					   "appdefaults", 
					   option, 
					   NULL);
    if(realm != NULL)
	def_val = krb5_config_get_bool_default(context, NULL, def_val,
					       "appdefaults", 
					       realm, 
					       option, 
					       NULL);
    if(appname != NULL) {
	def_val = krb5_config_get_bool_default(context, NULL, def_val, 
					       "appdefaults", 
					       appname, 
					       option, 
					       NULL);
	if(realm != NULL)
	    def_val = krb5_config_get_bool_default(context, NULL, def_val,
						   "appdefaults", 
						   appname, 
						   realm, 
						   option, 
						   NULL);
    }
    *ret_val = def_val;
}

void
krb5_appdefault_string(krb5_context context, const char *appname, 
		       krb5_const_realm realm, const char *option,
		       const char *def_val, char **ret_val)
{
    if(appname == NULL)
	appname = getprogname();

    def_val = krb5_config_get_string_default(context, NULL, def_val, 
					     "libdefaults", option, NULL);
    if(realm != NULL)
	def_val = krb5_config_get_string_default(context, NULL, def_val, 
						 "realms", realm, option, NULL);

    def_val = krb5_config_get_string_default(context, NULL, def_val, 
					     "appdefaults", 
					     option, 
					     NULL);
    if(realm != NULL)
	def_val = krb5_config_get_string_default(context, NULL, def_val,
						 "appdefaults", 
						 realm, 
						 option, 
						 NULL);
    if(appname != NULL) {
	def_val = krb5_config_get_string_default(context, NULL, def_val, 
						 "appdefaults", 
						 appname, 
						 option, 
						 NULL);
	if(realm != NULL)
	    def_val = krb5_config_get_string_default(context, NULL, def_val,
						     "appdefaults", 
						     appname, 
						     realm, 
						     option, 
						     NULL);
    }
    if(def_val != NULL)
	*ret_val = strdup(def_val);
    else
	*ret_val = NULL;
}

void
krb5_appdefault_time(krb5_context context, const char *appname,
		     krb5_const_realm realm, const char *option,
		     time_t def_val, time_t *ret_val)
{
    time_t t;
    char tstr[32];
    char *val;
    snprintf(tstr, sizeof(tstr), "%ld", (long)def_val);
    krb5_appdefault_string(context, appname, realm, option, tstr, &val);
    t = parse_time (val, NULL);
    free(val);
    *ret_val = t;
}
