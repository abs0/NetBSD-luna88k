divert(-1)
#
# Copyright (c) 1999, 2001 Sendmail, Inc. and its suppliers.
#	All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

divert(0)
VERSIONID(`Id: relay_mail_from.m4,v 8.3 2001/02/06 16:07:12 ca Exp')
VERSIONID(`$NetBSD: relay_mail_from.m4,v 1.4 2003/06/01 14:06:47 atatat Exp $')
divert(-1)

ifdef(`_ACCESS_TABLE_',
	`define(`_RELAY_DB_FROM_', 1)
	ifelse(_ARG_,`domain',`define(`_RELAY_DB_FROM_DOMAIN_', 1)')',
	`errprint(`*** ERROR: FEATURE(`relay_mail_from') requires FEATURE(`access_db')
')')
errprint(`*** WARNING: FEATURE(`relay_mail_from') may cause your system to act as open
	relay.  Use SMTP AUTH or STARTTLS instead.
')
