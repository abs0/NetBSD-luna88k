/* NETBSD_NATIVE is defined when gcc is integrated into the NetBSD
   source tree so it can be configured appropriately without using
   the GNU configure/build mechanism. */

#ifdef NETBSD_NATIVE

/* Look for the include files in the system-defined places.  */

#undef GPLUSPLUS_INCLUDE_DIR
#define GPLUSPLUS_INCLUDE_DIR "/usr/include/g++"

#undef STANDARD_INCLUDE_DIR
#define STANDARD_INCLUDE_DIR "/usr/include"

#undef INCLUDE_DEFAULTS
#define INCLUDE_DEFAULTS			\
  {						\
    { GPLUSPLUS_INCLUDE_DIR, "G++", 1, 1 },	\
    { STANDARD_INCLUDE_DIR, 0, 0, 0 },		\
    { 0, 0, 0, 0 }				\
  }

/* Under NetBSD, the normal location of the compiler back ends is the
   /usr/libexec directory.  */

#undef MD_EXEC_PREFIX
#define MD_EXEC_PREFIX			"/usr/libexec/"

/* Under NetBSD, the normal location of the various *crt*.o files is the
   /usr/lib directory.  */

#undef STANDARD_STARTFILE_PREFIX
#define STANDARD_STARTFILE_PREFIX	"/usr/lib/"

#endif /* NETBSD_NATIVE */


/* Provide a CPP_SPEC appropriate for NetBSD.  Current we just deal with
   the GCC option `-posix'.  We also deal with -pthread, which is required
   for the 2.95.3 STL to work.  */

#undef CPP_SPEC
#define CPP_SPEC \
  "%(cpp_cpu) %{posix:-D_POSIX_SOURCE} \
   %{pthread:-D_REENTRANT -D_PTHREADS}"

/* Provide an ASM_SPEC appropriate for NetBSD.  Currently we only deal
   with the options for generating PIC code.  */

#undef ASM_SPEC
#define ASM_SPEC " %| %{fpic:-k} %{fPIC:-k -K}"

/* Provide a LIB_SPEC appropriate for NetBSD.  Just select the appropriate
   libc, depending on whether we're doing profiling; if `-posix' is specified,
   link against the appropriate libposix first.  Don't include libc when
   linking a shared library.  */

#undef LIB_SPEC
#define LIB_SPEC							\
  "%{pthread:%{!p:%{!pg:-lpthread}}%{p:-lpthread_p}%{pg:-lpthread_p}}	\
   %{posix:%{!p:%{!pg:-lposix}}%{p:-lposix_p}%{pg:-lposix_p}}		\
   %{!shared:%{!symbolic:%{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}}}"

/* Provide a LIBGCC_SPEC appropriate for NetBSD.  We also want to exclude
   libgcc when -symbolic.  */

#undef  LIBGCC_SPEC
#ifdef  NETBSD_NATIVE
#define LIBGCC_SPEC "%{!symbolic:%{!shared:%{!p:%{!pg:-lgcc}}}%{shared:-lgcc_pic}%{p:-lgcc_p}%{pg:-lgcc_p}}"
#else
#define LIBGCC_SPEC "%{!symbolic:%{!shared:-lgcc}%{shared:-lgcc_pic}}"
#endif

/* #ifdef NETBSD_AOUT */

/* Provide a STARTFILE_SPEC appropriate for NetBSD a.out.  Here we
   provide support for the special GCC option -static.  */

#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
  "%{!shared:%{pg:gcrt0%O%s}%{!pg:%{p:mcrt0%O%s}%{!p:%{!static:crt0%O%s}%{static:scrt0%O%s}}}} %{shared:c++rt0%O%s}"

/* Provide a LINK_SPEC appropriate for NetBSD.  Here we provide support
   for the special GCC options -static, -assert, and -nostdlib.  */

#undef LINK_SPEC
#define LINK_SPEC \
  "%{nostdlib:-nostdlib} %{!shared:%{!nostdlib:%{!r*:%{!e*:-e start}}} -dc -dp %{static:-Bstatic}} %{shared:-Bshareable} %{R*} %{assert*}"

/* #endif NETBSD_AOUT */

/* This defines which switch letters take arguments. */
#undef SWITCH_TAKES_ARG
#define SWITCH_TAKES_ARG(CHAR) \
  (DEFAULT_SWITCH_TAKES_ARG(CHAR) \
   || (CHAR) == 'R')

/* We have atexit(3).  */

#define HAVE_ATEXIT

/* Implicit library calls should use memcpy, not bcopy, etc.  */

