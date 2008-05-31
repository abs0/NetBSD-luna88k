/* $NetBSD: linux_syscallargs.h,v 1.27 2005/02/26 23:58:19 perry Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.21 2005/02/26 23:10:19 perry Exp
 */

#ifndef _LINUX_SYS__SYSCALLARGS_H_
#define	_LINUX_SYS__SYSCALLARGS_H_

#ifdef	syscallarg
#undef	syscallarg
#endif

#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

struct linux_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct linux_sys_waitpid_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
};

struct linux_sys_creat_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct linux_sys_link_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct linux_sys_unlink_args {
	syscallarg(const char *) path;
};

struct linux_sys_execve_args {
	syscallarg(const char *) path;
	syscallarg(char **) argp;
	syscallarg(char **) envp;
};

struct linux_sys_chdir_args {
	syscallarg(const char *) path;
};

struct linux_sys_time_args {
	syscallarg(linux_time_t *) t;
};

struct linux_sys_mknod_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct linux_sys_chmod_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct linux_sys_lchown_args {
	syscallarg(const char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct linux_sys_stime_args {
	syscallarg(linux_time_t *) t;
};

struct linux_sys_ptrace_args {
	syscallarg(int) request;
	syscallarg(int) pid;
	syscallarg(int) addr;
	syscallarg(int) data;
};

struct linux_sys_alarm_args {
	syscallarg(unsigned int) secs;
};

struct linux_sys_utime_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_utimbuf *) times;
};

struct linux_sys_access_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct linux_sys_nice_args {
	syscallarg(int) incr;
};

struct linux_sys_kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};

struct linux_sys_rename_args {
	syscallarg(const char *) from;
	syscallarg(const char *) to;
};

struct linux_sys_mkdir_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct linux_sys_rmdir_args {
	syscallarg(const char *) path;
};

struct linux_sys_pipe_args {
	syscallarg(int *) pfds;
};

struct linux_sys_times_args {
	syscallarg(struct times *) tms;
};

struct linux_sys_brk_args {
	syscallarg(char *) nsize;
};

struct linux_sys_signal_args {
	syscallarg(int) signum;
	syscallarg(linux_handler_t) handler;
};

struct linux_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(caddr_t) data;
};

struct linux_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};

struct linux_sys_olduname_args {
	syscallarg(struct linux_old_utsname *) up;
};

struct linux_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct linux_old_sigaction *) nsa;
	syscallarg(struct linux_old_sigaction *) osa;
};

struct linux_sys_sigsetmask_args {
	syscallarg(linux_old_sigset_t) mask;
};

struct linux_sys_sigsuspend_args {
	syscallarg(caddr_t) restart;
	syscallarg(int) oldmask;
	syscallarg(int) mask;
};

struct linux_sys_sigpending_args {
	syscallarg(linux_old_sigset_t *) set;
};

struct linux_sys_setrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct orlimit *) rlp;
};

struct linux_sys_getrlimit_args {
	syscallarg(u_int) which;
	syscallarg(struct orlimit *) rlp;
};

struct linux_sys_gettimeofday_args {
	syscallarg(struct timeval *) tp;
	syscallarg(struct timezone *) tzp;
};

struct linux_sys_settimeofday_args {
	syscallarg(struct timeval *) tp;
	syscallarg(struct timezone *) tzp;
};

struct linux_sys_select_args {
	syscallarg(int) nfds;
	syscallarg(fd_set *) readfds;
	syscallarg(fd_set *) writefds;
	syscallarg(fd_set *) exceptfds;
	syscallarg(struct timeval *) timeout;
};

struct linux_sys_symlink_args {
	syscallarg(const char *) path;
	syscallarg(const char *) to;
};

struct linux_sys_readlink_args {
	syscallarg(const char *) name;
	syscallarg(char *) buf;
	syscallarg(int) count;
};

struct linux_sys_uselib_args {
	syscallarg(const char *) path;
};

