/*
 * Copyright (c) 1995, 1996, 1997 Kungliga Tekniska H?gskolan
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

#ifdef HAVE_CONFIG_H
#include <config.h>
__RCSID("$Heimdal: putenv.c,v 1.7 2000/03/26 23:08:24 assar Exp $"
        "$NetBSD: putenv.c,v 1.1.1.3 2002/09/12 12:41:42 joda Exp $");
#endif

#include <stdlib.h>

extern char **environ;

/*
 * putenv --
 *	String points to a string of the form name=value.
 *
 *      Makes the value of the environment variable name equal to
 *      value by altering an existing variable or creating a new one.
 */

int
putenv(const char *string)
{
    int i;
    const char *eq = (const char *)strchr(string, '=');
    int len;
    
    if (eq == NULL)
	return 1;
    len = eq - string;

    if(environ == NULL) {
	environ = malloc(sizeof(char*));
	if(environ == NULL)
	    return 1;
	environ[0] = NULL;
    }

    for(i = 0; environ[i] != NULL; i++)
	if(strncmp(string, environ[i], len) == 0) {
	    environ[i] = string;
	    return 0;
	}
    environ = realloc(environ, sizeof(char*) * (i + 2));
    if(environ == NULL)
	return 1;
    environ[i]   = string;
    environ[i+1] = NULL;
    return 0;
}
