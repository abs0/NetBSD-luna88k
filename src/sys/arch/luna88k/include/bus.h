/*	$OpenBSD: bus.h,v 1.3 2005/04/04 13:09:52 aoyama Exp $	*/
/*	$NetBSD: bus.h,v 1.9 1998/01/13 18:32:15 scottr Exp $	*/

/*-
 * Copyright (c) 1996, 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 1997 Scott Reynolds.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LUNA88K_BUS_H_
#define _LUNA88K_BUS_H_

/*
 * Bus address and size types
 */
typedef u_long bus_addr_t;
typedef u_long bus_size_t;

/*
 * Access methods for bus resources and address space.
 */
typedef u_long bus_space_handle_t;
typedef struct luna88k_bus_space_tag *bus_space_tag_t;

struct luna88k_bus_space_tag {
	/* 'stride' for each bytes reading/writing */
	uint8_t	bs_stride_1;
	uint8_t	bs_stride_2;
	uint8_t bs_stride_4;
	uint8_t	bs_stride_8;
};

/*
 *	int bus_space_map(bus_space_tag_t t, bus_addr_t addr,
 *	    bus_size_t size, int flags, bus_space_handle_t *bshp);
 *
 * Map a region of bus space.
 */

#define	BUS_SPACE_MAP_CACHEABLE		0x01
#define	BUS_SPACE_MAP_LINEAR		0x02

int	bus_space_map(bus_space_tag_t, bus_addr_t, bus_size_t,
	    int, bus_space_handle_t *);

/*
 *	void bus_space_unmap(bus_space_tag_t t,
 *	    bus_space_handle_t bsh, bus_size_t size);
 *
 * Unmap a region of bus space.
 */

void	bus_space_unmap(bus_space_tag_t, bus_space_handle_t, bus_size_t);

/*
 *	int bus_space_subregion(bus_space_tag_t t,
 *	    bus_space_handle_t bsh, bus_size_t offset, bus_size_t size,
 *	    bus_space_handle_t *nbshp);
 *
 * Get a new handle for a subregion of an already-mapped area of bus space.
 */

int	bus_space_subregion(bus_space_tag_t t, bus_space_handle_t bsh,
	    bus_size_t offset, bus_size_t size, bus_space_handle_t *nbshp);

/*
 *	int bus_space_alloc(bus_space_tag_t t, bus_addr_t, rstart,
 *	    bus_addr_t rend, bus_size_t size, bus_size_t align,
 *	    bus_size_t boundary, int flags, bus_addr_t *addrp,
 *	    bus_space_handle_t *bshp);
 *
 * Allocate a region of bus space.
 */

int	bus_space_alloc(bus_space_tag_t t, bus_addr_t rstart,
	    bus_addr_t rend, bus_size_t size, bus_size_t align,
	    bus_size_t boundary, int cacheable, bus_addr_t *addrp,
	    bus_space_handle_t *bshp);

/*
 *	int bus_space_free(bus_space_tag_t t,
 *	    bus_space_handle_t bsh, bus_size_t size);
 *
 * Free a region of bus space.
 */

void	bus_space_free(bus_space_tag_t t, bus_space_handle_t bsh,
	    bus_size_t size);

/*
 *	int luna88k_bus_space_probe(bus_space_tag_t t,
 *	    bus_space_handle_t bsh, bus_size_t offset, int sz);
 *
 * Probe the bus at t/bsh/offset, using sz as the size of the load.
 *
 * This is a machine-dependent extension, and is not to be used by
 * machine-independent code.
 */

int	luna88k_bus_space_probe(bus_space_tag_t t,
	    bus_space_handle_t bsh, bus_size_t offset, int sz);

/*
 *	u_intN_t bus_space_read_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset);
 *
 * Read a 1, 2, 4, or 8 byte quantity from bus space
 * described by tag/handle/offset.
 */

#define	bus_space_read_1(t, h, o)					\
    (*(volatile u_int8_t *)((h) + (t->bs_stride_1) * (o)))

#define	bus_space_read_2(t, h, o)					\
    (*(volatile u_int16_t *)((h) + (t->bs_stride_2) * (o)))

#define	bus_space_read_4(t, h, o)					\
    (*(volatile u_int32_t *)((h) + (t->bs_stride_4) * (o)))

#if 0	/* Cause a link error for bus_space_read_8 */
#define	bus_space_read_8(t, h, o)	!!! bus_space_read_8 unimplemented !!!
#endif