struct linux_sys_swapon_args {
	syscallarg(char *) name;
};

struct linux_sys_reboot_args {
	syscallarg(int) magic1;
	syscallarg(int) magic2;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};

struct linux_sys_readdir_args {
	syscallarg(int) fd;
	syscallarg(caddr_t) dent;
	syscallarg(unsigned int) count;
};

struct linux_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(long) length;
};

struct linux_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_statfs *) sp;
};

struct linux_sys_fstatfs_args {
	syscallarg(int) fd;
	syscallarg(struct linux_statfs *) sp;
};

struct linux_sys_ioperm_args {
	syscallarg(unsigned int) lo;
	syscallarg(unsigned int) hi;
	syscallarg(int) val;
};

struct linux_sys_socketcall_args {
	syscallarg(int) what;
	syscallarg(void *) args;
};

struct linux_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat *) sp;
};

struct linux_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat *) sp;
};

struct linux_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct linux_stat *) sp;
};

struct linux_sys_uname_args {
	syscallarg(struct linux_utsname *) up;
};

struct linux_sys_wait4_args {
	syscallarg(int) pid;
	syscallarg(int *) status;
	syscallarg(int) options;
	syscallarg(struct rusage *) rusage;
};

struct linux_sys_swapoff_args {
	syscallarg(const char *) path;
};

struct linux_sys_sysinfo_args {
	syscallarg(struct linux_sysinfo *) arg;
};

struct linux_sys_ipc_args {
	syscallarg(int) what;
	syscallarg(int) a1;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(caddr_t) ptr;
};

struct linux_sys_sigreturn_args {
	syscallarg(struct linux_sigcontext *) scp;
};

struct linux_sys_clone_args {
	syscallarg(int) flags;
	syscallarg(void *) stack;
};

struct linux_sys_setdomainname_args {
	syscallarg(char *) domainname;
	syscallarg(int) len;
};

struct linux_sys_new_uname_args {
	syscallarg(struct linux_utsname *) up;
};

struct linux_sys_mprotect_args {
	syscallarg(const void *) start;
	syscallarg(unsigned long) len;
	syscallarg(int) prot;
};

struct linux_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const linux_old_sigset_t *) set;
	syscallarg(linux_old_sigset_t *) oset;
};

struct linux_sys_getpgid_args {
	syscallarg(int) pid;
};

struct linux_sys_personality_args {
	syscallarg(int) per;
};

struct linux_sys_setfsuid_args {
	syscallarg(uid_t) uid;
};

struct linux_sys_llseek_args {
	syscallarg(int) fd;
	syscallarg(u_int32_t) ohigh;
	syscallarg(u_int32_t) olow;
	syscallarg(caddr_t) res;
	syscallarg(int) whence;
};

struct linux_sys_getdents_args {
	syscallarg(int) fd;
	syscallarg(struct linux_dirent *) dent;
	syscallarg(unsigned int) count;
};

struct linux_sys_new_select_args {
	syscallarg(int) nfds;
	syscallarg(fd_set *) readfds;
	syscallarg(fd_set *) writefds;
	syscallarg(fd_set *) exceptfds;
	syscallarg(struct timeval *) timeout;
};

struct linux_sys_msync_args {
	syscallarg(caddr_t) addr;
	syscallarg(int) len;
	syscallarg(int) fl;
};

struct linux_sys_fdatasync_args {
	syscallarg(int) fd;
};

struct linux_sys___sysctl_args {
	syscallarg(struct linux___sysctl *) lsp;
};

struct linux_sys_sched_setparam_args {
	syscallarg(pid_t) pid;
	syscallarg(const struct linux_sched_param *) sp;
};

struct linux_sys_sched_getparam_args {
	syscallarg(pid_t) pid;
	syscallarg(struct linux_sched_param *) sp;
};

