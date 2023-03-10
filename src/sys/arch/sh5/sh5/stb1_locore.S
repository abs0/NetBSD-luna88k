/*	$NetBSD: stb1_locore.S,v 1.14 2005/01/29 11:26:07 scw Exp $	*/

/*
 * Copyright 2002 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Steve C. Woodford for Wasabi Systems, Inc.
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
 *      This product includes software developed for the NetBSD Project by
 *      Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * ##########
 *
 * This is not a standalone file.
 * To use it, put #include <sh5/sh5/stb1_locore.S> in your port's locore.S
 *
 * ##########
 */

#include <sh5/sh5/stb1var.h>

/*
 * Manifest constants describing the TLB layout
 */
#define	STB1_TLB_IDX_STEP	16
#define	STB1_TLB_IDX_SHIFT	4
#define	STB1_xTLB_BIT		23
#define	STB1_DTLB(idx)		(0x00800000 + (STB1_TLB_IDX_STEP * (idx)))
#define	STB1_ITLB(idx)		(0x00000000 + (STB1_TLB_IDX_STEP * (idx)))


/*
 * Ditto for the cache
 */
#define	STB1_ICCR		(0x01600000)
#define	STB1_OCCR		(0x01e00000)
#define	STB1_OCACHETAG0(w,s)	(0x01800000 + ((w) * 65536) + ((s) * 16))


	_TEXT_SECTION
	_ALIGN_TEXT

/******************************************************************************
 *
 * This sub-routine is called very early on from a port's locore.S with
 * the MMU switched off and the TLB/Cache in an unknown state.
 *
 * It is our responsibility to initialise the TLB/Cache, set up a
 * wired TLB pair to map the kernel to its proper virtual address
 * space, and finally to enable the MMU.
 *
 * One entry, r2 holds the physical address of the base of the kernel.
 * This *MUST* be an exact multiple of SH5_KSEG0_SIZE.
 * r18 holds the return address. Note that the caller must ensure that
 * the return address is valid for the newly re-mapped kernel. Basically,
 * don't generate a return address using PC-relative addressing.
 *
 * On exit, although the MMU is enabled and the kernel is running mapped,
 * the pmap module has not yet been boot-strapped. Port-specific code
 * must *NOT* access any memory/device address outside the 512MB KSEG0
 * virtual address space until pmap_bootstrap has been called.
 */
Lsh5_stb1_init:
	add.l	r2, r63, r2		/* Sign-extend the upper 32bits of r2 */
	movi	3, r1			/* Inv/Enable the Caches */
	LDC32(STB1_ICCR, r0)		/* Instruction Cache Control Register */
	putcfg	r0, 0, r1		/* Inv & Enable instruction cache */
	putcfg	r0, 1, r63
	LDC32(STB1_OCCR, r0)		/* Operand Cache Control Register */
	putcfg	r0, 0, r1		/* Inv & Enable operand cache */
	putcfg	r0, 1, r63

	pta/u	1f, tr0			/* Pre-load branches */
	LDC32(STB1_DTLB(STB1_TLB_NSLOTS - 1), r0)	/* Top of DTLB */
	movi	STB1_ITLB(STB1_TLB_NSLOTS - 1), r1	/* Top of ITLB */
1:	putcfg	r0, 0, r63		/* Invalidate DTLB entry */
	putcfg	r0, 1, r63
	addi	r0, -STB1_TLB_IDX_STEP, r0	/* Next DTLB slot */
	putcfg	r1, 0, r63		/* Invalidate ITLB entry */
	putcfg	r1, 1, r63
	addi	r1, -STB1_TLB_IDX_STEP, r1	/* Next ITLB slot */
	bne/l	r1, r63, tr0		/* Back until all checked */

	/* Invalidate [DI]TLB#0 too */
	putcfg	r0, 0, r63
	putcfg	r1, 0, r63

	/* Fix up KSEG0 physical page and attributes in ITLB */
	movi	SH5_PTEL_CB_WRITEBACK | SH5_PTEL_SZ_512MB | SH5_PTEL_PR_X, r3
	or	r2, r3, r3
	putcfg	r1, 1, r3

	/* Ditto for DTLB */
	movi	SH5_PTEL_CB_WRITEBACK | SH5_PTEL_SZ_512MB | SH5_PTEL_PR_R | SH5_PTEL_PR_W, r3
	or	r2, r3, r3
	putcfg	r0, 1, r3

	/* Fix up the virtual page, shared and valid bits */