/*
 *	void bus_space_read_multi_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    u_intN_t *addr, size_t count);
 *
 * Read `count' 1, 2, 4, or 8 byte quantities from bus space
 * described by tag/handle/offset and copy into buffer provided.
 */

static __inline__ void
bus_space_read_multi_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t *dest, size_t count)
{
	while ((int)--count >= 0)
		*dest++ = bus_space_read_1(tag, handle, offset);
}

static __inline__ void
bus_space_read_multi_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t *dest, size_t count)
{
	while ((int)--count >= 0)
		*dest++ = bus_space_read_2(tag, handle, offset);
}

static __inline__ void
bus_space_read_multi_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t *dest, size_t count)
{
	while ((int)--count >= 0)
		*dest++ = bus_space_read_4(tag, handle, offset);
}

#if 0	/* Cause a link error for bus_space_read_multi_8 */
#define	bus_space_read_multi_8	!!! bus_space_read_multi_8 unimplemented !!!
#endif

/*
 *	void bus_space_read_region_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    u_intN_t *addr, size_t count);
 *
 * Read `count' 1, 2, 4, or 8 byte quantities from bus space
 * described by tag/handle and starting at `offset' and copy into
 * buffer provided.
 */

static __inline__ void
bus_space_read_region_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t *dest, size_t count)
{
	while ((int)--count >= 0)
		*dest++ = bus_space_read_1(tag, handle, offset++);
}

static __inline__ void
bus_space_read_region_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t *dest, size_t count)
{
	while ((int)--count >= 0) {
		*dest++ = bus_space_read_2(tag, handle, offset);
		offset += 2;
	}
}

static __inline__ void
bus_space_read_region_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t *dest, size_t count)
{
	while ((int)--count >= 0) {
		*dest++ = bus_space_read_4(tag, handle, offset);
		offset += 4;
	}
}

#if 0	/* Cause a link error for bus_space_read_region_8 */
#define	bus_space_read_region_8	!!! bus_space_read_region_8 unimplemented !!!
#endif

/*
 *	void bus_space_write_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    u_intN_t value);
 *
 * Write the 1, 2, 4, or 8 byte value `value' to bus space
 * described by tag/handle/offset.
 */

#define	bus_space_write_1(t, h, o, v)					\
    ((void)(*(volatile u_int8_t *)((h) + (t->bs_stride_1) * (o)) = (v)))

#define	bus_space_write_2(t, h, o, v)					\
    ((void)(*(volatile u_int16_t *)((h) + (t->bs_stride_2) * (o)) = (v)))

#define	bus_space_write_4(t, h, o, v)					\
    ((void)(*(volatile u_int32_t *)((h) + (t->bs_stride_4) * (o)) = (v)))

#if 0	/* Cause a link error for bus_space_write_8 */
#define	bus_space_write_8	!!! bus_space_write_8 not implemented !!!
#endif

/*
 *	void bus_space_write_multi_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    const u_intN_t *addr, size_t count);
 *
 * Write `count' 1, 2, 4, or 8 byte quantities from the buffer
 * provided to bus space described by tag/handle/offset.
 */

static __inline__ void
bus_space_write_multi_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t *dest, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_1(tag, handle, offset, *dest++);
}

static __inline__ void
bus_space_write_multi_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t *dest, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_2(tag, handle, offset, *dest++);
}

static __inline__ void
bus_space_write_multi_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t *dest, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_4(tag, handle, offset, *dest++);
}

#if 0	/* Cause a link error for bus_space_write_8 */
#define	bus_space_write_multi_8(t, h, o, a, c)				\
			!!! bus_space_write_multi_8 unimplemented !!!
#endif

/*
 *	void bus_space_write_region_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    const u_intN_t *addr, size_t count);
 *
 * Write `count' 1, 2, 4, or 8 byte quantities from the buffer provided
 * to bus space described by tag/handle starting at `offset'.
 */

static __inline__ void
bus_space_write_region_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t *dest, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_1(tag, handle, offset++, *dest++);
}

static __inline__ void
bus_space_write_region_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t *dest, size_t count)
{
	while ((int)--count >= 0) {
		bus_space_write_2(tag, handle, offset, *dest++);
		offset += 2;
	}
}

static __inline__ void
bus_space_write_region_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t *dest, size_t count)
{
	while ((int)--count >= 0) {
		bus_space_write_4(tag, handle, offset, *dest++);
		offset += 4;
	}
}

#if 0	/* Cause a link error for bus_space_write_region_8 */
#define	bus_space_write_region_8					\
			!!! bus_space_write_region_8 unimplemented !!!
#endif

