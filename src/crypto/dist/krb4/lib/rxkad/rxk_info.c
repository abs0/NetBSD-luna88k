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

#include "rxkad_locl.h"

__RCSID("$KTH-KRB: rxk_info.c,v 1.8 1999/12/02 16:58:55 joda Exp $"
      "$NetBSD: rxk_info.c,v 1.1.1.3 2002/09/12 12:22:10 joda Exp $");

int32
rxkad_GetServerInfo(struct rx_connection *con,
		    rxkad_level *level,
		    u_int32 *expiration,
		    char *name,
		    char *instance,
		    char *cell,
		    int32 *kvno)
{
  serv_con_data *cdat = (serv_con_data *) con->securityData;

  if (cdat && cdat->authenticated
      && (FT_ApproxTime() < cdat->expires) /* Use fast approx time */
      && cdat->user)
    {
      if (level)
	*level = cdat->cur_level;
      if (expiration)
	*expiration = cdat->expires;
      if (name)
	strcpy(name, cdat->user->name);
      if (instance)
	strcpy(instance, cdat->user->instance);
      if (cell)
	strcpy(cell, cdat->user->realm);
      if (kvno)
	*kvno = -1;		/* Where do we find this and who needs it? */
      return 0;
    }
  else
    return RXKADNOAUTH;
}