#ifndef _LP64
	LDSC32(SH5_KSEG0_BASE | SH5_PTEH_SH | SH5_PTEH_V, r2)
#else
	LDSC64(SH5_KSEG0_BASE | SH5_PTEH_SH | SH5_PTEH_V, r2)
#endif
	putcfg	r0, 0, r2
	putcfg	r1, 0, r2

	/*
	 * The TLB is primed, the cache is clean and enabled. All we have
	 * to do now is enable the MMU and FPU before returning to the caller.
	 */
	getcon	sr, r0
	LDUC32(SH5_CONREG_SR_ASID_MASK << SH5_CONREG_SR_ASID_SHIFT, r1)
	andc	r0, r1, r0
	LDUC32(SH5_CONREG_SR_FD, r1)
	andc	r0, r1, r0
	LDUC32(SH5_CONREG_SR_INIT|SH5_CONREG_SR_IMASK_ALL, r1)
	or	r0, r1, r0
	putcon	r0, ssr
	putcon	r18, spc
	synco
	rte
	nop
	nop
	nop
	nop
	nop


/******************************************************************************/

/*
 * TLB cookies are used to keep track of which TLB slot a particular mapping
 * was inserted into. This provides a quick way to invalidate the mapping
 * since we don't need to search the TLB (The sh5 CPU provides no h/w
 * assistance for invalidating specific mappings).
 *
 * There are 16-bits available in the cookie. We split that into two 8-bit
 * fields; one for the instruction TLB, the other for the data TLB.
 *
 * Each field is comprised of a "valid" bit and a TLB "index". The index
 * can range from 0-127. Since this CPU has only 64 TLB slots, values from
 * 64-127 are not used.
 */

#define	TLBCOOKIE_IDX_SHIFT_D	0
#define	TLBCOOKIE_IDX_SHIFT_I	8

#define	TLBCOOKIE_IDX_MASK	0x3f
#define	TLBCOOKIE_IDX_VALID	0x80


/******************************************************************************
 *
 * void _sh5_stb1_tlbinv_cookie(pteh_t pteh, tlbcookie_t tlbcookie)
 *
 * Callable from C code.
 *
 *  r2 - pteh
 *  r3 - tlbcookie
 *
 * Search the TLB for the mapping identified by `pteh' and invalidate
 * it if found. Use `tlbcookie' as a hint for where in the TLB the mapping
 * may be.
 */
ENTRY_NOPROFILE(_sh5_stb1_tlbinv_cookie)
	ptabs/u	r18, tr0		/* Get return address to tr0 */

	/*
	 * Short-circuit if both DTLB and ITLB indices are not valid
	 */
	LDC32((TLBCOOKIE_IDX_VALID << TLBCOOKIE_IDX_SHIFT_D) |		\
	      (TLBCOOKIE_IDX_VALID << TLBCOOKIE_IDX_SHIFT_I), r0)
	and	r3, r0, r0
	beq/u	r0, r63, tr0

	/*
	 * Prepare the PTE
	 */
	addz.l	r2, r63, r2		/* Zero-extend the PTEH */
	ori	r2, SH5_PTEH_V, r2	/* Only check valid entries */

	/*
	 * Disable async interrupts
	 */
	getcon	sr, r4
	pta/u	1f, tr1			/* Preload ITLB path */
	pta/u	2f, tr2			/* Preload exit path */
	ori	r4, SH5_CONREG_SR_IMASK_ALL, r1
	putcon	r1, sr

	/*
	 * DTLB index valid?
	 */
	andi	r3, TLBCOOKIE_IDX_VALID, r0
	beq/u	r0, r63, tr1		/* Nope. Go check ITLB */

	/*
	 * Construct the DTLB index
	 */
	shlli	r0, 16, r0		/* DTLB base */
	andi	r3, TLBCOOKIE_IDX_MASK, r1
