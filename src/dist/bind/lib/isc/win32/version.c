/*	$NetBSD: version.c,v 1.1.1.1 2004/05/17 23:45:08 christos Exp $	*/

/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: version.c,v 1.2.12.3 2004/03/08 09:05:01 marka Exp */

#include <versions.h>

#include <isc/version.h>

LIBISC_EXTERNAL_DATA const char isc_version[] = VERSION;

LIBISC_EXTERNAL_DATA const unsigned int isc_libinterface = LIBINTERFACE;
LIBISC_EXTERNAL_DATA const unsigned int isc_librevision = LIBREVISION;
LIBISC_EXTERNAL_DATA const unsigned int isc_libage = LIBAGE;