struct linux_sys_sched_setscheduler_args {
	syscallarg(pid_t) pid;
	syscallarg(int) policy;
	syscallarg(const struct linux_sched_param *) sp;
};

struct linux_sys_sched_getscheduler_args {
	syscallarg(pid_t) pid;
};

struct linux_sys_sched_get_priority_max_args {
	syscallarg(int) policy;
};

struct linux_sys_sched_get_priority_min_args {
	syscallarg(int) policy;
};

struct linux_sys_mremap_args {
	syscallarg(void *) old_address;
	syscallarg(size_t) old_size;
	syscallarg(size_t) new_size;
	syscallarg(u_long) flags;
};

struct linux_sys_setresuid_args {
	syscallarg(uid_t) ruid;
	syscallarg(uid_t) euid;
	syscallarg(uid_t) suid;
};

struct linux_sys_getresuid_args {
	syscallarg(uid_t *) ruid;
	syscallarg(uid_t *) euid;
	syscallarg(uid_t *) suid;
};

struct linux_sys_setresgid_args {
	syscallarg(gid_t) rgid;
	syscallarg(gid_t) egid;
	syscallarg(gid_t) sgid;
};

struct linux_sys_getresgid_args {
	syscallarg(gid_t *) rgid;
	syscallarg(gid_t *) egid;
	syscallarg(gid_t *) sgid;
};

struct linux_sys_rt_sigreturn_args {
	syscallarg(struct linux_rt_sigframe *) sfp;
};

struct linux_sys_rt_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct linux_sigaction *) nsa;
	syscallarg(struct linux_sigaction *) osa;
	syscallarg(size_t) sigsetsize;
};

struct linux_sys_rt_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const linux_sigset_t *) set;
	syscallarg(linux_sigset_t *) oset;
	syscallarg(size_t) sigsetsize;
};

struct linux_sys_rt_sigpending_args {
	syscallarg(linux_sigset_t *) set;
	syscallarg(size_t) sigsetsize;
};

struct linux_sys_rt_queueinfo_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
	syscallarg(void *) uinfo;
};

struct linux_sys_rt_sigsuspend_args {
	syscallarg(linux_sigset_t *) unewset;
	syscallarg(size_t) sigsetsize;
};

struct linux_sys_pread_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(size_t) nbyte;
	syscallarg(linux_off_t) offset;
};

struct linux_sys_pwrite_args {
	syscallarg(int) fd;
	syscallarg(char *) buf;
	syscallarg(size_t) nbyte;
	syscallarg(linux_off_t) offset;
};

struct linux_sys_chown_args {
	syscallarg(const char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct linux_sys_sigaltstack_args {
	syscallarg(const struct linux_sigaltstack *) ss;
	syscallarg(struct linux_sigaltstack *) oss;
};

struct linux_sys_ugetrlimit_args {
	syscallarg(int) which;
	syscallarg(struct rlimit *) rlp;
};

struct linux_sys_truncate64_args {
	syscallarg(const char *) path;
	syscallarg(off_t) length;
};

struct linux_sys_ftruncate64_args {
	syscallarg(unsigned int) fd;
	syscallarg(off_t) length;
};

struct linux_sys_stat64_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat64 *) sp;
};

struct linux_sys_lstat64_args {
	syscallarg(const char *) path;
	syscallarg(struct linux_stat64 *) sp;
};

struct linux_sys_fstat64_args {
	syscallarg(int) fd;
	syscallarg(struct linux_stat64 *) sp;
};

struct linux_sys_getdents64_args {
	syscallarg(int) fd;
	syscallarg(struct linux_dirent64 *) dent;
	syscallarg(unsigned int) count;
};

struct linux_sys_fcntl64_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};

struct linux_sys_exit_group_args {
	syscallarg(int) error_code;
};

struct linux_sys_statfs64_args {
	syscallarg(const char *) path;
	syscallarg(size_t) sz;
	syscallarg(struct linux_statfs64 *) sp;
};

