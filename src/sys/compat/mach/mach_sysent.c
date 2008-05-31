/* $NetBSD: mach_sysent.c,v 1.16 2005/02/26 23:58:20 perry Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.7 2005/02/26 23:10:20 perry Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_sysent.c,v 1.16 2005/02/26 23:58:20 perry Exp $");

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
#include <compat/mach/mach_types.h>
#include <compat/mach/mach_message.h>
#include <compat/mach/mach_clock.h>
#include <compat/mach/mach_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent mach_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 1 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 2 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 3 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 4 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 5 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 6 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 7 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 8 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 9 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 10 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 11 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 12 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 13 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 14 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 15 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 16 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 17 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 18 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 19 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 20 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 21 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 22 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 23 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 24 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 25 = unimplemented */
	{ 0, 0, 0,
	    mach_sys_reply_port },		/* 26 = reply_port */
	{ 0, 0, 0,
	    mach_sys_thread_self_trap },	/* 27 = thread_self_trap */
	{ 0, 0, 0,
	    mach_sys_task_self_trap },		/* 28 = task_self_trap */
	{ 0, 0, 0,
	    mach_sys_host_self_trap },		/* 29 = host_self_trap */
	{ 0, 0, 0,
	    sys_nosys },			/* 30 = unimplemented */
	{ 7, s(struct mach_sys_msg_trap_args), 0,
	    mach_sys_msg_trap },		/* 31 = msg_trap */
	{ 9, s(struct mach_sys_msg_overwrite_trap_args), 0,
	    mach_sys_msg_overwrite_trap },	/* 32 = msg_overwrite_trap */
	{ 1, s(struct mach_sys_semaphore_signal_trap_args), 0,
	    mach_sys_semaphore_signal_trap },	/* 33 = semaphore_signal_trap */
	{ 1, s(struct mach_sys_semaphore_signal_all_trap_args), 0,
	    mach_sys_semaphore_signal_all_trap },/* 34 = semaphore_signal_all_trap */
	{ 2, s(struct mach_sys_semaphore_signal_thread_trap_args), 0,
	    mach_sys_semaphore_signal_thread_trap },/* 35 = semaphore_signal_thread_trap */
	{ 1, s(struct mach_sys_semaphore_wait_trap_args), 0,
	    mach_sys_semaphore_wait_trap },	/* 36 = semaphore_wait_trap */
	{ 2, s(struct mach_sys_semaphore_wait_signal_trap_args), 0,
	    mach_sys_semaphore_wait_signal_trap },/* 37 = semaphore_wait_signal_trap */
	{ 3, s(struct mach_sys_semaphore_timedwait_trap_args), 0,
	    mach_sys_semaphore_timedwait_trap },/* 38 = semaphore_timedwait_trap */
	{ 4, s(struct mach_sys_semaphore_timedwait_signal_trap_args), 0,
	    mach_sys_semaphore_timedwait_signal_trap },/* 39 = semaphore_timedwait_signal_trap */
	{ 0, 0, 0,
	    sys_nosys },			/* 40 = unimplemented */
	{ 0, 0, 0,
	    mach_sys_init_process },		/* 41 = init_process */
	{ 0, 0, 0,
	    sys_nosys },			/* 42 = unimplemented */
	{ 5, s(struct mach_sys_map_fd_args), 0,
	    mach_sys_map_fd },			/* 43 = map_fd */
	{ 0, 0, 0,
	    sys_nosys },			/* 44 = unimplemented */
	{ 3, s(struct mach_sys_task_for_pid_args), 0,
	    mach_sys_task_for_pid },		/* 45 = task_for_pid */
	{ 2, s(struct mach_sys_pid_for_task_args), 0,
	    mach_sys_pid_for_task },		/* 46 = pid_for_task */
	{ 0, 0, 0,
	    sys_nosys },			/* 47 = unimplemented */
	{ 4, s(struct mach_sys_macx_swapon_args), 0,
	    mach_sys_macx_swapon },		/* 48 = macx_swapon */
	{ 2, s(struct mach_sys_macx_swapoff_args), 0,
	    mach_sys_macx_swapoff },		/* 49 = macx_swapoff */
	{ 0, 0, 0,
	    sys_nosys },			/* 50 = unimplemented */
	{ 4, s(struct mach_sys_macx_triggers_args), 0,
	    mach_sys_macx_triggers },		/* 51 = macx_triggers */
	{ 0, 0, 0,
	    sys_nosys },			/* 52 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 53 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 54 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 55 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 56 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 57 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 58 = unimplemented */
	{ 1, s(struct mach_sys_swtch_pri_args), 0,
	    mach_sys_swtch_pri },		/* 59 = swtch_pri */
	{ 0, 0, 0,
	    mach_sys_swtch },			/* 60 = swtch */
	{ 3, s(struct mach_sys_syscall_thread_switch_args), 0,
	    mach_sys_syscall_thread_switch },	/* 61 = syscall_thread_switch */
	{ 5, s(struct mach_sys_clock_sleep_trap_args), 0,
	    mach_sys_clock_sleep_trap },	/* 62 = clock_sleep_trap */
	{ 0, 0, 0,
	    sys_nosys },			/* 63 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 64 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 65 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 66 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 69 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 70 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 71 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 72 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 73 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 74 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 75 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 78 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 79 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 80 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 81 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 82 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 83 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 84 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 85 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 86 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 87 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 88 = unimplemented */
	{ 1, s(struct mach_sys_timebase_info_args), 0,
	    mach_sys_timebase_info },		/* 89 = timebase_info */
	{ 1, s(struct mach_sys_wait_until_args), 0,
	    mach_sys_wait_until },		/* 90 = wait_until */
	{ 0, 0, 0,
	    mach_sys_timer_create },		/* 91 = timer_create */
	{ 1, s(struct mach_sys_timer_destroy_args), 0,
	    mach_sys_timer_destroy },		/* 92 = timer_destroy */
	{ 2, s(struct mach_sys_timer_arm_args), 0,
	    mach_sys_timer_arm },		/* 93 = timer_arm */
	{ 2, s(struct mach_sys_timer_cancel_args), 0,
	    mach_sys_timer_cancel },		/* 94 = timer_cancel */
	{ 0, 0, 0,
	    mach_sys_get_time_base_info },	/* 95 = get_time_base_info */
	{ 0, 0, 0,
	    sys_nosys },			/* 96 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 97 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 98 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 99 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 100 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 101 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 102 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 103 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 104 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 105 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 106 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 107 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 108 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 109 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 110 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 111 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 112 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 113 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 114 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 115 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 116 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 117 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 118 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 120 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 121 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 122 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 123 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 124 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 125 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 126 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 127 = unimplemented */
};

