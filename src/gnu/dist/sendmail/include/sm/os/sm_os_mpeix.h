/* $NetBSD: sm_os_mpeix.h,v 1.1.1.2 2003/06/01 14:01:33 atatat Exp $ */
/*
 * Copyright (c) 2001 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	Id: sm_os_mpeix.h,v 1.2 2001/12/14 00:23:02 ca Exp
 */

/*
**  sm_os_mpeix.h -- platform definitions for HP MPE/iX
*/

#define SM_OS_NAME	"mpeix"

#ifndef SM_CONF_SHM
# define SM_CONF_SHM	1
#endif /* SM_CONF_SHM */

#ifndef SM_CONF_SEM
# define SM_CONF_SEM	2
#endif /* SM_CONF_SEM */

#ifndef SM_CONF_MSG
# define SM_CONF_MSG	1
#endif /* SM_CONF_MSG */

#define SM_CONF_SETITIMER	0

#ifndef SM_CONF_CANT_SETRGID
# define SM_CONF_CANT_SETRGID	1
#endif /* SM_CONF_CANT_SETRGID */
