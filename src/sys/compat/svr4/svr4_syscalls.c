/* $NetBSD: svr4_syscalls.c,v 1.72 2005/02/26 23:58:20 perry Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.52 2003/01/18 08:44:27 thorpej Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: svr4_syscalls.c,v 1.72 2005/02/26 23:58:20 perry Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_ntp.h"
#include "opt_sysv.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/sa.h>
#include <sys/syscallargs.h>
#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_time.h>
#include <compat/svr4/svr4_signal.h>
#include <compat/svr4/svr4_ucontext.h>
#include <compat/svr4/svr4_lwp.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/svr4/svr4_statvfs.h>
#include <compat/svr4/svr4_resource.h>
#include <compat/svr4/svr4_acl.h>
#endif /* _KERNEL_OPT */

const char *const svr4_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"wait",			/* 7 = wait */
	"creat",			/* 8 = creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"execv",			/* 11 = execv */
	"chdir",			/* 12 = chdir */
	"time",			/* 13 = time */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"break",			/* 17 = break */
	"stat",			/* 18 = stat */
	"lseek",			/* 19 = lseek */
	"getpid",			/* 20 = getpid */
	"#21 (unimplemented old_mount)",		/* 21 = unimplemented old_mount */
	"#22 (unimplemented System V umount)",		/* 22 = unimplemented System V umount */
	"setuid",			/* 23 = setuid */
	"getuid_with_euid",			/* 24 = getuid_with_euid */
	"#25 (unimplemented stime)",		/* 25 = unimplemented stime */
	"#26 (unimplemented ptrace)",		/* 26 = unimplemented ptrace */
	"alarm",			/* 27 = alarm */
	"fstat",			/* 28 = fstat */
	"pause",			/* 29 = pause */
	"utime",			/* 30 = utime */
	"#31 (unimplemented was stty)",		/* 31 = unimplemented was stty */
	"#32 (unimplemented was gtty)",		/* 32 = unimplemented was gtty */
	"access",			/* 33 = access */
	"nice",			/* 34 = nice */
	"#35 (unimplemented statfs)",		/* 35 = unimplemented statfs */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"#38 (unimplemented fstatfs)",		/* 38 = unimplemented fstatfs */
	"pgrpsys",			/* 39 = pgrpsys */
	"#40 (unimplemented xenix)",		/* 40 = unimplemented xenix */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"times",			/* 43 = times */
	"#44 (unimplemented profil)",		/* 44 = unimplemented profil */
	"#45 (unimplemented plock)",		/* 45 = unimplemented plock */
	"setgid",			/* 46 = setgid */
	"getgid_with_egid",			/* 47 = getgid_with_egid */
	"signal",			/* 48 = signal */
#ifdef SYSVMSG
	"msgsys",			/* 49 = msgsys */
#else
	"#49 (unimplemented msgsys)",		/* 49 = unimplemented msgsys */
#endif
	"sysarch",			/* 50 = sysarch */
	"#51 (unimplemented acct)",		/* 51 = unimplemented acct */
#ifdef SYSVSHM
	"shmsys",			/* 52 = shmsys */
#else
	"#52 (unimplemented shmsys)",		/* 52 = unimplemented shmsys */
#endif
#ifdef SYSVSEM
	"semsys",			/* 53 = semsys */
#else
	"#53 (unimplemented semsys)",		/* 53 = unimplemented semsys */
