/* $NetBSD: t-fopen.c,v 1.1.1.2 2003/06/01 14:01:38 atatat Exp $ */
#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: t-fopen.c,v 1.1.1.2 2003/06/01 14:01:38 atatat Exp $");
#endif

/*
 * Copyright (c) 2000-2002 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 */

#include <sm/gen.h>
SM_IDSTR(id, "@(#)Id: t-fopen.c,v 1.9 2002/02/06 23:57:45 ca Exp")

#include <fcntl.h>
#include <sm/io.h>
#include <sm/test.h>

/* ARGSUSED0 */
int
main(argc, argv)
	int argc;
	char *argv[];
{
	int m, r;
	SM_FILE_T *out;

	sm_test_begin(argc, argv, "test sm_io_fopen");
	out = sm_io_fopen("foo", O_WRONLY|O_APPEND|O_CREAT, 0666);
	SM_TEST(out != NULL);
	if (out != NULL)
	{
		(void) sm_io_fprintf(out, SM_TIME_DEFAULT, "foo\n");
		r = sm_io_getinfo(out, SM_IO_WHAT_MODE, &m);
		SM_TEST(r == 0);
		SM_TEST(m == SM_IO_WRONLY);
		sm_io_close(out, SM_TIME_DEFAULT);
	}
	return sm_test_end();
}
