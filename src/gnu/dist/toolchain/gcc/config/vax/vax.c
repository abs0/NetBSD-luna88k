/* Subroutines for insn-output.c for Vax.
   Copyright (C) 1987, 1994, 1995, 1997, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "insn-codes.h"
#include "conditions.h"
#include "insn-flags.h"
/*#include "function.h"*/
#include "output.h"
#include "insn-attr.h"
#ifdef VMS_TARGET
#include "tree.h"
#endif
#include "reload.h"
#include "recog.h"
/*#include "tm_p.h"*/

/* This is like nonimmediate_operand with a restriction on the type of MEM.  */

void
split_quadword_operands (operands, low, n)
     rtx *operands, *low;
     int n;
{
  int i;
  /* Split operands.  */

  for (i = 0; i < n; i++)
    low[i] = 0;
  for (i = 0; i < n; i++)
    {
      if (low[i])
	/* it's already been figured out */;
      else if (GET_CODE (operands[i]) == MEM
	       && (GET_CODE (XEXP (operands[i], 0)) == POST_INC))
	{
	  rtx addr = XEXP (operands[i], 0);
	  operands[i] = low[i] = gen_rtx_MEM (SImode, addr);
	  if (which_alternative == 0 && i == 0)
	    {
	      addr = XEXP (operands[i], 0);
	      operands[i+1] = low[i+1] = gen_rtx (MEM, SImode, addr);
	    }
	}
      else
	{
	  low[i] = operand_subword (operands[i], 0, 0, DImode);
	  operands[i] = operand_subword (operands[i], 1, 0, DImode);
	}
    }
}