#endif
	"ioctl",			/* 54 = ioctl */
	"#55 (unimplemented uadmin)",		/* 55 = unimplemented uadmin */
	"#56 (unimplemented exch)",		/* 56 = unimplemented exch */
	"utssys",			/* 57 = utssys */
	"fsync",			/* 58 = fsync */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"fcntl",			/* 62 = fcntl */
	"ulimit",			/* 63 = ulimit */
	"#64 (unimplemented reserved for unix/pc)",		/* 64 = unimplemented reserved for unix/pc */
	"#65 (unimplemented reserved for unix/pc)",		/* 65 = unimplemented reserved for unix/pc */
	"#66 (unimplemented reserved for unix/pc)",		/* 66 = unimplemented reserved for unix/pc */
	"#67 (unimplemented reserved for unix/pc)",		/* 67 = unimplemented reserved for unix/pc */
	"#68 (unimplemented reserved for unix/pc)",		/* 68 = unimplemented reserved for unix/pc */
	"#69 (unimplemented reserved for unix/pc)",		/* 69 = unimplemented reserved for unix/pc */
	"#70 (obsolete advfs)",		/* 70 = obsolete advfs */
	"#71 (obsolete unadvfs)",		/* 71 = obsolete unadvfs */
	"#72 (obsolete rmount)",		/* 72 = obsolete rmount */
	"#73 (obsolete rumount)",		/* 73 = obsolete rumount */
	"#74 (obsolete rfstart)",		/* 74 = obsolete rfstart */
	"#75 (obsolete sigret)",		/* 75 = obsolete sigret */
	"#76 (obsolete rdebug)",		/* 76 = obsolete rdebug */
	"#77 (obsolete rfstop)",		/* 77 = obsolete rfstop */
	"#78 (unimplemented rfsys)",		/* 78 = unimplemented rfsys */
	"rmdir",			/* 79 = rmdir */
	"mkdir",			/* 80 = mkdir */
	"getdents",			/* 81 = getdents */
	"#82 (obsolete libattach)",		/* 82 = obsolete libattach */
	"#83 (obsolete libdetach)",		/* 83 = obsolete libdetach */
	"#84 (unimplemented sysfs)",		/* 84 = unimplemented sysfs */
	"getmsg",			/* 85 = getmsg */
	"putmsg",			/* 86 = putmsg */
	"poll",			/* 87 = poll */
	"lstat",			/* 88 = lstat */
	"symlink",			/* 89 = symlink */
	"readlink",			/* 90 = readlink */
	"getgroups",			/* 91 = getgroups */
	"setgroups",			/* 92 = setgroups */
	"fchmod",			/* 93 = fchmod */
	"fchown",			/* 94 = fchown */
	"sigprocmask",			/* 95 = sigprocmask */
	"sigsuspend",			/* 96 = sigsuspend */
	"sigaltstack",			/* 97 = sigaltstack */
	"sigaction",			/* 98 = sigaction */
	"sigpending",			/* 99 = sigpending */
	"context",			/* 100 = context */
	"#101 (unimplemented evsys)",		/* 101 = unimplemented evsys */
	"#102 (unimplemented evtrapret)",		/* 102 = unimplemented evtrapret */
	"statvfs",			/* 103 = statvfs */
	"fstatvfs",			/* 104 = fstatvfs */
	"#105 (unimplemented)",		/* 105 = unimplemented */
	"#106 (unimplemented nfssvc)",		/* 106 = unimplemented nfssvc */
	"waitsys",			/* 107 = waitsys */
	"#108 (unimplemented sigsendsys)",		/* 108 = unimplemented sigsendsys */
	"hrtsys",			/* 109 = hrtsys */
	"#110 (unimplemented acancel)",		/* 110 = unimplemented acancel */
	"#111 (unimplemented async)",		/* 111 = unimplemented async */
	"#112 (unimplemented priocntlsys)",		/* 112 = unimplemented priocntlsys */
	"pathconf",			/* 113 = pathconf */
	"#114 (unimplemented mincore)",		/* 114 = unimplemented mincore */
	"mmap",			/* 115 = mmap */
	"mprotect",			/* 116 = mprotect */
	"munmap",			/* 117 = munmap */
	"fpathconf",			/* 118 = fpathconf */
	"vfork",			/* 119 = vfork */
	"fchdir",			/* 120 = fchdir */
	"readv",			/* 121 = readv */
	"writev",			/* 122 = writev */
	"xstat",			/* 123 = xstat */
	"lxstat",			/* 124 = lxstat */
	"fxstat",			/* 125 = fxstat */
	"xmknod",			/* 126 = xmknod */
	"#127 (unimplemented clocal)",		/* 127 = unimplemented clocal */
	"setrlimit",			/* 128 = setrlimit */
	"getrlimit",			/* 129 = getrlimit */
	"lchown",			/* 130 = lchown */
	"memcntl",			/* 131 = memcntl */
	"#132 (unimplemented getpmsg)",		/* 132 = unimplemented getpmsg */
	"#133 (unimplemented putpmsg)",		/* 133 = unimplemented putpmsg */
	"rename",			/* 134 = rename */
	"uname",			/* 135 = uname */
	"setegid",			/* 136 = setegid */
	"sysconfig",			/* 137 = sysconfig */
	"adjtime",			/* 138 = adjtime */
	"systeminfo",			/* 139 = systeminfo */
	"#140 (unimplemented)",		/* 140 = unimplemented */
	"seteuid",			/* 141 = seteuid */
	"#142 (unimplemented vtrace)",		/* 142 = unimplemented vtrace */
	"fork1",			/* 143 = fork1 */
	"#144 (unimplemented sigtimedwait)",		/* 144 = unimplemented sigtimedwait */
	"_lwp_info",			/* 145 = _lwp_info */
	"#146 (unimplemented yield)",		/* 146 = unimplemented yield */
	"#147 (unimplemented lwp_sema_wait)",		/* 147 = unimplemented lwp_sema_wait */
	"#148 (unimplemented lwp_sema_post)",		/* 148 = unimplemented lwp_sema_post */
	"#149 (unimplemented lwp_sema_trywait)",		/* 149 = unimplemented lwp_sema_trywait */
	"#150 (unimplemented)",		/* 150 = unimplemented */
	"#151 (unimplemented)",		/* 151 = unimplemented */
	"#152 (unimplemented modctl)",		/* 152 = unimplemented modctl */
	"fchroot",			/* 153 = fchroot */
	"utimes",			/* 154 = utimes */
	"vhangup",			/* 155 = vhangup */
	"gettimeofday",			/* 156 = gettimeofday */
	"getitimer",			/* 157 = getitimer */
	"setitimer",			/* 158 = setitimer */
	"_lwp_create",			/* 159 = _lwp_create */
	"_lwp_exit",			/* 160 = _lwp_exit */
	"_lwp_suspend",			/* 161 = _lwp_suspend */
	"_lwp_continue",			/* 162 = _lwp_continue */
	"_lwp_kill",			/* 163 = _lwp_kill */
	"_lwp_self",			/* 164 = _lwp_self */
	"_lwp_getprivate",			/* 165 = _lwp_getprivate */
	"_lwp_setprivate",			/* 166 = _lwp_setprivate */
	"_lwp_wait",			/* 167 = _lwp_wait */
	"#168 (unimplemented lwp_mutex_unlock)",		/* 168 = unimplemented lwp_mutex_unlock */
	"#169 (unimplemented lwp_mutex_lock)",		/* 169 = unimplemented lwp_mutex_lock */
	"#170 (unimplemented lwp_cond_wait)",		/* 170 = unimplemented lwp_cond_wait */
	"#171 (unimplemented lwp_cond_signal)",		/* 171 = unimplemented lwp_cond_signal */
	"#172 (unimplemented lwp_cond_broadcast)",		/* 172 = unimplemented lwp_cond_broadcast */
	"pread",			/* 173 = pread */
	"pwrite",			/* 174 = pwrite */
	"llseek",			/* 175 = llseek */
	"#176 (unimplemented inst_sync)",		/* 176 = unimplemented inst_sync */
	"#177 (unimplemented)",		/* 177 = unimplemented */
	"#178 (unimplemented kaio)",		/* 178 = unimplemented kaio */
	"#179 (unimplemented)",		/* 179 = unimplemented */
	"#180 (unimplemented)",		/* 180 = unimplemented */
	"#181 (unimplemented)",		/* 181 = unimplemented */
	"#182 (unimplemented)",		/* 182 = unimplemented */
	"#183 (unimplemented)",		/* 183 = unimplemented */
	"#184 (unimplemented tsolsys)",		/* 184 = unimplemented tsolsys */
	"acl",			/* 185 = acl */
	"auditsys",			/* 186 = auditsys */
	"#187 (unimplemented processor_bind)",		/* 187 = unimplemented processor_bind */
	"#188 (unimplemented processor_info)",		/* 188 = unimplemented processor_info */
	"#189 (unimplemented p_online)",		/* 189 = unimplemented p_online */
	"#190 (unimplemented sigqueue)",		/* 190 = unimplemented sigqueue */
	"#191 (unimplemented clock_gettime)",		/* 191 = unimplemented clock_gettime */
	"#192 (unimplemented clock_settime)",		/* 192 = unimplemented clock_settime */
	"#193 (unimplemented clock_getres)",		/* 193 = unimplemented clock_getres */
	"#194 (unimplemented timer_create)",		/* 194 = unimplemented timer_create */
	"#195 (unimplemented timer_delete)",		/* 195 = unimplemented timer_delete */
	"#196 (unimplemented timer_settime)",		/* 196 = unimplemented timer_settime */
	"#197 (unimplemented timer_gettime)",		/* 197 = unimplemented timer_gettime */
	"#198 (unimplemented timer_getoverrun)",		/* 198 = unimplemented timer_getoverrun */
	"nanosleep",			/* 199 = nanosleep */
	"facl",			/* 200 = facl */
	"#201 (unimplemented door)",		/* 201 = unimplemented door */
	"setreuid",			/* 202 = setreuid */
	"setregid",			/* 203 = setregid */
	"#204 (unimplemented install_utrap)",		/* 204 = unimplemented install_utrap */
	"#205 (unimplemented signotify)",		/* 205 = unimplemented signotify */
	"#206 (unimplemented schedctl)",		/* 206 = unimplemented schedctl */
	"#207 (unimplemented pset)",		/* 207 = unimplemented pset */
	"#208 (unimplemented)",		/* 208 = unimplemented */
	"resolvepath",			/* 209 = resolvepath */
	"#210 (unimplemented signotifywait)",		/* 210 = unimplemented signotifywait */
	"#211 (unimplemented lwp_sigredirect)",		/* 211 = unimplemented lwp_sigredirect */
	"#212 (unimplemented lwp_alarm)",		/* 212 = unimplemented lwp_alarm */
	"getdents64",			/* 213 = getdents64 */
	"mmap64",			/* 214 = mmap64 */
	"stat64",			/* 215 = stat64 */
	"lstat64",			/* 216 = lstat64 */
	"fstat64",			/* 217 = fstat64 */
	"statvfs64",			/* 218 = statvfs64 */
	"fstatvfs64",			/* 219 = fstatvfs64 */
	"setrlimit64",			/* 220 = setrlimit64 */
	"getrlimit64",			/* 221 = getrlimit64 */
	"pread64",			/* 222 = pread64 */
	"pwrite64",			/* 223 = pwrite64 */
	"creat64",			/* 224 = creat64 */
	"open64",			/* 225 = open64 */
	"#226 (unimplemented rpcsys)",		/* 226 = unimplemented rpcsys */
	"#227 (unimplemented)",		/* 227 = unimplemented */
	"#228 (unimplemented)",		/* 228 = unimplemented */
	"#229 (unimplemented)",		/* 229 = unimplemented */
	"socket",			/* 230 = socket */
	"socketpair",			/* 231 = socketpair */
	"bind",			/* 232 = bind */
	"listen",			/* 233 = listen */
	"accept",			/* 234 = accept */
	"connect",			/* 235 = connect */
	"shutdown",			/* 236 = shutdown */
	"recv",			/* 237 = recv */
	"recvfrom",			/* 238 = recvfrom */
	"recvmsg",			/* 239 = recvmsg */
	"send",			/* 240 = send */
	"sendmsg",			/* 241 = sendmsg */
	"sendto",			/* 242 = sendto */
	"getpeername",			/* 243 = getpeername */
	"getsockname",			/* 244 = getsockname */
	"getsockopt",			/* 245 = getsockopt */
	"setsockopt",			/* 246 = setsockopt */
	"#247 (unimplemented sockconfig)",		/* 247 = unimplemented sockconfig */
	"ntp_gettime",			/* 248 = ntp_gettime */
#if defined(NTP) || !defined(_KERNEL)
	"ntp_adjtime",			/* 249 = ntp_adjtime */
#else
	"#249 (excluded ntp_adjtime)",		/* 249 = excluded ntp_adjtime */
#endif
	"#250 (unimplemented lwp_mutex_unlock)",		/* 250 = unimplemented lwp_mutex_unlock */
	"#251 (unimplemented lwp_mutex_trylock)",		/* 251 = unimplemented lwp_mutex_trylock */
	"#252 (unimplemented lwp_mutex_init)",		/* 252 = unimplemented lwp_mutex_init */
	"#253 (unimplemented cladm)",		/* 253 = unimplemented cladm */
	"#254 (unimplemented lwp_sigtimedwait)",		/* 254 = unimplemented lwp_sigtimedwait */
	"#255 (unimplemented umount2)",		/* 255 = unimplemented umount2 */
};