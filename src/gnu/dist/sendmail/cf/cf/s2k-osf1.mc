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
#  This is a Berkeley-specific configuration file for OSF/1.
#  It applies only to the Sequoia 2000 Project at Berkeley,
#  and should not be used elsewhere.   It is provided on the sendmail
#  distribution as a sample only.  To create your own configuration
#  file, create an appropriate domain file in ../domain, change the
#  `DOMAIN' macro below to reference that file, and copy the result
#  to a name of your own choosing.
#

divert(0)dnl
VERSIONID(`Id: s2k-osf1.mc,v 8.13 1999/02/07 07:26:04 gshapiro Exp')
VERSIONID(`$NetBSD: s2k-osf1.mc,v 1.3 2003/06/01 14:06:44 atatat Exp $')
OSTYPE(osf1)dnl
DOMAIN(S2K.Berkeley.EDU)dnl
MAILER(local)dnl
MAILER(smtp)dnl
