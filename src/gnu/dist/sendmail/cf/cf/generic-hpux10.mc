divert(-1)
#
# Copyright (c) 1998, 1999, 2001 Sendmail, Inc. and its suppliers.
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
#  This is a generic configuration file for HP-UX 10.x.
#  It has support for local and SMTP mail only.  If you want to
#  customize it, copy it to a name appropriate for your environment
#  and do the modifications there.
#

divert(0)dnl
VERSIONID(`Id: generic-hpux10.mc,v 8.13 2001/05/29 17:29:52 ca Exp')
VERSIONID(`$NetBSD: generic-hpux10.mc,v 1.5 2003/06/01 14:06:43 atatat Exp $')
OSTYPE(hpux10)dnl
DOMAIN(generic)dnl
MAILER(local)dnl
MAILER(smtp)dnl
