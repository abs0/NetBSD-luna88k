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
VERSIONID(`Id: nocanonify.m4,v 8.12 1999/08/28 00:42:01 ca Exp')
VERSIONID(`$NetBSD: nocanonify.m4,v 1.3 2003/06/01 14:06:46 atatat Exp $')
divert(-1)

define(`_NO_CANONIFY_', 1)
ifelse(defn(`_ARG_'), `', `',
	strcasecmp(defn(`_ARG_'), `canonify_hosts'), `1',
	`define(`_CANONIFY_HOSTS_', 1)',
	`errprint(`*** ERROR: unknown parameter '"defn(`_ARG_')"` for FEATURE(`nocanonify')
')')
