/*	$OpenBSD: netfopen.c,v 1.1 2004/01/28 00:27:44 miod Exp $	*/

/*
 * bug routines -- assumes that the necessary sections of memory
 * are preserved.
 */
#include <sys/types.h>
#include <machine/prom.h>

#include "libbug.h"

/* returns 0: success, nonzero: error */
int
mvmeprom_netfopen(arg)
	struct mvmeprom_netfopen *arg;
{
	MVMEPROM_ARG1(arg);
	MVMEPROM_CALL(MVMEPROM_NETFOPEN);
	return (arg->status);
}
