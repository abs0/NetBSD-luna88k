divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
#  SCO UnixWare 2.1.2 ostype file
#
#	Contributed by Christopher Durham <chrisdu@SCO.COM> of SCO.
#
divert(0)
VERSIONID(`Id: sco-uw-2.1.m4,v 8.13 1999/04/24 05:37:43 gshapiro Exp')
VERSIONID(`$NetBSD: sco-uw-2.1.m4,v 1.3 2003/06/01 14:06:51 atatat Exp $')

define(`LOCAL_MAILER_PATH', `/usr/bin/rmail')dnl
_DEFIFNOT(`LOCAL_MAILER_FLAGS', `fhCEn9')dnl
define(`LOCAL_SHELL_FLAGS', `ehuP')dnl
define(`UUCP_MAILER_ARGS', `uux - -r -a$g -gmedium $h!rmail ($u)')dnl
define(`LOCAL_MAILER_ARGS',`rmail $u')dnl
define(`confEBINDIR', `/usr/lib')dnl
define(`confTIME_ZONE', `USE_TZ')dnl
