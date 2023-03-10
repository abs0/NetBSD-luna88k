divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1983 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
#	This ostype file is suitable for use on Solaris 2.x systems that
#	use mail as local mailer which are usually version before 2.5.
#


divert(0)
VERSIONID(`Id: solaris2.pre5.m4,v 8.1 1999/09/25 08:17:44 ca Exp')
VERSIONID(`$NetBSD: solaris2.pre5.m4,v 1.4 2003/06/01 14:06:51 atatat Exp $')
divert(-1)

_DEFIFNOT(`LOCAL_MAILER_FLAGS', `SnE9')
ifdef(`LOCAL_MAILER_ARGS',, `define(`LOCAL_MAILER_ARGS', `mail -f $g -d $u')')
ifdef(`UUCP_MAILER_ARGS',, `define(`UUCP_MAILER_ARGS', `uux - -r -a$g $h!rmail ($u)')')
define(`confEBINDIR', `/usr/lib')dnl
