/* Native-dependent code for BSD Unix running on ARM's, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 1999, 2002
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"

#ifndef FETCH_INFERIOR_REGISTERS
#error Not FETCH_INFERIOR_REGISTERS 
#endif /* !FETCH_INFERIOR_REGISTERS */

#include "arm-tdep.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>
#include "inferior.h"
#include "regcache.h"
#include "gdbcore.h"

#ifndef HAVE_GREGSET_T
typedef struct reg gregset_t;
#endif

#ifndef HAVE_FPREGSET_T
typedef struct fpreg fpregset_t;
#endif

#include "gregset.h"

extern int arm_apcs_32;

void
supply_gregset (gregset_t *gregsetp)
{
  int regno;
  CORE_ADDR r_pc;

  /* Integer registers.  */
  for (regno = ARM_A1_REGNUM; regno < ARM_SP_REGNUM; regno++)
    supply_register (regno, (char *) &gregsetp->r[regno]);

  supply_register (ARM_SP_REGNUM, (char *) &gregsetp->r_sp);
  supply_register (ARM_LR_REGNUM, (char *) &gregsetp->r_lr);
  /* This is ok: we're running native...  */
  r_pc = ADDR_BITS_REMOVE (gregsetp->r_pc);
  supply_register (ARM_PC_REGNUM, (char *) &r_pc);

  if (arm_apcs_32)
    supply_register (ARM_PS_REGNUM, (char *) &gregsetp->r_cpsr);
  else
    supply_register (ARM_PS_REGNUM, (char *) &gregsetp->r_pc);
}

void
supply_fpregset (fpregset_t *fpregsetp)
{
  int regno;

  for (regno = ARM_F0_REGNUM; regno <= ARM_F7_REGNUM; regno++)
    supply_register
      (regno, (char *) &fpregsetp->fpr[regno - ARM_F0_REGNUM]);

  supply_register (ARM_FPS_REGNUM, (char *) &fpregsetp->fpr_fpsr);
}

void
fill_gregset (gregset_t *gregsetp, int regno)
{
  if (-1 == regno)
    {
      int regnum;
      for (regnum = ARM_A1_REGNUM; regnum < ARM_SP_REGNUM; regnum++) 
	regcache_collect (regnum, (char *) &gregsetp->r[regnum]);
    }
  else if (regno >= ARM_A1_REGNUM && regno < ARM_SP_REGNUM)
    regcache_collect (regno, (char *) &gregsetp->r[regno]);

  if (ARM_SP_REGNUM == regno || -1 == regno)
    regcache_collect (ARM_SP_REGNUM, (char *) &gregsetp->r_sp);

  if (ARM_LR_REGNUM == regno || -1 == regno)
    regcache_collect (ARM_LR_REGNUM, (char *) &gregsetp->r_lr);

  /* XXX NH ADDR_BITS_REMOVE ? */
  if (ARM_PC_REGNUM == regno || -1 == regno)
    regcache_collect (ARM_PC_REGNUM, (char *) &gregsetp->r_pc);

  if (ARM_PS_REGNUM == regno || -1 == regno)
    {
      if (arm_apcs_32)
	regcache_collect (ARM_PS_REGNUM, (char *) &gregsetp->r_cpsr);
      else
	regcache_collect (ARM_PS_REGNUM, (char *) &gregsetp->r_pc);
    }
}

void
fill_fpregset (fpregset_t *fpregsetp, int regno)
{
  if (-1 == regno)
    {
       int regnum;
       for (regnum = ARM_F0_REGNUM; regnum <= ARM_F7_REGNUM; regnum++)
         regcache_collect(regnum, (char *) &fpregsetp->fpr[regnum - ARM_F0_REGNUM]);
    }
  else if (regno >= ARM_F0_REGNUM && regno <= ARM_F7_REGNUM)
    regcache_collect(regno, (char *) &fpregsetp->fpr[regno - ARM_F0_REGNUM]);

  if (ARM_FPS_REGNUM == regno || -1 == regno)
    regcache_collect (ARM_FPS_REGNUM, (char *) &fpregsetp->fpr_fpsr);
}

