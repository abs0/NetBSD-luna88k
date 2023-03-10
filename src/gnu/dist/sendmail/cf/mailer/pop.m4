PUSHDIVERT(-1)
#
# Copyright (c) 1998-2001 Sendmail, Inc. and its suppliers.
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

ifdef(`POP_MAILER_PATH',, `define(`POP_MAILER_PATH', /usr/lib/mh/spop)')
_DEFIFNOT(`POP_MAILER_FLAGS', `Penu')
ifdef(`POP_MAILER_ARGS',, `define(`POP_MAILER_ARGS', `pop $u')')
define(`_POP_QGRP', `ifelse(defn(`POP_MAILER_QGRP'),`',`', ` Q=POP_MAILER_QGRP,')')dnl

POPDIVERT

####################################
###   POP Mailer specification   ###
####################################

VERSIONID(`Id: pop.m4,v 8.22 2001/11/12 23:11:34 ca Exp')
VERSIONID(`$NetBSD: pop.m4,v 1.4 2003/06/01 14:06:49 atatat Exp $')

Mpop,		P=POP_MAILER_PATH, F=_MODMF_(CONCAT(`lsDFMq', POP_MAILER_FLAGS), `POP'), S=EnvFromL, R=EnvToL/HdrToL,
		T=DNS/RFC822/X-Unix,_POP_QGRP
		A=POP_MAILER_ARGS

LOCAL_CONFIG
# POP mailer is a pseudo-domain
CPPOP