#ifdef DIAGNOSTIC
	beq/u	r1, r63, tr1		/* DON'T ZAP DTLB INDEX #0 !!! */
#endif
	shlli	r1, STB1_TLB_IDX_SHIFT, r1
	or	r0, r1, r0		/* Required DTLB slot */

	/*
	 * Check if the DTLB slot still contains the mapping, and
	 * zap it if so.
	 */
	getcfg	r0, 0, r1		/* Fetch DTLB.PTEH entry */
	cmpeq	r1, r2, r5		/* r5 == 1 if same mapping. Else 0 */
	cmvne	r5, r63, r1		/* r1 == 0 is same mapping */
	putcfg	r0, 0, r1		/* Invalidate it if same */

	/*
	 * Now do the same for the ITLB
	 */
1:	shlri	r3, TLBCOOKIE_IDX_SHIFT_I, r3

	/*
	 * ITLB index valid?
	 */
	andi	r3, TLBCOOKIE_IDX_VALID, r0
	beq/u	r0, r63, tr2		/* Nope. All done */

	/*
	 * Construct the ITLB index
	 */
	andi	r3, TLBCOOKIE_IDX_MASK, r0
#ifdef DIAGNOSTIC
	beq/u	r0, r63, tr2		/* DON'T ZAP ITLB INDEX #0 !!! */
#endif
	shlli	r0, STB1_TLB_IDX_SHIFT, r0	/* Required ITLB slot */

	/*
	 * Check if the ITLB slot still contains the mapping, and
	 * zap it if so.
	 */
	getcfg	r0, 0, r1		/* Fetch ITLB.PTEH entry */
	cmpeq	r1, r2, r5		/* r5 == 1 if same mapping. Else 0 */
	cmvne	r5, r63, r1		/* r1 == 0 is same mapping */
	putcfg	r0, 0, r1		/* Invalidate it if same */

2:	putcon	r4, sr			/* Restore async interrupt mask */
	blink	tr0, r63



/******************************************************************************
 *
 * void __sh5_stb1_tlbinv_all(void)
 *
 * Callable from C code.
 *
 * Invalidate all instruction and data TLB entries with non-zero ASIDs.
 */
ENTRY_NOPROFILE(_sh5_stb1_tlbinv_all)
	getcon	sr, r5
	pta/u	1f, tr0			/* Pre-load branches */
	ptabs/u	r18, tr1		/* Get return address to tr1 */
	LDC32(STB1_DTLB(STB1_TLB_NSLOTS - 1), r0)	/* DTLB Top */
	movi	STB1_ITLB(STB1_TLB_NSLOTS - 1), r1	/* ITLB Top */
	movi	(SH5_PTEH_ASID_MASK << SH5_PTEH_ASID_SHIFT), r2
	ori	r5, SH5_CONREG_SR_IMASK_ALL, r4
	putcon	r4, sr			/* Block all async interrupts */

1:	getcfg	r0, 0, r3		/* Fetch DTLB entry */
	and	r3, r2, r4		/* Keep ASID */
	cmvne	r4, r63, r3		/* r3 == 0 if non-zero ASID */
	putcfg	r0, 0, r3		/* Invalidate DTLB entry */
	addi	r0, -STB1_TLB_IDX_STEP, r0	/* Next DTLB slot */
	getcfg	r1, 0, r3		/* Fetch ITLB entry */
	and	r3, r2, r4		/* Keep asid */
	cmvne	r4, r63, r3		/* r3 == 0 if non-zero ASID */
	putcfg	r1, 0, r3		/* Invalidate ITLB entry */
	addi	r1, -STB1_TLB_IDX_STEP, r1	/* Next ITLB slot */
	bne/l	r1, r63, tr0		/* Back until all checked */

	putcon	r5, sr			/* Restore async interrupt mask */
	blink	tr1, r63