#define TARGET_MEM_FUNCTIONS

/* Handle #pragma weak and #pragma pack.  */

#define HANDLE_SYSV_PRAGMA

/*
 * Some imports from svr4.h in support of shared libraries.
 * Currently, we need the DECLARE_OBJECT_SIZE stuff.
 */

/* Define the strings used for the .type, .size, and .set directives.
   These strings generally do not vary from one system running netbsd
   to another, but if a given system needs to use different pseudo-op
   names for these, they may be overridden in the file which includes
   this one.  */

#undef TYPE_ASM_OP
#undef SIZE_ASM_OP
#undef SET_ASM_OP
#define TYPE_ASM_OP	".type"
#define SIZE_ASM_OP	".size"
#define SET_ASM_OP	".set"

/* This is how we tell the assembler that a symbol is weak.  */

#undef ASM_WEAKEN_LABEL
#define ASM_WEAKEN_LABEL(FILE,NAME) \
  do { fputs ("\t.globl\t", FILE); assemble_name (FILE, NAME); \
       fputc ('\n', FILE); \
       fputs ("\t.weak\t", FILE); assemble_name (FILE, NAME); \
       fputc ('\n', FILE); } while (0)

/* The following macro defines the format used to output the second
   operand of the .type assembler directive.  Different svr4 assemblers
   expect various different forms for this operand.  The one given here
   is just a default.  You may need to override it in your machine-
   specific tm.h file (depending upon the particulars of your assembler).  */

#undef TYPE_OPERAND_FMT
#define TYPE_OPERAND_FMT	"@%s"

/* Write the extra assembler code needed to declare a function's result.
   Most svr4 assemblers don't require any special declaration of the
   result value, but there are exceptions.  */

#ifndef ASM_DECLARE_RESULT
#define ASM_DECLARE_RESULT(FILE, RESULT)
#endif

/* NetBSD a.out support begins here. */

#ifndef NETBSD_ELF

#undef DWARF_DEBUGGING_INFO	/* XXX */
#undef DWARF2_DEBUGGING_INFO	/* XXX */

/* Write the extra assembler code needed to declare a function properly.
   Some svr4 assemblers need to also have something extra said about the
   function's return value.  We allow for that here.  */

#undef ASM_DECLARE_FUNCTION_NAME
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)			\
  do {									\
    fprintf (FILE, "\t%s\t ", TYPE_ASM_OP);				\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "function");			\
    putc ('\n', FILE);							\
    ASM_DECLARE_RESULT (FILE, DECL_RESULT (DECL));			\
    ASM_OUTPUT_LABEL(FILE, NAME);					\
  } while (0)

/* Write the extra assembler code needed to declare an object properly.  */

#undef ASM_DECLARE_OBJECT_NAME
#define ASM_DECLARE_OBJECT_NAME(FILE, NAME, DECL)			\
  do {									\
    fprintf (FILE, "\t%s\t ", TYPE_ASM_OP);				\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "object");				\
    putc ('\n', FILE);							\
    size_directive_output = 0;						\
    if (!flag_inhibit_size_directive && DECL_SIZE (DECL))		\
      {									\
	size_directive_output = 1;					\
	fprintf (FILE, "\t%s\t ", SIZE_ASM_OP);				\
	assemble_name (FILE, NAME);					\
	fprintf (FILE, ",%d\n",  int_size_in_bytes (TREE_TYPE (DECL)));	\
      }									\
    ASM_OUTPUT_LABEL(FILE, NAME);					\
  } while (0)

/* Output the size directive for a decl in rest_of_decl_compilation
   in the case where we did not do so before the initializer.
   Once we find the error_mark_node, we know that the value of
   size_directive_output was set
   by ASM_DECLARE_OBJECT_NAME when it was run for the same decl.  */

#undef ASM_FINISH_DECLARE_OBJECT
#define ASM_FINISH_DECLARE_OBJECT(FILE, DECL, TOP_LEVEL, AT_END)	 \
do {									 \
     char *name = XSTR (XEXP (DECL_RTL (DECL), 0), 0);			 \
     if (!flag_inhibit_size_directive && DECL_SIZE (DECL)		 \
         && ! AT_END && TOP_LEVEL					 \
	 && DECL_INITIAL (DECL) == error_mark_node			 \
	 && !size_directive_output)					 \
       {								 \
	 size_directive_output = 1;					 \
	 fprintf (FILE, "\t%s\t ", SIZE_ASM_OP);			 \
	 assemble_name (FILE, name);					 \
	 fprintf (FILE, ",%d\n",  int_size_in_bytes (TREE_TYPE (DECL))); \
       }								 \
   } while (0)

