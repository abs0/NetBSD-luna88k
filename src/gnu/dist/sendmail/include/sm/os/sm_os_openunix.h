/* $NetBSD: sm_os_openunix.h,v 1.1.1.2 2003/06/01 14:01:33 atatat Exp $ */
/*
 * Copyright (c) 2001 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	Id: sm_os_openunix.h,v 1.5 2001/11/11 16:32:00 ca Exp
 */

#define SM_OS_NAME	"openunix"

/* needs alarm(), our sleep() otherwise hangs. */
#define SM_CONF_SETITIMER	0

/* long long seems to work */
#define SM_CONF_LONGLONG	1

/* don't use flock() in mail.local.c */
#define LDA_USE_LOCKF	1

#ifndef SM_CONF_SHM
# define SM_CONF_SHM	1
#endif /* SM_CONF_SHM */
