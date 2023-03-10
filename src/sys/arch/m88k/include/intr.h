/*	$OpenBSD: intr.h,v 1.7 2006/03/13 19:39:52 brad Exp $	*/
/*
 * Copyright (C) 2000 Steve Murphree, Jr.
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#ifndef _M88K_INTR_H_
#define _M88K_INTR_H_

#ifdef _KERNEL
#ifndef _LOCORE
unsigned setipl(unsigned level);
unsigned raiseipl(unsigned level);
int spl0(void);
#endif /* _LOCORE */

/*
 * Hardware interrupt masks
 */
#define splbio()		raiseipl(IPL_BIO)
#define splnet()		raiseipl(IPL_NET)
#define spltty()		raiseipl(IPL_TTY)
#define splclock()		raiseipl(IPL_CLOCK)
#define splstatclock()		raiseipl(IPL_STATCLOCK)

/*
 * Software interrupt masks
 *
 * NOTE: spllowersoftclock() is used by hardclock() to lower the priority from
 * clock to softclock before it calls softclock().
 */
#define splsoftclock()		raiseipl(IPL_SOFTCLOCK)
#define splsoftnet()		raiseipl(IPL_SOFTNET)
#define	spllowersoftclock()	setipl(IPL_SOFTCLOCK)

/*
 * Miscellaneous
 */
#define	splsched()		raiseipl(IPL_SCHED)
#define splvm()			raiseipl(IPL_VM)
#define splhigh()		setipl(IPL_HIGH)

#define	spllock()		splhigh()

#define splx(x)			((x) ? setipl((x)) : spl0())

#endif /* _KERNEL */
#endif /* _M88K_INTR_H_ */