/******************************************************************************
 *
 * __sh5_stb1_tlbload(r0 = PTEH, r1 = PTEL, r2 = Return Address,
 *                    r4 = Pointer to pte/kpte)
 *
 * Load the appropriate TLB with the PTEL and PTEH values.
 *
 * r3, tr0, and tr1 are available.
 * r1 and r2 need not be preserved.
 *
 * Returns a 16-bit "cookie" in r0, which will be passed to the TLB
 * invalidate function, above, when the PTE is to be purged from the
 * TLB.
 *
 * NOT callable by C code (despite the use of ENTRY_NOPROFILE()).
 *
 * This subroutine will be called from within the generic TLB miss
 * handler. As such, it is executing on the TLB miss stack with all
 * exceptions disabled. Therefore, don't touch anything which might
 * cause *another* TLB miss!
 *
 * The subroutine is free to jump to ASENTRY(Ltlbmiss_dotrap) if the
 * PTE's protection bits are incompatible with type of TLB miss.
 * For example, an Instruction TLB miss for a non-executable page.
 */
ENTRY_NOPROFILE(_sh5_stb1_tlbload)
	ptabs/u	r2, tr1			/* Stash the return address */
	LEAF(Ltlbmiss_dotrap, r3)
	ptabs/u	r3, tr0

	getcon	expevt, r2		/* Fetch TLB miss trap number */
	add.l	r0, r63, r0		/* Sign-extend PTEH */
	ori	r0, SH5_PTEH_V, r0	/* Ensure PTEH Valid bit is set */
	add.l	r1, r63, r1		/* Sign-extend PTEL */

	/*
	 * Determine what kind of TLB miss we're dealing with.
	 */
	addi	r2, -T_WTLBMISS, r2

	/*
	 *  r2 = 0: DTLB miss (write)	[0x0000000000000000]
	 *  r2 > 0: ITLB miss		[0x00000000000009e0]
	 *  r2 < 0: DTLB miss (read)	[0xffffffffffffffe0]
	 */

	/*
	 * For "write" misses, verify the page is actually writable
	 */
	andi	r1, SH5_PTEL_PR_W, r3	/* r3 == 0 if read-only page */
	beq/u	r2, r3, tr0		/* Writing to read-only page. Trap it */

	/*
	 * Simplify the TLB miss type to just DTLB or ITLB
	 */
	addi	r2, -1, r2		/* DTLB: r2 < 0. ITLB r2 > 0 */
	cmpgt	r63, r2, r2		/* r2 == 0 for ITLB, r2 == 1 for DTLB */

	/*
	 * For ITLB misses, verify the page is actually executable
	 */
	andi	r1, SH5_PTEL_PR_X, r3	/* r3 == 0 if page not executable */
	beq/u	r2, r3, tr0		/* Executing non-exec page. Trap it */

	/*
	 * If ITLB miss, clear PTEL read/write bits
	 */
	movi	SH5_PTEL_PR_R | SH5_PTEL_PR_W, r3
	cmvne	r2, r63, r3		/* If DTLB miss, r3 == 0 */
	andc	r1, r3, r1		/* This is a no-op if DTLB miss */

	/*
	 * If DTLB miss, clear PTEL execute bit
	 */
	movi	SH5_PTEL_PR_X, r3
	cmveq	r2, r63, r3		/* If ITLB miss, r3 == 0 */
	andc	r1, r3, r1		/* This is a no-op if ITLB miss */

	/*
	 * Determine which TLB to load
	 */
	shlli	r2, STB1_xTLB_BIT, r2	/* Magic! */

	/*
	 * r0 == PTEH
	 * r1 == PTEL, with unecessary bits masked off
	 * r2 == 0x00800000 for DTLB, or 0x00000000 for ITLB
	 */

	/*
	 * Pick a pseudo-random TLB slot.
	 * Note: If we pick TLB slot zero, bump the slot index to 1.
	 * This tends to put a slightly higher load on slot 1, but it's
	 * by far the quickest way to protect slot 0.
	 */
	getcon	ctc, r3			/* Fetch free-running counter */
	shlri	r3, 2, r3		/* The 2 LSBs tend not to be random */
	andi	r3, STB1_TLB_NSLOTS-1, r3	/* Generate random TLB index */
	shlli	r3, STB1_TLB_IDX_SHIFT, r3	/* Index *= STB1_TLB_IDX_STEP */
	or	r2, r3, r2		/* Point to target TLB slot */
	cmpeq	r3, r63, r3		/* TLB slot #0? */
	shlli	r3, STB1_TLB_IDX_SHIFT, r3
	or	r2, r3, r2		/* Don't trash TLB slot #0 ! */

	/*
	 * r2 == pseudo-random [ID]TLB index
	 */
	putcfg	r2, 0, r63		/* Make sure PTEH.V is clear */
	synci
	putcfg	r2, 1, r1		/* Store PTEL */
	putcfg	r2, 0, r0		/* Store PTEH */

	/*
	 * Generate TLB cookie
	 */
	ld.w	r4, SH5_PTE_TLBCOOKIE, r3	/* Pre-fetch TLB cookie */
	shlri	r2, STB1_TLB_IDX_SHIFT, r0	/* Get TLB index */
	movi	TLBCOOKIE_IDX_MASK, r1
	shlri	r0, 16, r2
	and	r0, r1, r0			/* Ditch unwanted bits */
	xori	r2, 8, r2			/* Magic! r2 = shift count */
	ori	r0, TLBCOOKIE_IDX_VALID, r0	/* Mark it as valid */
	shlld	r0, r2, r0
	shlld	r1, r2, r1
	andc	r3, r1, r3			/* Clear out old cookie value */
	or	r3, r0, r3			/* Insert new cookie */
	st.w	r4, SH5_PTE_TLBCOOKIE, r3
	blink	tr1, r63



