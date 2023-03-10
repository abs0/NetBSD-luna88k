/*
 * Copyright (c) 1995 - 2000 Kungliga Tekniska H?gskolan
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

/* $KTH-KRB: sl.h,v 1.9 2001/01/26 14:58:41 joda Exp $
   $NetBSD: sl.h,v 1.1.1.4 2002/09/12 12:22:13 joda Exp $ */

#ifndef _SL_H
#define _SL_H

#define SL_BADCOMMAND -1

typedef int (*cmd_func)(int, char **);

struct sl_cmd {
  char *name;
  cmd_func func;
  char *usage;
  char *help;
};

typedef struct sl_cmd SL_cmd;

void sl_help (SL_cmd *, int argc, char **argv);
int  sl_loop (SL_cmd *, const char *prompt);
int  sl_command_loop (SL_cmd *cmds, const char *prompt, void **data);
int  sl_command (SL_cmd *cmds, int argc, char **argv);
int sl_make_argv(char*, int*, char***);
void sl_apropos (SL_cmd *cmd, const char *topic);


#endif /* _SL_H */