/* This is how to declare the size of a function.  */

#undef ASM_DECLARE_FUNCTION_SIZE
#define ASM_DECLARE_FUNCTION_SIZE(FILE, FNAME, DECL)			\
  do {									\
    if (!flag_inhibit_size_directive)					\
      {									\
        char label[256];						\
	static int labelno;						\
	labelno++;							\
	ASM_GENERATE_INTERNAL_LABEL (label, "Lfe", labelno);		\
	ASM_OUTPUT_INTERNAL_LABEL (FILE, "Lfe", labelno);		\
	fprintf (FILE, "\t%s\t ", SIZE_ASM_OP);				\
	assemble_name (FILE, (FNAME));					\
        fprintf (FILE, ",");						\
	assemble_name (FILE, label);					\
        fprintf (FILE, "-");						\
	assemble_name (FILE, (FNAME));					\
	putc ('\n', FILE);						\
      }									\
  } while (0)

#endif

/* NetBSD ELF support begins here. */

#ifdef NETBSD_ELF

#undef DWARF_DEBUGGING_INFO	/* XXX */
#undef DWARF2_DEBUGGING_INFO	/* XXX */

/* Provide a STARTFILE_SPEC appropriate for NetBSD ELF targets.  Here we
   provide support for the special GCC option -static.  On ELF targets,
   we also add the crtbegin.o file which provides part of the support
   for getting C++ file-scope static objects constructed before entering
   `main'. */

#undef STARTFILE_SPEC
#define	STARTFILE_SPEC \
 "%{!shared: \
     %{pg:gcrt0%O%s} \
     %{!pg: \
        %{p:gcrt0%O%s} \
        %{!p:crt0%O%s}}} \
   %:if-exists(crti%O%s) \
   %{!shared:crtbegin%O%s} %{shared:crtbeginS%O%s}"

/* Provide an ENDFILE_SPEC appropriate for NetBSD ELF targets.  Here we
   add crtend.o, which provides part of the support for getting C++
   file-scope static objects deconstructed after exiting `main'. */

#undef ENDFILE_SPEC
#define	ENDFILE_SPEC \
 "%{!shared:crtend%O%s} %{shared:crtendS%O%s} \
  %:if-exists(crtn%O%s)"

/* Provide a LINK_SPEC appropriate for a NetBSD ELF target.  */

#undef LINK_SPEC
#define	LINK_SPEC \
 "%{assert*} %{R*} \
  %{shared:-shared} \
  %{!shared: \
    -dc -dp \
    %{!nostdlib:%{!r*:%{!e*:-e __start}}} \
    %{!static: \
      %{rdynamic:-export-dynamic} \
      %{!dynamic-linker:-dynamic-linker /usr/libexec/ld.elf_so}} \
    %{static:-static}}"

#endif /* NETBSD_ELF */


/* Attempt to turn on execute permission for the stack.  This may be    
   used by TRANSFER_FROM_TRAMPOLINE of the target needs it (that is,    
   if the target machine can change execute permissions on a page).     

   There is no way to query the execute permission of the stack, so     
   we always issue the mprotect() call.          
                                                 
   Note that we go out of our way to use namespace-non-invasive calls   
   here.  Unfortunately, there is no libc-internal name for mprotect().  
                                                                        
   Also note that no errors should be emitted by this code; it is considered
   dangerous for library calls to send messages to stdout/stderr.  */   
    
#define NETBSD_ENABLE_EXECUTE_STACK					\
extern void __enable_execute_stack (void *);				\
void									\
__enable_execute_stack (addr)						\
     void *addr;							\
{									\
  extern int mprotect (void *, size_t, int);				\
  extern int __sysctl (int *, unsigned int, void *, size_t *,		\
		       void *, size_t);					\
									\
  static int size;							\
  static long mask;							\
									\
  char *page, *end;							\
									\
  if (size == 0)							\
    {									\
      int mib[2];							\
      size_t len;							\
									\
      mib[0] = 6; /* CTL_HW */						\
      mib[1] = 7; /* HW_PAGESIZE */					\
      len = sizeof (size);						\
      (void) __sysctl (mib, 2, &size, &len, NULL, 0);			\
      mask = ~((long) size - 1);					\
    }									\
									\
  page = (char *) (((long) addr) & mask);				\
  end  = (char *) ((((long) (addr + TRAMPOLINE_SIZE)) & mask) + size);	\
									\
  /* 7 == PROT_READ | PROT_WRITE | PROT_EXEC */				\
  (void) mprotect (page, end - page, 7);				\
}
