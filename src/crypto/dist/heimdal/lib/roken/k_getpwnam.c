/*
 * Copyright (c) 1995, 1996, 1997, 1998, 1999 Kungliga Tekniska H?gskolan
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
__RCSID("$Heimdal: k_getpwnam.c,v 1.9 1999/12/02 16:58:47 joda Exp $"
        "$NetBSD: k_getpwnam.c,v 1.1.1.3 2002/09/12 12:41:42 joda Exp $");
#endif /* HAVE_CONFIG_H */

#include "roken.h"
#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

struct passwd *
k_getpwnam (const char *user)
{
     struct passwd *p;

     p = getpwnam (user);
#if defined(HAVE_GETSPNAM) && defined(HAVE_STRUCT_SPWD)
     if(p)
     {
	  struct spwd *spwd;

	  spwd = getspnam (user);
	  if (spwd)
	       p->pw_passwd = spwd->sp_pwdp;
	  endspent ();
     }
#else
     endpwent ();
#endif
     return p;
}
