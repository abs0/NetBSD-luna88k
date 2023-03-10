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
#  This is a generic configuration file for SunOS 4.1.x.
#  It has support for local and SMTP mail only.  If you want to
#  customize it, copy it to a name appropriate for your environment
#  and do the modifications there.
#

divert(0)dnl
VERSIONID(`Id: generic-sunos4.1.mc,v 8.11 1999/02/07 07:26:03 gshapiro Exp')
VERSIONID(`$NetBSD: generic-sunos4.1.mc,v 1.3 2003/06/01 14:06:44 atatat Exp $')
OSTYPE(sunos4.1)dnl
DOMAIN(generic)dnl
MAILER(local)dnl
MAILER(smtp)dnl