print_operand_address (file, addr)
     FILE *file;
     register rtx addr;
{
  register rtx reg1, breg, ireg;
  rtx offset;
  rtx orig_addr = addr;

#if 0
  if (GET_CODE (addr) == PLUS && GET_CODE (XEXP (addr, 1)) == CONST)
    debug_rtx (addr);
#endif
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
  if (flag_pic && GET_CODE (addr) == CONST &&
      GET_CODE (XEXP (addr, 0)) == PLUS &&
      GET_CODE (XEXP (XEXP (addr, 0), 0)) == SYMBOL_REF &&
      !SYMBOL_REF_FLAG (XEXP (XEXP (addr, 0), 0)) &&
      GET_CODE (XEXP (XEXP (addr, 0), 1)) == CONST_INT)
    {
      fatal_insn ("Non-PIC operand escaped:\n", addr);
    }
#endif

 retry:
  switch (GET_CODE (addr))
    {
    case MEM:
      fprintf (file, "*");
      addr = XEXP (addr, 0);
      goto retry;

    case REG:
      if (REGNO (addr) >= 16)
	{
	  debug_rtx (orig_addr);
	  abort ();
	}
      fprintf (file, "(%s%s)", REGISTER_PREFIX, reg_names[REGNO (addr)]);
      break;

    case PRE_DEC:
      fprintf (file, "-(%s%s)", REGISTER_PREFIX, reg_names[REGNO (XEXP (addr, 0))]);
      break;

    case POST_INC:
      fprintf (file, "(%s%s)+", REGISTER_PREFIX, reg_names[REGNO (XEXP (addr, 0))]);
      break;

    case PLUS:
      /* There can be either two or three things added here.  One must be a
	 REG.  One can be either a REG or a MULT of a REG and an appropriate
	 constant, and the third can only be a constant or a MEM.

	 We get these two or three things and put the constant or MEM in
	 OFFSET, the MULT or REG in IREG, and the REG in BREG.  If we have
	 a register and can't tell yet if it is a base or index register,
	 put it into REG1.  */

      reg1 = 0; ireg = 0; breg = 0; offset = 0;

      if (CONSTANT_ADDRESS_P (XEXP (addr, 0))
	  || GET_CODE (XEXP (addr, 0)) == MEM)
	{
	  offset = XEXP (addr, 0);
	  addr = XEXP (addr, 1);
	}
      else if (CONSTANT_ADDRESS_P (XEXP (addr, 1))
	       || GET_CODE (XEXP (addr, 1)) == MEM)
	{
	  offset = XEXP (addr, 1);
	  addr = XEXP (addr, 0);
	}
      else if (GET_CODE (XEXP (addr, 1)) == MULT)
	{
	  ireg = XEXP (addr, 1);
	  addr = XEXP (addr, 0);
	}
      else if (GET_CODE (XEXP (addr, 0)) == MULT)
	{
	  ireg = XEXP (addr, 0);
	  addr = XEXP (addr, 1);
	}
      else if (GET_CODE (XEXP (addr, 1)) == REG)
	{
	  reg1 = XEXP (addr, 1);
	  addr = XEXP (addr, 0);
	}
      else if (GET_CODE (XEXP (addr, 0)) == REG)
	{
	  reg1 = XEXP (addr, 0);
	  addr = XEXP (addr, 1);
	}
      else
	{
	  debug_rtx (orig_addr);
	  abort ();
	}

      if (GET_CODE (addr) == REG)
	{
	  if (reg1)
	    ireg = addr;
	  else
	    reg1 = addr;
	}
      else if (GET_CODE (addr) == MULT)
	ireg = addr;
      else if (GET_CODE (addr) == PLUS)
	{
	  if (CONSTANT_ADDRESS_P (XEXP (addr, 0))
	      || GET_CODE (XEXP (addr, 0)) == MEM)
	    {
	      if (offset)
		{
		  if (GET_CODE (offset) == CONST_INT)
		    offset = plus_constant (XEXP (addr, 0), INTVAL (offset));
		  else if (GET_CODE (XEXP (addr, 0)) == CONST_INT)
		    offset = plus_constant (offset, INTVAL (XEXP (addr, 0)));
		  else
		    {
		      debug_rtx (orig_addr);
		      abort ();
		    }
		}
	      offset = XEXP (addr, 0);
	    }
	  else if (GET_CODE (XEXP (addr, 0)) == REG)
	    {
	      if (reg1)
		ireg = reg1, breg = XEXP (addr, 0), reg1 = 0;
	      else
		reg1 = XEXP (addr, 0);
	    }
	  else if (GET_CODE (XEXP (addr, 0)) == MULT)
	    {
	      if (ireg)
		{
		  debug_rtx (orig_addr);
		  abort ();
		}
	      ireg = XEXP (addr, 0);
	    }
	  else
	    {
	      debug_rtx (orig_addr);
	      abort ();
	    }

	  if (CONSTANT_ADDRESS_P (XEXP (addr, 1))
	      || GET_CODE (XEXP (addr, 1)) == MEM)
	    {
	      if (offset)
		{
		  if (GET_CODE (offset) == CONST_INT)
		    offset = plus_constant (XEXP (addr, 1), INTVAL (offset));
		  else if (GET_CODE (XEXP (addr, 1)) == CONST_INT)
		    offset = plus_constant (offset, INTVAL (XEXP (addr, 1)));
		  else
		    {
		      debug_rtx (orig_addr);
		      abort ();
		    }
		}
	      offset = XEXP (addr, 1);
	    }
	  else if (GET_CODE (XEXP (addr, 1)) == REG)
	    {
	      if (reg1)
		ireg = reg1, breg = XEXP (addr, 1), reg1 = 0;
	      else
		reg1 = XEXP (addr, 1);
	    }
	  else if (GET_CODE (XEXP (addr, 1)) == MULT)
	    {
	      if (ireg)
		{
		  debug_rtx (orig_addr);
		  abort ();
		}
	      ireg = XEXP (addr, 1);
	    }
	  else if (GET_CODE (addr) == SYMBOL_REF)
	    {
	      output_addr_const (file, addr);
	      if (offset != 0)
		{
		  fputc ('+', file);
		  output_address (offset);
		  offset = 0;
		}
	    }
	  else
	    {
	      debug_rtx (orig_addr);
	      abort ();
	    }
	}
      else
	{
	  debug_rtx (orig_addr);
	  abort ();
	}

      /* If REG1 is non-zero, figure out if it is a base or index register.  */
      if (reg1)
	{
	  if (breg != 0
	      || GET_CODE (addr) == SYMBOL_REF
	      || (offset
		   && (GET_CODE (offset) == MEM
		       || GET_CODE (offset) == SYMBOL_REF
		       || GET_CODE (offset) == CONST)))
	    {
	      if (ireg)
		{
		  debug_rtx (orig_addr);
		  abort ();
		}
	      ireg = reg1;
	    }
	  else
	    breg = reg1;
	}

      if (offset != 0)
	output_address (offset);

      if (breg != 0)
	{
	  if (REGNO (breg) >= 16)
	    {
	      debug_rtx (orig_addr);
	      abort ();
	    }
	  fprintf (file, "(%s%s)", REGISTER_PREFIX, reg_names[REGNO (breg)]);
	}

      if (ireg != 0)
	{
	  if (GET_CODE (ireg) == MULT)
	    ireg = XEXP (ireg, 0);
	  if (GET_CODE (ireg) != REG)
	    {
	      debug_rtx (orig_addr);
	      abort ();
	    }
	  fprintf (file, "[%s%s]", REGISTER_PREFIX, reg_names[REGNO (ireg)]);
	}
      break;

    default:
      output_addr_const (file, addr);
    }
}

const char *
rev_cond_name (op)
     rtx op;
{
  switch (GET_CODE (op))
    {
    case EQ:
      return "neq";
    case NE:
      return "eql";
    case LT:
      return "geq";
    case LE:
      return "gtr";
    case GT:
      return "leq";
    case GE:
      return "lss";
    case LTU:
      return "gequ";
    case LEU:
      return "gtru";
    case GTU:
      return "lequ";
    case GEU:
      return "lssu";

    default:
      abort ();
    }
}