/******************************************************************************
 *
 * SH-5 Cache Ops
 *
 * Very Important Note: These Cache Ops MUST NOT CAUSE A TLB MISS EXCEPTION.
 *
 * The SH-5's cache can be somewhat painful to manage, at least the
 * implementation of this particular CPU's cache is.
 *
 * The instruction cache is virtually indexed and tagged, therefore it needs
 * to be invalidated on each and every context switch. We also provide hooks
 * such that specific regions of the instruction cache can be invalidated.
 *
 * The data cache is virtually indexed and physically tagged, therefore it
 * does not need to be purged on every context switch.
 *
 * Both cache tags contain the ASID which caused the line to be cached.
 * Unfortunately, the hardware does not use this ASID when matching a
 * cacheline, making it pretty well useless.
 *
 * Also, the cache is very tighly coupled to the TLB such that the purge/
 * invalidate operations (icbi, ocbi, ocbp, ocbwb) always result in a TLB
 * lookup. This has the side-effect of causing a TLB miss exception if the
 * mapping is not currently resident in the TLB.
 *
 * This isn't just an unpleasant side-effect, it is Really Nasty. For
 * example, the TLB miss handler may have to call pmap_pte_spill() in
 * order to resolve a mapping which it couldn't find in the PTE group.
 * pmap_pte_spill() may have to evict some other mapping from the PTE
 * group because there were no free slots. We really want to sync the
 * cache for the evictee as it makes life so much easier elsewhere.
 * However, if the evictee is not mapped in the TLB then syncing the
 * cache causes a recursive TLB miss... *boom*.
 *
 * Fortunately, we have a cunning plan to ensure we never cause a TLB
 * miss exception.
 *
 * All the cache-op handlers are passed the VA/PA pair which are to
 * be purged. We use these to stuff the TLB with a temporary unshared
 * mapping (using ASID #1, which the kernel reserves for just this
 * purpose) for the region to be purged/invalidated. This guarantees
 * we won't cause a TLB miss exception. We then switch to ASID #1 in
 * the SR (via an rte), do the cache op, then switch back to the
 * original ASID.
 *
 * While we could just over-write any old TLB slot (except #0 ;-), we
 * use slot (STB1_TLB_NSLOTS - 1) just for convenience. We also save
 * and restore any existing mapping to avoid the overhead of another TLB
 * miss later on.
 *
 * What a crock.
 *
 * IMPORTANT!
 * Callers of the cacheops routines MUST ENSURE that the specified region
 * DOES NOT CROSS A 4KB PAGE BOUNDARY.
 */