struct linux_sys_fstatfs64_args {
	syscallarg(int) fd;
	syscallarg(size_t) sz;
	syscallarg(struct linux_statfs64 *) sp;
};

/*
 * System call prototypes.
 */

int	linux_sys_nosys(struct lwp *, void *, register_t *);

int	sys_exit(struct lwp *, void *, register_t *);

int	sys_fork(struct lwp *, void *, register_t *);

int	sys_read(struct lwp *, void *, register_t *);

int	sys_write(struct lwp *, void *, register_t *);

int	linux_sys_open(struct lwp *, void *, register_t *);

int	sys_close(struct lwp *, void *, register_t *);

int	linux_sys_waitpid(struct lwp *, void *, register_t *);

int	linux_sys_creat(struct lwp *, void *, register_t *);

int	linux_sys_link(struct lwp *, void *, register_t *);

int	linux_sys_unlink(struct lwp *, void *, register_t *);

int	linux_sys_execve(struct lwp *, void *, register_t *);

int	linux_sys_chdir(struct lwp *, void *, register_t *);

int	linux_sys_time(struct lwp *, void *, register_t *);

int	linux_sys_mknod(struct lwp *, void *, register_t *);

int	linux_sys_chmod(struct lwp *, void *, register_t *);

int	linux_sys_lchown(struct lwp *, void *, register_t *);

int	compat_43_sys_lseek(struct lwp *, void *, register_t *);

int	sys_getpid(struct lwp *, void *, register_t *);

int	sys_setuid(struct lwp *, void *, register_t *);

int	sys_getuid(struct lwp *, void *, register_t *);

int	linux_sys_stime(struct lwp *, void *, register_t *);

int	linux_sys_ptrace(struct lwp *, void *, register_t *);

int	linux_sys_alarm(struct lwp *, void *, register_t *);

int	linux_sys_pause(struct lwp *, void *, register_t *);

int	linux_sys_utime(struct lwp *, void *, register_t *);

int	linux_sys_access(struct lwp *, void *, register_t *);

int	linux_sys_nice(struct lwp *, void *, register_t *);

int	sys_sync(struct lwp *, void *, register_t *);

int	linux_sys_kill(struct lwp *, void *, register_t *);

int	linux_sys_rename(struct lwp *, void *, register_t *);

int	linux_sys_mkdir(struct lwp *, void *, register_t *);

int	linux_sys_rmdir(struct lwp *, void *, register_t *);

int	sys_dup(struct lwp *, void *, register_t *);

int	linux_sys_pipe(struct lwp *, void *, register_t *);

int	linux_sys_times(struct lwp *, void *, register_t *);

int	linux_sys_brk(struct lwp *, void *, register_t *);

int	sys_setgid(struct lwp *, void *, register_t *);

int	sys_getgid(struct lwp *, void *, register_t *);

int	linux_sys_signal(struct lwp *, void *, register_t *);

int	sys_geteuid(struct lwp *, void *, register_t *);

int	sys_getegid(struct lwp *, void *, register_t *);

int	sys_acct(struct lwp *, void *, register_t *);

int	linux_sys_ioctl(struct lwp *, void *, register_t *);

int	linux_sys_fcntl(struct lwp *, void *, register_t *);

int	sys_setpgid(struct lwp *, void *, register_t *);

int	linux_sys_olduname(struct lwp *, void *, register_t *);

int	sys_umask(struct lwp *, void *, register_t *);

int	sys_chroot(struct lwp *, void *, register_t *);

int	sys_dup2(struct lwp *, void *, register_t *);

int	sys_getppid(struct lwp *, void *, register_t *);

int	sys_getpgrp(struct lwp *, void *, register_t *);

int	sys_setsid(struct lwp *, void *, register_t *);

int	linux_sys_sigaction(struct lwp *, void *, register_t *);

int	linux_sys_siggetmask(struct lwp *, void *, register_t *);

int	linux_sys_sigsetmask(struct lwp *, void *, register_t *);