/*
 *	void bus_space_set_multi_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset, u_intN_t val,
 *	    size_t count);
 *
 * Write the 1, 2, 4, or 8 byte value `val' to bus space described
 * by tag/handle/offset `count' times.
 */

static __inline__ void
bus_space_set_multi_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t value, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_1(tag, handle, offset, value);
}

static __inline__ void
bus_space_set_multi_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t value, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_2(tag, handle, offset, value);
}

static __inline__ void
bus_space_set_multi_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t value, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_4(tag, handle, offset, value);
}

#if 0	/* Cause a link error for bus_space_set_multi_8 */
#define	bus_space_set_multi_8						\
			!!! bus_space_set_multi_8 unimplemented !!!
#endif

/*
 *	void bus_space_set_region_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset, u_intN_t val,
 *	    size_t count);
 *
 * Write `count' 1, 2, 4, or 8 byte value `val' to bus space described
 * by tag/handle starting at `offset'.
 */

static __inline__ void
bus_space_set_region_1(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int8_t value, size_t count)
{
	while ((int)--count >= 0)
		bus_space_write_1(tag, handle, offset++, value);
}

static __inline__ void
bus_space_set_region_2(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int16_t value, size_t count)
{
	while ((int)--count >= 0) {
		bus_space_write_2(tag, handle, offset, value);
		offset += 2;
	}
}

static __inline__ void
bus_space_set_region_4(bus_space_tag_t tag, bus_space_handle_t handle,
    bus_addr_t offset, u_int32_t value, size_t count)
{
	while ((int)--count >= 0) {
		bus_space_write_4(tag, handle, offset, value);
		offset += 4;
	}
}

#if 0	/* Cause a link error for bus_space_set_region_8 */
#define	bus_space_set_region_8						\
			!!! bus_space_set_region_8 unimplemented !!!
#endif

/*
 *	void bus_space_copy_N(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh1, bus_size_t off1,
 *	    bus_space_handle_t bsh2, bus_size_t off2,
 *	    size_t count);
 *
 * Copy `count' 1, 2, 4, or 8 byte values from bus space starting
 * at tag/bsh1/off1 to bus space starting at tag/bsh2/off2.
 */

#define	__LUNA88K_copy_region_N(BYTES)					\
static __inline void __CONCAT(bus_space_copy_region_,BYTES)		\
	    (bus_space_tag_t,						\
	    bus_space_handle_t bsh1, bus_size_t off1,			\
	    bus_space_handle_t bsh2, bus_size_t off2,			\
	    bus_size_t count);						\
									\
static __inline void							\
__CONCAT(bus_space_copy_region_,BYTES)(t, h1, o1, h2, o2, c)		\
	bus_space_tag_t t;						\
	bus_space_handle_t h1, h2;					\
	bus_size_t o1, o2, c;						\
{									\
	bus_size_t o;							\
									\
	if ((h1 + o1) >= (h2 + o2)) {					\
		/* src after dest: copy forward */			\
		for (o = 0; c != 0; c--, o += BYTES)			\
			__CONCAT(bus_space_write_,BYTES)(t, h2, o2 + o,	\
			    __CONCAT(bus_space_read_,BYTES)(t, h1, o1 + o)); \
	} else {							\
		/* dest after src: copy backwards */			\
		for (o = (c - 1) * BYTES; c != 0; c--, o -= BYTES)	\
			__CONCAT(bus_space_write_,BYTES)(t, h2, o2 + o,	\
			    __CONCAT(bus_space_read_,BYTES)(t, h1, o1 + o)); \
	}								\
}
__LUNA88K_copy_region_N(1)
__LUNA88K_copy_region_N(2)
__LUNA88K_copy_region_N(4)
#if 0	/* Cause a link error for bus_space_copy_8 */
#define	bus_space_copy_8						\
			!!! bus_space_copy_8 unimplemented !!!
#endif

#undef __LUNA88K_copy_region_N

/*
 * Bus read/write barrier methods.
 *
 *	void bus_space_barrier(bus_space_tag_t tag,
 *	    bus_space_handle_t bsh, bus_size_t offset,
 *	    bus_size_t len, int flags);
 *
 */
#define	bus_space_barrier(t, h, o, l, f)	\
	((void)((void)(t), (void)(h), (void)(o), (void)(l), (void)(f)))
#define	BUS_SPACE_BARRIER_READ	0x01		/* force read barrier */
#define	BUS_SPACE_BARRIER_WRITE	0x02		/* force write barrier */

#endif /* _LUNA88K_BUS_H_ */
