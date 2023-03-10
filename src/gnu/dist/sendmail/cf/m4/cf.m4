divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1983, 1995 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

#
#  This file is included so that multiple includes of cf.m4 will work
#

# figure out where the CF files live
ifdef(`_CF_DIR_', `',
	`ifelse(__file__, `__file__',
		`define(`_CF_DIR_', `../')',
		`define(`_CF_DIR_',
			substr(__file__, 0, eval(len(__file__) - 8)))')')

divert(0)dnl
ifdef(`OSTYPE', `dnl',
`include(_CF_DIR_`'m4/cfhead.m4)dnl
VERSIONID(`Id: cf.m4,v 8.32 1999/02/07 07:26:14 gshapiro Exp')')
VERSIONID(`$NetBSD: cf.m4,v 1.3 2003/06/01 14:06:48 atatat Exp $')')