int	sys_setreuid(struct lwp *, void *, register_t *);

int	sys_setregid(struct lwp *, void *, register_t *);

int	linux_sys_sigsuspend(struct lwp *, void *, register_t *);

int	linux_sys_sigpending(struct lwp *, void *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, void *, register_t *);

int	linux_sys_setrlimit(struct lwp *, void *, register_t *);

int	linux_sys_getrlimit(struct lwp *, void *, register_t *);

int	sys_getrusage(struct lwp *, void *, register_t *);

int	linux_sys_gettimeofday(struct lwp *, void *, register_t *);

int	linux_sys_settimeofday(struct lwp *, void *, register_t *);

int	sys_getgroups(struct lwp *, void *, register_t *);

int	sys_setgroups(struct lwp *, void *, register_t *);

int	linux_sys_select(struct lwp *, void *, register_t *);

int	linux_sys_symlink(struct lwp *, void *, register_t *);

int	compat_43_sys_lstat(struct lwp *, void *, register_t *);

int	linux_sys_readlink(struct lwp *, void *, register_t *);

int	linux_sys_uselib(struct lwp *, void *, register_t *);

int	linux_sys_swapon(struct lwp *, void *, register_t *);

int	linux_sys_reboot(struct lwp *, void *, register_t *);

int	linux_sys_readdir(struct lwp *, void *, register_t *);

int	linux_sys_mmap(struct lwp *, void *, register_t *);

int	sys_munmap(struct lwp *, void *, register_t *);

int	linux_sys_truncate(struct lwp *, void *, register_t *);

int	compat_43_sys_ftruncate(struct lwp *, void *, register_t *);

int	sys_fchmod(struct lwp *, void *, register_t *);

int	sys___posix_fchown(struct lwp *, void *, register_t *);

int	sys_getpriority(struct lwp *, void *, register_t *);

int	sys_setpriority(struct lwp *, void *, register_t *);

int	sys_profil(struct lwp *, void *, register_t *);

int	linux_sys_statfs(struct lwp *, void *, register_t *);

int	linux_sys_fstatfs(struct lwp *, void *, register_t *);

int	linux_sys_ioperm(struct lwp *, void *, register_t *);

int	linux_sys_socketcall(struct lwp *, void *, register_t *);

int	sys_setitimer(struct lwp *, void *, register_t *);

int	sys_getitimer(struct lwp *, void *, register_t *);

int	linux_sys_stat(struct lwp *, void *, register_t *);

int	linux_sys_lstat(struct lwp *, void *, register_t *);

int	linux_sys_fstat(struct lwp *, void *, register_t *);

int	linux_sys_uname(struct lwp *, void *, register_t *);

int	linux_sys_wait4(struct lwp *, void *, register_t *);

int	linux_sys_swapoff(struct lwp *, void *, register_t *);

int	linux_sys_sysinfo(struct lwp *, void *, register_t *);

int	linux_sys_ipc(struct lwp *, void *, register_t *);

int	sys_fsync(struct lwp *, void *, register_t *);

int	linux_sys_sigreturn(struct lwp *, void *, register_t *);

int	linux_sys_clone(struct lwp *, void *, register_t *);

int	linux_sys_setdomainname(struct lwp *, void *, register_t *);

int	linux_sys_new_uname(struct lwp *, void *, register_t *);

int	linux_sys_mprotect(struct lwp *, void *, register_t *);

int	linux_sys_sigprocmask(struct lwp *, void *, register_t *);

int	linux_sys_getpgid(struct lwp *, void *, register_t *);

int	sys_fchdir(struct lwp *, void *, register_t *);

int	linux_sys_personality(struct lwp *, void *, register_t *);

int	linux_sys_setfsuid(struct lwp *, void *, register_t *);

int	linux_sys_getfsuid(struct lwp *, void *, register_t *);

int	linux_sys_llseek(struct lwp *, void *, register_t *);