static void
fetch_register (int regno)
{
  struct reg inferior_registers;
  int ret;

  ret = ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch general register");
      return;
    }

  switch (regno)
    {
    case ARM_SP_REGNUM:
      supply_register (ARM_SP_REGNUM, (char *) &inferior_registers.r_sp);
      break;

    case ARM_LR_REGNUM:
      supply_register (ARM_LR_REGNUM, (char *) &inferior_registers.r_lr);
      break;

    case ARM_PC_REGNUM:
      /* This is ok: we're running native... */
      inferior_registers.r_pc = ADDR_BITS_REMOVE (inferior_registers.r_pc);
      supply_register (ARM_PC_REGNUM, (char *) &inferior_registers.r_pc);
      break;

    case ARM_PS_REGNUM:
      if (arm_apcs_32)
	supply_register (ARM_PS_REGNUM, (char *) &inferior_registers.r_cpsr);
      else
	supply_register (ARM_PS_REGNUM, (char *) &inferior_registers.r_pc);
      break;

    default:
      supply_register (regno, (char *) &inferior_registers.r[regno]);
      break;
    }
}

static void
fetch_regs (void)
{
  struct reg inferior_registers;
  int ret;
  int regno;

  ret = ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch general registers");
      return;
    }

  supply_gregset (&inferior_registers);
}

static void
fetch_fp_register (int regno)
{
  struct fpreg inferior_fp_registers;
  int ret;

  ret = ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_fp_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch floating-point register");
      return;
    }

  switch (regno)
    {
    case ARM_FPS_REGNUM:
      supply_register (ARM_FPS_REGNUM,
		       (char *) &inferior_fp_registers.fpr_fpsr);
      break;

    default:
      supply_register
	(regno, (char *) &inferior_fp_registers.fpr[regno - ARM_F0_REGNUM]);
      break;
    }
}

static void
fetch_fp_regs (void)
{
  struct fpreg inferior_fp_registers;
  int ret;
  int regno;

  ret = ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_fp_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch general registers");
      return;
    }

  supply_fpregset (&inferior_fp_registers);
}

void
fetch_inferior_registers (int regno)
{
  if (regno >= 0)
    {
      if (regno < ARM_F0_REGNUM || regno > ARM_FPS_REGNUM)
	fetch_register (regno);
      else
	fetch_fp_register (regno);
    }
  else
    {
      fetch_regs ();
      fetch_fp_regs ();
    }
}


