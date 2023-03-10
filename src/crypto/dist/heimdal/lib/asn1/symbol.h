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

/* $Heimdal: symbol.h,v 1.6 2001/09/25 13:39:27 assar Exp $
   $NetBSD: symbol.h,v 1.1.1.3 2002/09/12 12:41:40 joda Exp $ */

#ifndef _SYMBOL_H
#define _SYMBOL_H

enum typetype { TInteger, TOctetString, TBitString, TSequence, TSequenceOf,
		TGeneralizedTime, TGeneralString, TApplication, TType, 
		TUInteger, TEnumerated, TOID };

typedef enum typetype Typetype;

struct type;

struct member {
  char *name;
  char *gen_name;
  int val;
  int optional;
  struct type *type;
  struct member *next, *prev;
};

typedef struct member Member;

struct symbol;

struct type {
  Typetype type;
  int application;
  Member *members;
  struct type *subtype;
  struct symbol *symbol;
};

typedef struct type Type;

struct symbol {
  char *name;
  char *gen_name;
  enum { SUndefined, SConstant, Stype } stype;
  int constant;
  Type *type;
};

typedef struct symbol Symbol;

void initsym (void);
Symbol *addsym (char *);
void output_name (char *);
#endif