int
vax_float_literal(c)
    register rtx c;
{
  register enum machine_mode mode;
#if HOST_FLOAT_FORMAT == VAX_FLOAT_FORMAT
  int i;
  union {double d; int i[2];} val;
#endif

  if (GET_CODE (c) != CONST_DOUBLE)
    return 0;

  mode = GET_MODE (c);

  if (c == const_tiny_rtx[(int) mode][0]
      || c == const_tiny_rtx[(int) mode][1]
      || c == const_tiny_rtx[(int) mode][2])
    return 1;

#if HOST_FLOAT_FORMAT == VAX_FLOAT_FORMAT

  val.i[0] = CONST_DOUBLE_LOW (c);
  val.i[1] = CONST_DOUBLE_HIGH (c);

  for (i = 0; i < 7; i ++)
    if (val.d == 1 << i || val.d == 1 / (1 << i))
      return 1;
#endif
  return 0;
}


/* Return the cost in cycles of a memory address, relative to register
   indirect.

   Each of the following adds the indicated number of cycles:

   1 - symbolic address
   1 - pre-decrement
   1 - indexing and/or offset(register)
   2 - indirect */


int
vax_address_cost (addr)
    register rtx addr;
{
  int reg = 0, indexed = 0, indir = 0, offset = 0, predec = 0;
  rtx plus_op0 = 0, plus_op1 = 0;
 restart:
  switch (GET_CODE (addr))
    {
    case PRE_DEC:
      predec = 1;
    case REG:
    case SUBREG:
    case POST_INC:
      reg = 1;
      break;
    case MULT:
      indexed = 1;	/* 2 on VAX 2 */
      break;
    case CONST_INT:
      /* byte offsets cost nothing (on a VAX 2, they cost 1 cycle) */
      if (offset == 0)
	offset = (unsigned)(INTVAL(addr)+128) > 256;
      break;
    case CONST:
    case SYMBOL_REF:
      offset = 1;	/* 2 on VAX 2 */
      break;
    case LABEL_REF:	/* this is probably a byte offset from the pc */
      if (offset == 0)
	offset = 1;
      break;
    case PLUS:
      if (plus_op0)
	plus_op1 = XEXP (addr, 0);
      else
	plus_op0 = XEXP (addr, 0);
      addr = XEXP (addr, 1);
      goto restart;
    case MEM:
      indir = 2;	/* 3 on VAX 2 */
      addr = XEXP (addr, 0);
      goto restart;
    default:
      break;
    }

  /* Up to 3 things can be added in an address.  They are stored in
     plus_op0, plus_op1, and addr.  */

  if (plus_op0)
    {
      addr = plus_op0;
      plus_op0 = 0;
      goto restart;
    }
  if (plus_op1)
    {
      addr = plus_op1;
      plus_op1 = 0;
      goto restart;
    }
  /* Indexing and register+offset can both be used (except on a VAX 2)
     without increasing execution time over either one alone. */
  if (reg && indexed && offset)
    return reg + indir + offset + predec;
  return reg + indexed + indir + offset + predec;
}


/* Cost of an expression on a VAX.  This version has costs tuned for the
   CVAX chip (found in the VAX 3 series) with comments for variations on
   other models.  */

