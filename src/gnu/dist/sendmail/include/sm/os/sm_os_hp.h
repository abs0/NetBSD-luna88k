/* $NetBSD: sm_os_hp.h,v 1.1.1.2 2003/06/01 14:01:33 atatat Exp $ */
/*
 * Copyright (c) 2000-2001 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 *	Id: sm_os_hp.h,v 1.8 2001/10/31 15:36:56 ca Exp
 */

/*
**  sm_os_hp.h -- platform definitions for HP
*/

#define SM_OS_NAME	"hp"

#ifndef SM_CONF_SHM
# define SM_CONF_SHM	1
#endif /* SM_CONF_SHM */
#ifndef SM_CONF_SEM
# define SM_CONF_SEM	2
#endif /* SM_CONF_SEM */
#ifndef SM_CONF_MSG
# define SM_CONF_MSG	1
#endif /* SM_CONF_MSG */

/* max/min buffer size of other than regular files */
#ifndef SM_IO_MAX_BUF
# define SM_IO_MAX_BUF	8192
#endif /* SM_IO_MAX_BUF */
#ifndef SM_IO_MIN_BUF
# define SM_IO_MIN_BUF	4096
#endif /* SM_IO_MIN_BUF */
