# This file is automatically generated.  DO NOT EDIT!
# Generated from: 	NetBSD: mknative-binutils,v 1.2 2003/11/27 10:52:53 mrg Exp 
#
G_libbfd_la_DEPENDENCIES=elf64-x86-64.lo elf64.lo elf.lo elflink.lo elf-strtab.lo elf-eh-frame.lo dwarf1.lo elf32-i386.lo elf32.lo i386netbsd.lo aout32.lo coff-i386.lo cofflink.lo efi-app-ia32.lo peigen.lo elf64-gen.lo elf32-gen.lo cpu-i386.lo netbsd-core.lo ofiles
G_libbfd_la_OBJECTS=archive.lo archures.lo bfd.lo bfdio.lo bfdwin.lo  cache.lo coffgen.lo corefile.lo format.lo init.lo libbfd.lo opncls.lo  reloc.lo section.lo syms.lo targets.lo hash.lo linker.lo srec.lo  binary.lo tekhex.lo ihex.lo stabs.lo stab-syms.lo merge.lo dwarf2.lo  simple.lo archive64.lo
G_DEFS=-DHAVE_CONFIG_H -I. -I${GNUHOSTDIST}/bfd -I.
G_INCLUDES=-D_GNU_SOURCE  -DNETBSD_CORE   -I. -I${GNUHOSTDIST}/bfd -I${GNUHOSTDIST}/bfd/../include  -I${GNUHOSTDIST}/bfd/../intl -I../intl
G_TDEFAULTS=-DDEFAULT_VECTOR=bfd_elf64_x86_64_vec -DSELECT_VECS='&bfd_elf64_x86_64_vec,&bfd_elf32_i386_vec,&i386netbsd_vec,&i386coff_vec,&bfd_efi_app_ia32_vec,&bfd_elf64_little_generic_vec,&bfd_elf64_big_generic_vec,&bfd_elf32_little_generic_vec,&bfd_elf32_big_generic_vec' -DSELECT_ARCHITECTURES='&bfd_i386_arch' -DHAVE_bfd_elf64_x86_64_vec -DHAVE_bfd_elf32_i386_vec -DHAVE_i386netbsd_vec -DHAVE_i386coff_vec -DHAVE_bfd_efi_app_ia32_vec -DHAVE_bfd_elf64_little_generic_vec -DHAVE_bfd_elf64_big_generic_vec -DHAVE_bfd_elf32_little_generic_vec -DHAVE_bfd_elf32_big_generic_vec