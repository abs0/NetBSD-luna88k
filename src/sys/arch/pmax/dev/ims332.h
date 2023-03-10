/*	$NetBSD: ims332.h,v 1.6 2003/08/07 16:29:09 agc Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell and Rick Macklem.
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
 *	@(#)ims332.h	8.1 (Berkeley) 6/10/93
 */

/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 *	Defines for the Inmos IMS-G332 Colour video controller
 * 	Author: Alessandro Forin, Carnegie Mellon University
 *	See: IMS G332 Colour Video Controller, 1990 Databook, pg 139-163,
 *		Inmos, Ltd.
 */

/*
 * Although the chip is built to be memory-mapped
 * it can be programmed for 32 or 64 bit addressing.
 * Moreover, the hardware bits have been twisted
 * even more on the machine I am writing this for.
 * So I'll just define the chip's offsets and leave
 * it to the implementation to define the rest.
 */
#define	IMS332_REG_BOOT			0x000	/* boot time config */

#define	IMS332_REG_HALF_SYNCH		0x021	/* datapath registers */
#define	IMS332_REG_BACK_PORCH		0x022
#define	IMS332_REG_DISPLAY		0x023
#define	IMS332_REG_SHORT_DIS		0x024
#define	IMS332_REG_BROAD_PULSE		0x025
#define	IMS332_REG_V_SYNC		0x026
#define	IMS332_REG_V_PRE_EQUALIZE	0x027
#define	IMS332_REG_V_POST_EQUALIZE	0x028
#define	IMS332_REG_V_BLANK		0x029
#define	IMS332_REG_V_DISPLAY		0x02a
#define	IMS332_REG_LINE_TIME		0x02b
#define	IMS332_REG_LINE_START		0x02c
#define	IMS332_REG_MEM_INIT		0x02d
#define	IMS332_REG_XFER_DELAY		0x02e

#define	IMS332_REG_COLOR_MASK		0x040	/* color mask register */

#define	IMS332_REG_CSR_A		0x060

#define	IMS332_REG_CSR_B		0x070

#define	IMS332_REG_TOP_SCREEN		0x080	/* top-of-screen offset */

#define	IMS332_REG_CURSOR_LUT_0		0x0a1	/* cursor palette */
#define	IMS332_REG_CURSOR_LUT_1		0x0a2
#define	IMS332_REG_CURSOR_LUT_2		0x0a3

#define	IMS332_REG_RGB_CKSUM_0		0x0c0	/* test registers */
#define	IMS332_REG_RGB_CKSUM_1		0x0c1
#define	IMS332_REG_RGB_CKSUM_2		0x0c2

#define	IMS332_REG_CURSOR_LOC		0x0c7	/* cursor location */

#define	IMS332_REG_LUT_BASE		0x100	/* color palette */
#define	IMS332_REG_LUT_END		0x1ff

#define	IMS332_REG_CURSOR_RAM		0x200	/* cursor bitmap */
#define	IMS332_REG_CURSOR_RAM_END	0x3ff

/*
 * Control register A
 */

#define IMS332_CSR_A_VTG_ENABLE		0x000001	/* vertical timing generator */
#define IMS332_CSR_A_INTERLACED		0x000002	/* screen format */
#define IMS332_CSR_A_CCIR		0x000004	/* default is EIA */
#define IMS332_CSR_A_SLAVE_SYNC		0x000008	/* else from our pll */
#define IMS332_CSR_A_PLAIN_SYNC		0x000010	/* else tesselated */
#define IMS332_CSR_A_SEPARATE_SYNC	0x000020	/* else composite */
#define IMS332_CSR_A_VIDEO_ONLY		0x000040	/* else video+sync */
#define IMS332_CSR_A_BLANK_PEDESTAL	0x000080	/* blank level */
#define IMS332_CSR_A_CBLANK_IS_OUT	0x000100
#define IMS332_CSR_A_CBLANK_NO_DELAY	0x000200
#define IMS332_CSR_A_FORCE_BLANK	0x000400
#define IMS332_CSR_A_BLANK_DISABLE	0x000800
#define IMS332_CSR_A_VRAM_INCREMENT	0x003000
#	define IMS332_VRAM_INC_1	0x000000
#	define IMS332_VRAM_INC_256	0x001000	/* except interlaced->2 */
#	define IMS332_VRAM_INC_512	0x002000
#	define IMS332_VRAM_INC_1024	0x003000
#define IMS332_CSR_A_DMA_DISABLE	0x004000
#define IMS332_CSR_A_SYNC_DELAY_MASK	0x038000	/* 0-7 VTG clk delays */
#define IMS332_CSR_A_PIXEL_INTERLEAVE	0x040000
#define IMS332_CSR_A_DELAYED_SAMPLING	0x080000
#define IMS332_CSR_A_BITS_PER_PIXEL	0x700000
#	define IMS332_BPP_1		0x000000
#	define IMS332_BPP_2		0x100000
#	define IMS332_BPP_4		0x200000
#	define IMS332_BPP_8		0x300000
#	define IMS332_BPP_15		0x400000
#	define IMS332_BPP_16		0x500000
#define IMS332_CSR_A_DISABLE_CURSOR	0x800000


/*
 * Control register B is mbz
 */

/*
 * Boot register
 */

#define	IMS332_BOOT_PLL			0x00001f	/* xPLL, binary */
#define	IMS332_BOOT_CLOCK_PLL		0x000020	/* else xternal */
#define	IMS332_BOOT_64_BIT_MODE		0x000040	/* else 32 */
#define	IMS332_BOOT_xxx			0xffff80	/* reserved, mbz */

int	ims332init __P((struct fbinfo *));
void	ims332InitColorMap __P((struct fbinfo *));
int	ims332LoadColorMap __P((struct fbinfo *, const u_char *, int, int));
int	ims332GetColorMap __P((struct fbinfo *, u_char *, int, int));
int	ims332_video_off __P((struct fbinfo *));
int	ims332_video_on __P((struct fbinfo *));
void	ims332PosCursor __P((struct fbinfo *, int, int));
void	ims332RestoreCursorColor __P((struct fbinfo *));
void	ims332CursorColor __P((struct fbinfo *, unsigned int []));
void	ims332LoadCursor __P((struct fbinfo *, u_short *));
