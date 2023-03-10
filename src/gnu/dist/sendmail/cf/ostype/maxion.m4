divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1996 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
#	Concurrent Computer Corporation Maxion system support contributed
#	by Donald R. Laster Jr. <Laster@access.digex.com>.
#

divert(0)
VERSIONID(`Id: maxion.m4,v 8.17 1999/10/21 00:31:39 gshapiro Exp')
VERSIONID(`$NetBSD: maxion.m4,v 1.3 2003/06/01 14:06:50 atatat Exp $')

define(`QUEUE_DIR',         `/var/spool/mqueue')dnl
define(`STATUS_FILE',       `/var/adm/log/sendmail.st')dnl
define(`LOCAL_MAILER_PATH', `/usr/bin/mail')dnl
define(`LOCAL_SHELL_FLAGS', `ehuP')dnl
define(`LOCAL_MAILER_ARGS', `mail $u')dnl
define(`UUCP_MAILER_ARGS',  `uux - -r -a$g -gmedium $h!rmail ($u)')dnl
define(`confEBINDIR',	    `/usr/ucblib')dnl
divert(-1)