/******************************************************************************
 *
 * void _sh5_stb1_cache_dpurge(vaddr_t va, paddr_t pa, vsize_t len)
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_dpurge)
	pta/l	Lstb1_cacheop_switch_asid, tr1
	blink	tr1, r5
	ocbp	r2, 0			/* Purge a line of dcache */
	addi	r2, STB1_CACHE_LINE_SIZE, r2	/* Point to next line */
	bgtu/l	r4, r2, tr0		/* r4 -> end of region to purge */
	synco
	blink	tr1, r63		/* Back to restore ASID */

/******************************************************************************
 *
 * void _sh5_stb1_cache_dpurge_iinv(vaddr_t start, paddr_t pa, vsize_t len)
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_dpurge_iinv)
	pta/l	Lstb1_cacheop_switch_asid, tr1
	blink	tr1, r5
	ocbp	r2, 0			/* Purge a line of dcache */
	icbi	r2, 0			/* Invalidate a line of icache */
	addi	r2, STB1_CACHE_LINE_SIZE, r2	/* Point to next line */
	bgtu/l	r4, r2, tr0		/* r4 -> end of region to purge/inv */
	synco
	synci
	blink	tr1, r63		/* Back to restore ASID */

/******************************************************************************
 *
 * void _sh5_stb1_cache_dinv(vaddr_t start, paddr_t pa, vsize_t len)
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_dinv)
	pta/l	Lstb1_cacheop_switch_asid, tr1
	blink	tr1, r5
	ocbi	r2, 0			/* Invalidate a line of dcache */
	addi	r2, STB1_CACHE_LINE_SIZE, r2	/* Point to next line */
	bgtu/l	r4, r2, tr0		/* r4 -> end of region to inv */
	synco
	blink	tr1, r63		/* Back to restore ASID */

/******************************************************************************
 *
 * void _sh5_stb1_cache_dinv_iinv(vaddr_t start, paddr_t pa, vsize_t len)
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_dinv_iinv)
	pta/l	Lstb1_cacheop_switch_asid, tr1
	blink	tr1, r5
	ocbi	r2, 0			/* Invalidate a line of dcache */
	icbi	r2, 0			/* Invalidate a line of icache */
	addi	r2, STB1_CACHE_LINE_SIZE, r2	/* Point to next line */
	bgtu/l	r4, r2, tr0		/* r4 -> end of region to inv */
	synco
	synci
	blink	tr1, r63		/* Back to restore ASID */

/******************************************************************************
 *
 * void _sh5_stb1_cache_iinv(vaddr_t start, paddr_t pa, vsize_t len)
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_iinv)
	pta/l	Lstb1_cacheop_switch_asid, tr1
	blink	tr1, r5
	icbi	r2, 0			/* Invalidate a line of icache */
	addi	r2, STB1_CACHE_LINE_SIZE, r2	/* Point to next line */
	bgtu/l	r4, r2, tr0		/* r4 -> end of region to inv */
	synci
	blink	tr1, r63		/* Back to restore ASID */

/******************************************************************************
 *
 * void _sh5_stb1_cache_iinv_all(void)
 *
 * This one's easy.
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_iinv_all)
	ptabs/l	r18, tr0
	movi	3, r1			/* Inv/Enable the Cache */
	synco
	synci
	LDC32(STB1_ICCR, r0)		/* Instruction Cache Control Register */
	putcfg	r0, 0, r1		/* Inv & Enable instruction cache */
	synco
	synci
	blink	tr0, r63

/******************************************************************************
 *
 * void _sh5_stb1_cache_purge_all(void)
 *
 * This one's just stupid. The SH5 has no easy was to writeback and invalidate
 * all cachelines. We have to force all the cachelines out by accessing
 * the fixed KSEG0 mapping in such a way that we cover all cache sets. This
 * will purge any dirty cachelines. We can then safely invalidate the
 * operand and insn caches.
 */
ENTRY_NOPROFILE(_sh5_stb1_cache_purge_all)
	ptabs/u	r18, tr0
	pta/l	1f, tr1
	movi	(STB1_CACHE_NSETS * STB1_CACHE_LINE_SIZE), r0

	/*
	 * r1-r4 are associated with each of the 4 ways
	 */