int
vax_rtx_cost (x)
    register rtx x;
{
  register enum rtx_code code = GET_CODE (x);
  enum machine_mode mode = GET_MODE (x);
  register int c;
  int i = 0;				/* may be modified in switch */
  const char *fmt = GET_RTX_FORMAT (code); /* may be modified in switch */

  switch (code)
    {
    case POST_INC:
      return 2;
    case PRE_DEC:
      return 3;
    case MULT:
      switch (mode)
	{
	case DFmode:
	  c = 16;		/* 4 on VAX 9000 */
	  break;
	case SFmode:
	  c = 9;		/* 4 on VAX 9000, 12 on VAX 2 */
	  break;
	case DImode:
	  c = 16;		/* 6 on VAX 9000, 28 on VAX 2 */
	  break;
	case SImode:
	case HImode:
	case QImode:
	  c = 10;		/* 3-4 on VAX 9000, 20-28 on VAX 2 */
	  break;
	default:
	  break;
	}
      break;
    case UDIV:
      c = 17;
      break;
    case DIV:
      if (mode == DImode)
	c = 30;	/* highly variable */
      else if (mode == DFmode)
	/* divide takes 28 cycles if the result is not zero, 13 otherwise */
	c = 24;
      else
	c = 11;			/* 25 on VAX 2 */
      break;
    case MOD:
      c = 23;
      break;
    case UMOD:
      c = 29;
      break;
    case FLOAT:
      c = 6 + (mode == DFmode) + (GET_MODE (XEXP (x, 0)) != SImode);
      /* 4 on VAX 9000 */
      break;
    case FIX:
      c = 7;			/* 17 on VAX 2 */
      break;
    case ASHIFT:
    case LSHIFTRT:
    case ASHIFTRT:
      if (mode == DImode)
	c = 12;
      else
	c = 10;			/* 6 on VAX 9000 */
      break;
    case ROTATE:
    case ROTATERT:
      c = 6;			/* 5 on VAX 2, 4 on VAX 9000 */
      if (GET_CODE (XEXP (x, 1)) == CONST_INT)
	fmt = "e";	/* all constant rotate counts are short */
      break;
    case PLUS:
      /* Check for small negative integer operand: subl2 can be used with
	 a short positive constant instead.  */
      if (GET_CODE (XEXP (x, 1)) == CONST_INT)
	if ((unsigned)(INTVAL (XEXP (x, 1)) + 63) < 127)
	  fmt = "e";
    case MINUS:
      c = (mode == DFmode) ? 13 : 8;	/* 6/8 on VAX 9000, 16/15 on VAX 2 */
    case IOR:
    case XOR:
      c = 3;
      break;
    case AND:
      /* AND is special because the first operand is complemented. */
      c = 3;
      if (GET_CODE (XEXP (x, 0)) == CONST_INT)
	{
	  if ((unsigned)~INTVAL (XEXP (x, 0)) > 63)
	    c = 4;
	  fmt = "e";
	  i = 1;
	}
      break;
    case NEG:
      if (mode == DFmode)
	return 9;
      else if (mode == SFmode)
	return 6;
      else if (mode == DImode)
	return 4;
    case NOT:
      return 2;
    case ZERO_EXTRACT:
    case SIGN_EXTRACT:
      c = 15;
      break;
    case MEM:
      if (mode == DImode || mode == DFmode)
	c = 5;				/* 7 on VAX 2 */
      else
	c = 3;				/* 4 on VAX 2 */
      x = XEXP (x, 0);
      if (GET_CODE (x) == REG || GET_CODE (x) == POST_INC)
	return c;
      return c + vax_address_cost (x);
    default:
      c = 3;
      break;
    }


  /* Now look inside the expression.  Operands which are not registers or
     short constants add to the cost.

     FMT and I may have been adjusted in the switch above for instructions
     which require special handling */

  while (*fmt++ == 'e')
    {
      register rtx op = XEXP (x, i++);
      code = GET_CODE (op);

      /* A NOT is likely to be found as the first operand of an AND
	 (in which case the relevant cost is of the operand inside
	 the not) and not likely to be found anywhere else.  */
      if (code == NOT)
	op = XEXP (op, 0), code = GET_CODE (op);

      switch (code)
	{
	case CONST_INT:
	  if ((unsigned)INTVAL (op) > 63 && GET_MODE (x) != QImode)
	    c += 1;		/* 2 on VAX 2 */
	  break;
	case CONST:
	case LABEL_REF:
	case SYMBOL_REF:
	  c += 1;		/* 2 on VAX 2 */
	  break;
	case CONST_DOUBLE:
	  if (GET_MODE_CLASS (GET_MODE (op)) == MODE_FLOAT)
	    {
	      /* Registers are faster than floating point constants -- even
		 those constants which can be encoded in a single byte.  */
	      if (vax_float_literal (op))
		c++;
	      else
		c += (GET_MODE (x) == DFmode) ? 3 : 2;
	    }
	  else
	    {
	      if (CONST_DOUBLE_HIGH (op) != 0
		  || (unsigned)CONST_DOUBLE_LOW (op) > 63)
		c += 2;
	    }
	  break;
	case MEM:
	  c += 1;		/* 2 on VAX 2 */
	  if (GET_CODE (XEXP (op, 0)) != REG)
	    c += vax_address_cost (XEXP (op, 0));
	  break;
	case REG:
	case SUBREG:
	  break;
	default:
	  c += 1;
	  break;
	}
    }
  return c;
}

/* Check a `double' value for validity for a particular machine mode.  */

static const char *const float_strings[] =
{
   "1.70141173319264430e+38", /* 2^127 (2^24 - 1) / 2^24 */
  "-1.70141173319264430e+38",
   "2.93873587705571877e-39", /* 2^-128 */
  "-2.93873587705571877e-39"
};

static REAL_VALUE_TYPE float_values[4];

static int inited_float_values = 0;


int
check_float_value (mode, d, overflow)
     enum machine_mode mode;
     REAL_VALUE_TYPE *d;
     int overflow;
{
  if (inited_float_values == 0)
    {
      int i;
      for (i = 0; i < 4; i++)
	{
	  float_values[i] = REAL_VALUE_ATOF (float_strings[i], DFmode);
	}

      inited_float_values = 1;
    }

  if (overflow)
    {
      bcopy ((char *) &float_values[0], (char *) d, sizeof (REAL_VALUE_TYPE));
      return 1;
    }

  if ((mode) == SFmode)
    {
      REAL_VALUE_TYPE r;
      memcpy (&r, d, sizeof (REAL_VALUE_TYPE));
      if (REAL_VALUES_LESS (float_values[0], r))
	{
	  bcopy ((char *) &float_values[0], (char *) d,
		 sizeof (REAL_VALUE_TYPE));
	  return 1;
	}
      else if (REAL_VALUES_LESS (r, float_values[1]))
	{
	  bcopy ((char *) &float_values[1], (char*) d,
		 sizeof (REAL_VALUE_TYPE));
	  return 1;
	}
      else if (REAL_VALUES_LESS (dconst0, r)
		&& REAL_VALUES_LESS (r, float_values[2]))
	{
	  bcopy ((char *) &dconst0, (char *) d, sizeof (REAL_VALUE_TYPE));
	  return 1;
	}
      else if (REAL_VALUES_LESS (r, dconst0)
		&& REAL_VALUES_LESS (float_values[3], r))
	{
	  bcopy ((char *) &dconst0, (char *) d, sizeof (REAL_VALUE_TYPE));
	  return 1;
	}
    }

  return 0;
}

