/* $NetBSD: handler.c,v 1.7 2005/03/15 02:14:16 atatat Exp $ */
#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: handler.c,v 1.7 2005/03/15 02:14:16 atatat Exp $");
#endif

/*
 *  Copyright (c) 1999-2003 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 */

#include <sm/gen.h>
SM_RCSID("@(#)Id: handler.c,v 8.36 2003/09/08 21:27:14 yuri Exp")

#include "libmilter.h"


/*
**  HANDLE_SESSION -- Handle a connected session in its own context
**
**	Parameters:
**		ctx -- context structure
**
**	Returns:
**		MI_SUCCESS/MI_FAILURE
*/

int
mi_handle_session(ctx)
	SMFICTX_PTR ctx;
{
	int ret;

	if (ctx == NULL)
		return MI_FAILURE;
	ctx->ctx_id = (sthread_t) sthread_get_id();

	/*
	**  Detach so resources are free when the thread returns.
	**  If we ever "wait" for threads, this call must be removed.
	*/

	if (pthread_detach(ctx->ctx_id) != 0)
		ret = MI_FAILURE;
	else
		ret = mi_engine(ctx);
	if (ValidSocket(ctx->ctx_sd))
	{
		(void) closesocket(ctx->ctx_sd);
		ctx->ctx_sd = INVALID_SOCKET;
	}
	if (ctx->ctx_reply != NULL)
	{
		free(ctx->ctx_reply);
		ctx->ctx_reply = NULL;
	}
	if (ctx->ctx_privdata != NULL)
	{
		smi_log(SMI_LOG_WARN,
			"%s: private data not NULL",
			ctx->ctx_smfi->xxfi_name);
	}
	mi_clr_macros(ctx, 0);
	free(ctx);
	ctx = NULL;
	return ret;
}