#ifndef _LP64
	LDSC32(SH5_KSEG0_BASE, r1)
#else
	LDSC64(SH5_KSEG0_BASE, r1)
#endif
	add	r1, r0, r2
	add	r2, r0, r3
	add	r3, r0, r4

1:	addi	r0, -STB1_CACHE_LINE_SIZE, r0
	ldx.q	r1, r0, r5
	ldx.q	r2, r0, r5
	ldx.q	r3, r0, r5
	ldx.q	r4, r0, r5
	bnei/l	r0, 0, tr1
	synci
	synco

	movi	3, r1			/* Inv/Enable the Cache */
	LDC32(STB1_ICCR, r0)		/* Instruction Cache Control Register */
	putcfg	r0, 0, r1		/* Inv & Enable instruction cache */
	LDC32(STB1_OCCR, r0)		/* Operand Cache Control Register */
	putcfg	r0, 0, r1		/* Inv & Enable operand cache */
	synci
	synco
	blink	tr0, r63


/******************************************************************************
 *
 * Set things up so that the cache purge/invalidate operations won't cause
 * a TLB miss.
 *
 * On entry:
 *
 *  r2 = Virtual address of region to be purged/invalidated.
 *  r3 = Physical address of regions to be purged/invalidated.
 *  r4 = Length of region.
 *  r5 = Address of cache-ops routine to call.
 *
 * The cache-ops routines expect the following registers on entry:
 *
 *  r2  = The virtual address of region to be purged/invalidated.
 *  r4  = The virtual address+1 of the end of the region.
 *  tr0 = The cache-ops routine (same as was passed in r5)
 *  tr1 = Return address.
 */
Lstb1_cacheop_switch_asid:
#ifndef _LP64
	add.l	r2, r4, r4		/* r4 == end of region to purge/inv */
	add.l	r2, r63, r2
#else
	add	r2, r4, r4		/* r4 == end of region to purge/inv */
#endif

	/*
	 * There's no need to do this dance if the region is in KSEG0.
	 */
	pta/l	1f, tr2
	ptabs/u	r18, tr1
	ptabs/u	r5, tr0

#ifndef _LP64
	LDSC32(SH5_KSEG0_BASE, r0)
#else
	LDSC64(SH5_KSEG0_BASE, r0)
#endif
	bgtu/l	r0, r2, tr2		/* Region is below KSEG0 (user space) */

#ifndef _LP64
	LDSC32(SH5_KSEG1_BASE, r0)
#else
	LDSC64(SH5_KSEG1_BASE, r0)
#endif
	/*
	 * If the region is in KSEG0, which has a permanent TLB entry.
	 * Just call the cacheop handler directly and let it return to
	 * the original caller.
	 */
	bgtu/u	r0, r2, tr0

	/*
	 * The region is in user-space or KSEG1.
	 * It needs a temporary TLB entry
	 */
