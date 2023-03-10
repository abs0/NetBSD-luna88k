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
VERSIONID(`Id: nouucp.m4,v 8.13 1999/11/24 18:37:07 ca Exp')
VERSIONID(`$NetBSD: nouucp.m4,v 1.3 2003/06/01 14:06:46 atatat Exp $')
divert(-1)

ifelse(defn(`_ARG_'), `', 
	`errprint(`*** ERROR: missing argument for FEATURE(nouucp):
		use `reject' or `nospecial'. See cf/README.
')define(`_NO_UUCP_', `e')',
	substr(_ARG_,0,1), `r', `define(`_NO_UUCP_', `r')',
	substr(_ARG_,0,1), `n', `define(`_NO_UUCP_', `n')',
	`errprint(`*** ERROR: illegal argument _ARG_ for FEATURE(nouucp)
')
	')