/* Nonzero if X is a hard reg that can be used as an index.  */
#define XREG_OK_FOR_INDEX_P(X, STRICT) (!(STRICT) || REGNO_OK_FOR_INDEX_P (REGNO (X)))
/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define XREG_OK_FOR_BASE_P(X, STRICT) (!(STRICT) || REGNO_OK_FOR_BASE_P (REGNO (X)))

#ifdef NO_EXTERNAL_INDIRECT_ADDRESS

/* Re-definition of CONSTANT_ADDRESS_P, which is true only when there
   are no SYMBOL_REFs for external symbols present and allow valid
   addressing modes.  */

#define INDIRECTABLE_CONSTANT_ADDRESS_P(X, INDEXED, INDIRECT)		\
  (GET_CODE (X) == LABEL_REF 						\
   || (!INDEXED && GET_CODE (X) == SYMBOL_REF				\
       && (!INDIRECT || SYMBOL_REF_FLAG (X)))				\
   || (!INDEXED && GET_CODE (X) == CONST				\
       && GET_CODE (XEXP ((X), 0)) == PLUS				\
       && GET_CODE (XEXP (XEXP ((X), 0), 0)) == SYMBOL_REF		\
       && ((!INDIRECT && !flag_pic)					\
           || SYMBOL_REF_FLAG (XEXP (XEXP ((X), 0), 0))))		\
   || GET_CODE (X) == CONST_INT)

/* Non-zero if X is an address which can be indirected.  External symbols
   could be in a sharable image library, so we disallow those.  */

#define INDIRECTABLE_ADDRESS_P(X, STRICT, INDEXED, INDIRECT)		\
  (INDIRECTABLE_CONSTANT_ADDRESS_P (X, INDEXED, INDIRECT) 		\
   || (GET_CODE (X) == REG && XREG_OK_FOR_BASE_P (X, STRICT))		\
   || (GET_CODE (X) == PLUS						\
       && GET_CODE (XEXP (X, 0)) == REG					\
       && XREG_OK_FOR_BASE_P (XEXP (X, 0), STRICT)			\
       && GET_CODE (XEXP (X, 1)) != SYMBOL_REF				\
       && !(GET_CODE (XEXP (X, 1)) == CONST				\
	    && GET_CODE (XEXP (XEXP (X, 1), 0)) == PLUS			\
	    && GET_CODE (XEXP (XEXP (XEXP (X, 1), 0), 0)) == SYMBOL_REF) \
       && INDIRECTABLE_CONSTANT_ADDRESS_P (XEXP (X, 1), INDEXED, INDIRECT)))

#else /* not NO_EXTERNAL_INDIRECT_ADDRESS */

#define INDIRECTABLE_CONSTANT_ADDRESS_P(X, INDEXED, INDIRECT) \
   CONSTANT_ADDRESS_P (X)

/* Non-zero if X is an address which can be indirected.  */
#define INDIRECTABLE_ADDRESS_P(X, STRICT, INDEXED, INDIRECT)	\
  (INDIRECTABLE_CONSTANT_ADDRESS_P (X, INDEXED, INDIRECT)	\
   || (GET_CODE (X) == REG && XREG_OK_FOR_BASE_P (X, STRICT))	\
   || (GET_CODE (X) == PLUS					\
       && GET_CODE (XEXP (X, 0)) == REG				\
       && XREG_OK_FOR_BASE_P (XEXP (X, 0), STRICT)		\
       && CONSTANT_ADDRESS_P (XEXP (X, 1))))

#endif /* not NO_EXTERNAL_INDIRECT_ADDRESS */

/* Go to ADDR if X is a valid address not using indexing.
   (This much is the easy part.)  */
#define GO_IF_NONINDEXED_ADDRESS(X, ADDR, STRICT, INDEXED)		\
{ register rtx xfoob = (X);						\
  if (GET_CODE (X) == REG)						\
    {									\
      extern rtx *reg_equiv_mem;					\
      if (! reload_in_progress)						\
	goto ADDR;							\
      if (!STRICT)							\
	{								\
	  if ((xfoob = reg_equiv_mem[REGNO (xfoob)]) == 0)		\
	    goto ADDR;							\
	}								\
      if (INDIRECTABLE_ADDRESS_P (xfoob, STRICT, INDEXED, 0))		\
	goto ADDR;							\
    }									\
  if (INDIRECTABLE_CONSTANT_ADDRESS_P (X, INDEXED, 0)) goto ADDR;	\
  if (INDIRECTABLE_ADDRESS_P (X, STRICT, INDEXED, 0)) goto ADDR;	\
  xfoob = XEXP (X, 0);							\
  if (GET_CODE (X) == MEM						\
      && INDIRECTABLE_ADDRESS_P (xfoob, STRICT, INDEXED, !TARGET_INDIRECT)) \
    goto ADDR;								\
  if ((GET_CODE (X) == PRE_DEC || GET_CODE (X) == POST_INC)		\
      && GET_CODE (xfoob) == REG					\
      && XREG_OK_FOR_BASE_P (xfoob, STRICT))				\
    goto ADDR; }