1:	pta/u	Lstb1_cacheops_return, tr1	/* Cache-ops return here */

	/*
	 * First, block all exceptions. r19 is a handy caller-saved
	 * register which we'll use to restore SR when done.
	 */
	getcon	sr, r19
	LDC32(SH5_CONREG_SR_BL, r1)
	or	r19, r1, r0
	putcon	r0, sr			/* Exceptions are now blocked */

	/*
	 * Next, prime SSR and SPC with the correct ASID and cacheop
	 * handler address.
	 *
	 * NOTE: It is **IMPERATIVE** that we preserve the current SSR/SPC
	 * values as we could be called as a result of a TLB miss (via the
	 * pmap_pte_spill() function). In this case, SSR/SPC are *live*.
	 */
	getcon	ssr, r36		/* Save SSR in caller-saved r36 */
	getcon	spc, r37		/* Save SPC in caller-saved r37 */
	LDC32(SH5_CONREG_SR_ASID_MASK << SH5_CONREG_SR_ASID_SHIFT, r1)
	andc	r0, r1, r0		/* Clear the current ASID */
	LDC32(PMAP_ASID_CACHEOPS << SH5_CONREG_SR_ASID_SHIFT, r1)
	or	r0, r1, r0
	gettr	tr0, r1			/* Fetch handler address */
	putcon	r0, ssr
	putcon	r1, spc

	/*
	 * Save the current contents of [ID]TLB Slot #STB1_TLB_NSLOTS-1
	 *
	 * r20/r21 = The contents of DTLB#STB1_TLB_NSLOTS-1 - PTEH/PTEL.
	 * r22/r23 = The contents of ITLB#STB1_TLB_NSLOTS-1 - PTEH/PTEL.
	 */
	LDC32(STB1_DTLB(STB1_TLB_NSLOTS - 1), r0)	/* Top of DTLB */
	movi	STB1_ITLB(STB1_TLB_NSLOTS - 1), r1	/* Top of ITLB */
	getcfg	r0, 0, r20		/* Save Data PTEH */
	getcfg	r0, 1, r21		/* Save Data PTEL */
	add.l	r20, r63, r20
	add.l	r21, r63, r21
	putcfg	r0, 0, r63		/* Invalidate the slot */
	getcfg	r1, 0, r22		/* Save Insn PTEH */
	getcfg	r1, 1, r23		/* Save Insn PTEL */
	add.l	r22, r63, r22
	add.l	r23, r63, r23
	putcfg	r1, 0, r63		/* Invalidate the slot */

	/*
	 * Fabricate PTEH/PTEL values and set up the temporary mapping.
	 *
	 * r2 = start VA
	 * r3 = start PA
	 */
	movi	SH5_PTE_PN_MASK_MOVI, r5
	and	r3, r5, r3		/* r3 will be PTEL */
	and	r2, r5, r5		/* r5 will be PTEH */
	movi	SH5_PTEH_V | (PMAP_ASID_CACHEOPS << SH5_PTEH_ASID_SHIFT), r6
	or	r5, r6, r5		/* PTEH is now complete */

	/* Fix up the appropriate PTEL for the DTLB, and load it */
	movi	SH5_PTEL_CB_WRITEBACK | SH5_PTEL_SZ_4KB | SH5_PTEL_PR_R | SH5_PTEL_PR_W, r6
	or	r3, r6, r6		/* DTLB PTEL is now complete in r6 */
	add.l	r6, r63, r6
	add.l	r5, r63, r5
	putcfg	r0, 1, r6		/* Load DTLB.PTEL */
	putcfg	r0, 0, r5		/* Load DTLB.PTEH */

	/* Fix up the appropriate PTEL for the ITLB, and load it */
	movi	SH5_PTEL_CB_WRITEBACK | SH5_PTEL_SZ_4KB | SH5_PTEL_PR_X, r6
	or	r3, r6, r6		/* ITLB PTEL is now complete in r6 */
	add.l	r6, r63, r6
	putcfg	r1, 1, r6		/* Load ITLB.PTEL */
	putcfg	r1, 0, r5		/* Load ITLB.PTEH */

	/*
	 * We're fit to go. Just "rte" to the cacheops handler
	 */
	synco
	synci
	rte
	nop
	nop
	nop
	nop
	nop

/*
 * The cacheops handlers return here when they're done.
 */
Lstb1_cacheops_return:
	/*
	 * Restore the previous contents of [ID]TLB #STB1_TLB_NSLOTS-1.
	 */
	putcfg	r0, 0, r63			/* Invalidate the slot */
	putcfg	r0, 0, r20			/* Restore Data PTEH */
	putcfg	r0, 1, r21			/* Restore Data PTEL */
	putcfg	r1, 0, r63			/* Invalidate the slot */
	putcfg	r1, 0, r22			/* Restore Insn PTEH */
	putcfg	r1, 1, r23			/* Restore Insn PTEL */

	/*
	 * Restore the original SR/SSR/SPC, and return to the original caller.
	 */
	putcon	r36, pssr
	putcon	r37, pspc
	putcon	r19, ssr
	putcon	r18, spc
	synco
	synci
	rte
	nop
	nop
	nop
	nop
	nop
