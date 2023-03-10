/*	$OpenBSD: mem.c,v 1.22 2004/08/02 08:35:00 miod Exp $ */

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mem.c	8.3 (Berkeley) 1/12/94
 */

/*
 * Memory special file
 */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <sys/malloc.h>

#include <uvm/uvm_extern.h>

static int	mmopen(dev_t dev, int flag, int mode, struct proc *p);
static int	mmclose(dev_t dev, int flag, int mode, struct proc *p);
static int	mmrw(dev_t dev, struct uio *uio, int flags);
static paddr_t	mmmmap(dev_t dev, off_t off, int prot);
static int	mmioctl(dev_t dev, u_long cmd, caddr_t data, int flags, struct proc *p);

#define mmread  mmrw
#define mmwrite mmrw
const struct cdevsw mem_cdevsw = {
	      mmopen,
	      mmclose,
	      mmread,
	      mmwrite,
	      mmioctl,
	      nostop,
	      notty,
	      nopoll,
	      mmmmap,
	      nokqfilter,
	      D_TTY
};

static caddr_t zeropage;

/*ARGSUSED*/
int
mmopen(dev_t dev, int flag, int mode, struct proc *p)
{
	switch (minor(dev)) {
		case DEV_MEM:
		case DEV_KMEM:
		case DEV_NULL:
		case DEV_ZERO:
			return (0);
		default:
			return (ENXIO);
	}
}

/*ARGSUSED*/
int
mmclose(dev_t dev, int flag, int mode, struct proc *p)
{

	return (0);
}

/*ARGSUSED*/
int
mmrw(dev_t dev, struct uio *uio, int flags)
{
	vaddr_t o, v;
	int c;
	struct iovec *iov;
	int error = 0;
	static int physlock = 0;
	extern caddr_t vmmap;

	if (minor(dev) == 0) {
		/* lock against other uses of shared vmmap */
		while (physlock > 0) {
			physlock++;
			error = tsleep((caddr_t)&physlock, PZERO | PCATCH,
			    "mmrw", 0);
			if (error)
				return (error);
		}
		physlock = 1;
	}
	while (uio->uio_resid > 0 && error == 0) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("mmrw");
			continue;
		}
		switch (minor(dev)) {

		case DEV_MEM:
			/* move one page at a time */
			v = uio->uio_offset;
			if (v > last_addr) {
				error = EFAULT;
				goto unlock;
			}
			pmap_enter(pmap_kernel(), (vaddr_t)vmmap,
			    trunc_page(v),
			    uio->uio_rw == UIO_READ ? VM_PROT_READ : VM_PROT_WRITE,
			    (uio->uio_rw == UIO_READ ? VM_PROT_READ : VM_PROT_WRITE) | PMAP_WIRED);
			pmap_update(pmap_kernel());
			o = uio->uio_offset & PGOFSET;
			c = min(uio->uio_resid, (int)(NBPG - o));
			error = uiomove((caddr_t)vmmap + o, c, uio);
			pmap_remove(pmap_kernel(), (vaddr_t)vmmap,
			    (vaddr_t)vmmap + NBPG);
			pmap_update(pmap_kernel());
			continue;

		case DEV_KMEM:
			v = uio->uio_offset;
			c = min(iov->iov_len, MAXPHYS);
			if (!uvm_kernacc((caddr_t)v, c,
			    uio->uio_rw == UIO_READ ? B_READ : B_WRITE))
				return (EFAULT);
			if (v < NBPG) {
#ifdef DEBUG
				/*
				 * For now, return zeros on read of page 0
				 * and EFAULT for writes.
				 */
				if (uio->uio_rw == UIO_READ) {
					if (zeropage == NULL) {
						zeropage = (caddr_t)
						    malloc(PAGE_SIZE, M_TEMP,
						    M_WAITOK);
						bzero(zeropage, PAGE_SIZE);
					}
					c = min(c, NBPG - (int)v);
					v = (vaddr_t)zeropage;
				} else
#endif
					return (EFAULT);
			}
			error = uiomove((caddr_t)v, c, uio);
			continue;

		case DEV_NULL:
			if (uio->uio_rw == UIO_WRITE)
				uio->uio_resid = 0;
			return (0);

/* XXX should add vme bus so that we can do user level probes */

		case DEV_ZERO:
			if (uio->uio_rw == UIO_WRITE) {
				c = iov->iov_len;
				break;
			}
			if (zeropage == NULL) {
				zeropage = (caddr_t)
				    malloc(PAGE_SIZE, M_TEMP, M_WAITOK);
				bzero(zeropage, PAGE_SIZE);
			}
			c = min(iov->iov_len, PAGE_SIZE);
			error = uiomove(zeropage, c, uio);
			continue;

		default:
			return (ENXIO);
		}
		if (error)
			break;
		iov->iov_base = (char*)iov->iov_base + c;
		iov->iov_len -= c;
		uio->uio_offset += c;
		uio->uio_resid -= c;
	}
	if (minor(dev) == 0) {
unlock:
		if (physlock > 1)
			wakeup((caddr_t)&physlock);
		physlock = 0;
	}
	return (error);
}

paddr_t
mmmmap(dev_t dev, off_t off, int prot)
{
	return (-1);
}

/*ARGSUSED*/
int
mmioctl(dev_t dev, u_long cmd, caddr_t data, int flags, struct proc *p)
{
	return (EOPNOTSUPP);
}