int	linux_sys_getdents(struct lwp *, void *, register_t *);

int	linux_sys_new_select(struct lwp *, void *, register_t *);

int	sys_flock(struct lwp *, void *, register_t *);

int	linux_sys_msync(struct lwp *, void *, register_t *);

int	sys_readv(struct lwp *, void *, register_t *);

int	sys_writev(struct lwp *, void *, register_t *);

int	sys_getsid(struct lwp *, void *, register_t *);

int	linux_sys_fdatasync(struct lwp *, void *, register_t *);

int	linux_sys___sysctl(struct lwp *, void *, register_t *);

int	sys_mlock(struct lwp *, void *, register_t *);

int	sys_munlock(struct lwp *, void *, register_t *);

int	sys_mlockall(struct lwp *, void *, register_t *);

int	sys_munlockall(struct lwp *, void *, register_t *);

int	linux_sys_sched_setparam(struct lwp *, void *, register_t *);

int	linux_sys_sched_getparam(struct lwp *, void *, register_t *);

int	linux_sys_sched_setscheduler(struct lwp *, void *, register_t *);

int	linux_sys_sched_getscheduler(struct lwp *, void *, register_t *);

int	linux_sys_sched_yield(struct lwp *, void *, register_t *);

int	linux_sys_sched_get_priority_max(struct lwp *, void *, register_t *);

int	linux_sys_sched_get_priority_min(struct lwp *, void *, register_t *);

int	sys_nanosleep(struct lwp *, void *, register_t *);

int	linux_sys_mremap(struct lwp *, void *, register_t *);

int	linux_sys_setresuid(struct lwp *, void *, register_t *);

int	linux_sys_getresuid(struct lwp *, void *, register_t *);

int	sys_poll(struct lwp *, void *, register_t *);

int	linux_sys_setresgid(struct lwp *, void *, register_t *);

int	linux_sys_getresgid(struct lwp *, void *, register_t *);

int	linux_sys_rt_sigreturn(struct lwp *, void *, register_t *);

int	linux_sys_rt_sigaction(struct lwp *, void *, register_t *);

int	linux_sys_rt_sigprocmask(struct lwp *, void *, register_t *);

int	linux_sys_rt_sigpending(struct lwp *, void *, register_t *);

int	linux_sys_rt_queueinfo(struct lwp *, void *, register_t *);

int	linux_sys_rt_sigsuspend(struct lwp *, void *, register_t *);

int	linux_sys_pread(struct lwp *, void *, register_t *);

int	linux_sys_pwrite(struct lwp *, void *, register_t *);

int	linux_sys_chown(struct lwp *, void *, register_t *);

int	sys___getcwd(struct lwp *, void *, register_t *);

int	linux_sys_sigaltstack(struct lwp *, void *, register_t *);

int	sys___vfork14(struct lwp *, void *, register_t *);

int	linux_sys_ugetrlimit(struct lwp *, void *, register_t *);

int	linux_sys_mmap2(struct lwp *, void *, register_t *);

int	linux_sys_truncate64(struct lwp *, void *, register_t *);

int	linux_sys_ftruncate64(struct lwp *, void *, register_t *);

int	linux_sys_stat64(struct lwp *, void *, register_t *);

int	linux_sys_lstat64(struct lwp *, void *, register_t *);

int	linux_sys_fstat64(struct lwp *, void *, register_t *);

int	linux_sys_getdents64(struct lwp *, void *, register_t *);

int	linux_sys_fcntl64(struct lwp *, void *, register_t *);

int	sys_mincore(struct lwp *, void *, register_t *);

int	sys_madvise(struct lwp *, void *, register_t *);

int	linux_sys_exit_group(struct lwp *, void *, register_t *);

int	linux_sys_statfs64(struct lwp *, void *, register_t *);

int	linux_sys_fstatfs64(struct lwp *, void *, register_t *);

#endif /* _LINUX_SYS__SYSCALLARGS_H_ */