/* 1 if PROD is either a reg times size of mode MODE
   or just a reg, if MODE is just one byte.
   This macro's expansion uses the temporary variables xfoo0 and xfoo1
   that must be declared in the surrounding context.  */
#define INDEX_TERM_P(PROD, MODE, STRICT)   \
(GET_MODE_SIZE (MODE) == 1						\
 ? (GET_CODE (PROD) == REG && XREG_OK_FOR_BASE_P (PROD, STRICT))	\
 : (GET_CODE (PROD) == MULT						\
    &&									\
    (xfoo0 = XEXP (PROD, 0), xfoo1 = XEXP (PROD, 1),			\
     ((GET_CODE (xfoo0) == CONST_INT					\
       && GET_CODE (xfoo1) == REG					\
       && INTVAL (xfoo0) == (int)GET_MODE_SIZE (MODE)			\
       && XREG_OK_FOR_INDEX_P (xfoo1, STRICT))				\
      ||								\
      (GET_CODE (xfoo1) == CONST_INT					\
       && GET_CODE (xfoo0) == REG					\
       && INTVAL (xfoo1) == (int)GET_MODE_SIZE (MODE)			\
       && XREG_OK_FOR_INDEX_P (xfoo0, STRICT))))))

/* Go to ADDR if X is the sum of a register
   and a valid index term for mode MODE.  */
#define GO_IF_REG_PLUS_INDEX(X, MODE, ADDR, STRICT)	\
{ register rtx xfooa;					\
  if (GET_CODE (X) == PLUS)				\
    { if (GET_CODE (XEXP (X, 0)) == REG			\
	  && XREG_OK_FOR_BASE_P (XEXP (X, 0), STRICT)	\
	  && (xfooa = XEXP (X, 1),			\
	      INDEX_TERM_P (xfooa, MODE, STRICT)))	\
	goto ADDR;					\
      if (GET_CODE (XEXP (X, 1)) == REG			\
	  && XREG_OK_FOR_BASE_P (XEXP (X, 1), STRICT)	\
	  && (xfooa = XEXP (X, 0),			\
	      INDEX_TERM_P (xfooa, MODE, STRICT)))	\
	goto ADDR; } }

int
legitimate_pic_operand_p(x, strict)
     register rtx x;
     int strict ATTRIBUTE_UNUSED;
{
  if (GET_CODE (x) != SYMBOL_REF
      && !(GET_CODE (x) == CONST
	  && GET_CODE (XEXP (x, 0)) == PLUS
	  && GET_CODE (XEXP (XEXP (x, 0), 0)) == SYMBOL_REF))
    {
      return 1;
    }
  return 0;
}

int
legitimate_address_p(mode, xbar, strict)
     enum machine_mode mode;
     register rtx xbar;
     int strict;
{
  register rtx xfoo, xfoo0, xfoo1;
  int from = __LINE__;
  GO_IF_NONINDEXED_ADDRESS (xbar, win, strict, 0);
  if (GET_CODE (xbar) == PLUS)
    {
      /* Handle <address>[index] represented with index-sum outermost */
      xfoo = XEXP (xbar, 0);
      if (INDEX_TERM_P (xfoo, mode, strict))
	{
	  from = __LINE__;
	  GO_IF_NONINDEXED_ADDRESS (XEXP (xbar, 1), win, strict, 0);
	}
      xfoo = XEXP (xbar, 1);
      if (INDEX_TERM_P (xfoo, mode, strict))
	{
	  from = __LINE__;
	  GO_IF_NONINDEXED_ADDRESS (XEXP (xbar, 0), win, strict, 0);
	}
      /* Handle offset(reg)[index] with offset added outermost */
      if (INDIRECTABLE_CONSTANT_ADDRESS_P (XEXP (xbar, 0), 1, 0))
	{
	  from = __LINE__;
	   if (GET_CODE (XEXP (xbar, 1)) == REG
	      && XREG_OK_FOR_BASE_P (XEXP (xbar, 1), strict))
	    goto win;
	  from = __LINE__;
	  GO_IF_REG_PLUS_INDEX (XEXP (xbar, 1), mode, win, strict);
	}
      if (INDIRECTABLE_CONSTANT_ADDRESS_P (XEXP (xbar, 1), 1, 0))
	{
	  from = __LINE__;
	  if (GET_CODE (XEXP (xbar, 0)) == REG
	      && XREG_OK_FOR_BASE_P (XEXP (xbar, 0), strict))
	    goto win;
	  from = __LINE__;
	  GO_IF_REG_PLUS_INDEX (XEXP (xbar, 0), mode, win, strict);
	}
  }
  return 0;

 win:
#if 0
  if (strict)
    {
      fprintf(stderr, "line=%d\n", from);
      debug_rtx (xbar);
    }
#endif
  if (flag_pic && GET_CODE (xbar) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
	     && !SYMBOL_REF_FLAG (xbar)
#endif
	     && mode == DImode)
    return 0;
  return 1;
}

