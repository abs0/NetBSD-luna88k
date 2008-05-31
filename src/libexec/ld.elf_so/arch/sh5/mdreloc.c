/*	$NetBSD: mdreloc.c,v 1.3 2003/04/21 11:54:46 scw Exp $	*/

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "debug.h"
#include "rtld.h"

/*
 * Flag added to Elf_Sym->dt_other to indicate SHmedia code
 */
#define	STO_SH5_ISA32	(1 << 2)

void _rtld_bind_start(void);
void _rtld_relocate_nonplt_self(Elf_Dyn *, Elf_Addr);
caddr_t _rtld_bind(const Obj_Entry *, Elf_Word);

void
_rtld_setup_pltgot(const Obj_Entry *obj)
{
	obj->pltgot[1] = (Elf_Addr) obj;
	obj->pltgot[2] = (Elf_Addr) &_rtld_bind_start;
}

void
_rtld_relocate_nonplt_self(Elf_Dyn *dynp, Elf_Addr relocbase)
{
	const Elf_Rela *rela = 0, *relalim;
	Elf_Addr relasz = 0;
	Elf_Addr *where;

	for (; dynp->d_tag != DT_NULL; dynp++) {
		switch (dynp->d_tag) {
		case DT_RELA:
			rela = (const Elf_Rela *)(relocbase + dynp->d_un.d_ptr);
			break;
		case DT_RELASZ:
			relasz = dynp->d_un.d_val;
			break;
		}
	}
	relalim = (const Elf_Rela *)((caddr_t)rela + relasz);
	for (; rela < relalim; rela++) {
		where = (Elf_Addr *)(relocbase + rela->r_offset);
		*where = (Elf_Addr)(relocbase + rela->r_addend);
	}
}

/*
 * It is possible for the compiler to emit relocations for unaligned data.
 * We handle this situation with these inlines.
 */
#define	RELOC_ALIGNED_P(x) \
	(((intptr_t)(x) & (sizeof(void *) - 1)) == 0)

static __inline Elf_Addr
load_ptr(void *where)
{
	Elf_Addr res;

	memcpy(&res, where, sizeof(res));

	return (res);
}

static __inline void
store_ptr(void *where, Elf_Addr val)
{

	memcpy(where, &val, sizeof(val));
}

int
_rtld_relocate_nonplt_objects(const Obj_Entry *obj)
{
	const Elf_Rela *rela;

	for (rela = obj->rela; rela < obj->relalim; rela++) {
		Elf_Addr        *where;
		const Elf_Sym	*def;
		const Obj_Entry	*defobj;
		Elf_Addr	 tmp;
		unsigned long	 symnum;

		where = (Elf_Addr *)(obj->relocbase + rela->r_offset);
		symnum = ELF_R_SYM(rela->r_info);

		switch (ELF_R_TYPE(rela->r_info)) {
		case R_TYPE(NONE):
			break;

		case R_TYPE(DIR32):
		case R_TYPE(GLOB_DAT):
			def = _rtld_find_symdef(symnum, obj, &defobj, false);
			if (def == NULL)
				return -1;

			tmp = (Elf_Addr)(defobj->relocbase + def->st_value +
			    rela->r_addend);
			if (def->st_other & STO_SH5_ISA32)
				tmp |= 1;
			if (__predict_true(RELOC_ALIGNED_P(where))) {
				if (*where != tmp)
					*where = tmp;
			} else {
				if (load_ptr(where) != tmp)
					store_ptr(where, tmp);
			}
			rdbg(("32/GLOB_DAT %s in %s --> %p in %s",
			    obj->strtab + obj->symtab[symnum].st_name,
			    obj->path, (void *)(intptr_t)tmp, defobj->path));
			break;

		case R_TYPE(RELATIVE):
			if (__predict_true(RELOC_ALIGNED_P(where))) {
				if (__predict_true(rela->r_addend) == 0)
					*where += (Elf_Addr)obj->relocbase;
				else
					*where = (Elf_Addr)obj->relocbase +
					    rela->r_addend;
			} else {
				if (__predict_true(rela->r_addend) == 0) {
					tmp = load_ptr(where);
					tmp += (Elf_Addr)obj->relocbase;
				} else {
					tmp = (Elf_Addr)obj->relocbase +
					    rela->r_addend;
				}
				store_ptr(where, tmp);
			}
			rdbg(("RELATIVE in %s --> %p", obj->path,
			    (void *)*where));
			break;

		case R_TYPE(COPY):
			/*
			 * These are deferred until all other relocations have
			 * been done.  All we do here is make sure that the
			 * COPY relocation is not in a shared library.  They
			 * are allowed only in executable files.
			 */
			if (obj->isdynamic) {
				_rtld_error(
			"%s: Unexpected R_COPY relocation in shared library",
				    obj->path);
				return -1;
			}
			rdbg(("COPY (avoid in main)"));
			break;

		default:
			rdbg(("sym = %lu, type = %lu, offset = %p, "
			    "addend = %p, contents = %p, symbol = %s",
			    symnum, (u_long)ELF_R_TYPE(rela->r_info),
			    (void *)rela->r_offset, (void *)rela->r_addend,
			    (void *)*where,
			    obj->strtab + obj->symtab[symnum].st_name));
			_rtld_error("%s: Unsupported relocation type %ld "
			    "in non-PLT relocations\n",
			    obj->path, (u_long) ELF_R_TYPE(rela->r_info));
			return -1;
		}
	}
	return 0;
}

int
_rtld_relocate_plt_lazy(const Obj_Entry *obj)
{
	const Elf_Rela *rela;

	if (!obj->relocbase)
		return 0;

	for (rela = obj->pltrela; rela < obj->pltrelalim; rela++) {
		Elf_Addr *where = (Elf_Addr *)(obj->relocbase + rela->r_offset);

		assert(ELF_R_TYPE(rela->r_info) == R_TYPE(JMP_SLOT));

		/* Just relocate the GOT slots pointing into the PLT */
		*where += (Elf_Addr)obj->relocbase;
		rdbg(("fixup !main in %s --> %p", obj->path, (void *)*where));
	}

	return 0;
}

caddr_t
_rtld_bind(const Obj_Entry *obj, Elf_Word reloff)
{
	const Elf_Rela *rela = (const Elf_Rela *)((caddr_t)obj->pltrela + reloff);
	Elf_Addr *where = (Elf_Addr *)(obj->relocbase + rela->r_offset);
	Elf_Addr new_value;
	const Elf_Sym  *def;
	const Obj_Entry *defobj;

	assert(ELF_R_TYPE(rela->r_info) == R_TYPE(JMP_SLOT));

	def = _rtld_find_symdef(ELF_R_SYM(rela->r_info), obj, &defobj, true);
	if (def == NULL)
		_rtld_die();

	new_value = (Elf_Addr)(defobj->relocbase + def->st_value);
	if (def->st_other & STO_SH5_ISA32)
		new_value |= 1;
	rdbg(("bind now/fixup in %s --> old=%p new=%p",
	    defobj->strtab + def->st_name, (void *)*where, (void *)new_value));
	if (*where != new_value)
		*where = new_value;

	return (caddr_t)new_value;
}

Elf_Addr
_rtld_function_descriptor_alloc(const Obj_Entry *obj, const Elf_Sym *def,
    Elf_Addr addend)
{
	Elf_Addr pc;

	pc = (Elf_Addr)(obj->relocbase + addend);

	if (def) {
		pc += def->st_value;
		if (def->st_other & STO_SH5_ISA32)
			pc |= 1;
	}

	return (pc);
}

const void *
_rtld_function_descriptor_function(const void *addr)
{

	return (addr);
}
