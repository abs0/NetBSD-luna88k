/*
 * Copyright (c) 1997, 1998 Kungliga Tekniska H?gskolan
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
__RCSID("$Heimdal: parse_time.c,v 1.6 2003/03/07 15:51:06 lha Exp $"
        "$NetBSD: parse_time.c,v 1.1.1.4 2003/05/15 20:28:49 lha Exp $");
#endif

#include <parse_units.h>
#include "parse_time.h"

static struct units time_units[] = {
    {"year",	365 * 24 * 60 * 60},
    {"month",	30 * 24 * 60 * 60},
    {"week",	7 * 24 * 60 * 60},
    {"day",	24 * 60 * 60},
    {"hour",	60 * 60},
    {"h",	60 * 60},
    {"minute",	60},
    {"m",	60},
    {"second",	1},
    {"s",	1},
    {NULL, 0},
};

int
parse_time (const char *s, const char *def_unit)
{
    return parse_units (s, time_units, def_unit);
}

size_t
unparse_time (int t, char *s, size_t len)
{
    return unparse_units (t, time_units, s, len);
}

size_t
unparse_time_approx (int t, char *s, size_t len)
{
    return unparse_units_approx (t, time_units, s, len);
}

void
print_time_table (FILE *f)
{
    print_units_table (time_units, f);
}