int
vax_symbolic_operand (op, mode)
     register rtx op;
     enum machine_mode mode ATTRIBUTE_UNUSED;
{
  if (!general_operand(op, mode))
    return 0;
  if (GET_CODE (op) == SYMBOL_REF
	 || GET_CODE (op) == LABEL_REF
	 || (GET_CODE (op) == CONST
	     && GET_CODE (XEXP (op, 0)) == PLUS
	     && GET_CODE (XEXP (XEXP (op, 0), 0)) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
	     && (SYMBOL_REF_FLAG (XEXP (XEXP (op, 0), 0)) || !flag_pic)
#endif
	     && GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT)
	 || (GET_CODE (op) == PLUS
	     && GET_CODE (XEXP (op, 1)) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
	     && (SYMBOL_REF_FLAG (XEXP (op, 1)) || !flag_pic)
#endif
	     && GET_CODE (XEXP (op, 0)) == CONST_INT)
	 || (GET_CODE (op) == PLUS
	     && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
	     && (SYMBOL_REF_FLAG (XEXP (op, 0)) || !flag_pic)
#endif
	     && GET_CODE (XEXP (op, 1)) == CONST_INT))
    {
      return 1;
    }
  return 0;
}

int
vax_nonsymbolic_operand (op, mode)
     register rtx op;
     enum machine_mode mode ATTRIBUTE_UNUSED;
{
  if (!general_operand(op, mode))
    return 0;
  if (GET_CODE (op) == SYMBOL_REF
	 || GET_CODE (op) == LABEL_REF
	 || (GET_CODE (op) == CONST
	     && GET_CODE (XEXP (op, 0)) == PLUS
	     && GET_CODE (XEXP (XEXP (op, 0), 0)) == SYMBOL_REF)
	 || (GET_CODE (op) == MEM
	     && GET_CODE (XEXP (op, 0)) == CONST
	     && GET_CODE (XEXP (XEXP (op, 0), 0)) == PLUS
	     && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 0)) == SYMBOL_REF)
	 || (GET_CODE (op) == PLUS
	     && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	     && GET_CODE (XEXP (op, 1)) == CONST_INT))
    return 0;
#if 0
  if (GET_CODE (op) == PLUS)
    debug_rtx (op);
#endif
  if (vax_symbolic_operand (op, mode))
    return 0;
#if 0
  if (GET_CODE (op) != CONST_INT && GET_CODE (op) != REG &&
      GET_CODE (op) != MEM)
    debug_rtx (op);
#endif
  return 1;
}

int
vax_lvalue_operand(op, mode)
     register rtx op;
     enum machine_mode mode;
{
  if (!general_operand(op, mode))
    return 0;
  return GET_CODE (op) == REG
	|| GET_CODE (op) == SUBREG
	|| GET_CODE (op) == MEM
	|| GET_CODE (op) == CONCAT
	|| GET_CODE (op) == PARALLEL
	|| GET_CODE (op) == STRICT_LOW_PART;
}

int
vax_general_operand(op, mode)
     register rtx op;
     enum machine_mode mode;
{
  if (!general_operand(op, mode))
    return 0;
  if (!flag_pic)
    return 1;
  if ((GET_CODE (op) == CONST
       && GET_CODE (XEXP (op, 0)) == PLUS
       && GET_CODE (XEXP (XEXP (op, 0), 0)) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
       && !SYMBOL_REF_FLAG (XEXP (XEXP (op, 0), 0))
#endif
      ) || (GET_CODE (op) == MEM
          && GET_CODE (XEXP (op, 0)) == CONST
          && GET_CODE (XEXP (XEXP (op, 0), 0)) == PLUS
          && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 0)) == SYMBOL_REF
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
          && !SYMBOL_REF_FLAG (XEXP (XEXP (op, 0), 0))
#endif
      ) || (GET_CODE (op) == PLUS
          && GET_CODE (XEXP (op, 0)) == SYMBOL_REF
          && GET_CODE (XEXP (op, 1)) == CONST_INT
#ifdef NO_EXTERNAL_INDIRECT_ADDRESS
          && !SYMBOL_REF_FLAG (XEXP (op, 0))
#endif
     ))
    return 0;
#if 0
  debug_rtx (op);
#endif
  return 1;
}

int
vax_reg_used_p(operand, reg)
     rtx operand;
     int reg;
{
   if (GET_CODE (operand) == REG && REGNO (operand) == reg)
     return 1;
   if (GET_CODE (operand) == MEM ||
       GET_CODE (operand) == PRE_DEC ||
       GET_CODE (operand) == POST_DEC)
     return vax_reg_used_p (XEXP(operand, 0), reg);
   if (GET_CODE (operand) == PLUS ||
       GET_CODE (operand) == MULT)
     return vax_reg_used_p (XEXP(operand, 0), reg) ||
	    vax_reg_used_p (XEXP(operand, 1), reg);
   return 0;
}

#ifdef VMS_TARGET
/* Additional support code for VMS target. */

