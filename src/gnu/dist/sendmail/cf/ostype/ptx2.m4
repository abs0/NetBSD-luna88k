divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1994 Eric P. Allman.  All rights reserved.
# Copyright (c) 1994
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

# Support for DYNIX/ptx 2.x.

divert(0)
VERSIONID(`Id: ptx2.m4,v 8.17 1999/04/24 05:37:43 gshapiro Exp')
VERSIONID(`$NetBSD: ptx2.m4,v 1.3 2003/06/01 14:06:51 atatat Exp $')
ifdef(`QUEUE_DIR',, `define(`QUEUE_DIR', /usr/spool/mqueue)')dnl
define(`LOCAL_MAILER_PATH', `/bin/mail')dnl
_DEFIFNOT(`LOCAL_MAILER_FLAGS', `fmn9')dnl
define(`LOCAL_SHELL_FLAGS', `eu')dnl
define(`confEBINDIR', `/usr/lib')dnl
