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

#
#  This is a Berkeley-specific configuration file for a specific
#  machine in the Computer Science Division at Berkeley, and should
#  not be used elsewhere.   It is provided on the sendmail distribution
#  as a sample only.
#
#  This file is for our mail spool machine.  For a while we were using
#  "root.machinename" instead of "root+machinename", so this is included
#  for back compatibility.
#

divert(0)dnl
VERSIONID(`Id: mailspool.cs.mc,v 8.12 1999/02/07 07:26:04 gshapiro Exp')
VERSIONID(`$NetBSD: mailspool.cs.mc,v 1.3 2003/06/01 14:06:44 atatat Exp $')
OSTYPE(sunos4.1)dnl
DOMAIN(CS.Berkeley.EDU)dnl
MAILER(local)dnl
MAILER(smtp)dnl

LOCAL_CONFIG
CDroot sys-custodian

LOCAL_RULE_3
R$=D . $+		$1 + $2
