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

divert(0)
VERSIONID(`Id: unknown.m4,v 8.9 1999/02/07 07:26:24 gshapiro Exp')
VERSIONID(`$NetBSD: unknown.m4,v 1.3 2003/06/01 14:06:51 atatat Exp $')
errprint(`*** ERROR: You have not specified a valid operating system type.')
errprint(`	Use the OSTYPE macro to select a valid system type.  This')
errprint(`	is necessary in order to get the proper pathnames and flags')
errprint(`	appropriate for your environment.')