/* Linked list of all externals that are to be emitted when optimizing
   for the global pointer if they haven't been declared by the end of
   the program with an appropriate .comm or initialization.  */

static
struct extern_list {
  struct extern_list *next;	/* next external */
  const char *name;		/* name of the external */
  int size;			/* external's actual size */
  int in_const;			/* section type flag */
} *extern_head = 0, *pending_head = 0;

/* Check whether NAME is already on the external definition list.  If not,
   add it to either that list or the pending definition list.  */

void
vms_check_external (decl, name, pending)
     tree decl;
     const char *name;
     int pending;
{
  register struct extern_list *p, *p0;

  for (p = extern_head; p; p = p->next)
    if (!strcmp (p->name, name))
      return;

  for (p = pending_head, p0 = 0; p; p0 = p, p = p->next)
    if (!strcmp (p->name, name))
      {
	if (pending)
	  return;

	/* Was pending, but has now been defined; move it to other list.  */
	if (p == pending_head)
	  pending_head = p->next;
	else
	  p0->next = p->next;
	p->next = extern_head;
	extern_head = p;
	return;
      }

  /* Not previously seen; create a new list entry.  */
  p = (struct extern_list *)permalloc ((long) sizeof (struct extern_list));
  p->name = name;

  if (pending)
    {
      /* Save the size and section type and link to `pending' list.  */
      p->size = (DECL_SIZE (decl) == 0) ? 0 :
	TREE_INT_CST_LOW (size_binop (CEIL_DIV_EXPR, DECL_SIZE (decl),
				      size_int (BITS_PER_UNIT)));
      p->in_const = (TREE_READONLY (decl) && ! TREE_THIS_VOLATILE (decl));

      p->next = pending_head;
      pending_head = p;
    }
  else
    {
      /* Size and section type don't matter; link to `declared' list.  */
      p->size = p->in_const = 0;        /* arbitrary init */

      p->next = extern_head;
      extern_head = p;
    }
  return;
}

void
vms_flush_pending_externals (file)
     FILE *file;
{
  register struct extern_list *p;

  while (pending_head)
    {
      /* Move next pending declaration to the "done" list.  */
      p = pending_head;
      pending_head = p->next;
      p->next = extern_head;
      extern_head = p;

      /* Now output the actual declaration.  */
      if (p->in_const)
	const_section ();
      else
	data_section ();
      fputs (".comm ", file);
      assemble_name (file, p->name);
      fprintf (file, ",%d\n", p->size);
    }
}
#endif /* VMS_TARGET */

#ifdef VMS
/* Additional support code for VMS host. */

#ifdef QSORT_WORKAROUND
  /*
	Do not use VAXCRTL's qsort() due to a severe bug:  once you've
	sorted something which has a size that's an exact multiple of 4
	and is longword aligned, you cannot safely sort anything which
	is either not a multiple of 4 in size or not longword aligned.
	A static "move-by-longword" optimization flag inside qsort() is
	never reset.  This is known of affect VMS V4.6 through VMS V5.5-1,
	and was finally fixed in VMS V5.5-2.

	In this work-around an insertion sort is used for simplicity.
	The qsort code from glibc should probably be used instead.
   */
void
not_qsort (array, count, size, compare)
     void *array;
     unsigned count, size;
     int (*compare)();
{

  if (size == sizeof (short))
    {
      register int i;
      register short *next, *prev;
      short tmp, *base = array;

      for (next = base, i = count - 1; i > 0; i--)
	{
	  prev = next++;
	  if ((*compare)(next, prev) < 0)
	    {
	      tmp = *next;
	      do  *(prev + 1) = *prev;
		while (--prev >= base ? (*compare)(&tmp, prev) < 0 : 0);
	      *(prev + 1) = tmp;
	    }
	}
    }
  else if (size == sizeof (long))
    {
      register int i;
      register long *next, *prev;
      long tmp, *base = array;

      for (next = base, i = count - 1; i > 0; i--)
	{
	  prev = next++;
	  if ((*compare)(next, prev) < 0)
	    {
	      tmp = *next;
	      do  *(prev + 1) = *prev;
		while (--prev >= base ? (*compare)(&tmp, prev) < 0 : 0);
	      *(prev + 1) = tmp;
	    }
	}
    }
  else  /* arbitrary size */
    {
      register int i;
      register char *next, *prev, *tmp = alloca (size), *base = array;

      for (next = base, i = count - 1; i > 0; i--)
	{   /* count-1 forward iterations */
	  prev = next,  next += size;		/* increment front pointer */
	  if ((*compare)(next, prev) < 0)
	    {	/* found element out of order; move others up then re-insert */
	      memcpy (tmp, next, size);		/* save smaller element */
	      do { memcpy (prev + size, prev, size); /* move larger elem. up */
		   prev -= size;		/* decrement back pointer */
		 } while (prev >= base ? (*compare)(tmp, prev) < 0 : 0);
	      memcpy (prev + size, tmp, size);	/* restore small element */
	    }
	}
#ifdef USE_C_ALLOCA
      alloca (0);
#endif
    }

  return;
}
#endif /* QSORT_WORKAROUND */

#endif /* VMS */
