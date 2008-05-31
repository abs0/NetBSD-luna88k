/*	$NetBSD: rthdr.c,v 1.14 2003/06/06 08:13:45 itojun Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: rthdr.c,v 1.14 2003/06/06 08:13:45 itojun Exp $");
#endif /* LIBC_SCCS and not lint */

#include "namespace.h"
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip6.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef __weak_alias
__weak_alias(inet6_rthdr_add,_inet6_rthdr_add)
__weak_alias(inet6_rthdr_getaddr,_inet6_rthdr_getaddr)
__weak_alias(inet6_rthdr_getflags,_inet6_rthdr_getflags)
__weak_alias(inet6_rthdr_init,_inet6_rthdr_init)
__weak_alias(inet6_rthdr_lasthop,_inet6_rthdr_lasthop)
__weak_alias(inet6_rthdr_segments,_inet6_rthdr_segments)
__weak_alias(inet6_rthdr_space,_inet6_rthdr_space)
#endif

size_t
inet6_rthdr_space(type, seg)
	int type, seg;
{
	switch (type) {
	case IPV6_RTHDR_TYPE_0:
		if (seg < 1 || seg > 23)
			return (0);
		return (CMSG_SPACE(sizeof(struct in6_addr) * seg +
		    sizeof(struct ip6_rthdr0)));
	default:
		return (0);
	}
}

struct cmsghdr *
inet6_rthdr_init(bp, type)
	void *bp;
	int type;
{
	struct cmsghdr *ch;
	struct ip6_rthdr *rthdr;

	_DIAGASSERT(bp != NULL);

	ch = (struct cmsghdr *)bp;
	rthdr = (struct ip6_rthdr *)(void *)CMSG_DATA(ch);

	ch->cmsg_level = IPPROTO_IPV6;
	ch->cmsg_type = IPV6_RTHDR;

	switch (type) {
	case IPV6_RTHDR_TYPE_0:
		ch->cmsg_len = CMSG_LEN(sizeof(struct ip6_rthdr0));
		(void)memset(rthdr, 0, sizeof(struct ip6_rthdr0));
		rthdr->ip6r_type = IPV6_RTHDR_TYPE_0;
		return (ch);
	default:
		return (NULL);
	}
}

int
inet6_rthdr_add(cmsg, addr, flags)
	struct cmsghdr *cmsg;
	const struct in6_addr *addr;
	u_int flags;
{
	struct ip6_rthdr *rthdr;

	_DIAGASSERT(cmsg != NULL);
	_DIAGASSERT(addr != NULL);

	rthdr = (struct ip6_rthdr *)(void *)CMSG_DATA(cmsg);

	switch (rthdr->ip6r_type) {
	case IPV6_RTHDR_TYPE_0:
	{
		struct ip6_rthdr0 *rt0 = (struct ip6_rthdr0 *)(void *)rthdr;
		if (flags != IPV6_RTHDR_LOOSE)
			return (-1);
		if (rt0->ip6r0_segleft == 23)
			return (-1);
		rt0->ip6r0_segleft++;
		(void)memcpy(((caddr_t)(void *)rt0) +
		    ((rt0->ip6r0_len + 1) << 3), addr, sizeof(struct in6_addr));
		rt0->ip6r0_len += sizeof(struct in6_addr) >> 3;
		cmsg->cmsg_len = CMSG_LEN((rt0->ip6r0_len + 1) << 3);
		break;
	}
	default:
		return (-1);
	}

	return (0);
}

int
inet6_rthdr_lasthop(cmsg, flags)
	struct cmsghdr *cmsg;
	unsigned int flags;
{
	struct ip6_rthdr *rthdr;

	_DIAGASSERT(cmsg != NULL);

	rthdr = (struct ip6_rthdr *)(void *)CMSG_DATA(cmsg);

	switch (rthdr->ip6r_type) {
	case IPV6_RTHDR_TYPE_0:
	{
		struct ip6_rthdr0 *rt0 = (struct ip6_rthdr0 *)(void *)rthdr;
		if (flags != IPV6_RTHDR_LOOSE)
			return (-1);
		if (rt0->ip6r0_segleft > 23)
			return (-1);
		break;
	}
	default:
		return (-1);
	}

	return (0);
}

#if 0
int
inet6_rthdr_reverse(in, out)
	const struct cmsghdr *in;
	struct cmsghdr *out;
{

	return (-1);
}
#endif

int
inet6_rthdr_segments(cmsg)
	const struct cmsghdr *cmsg;
{
	const struct ip6_rthdr *rthdr;

	_DIAGASSERT(cmsg != NULL);

	/*LINTED const castaway*/
	rthdr = (const struct ip6_rthdr *)(const void *)CMSG_DATA(cmsg);

	switch (rthdr->ip6r_type) {
	case IPV6_RTHDR_TYPE_0:
	{
		const struct ip6_rthdr0 *rt0 =
		    (const struct ip6_rthdr0 *)(const void *)rthdr;

		if (rt0->ip6r0_len % 2 || 46 < rt0->ip6r0_len)
			return (-1);

		return (rt0->ip6r0_len * 8) / sizeof(struct in6_addr);
	}

	default:
		return (-1);
	}
}

struct in6_addr *
inet6_rthdr_getaddr(cmsg, idx)
	struct cmsghdr *cmsg;
	int idx;
{
	struct ip6_rthdr *rthdr;

	_DIAGASSERT(cmsg != NULL);

	rthdr = (struct ip6_rthdr *)(void *)CMSG_DATA(cmsg);

	switch (rthdr->ip6r_type) {
	case IPV6_RTHDR_TYPE_0:
	{
		struct ip6_rthdr0 *rt0 = (struct ip6_rthdr0 *)(void *)rthdr;
		int naddr;

		if (rt0->ip6r0_len % 2 || 46 < rt0->ip6r0_len)
			return NULL;
		naddr = (rt0->ip6r0_len * 8) / sizeof(struct in6_addr);
		if (idx <= 0 || naddr < idx)
			return NULL;
		return ((struct in6_addr *)(void *)(rt0 + 1)) + idx;
	}

	default:
		return NULL;
	}
}

int
inet6_rthdr_getflags(cmsg, idx)
	const struct cmsghdr *cmsg;
	int idx;
{
	const struct ip6_rthdr *rthdr;

	_DIAGASSERT(cmsg != NULL);

	/*LINTED const castaway*/
	rthdr = (const struct ip6_rthdr *)(const void *)CMSG_DATA(cmsg);

	switch (rthdr->ip6r_type) {
	case IPV6_RTHDR_TYPE_0:
	{
		const struct ip6_rthdr0 *rt0 = (const struct ip6_rthdr0 *)
		(const void *)rthdr;
		int naddr;

		if (rt0->ip6r0_len % 2 || 46 < rt0->ip6r0_len)
			return (-1);
		naddr = (rt0->ip6r0_len * 8) / sizeof(struct in6_addr);
		if (idx < 0 || naddr < idx)
			return (-1);
		return IPV6_RTHDR_LOOSE;
	}

	default:
		return (-1);
	}
}