static void
store_register (int regno)
{
  struct reg inferior_registers;
  int ret;

  ret = ptrace (PT_GETREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch general registers");
      return;
    }

  switch (regno)
    {
    case ARM_SP_REGNUM:
      regcache_collect (ARM_SP_REGNUM, (char *) &inferior_registers.r_sp);
      break;

    case ARM_LR_REGNUM:
      regcache_collect (ARM_LR_REGNUM, (char *) &inferior_registers.r_lr);
      break;

    case ARM_PC_REGNUM:
      if (arm_apcs_32)
	regcache_collect (ARM_PC_REGNUM, (char *) &inferior_registers.r_pc);
      else
	{
	  unsigned pc_val;

	  regcache_collect (ARM_PC_REGNUM, (char *) &pc_val);
	  
	  pc_val = ADDR_BITS_REMOVE (pc_val);
	  inferior_registers.r_pc
	    ^= ADDR_BITS_REMOVE (inferior_registers.r_pc);
	  inferior_registers.r_pc |= pc_val;
	}
      break;

    case ARM_PS_REGNUM:
      if (arm_apcs_32)
	regcache_collect (ARM_PS_REGNUM, (char *) &inferior_registers.r_cpsr);
      else
	{
	  unsigned psr_val;

	  regcache_collect (ARM_PS_REGNUM, (char *) &psr_val);

	  psr_val ^= ADDR_BITS_REMOVE (psr_val);
	  inferior_registers.r_pc = ADDR_BITS_REMOVE (inferior_registers.r_pc);
	  inferior_registers.r_pc |= psr_val;
	}
      break;

    default:
      regcache_collect (regno, (char *) &inferior_registers.r[regno]);
      break;
    }

  ret = ptrace (PT_SETREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    warning ("unable to write register %d to inferior", regno);
}

static void
store_regs (void)
{
  struct reg inferior_registers;
  int ret;
  int regno;


  for (regno = ARM_A1_REGNUM; regno < ARM_SP_REGNUM; regno++)
    regcache_collect (regno, (char *) &inferior_registers.r[regno]);

  regcache_collect (ARM_SP_REGNUM, (char *) &inferior_registers.r_sp);
  regcache_collect (ARM_LR_REGNUM, (char *) &inferior_registers.r_lr);

  if (arm_apcs_32)
    {
      regcache_collect (ARM_PC_REGNUM, (char *) &inferior_registers.r_pc);
      regcache_collect (ARM_PS_REGNUM, (char *) &inferior_registers.r_cpsr);
    }
  else
    {
      unsigned pc_val;
      unsigned psr_val;

      regcache_collect (ARM_PC_REGNUM, (char *) &pc_val);
      regcache_collect (ARM_PS_REGNUM, (char *) &psr_val);
	  
      pc_val = ADDR_BITS_REMOVE (pc_val);
      psr_val ^= ADDR_BITS_REMOVE (psr_val);

      inferior_registers.r_pc = pc_val | psr_val;
    }

  ret = ptrace (PT_SETREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    warning ("unable to store general registers");
}

static void
store_fp_register (int regno)
{
  struct fpreg inferior_fp_registers;
  int ret;

  ret = ptrace (PT_GETFPREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_fp_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    {
      warning ("unable to fetch floating-point registers");
      return;
    }

  switch (regno)
    {
    case ARM_FPS_REGNUM:
      regcache_collect (ARM_FPS_REGNUM,
			(char *) &inferior_fp_registers.fpr_fpsr);
      break;

    default:
      regcache_collect
	(regno, (char *) &inferior_fp_registers.fpr[regno - ARM_F0_REGNUM]);
      break;
    }

  ret = ptrace (PT_SETFPREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_fp_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    warning ("unable to write register %d to inferior", regno);
}

static void
store_fp_regs (void)
{
  struct fpreg inferior_fp_registers;
  int ret;
  int regno;


  for (regno = ARM_F0_REGNUM; regno <= ARM_F7_REGNUM; regno++)
    regcache_collect
      (regno, (char *) &inferior_fp_registers.fpr[regno - ARM_F0_REGNUM]);

  regcache_collect (ARM_FPS_REGNUM, (char *) &inferior_fp_registers.fpr_fpsr);

  ret = ptrace (PT_SETFPREGS, PIDGET (inferior_ptid),
		(PTRACE_ARG3_TYPE) &inferior_fp_registers, TIDGET (inferior_ptid));

  if (ret < 0)
    warning ("unable to store floating-point registers");
}

void
store_inferior_registers (int regno)
{
  if (regno >= 0)
    {
      if (regno < ARM_F0_REGNUM || regno > ARM_FPS_REGNUM)
	store_register (regno);
      else
	store_fp_register (regno);
    }
  else
    {
      store_regs ();
      store_fp_regs ();
    }
}

struct md_core
{
  struct reg intreg;
  struct fpreg freg;
};

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
		      int which, CORE_ADDR ignore)
{
  struct md_core *core_reg = (struct md_core *) core_reg_sect;
  int regno;
  CORE_ADDR r_pc;

  supply_gregset (&core_reg->intreg);
  supply_fpregset (&core_reg->freg);
}

static void
fetch_elfcore_registers (char *core_reg_sect, unsigned core_reg_size,
			 int which, CORE_ADDR ignore)
{
  struct reg gregset;
  struct fpreg fpregset;

  switch (which)
    {
    case 0:	/* Integer registers.  */
      if (core_reg_size != sizeof (struct reg))
	warning ("wrong size of register set in core file");
      else
	{
	  /* The memcpy may be unnecessary, but we can't really be sure
	     of the alignment of the data in the core file.  */
	  memcpy (&gregset, core_reg_sect, sizeof (gregset));
	  supply_gregset (&gregset);
	}
      break;

    case 2:
      if (core_reg_size != sizeof (struct fpreg))
	warning ("wrong size of FPA register set in core file");
      else
	{
	  /* The memcpy may be unnecessary, but we can't really be sure
	     of the alignment of the data in the core file.  */
	  memcpy (&fpregset, core_reg_sect, sizeof (fpregset));
	  supply_fpregset (&fpregset);
	}
      break;

    default:
      /* Don't know what kind of register request this is; just ignore it.  */
      break;
    }
}

static struct core_fns arm_netbsd_core_fns =
{
  bfd_target_unknown_flavour,		/* core_flovour.  */
  default_check_format,			/* check_format.  */
  default_core_sniffer,			/* core_sniffer.  */
  fetch_core_registers,			/* core_read_registers.  */
  NULL
};

static struct core_fns arm_netbsd_elfcore_fns =
{
  bfd_target_elf_flavour,		/* core_flovour.  */
  default_check_format,			/* check_format.  */
  default_core_sniffer,			/* core_sniffer.  */
  fetch_elfcore_registers,		/* core_read_registers.  */
  NULL
};

void
_initialize_arm_netbsd_nat (void)
{
  add_core_fns (&arm_netbsd_core_fns);
  add_core_fns (&arm_netbsd_elfcore_fns);
}
