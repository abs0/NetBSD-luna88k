/* $NetBSD: sm_os_aix.h,v 1.1.1.4 2005/03/15 02:05:52 atatat Exp $ */
/*
 * Copyright (c) 2000-2001, 2003 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	Id: sm_os_aix.h,v 1.11 2003/04/28 23:42:23 ca Exp
 */

/*
**  sm_os_aix.h -- platform definitions for AIX
*/

#define SM_OS_NAME	"aix"

#ifndef SM_CONF_SHM
# define SM_CONF_SHM	1
#endif /* SM_CONF_SHM */
#ifndef SM_CONF_SEM
# define SM_CONF_SEM	2
#endif /* SM_CONF_SEM */
#ifndef SM_CONF_MSG
# define SM_CONF_MSG	1
#endif /* SM_CONF_MSG */

/* AIX 3 doesn't have a prototype for syslog()? */
#ifdef _AIX3
# ifndef _AIX4
#  ifndef SM_CONF_SYSLOG
#   define SM_CONF_SYSLOG	0
#  endif /* SM_CONF_SYSLOG */
# endif /* ! _AIX4 */
#endif /* _AIX3 */

#if _AIX5 >= 50200
# define SM_CONF_LONGLONG	1
#endif /* _AIX5 >= 50200 */
