# 	$NetBSD: POWEROFSEVEN.inc,v 1.32.2.3 2005/07/18 21:13:48 riz Exp $
#
# from: NetBSD: GENERIC32,v 1.13 2001/02/28 00:08:18 mjacob Exp
#
# Configuration for Ultra5/Ultra10

include "arch/sparc64/conf/std.sparc64"

#options 	INCLUDE_CONFIG_FILE	# embed config file in kernel binary

maxusers	64

## System kernel configuration.  See options(4) for more detail.


# Options for variants of the Sun SPARC architecure.
# We currently support three architecture types; at least one is required.
options 	SUN4U		# sun4u - UltraSPARC

## System options specific to the sparc machine type

options 	BLINK			# Blink the system LED

#### System options that are the same for all ports

## Root device configuration: change the ?'s if you are going to use a
## nonstandard root partition (other than where the kernel is booted from)
## and/or nonstandard root type (not ffs or nfs).  Normally this can be
## automagically determined at boot time.

config		netbsd	root on ? type ?

## UVM options.
#options 	UVM_PAGE_TRKOWN
#options 	UVMHIST
#options 	UVMHIST_PRINT	# Loud!

## System call tracing (see ktrace(1)).
options 	KTRACE

## Collect statistics on kernel malloc's and free's.  This does have a
## significant performance hit on slower machines, so it is intended for
## diagnostic use only.
options 	KMEMSTATS

## System V compatible IPC subsystem.  (msgctl(2), semctl(2), and shmctl(2))
options 	SYSVMSG		# System V message queues
options 	SYSVSEM		# System V semaphores
options 	SYSVSHM		# System V shared memory
#options 	SHMMAXPGS=1024	# 1024 pages is the default

## Loadable kernel module support; still under development.
options 	LKM

options 	USERCONF	# userconf(4) support
#options 	PIPE_SOCKETPAIR	# smaller, but slower pipe(2)
#options 	SYSCTL_INCLUDE_DESCR	# Include sysctl descriptions in kernel

## NFS boot options; default on sparc is the bootparam protocol
options 	NFS_BOOT_BOOTPARAM
#options 	NFS_BOOT_BOOTP
#options 	NFS_BOOT_DHCP

#### Debugging options

## The DDB in-kernel debugger runs at panic (unless DDB_ONPANIC=0), or at
## serial console break or keyboard reset, where the PROM would normally
## intercept.  DDB_HISTORY_SIZE adds up/down arrow command history.
# we enable DDB in GENERIC for now.
options 	DDB			# kernel dynamic debugger
options 	DDB_HISTORY_SIZE=100	# enable history editing in DDB
#options 	DDB_ONPANIC		# see also sysctl(8): `ddb.onpanic'

## Compile the kernel with debugging symbols (`netbsd.gdb' is the debug file),
## such that gdb(1) can be used on a kernel coredump.

makeoptions	DEBUG="-g"
makeoptions	COPTS="-O2 -mcpu=ultrasparc"


## Adds code to the kernel that does internal consistency checks, and will
## cause the kernel to panic if corruption of internal data structures
## is detected.
#options 	DIAGNOSTIC	# extra kernel sanity checking

## Enable (possibly expensive) debugging code that may also display messages
## on the system console
#options 	DEBUG

## Make SCSI/PCI error messages more verbose when explaining their meanings.
options 	SCSIVERBOSE
options 	PCIVERBOSE
#options 	PCI_CONFIG_DUMP	# verbosely dump PCI config space

## `INSECURE' turns off the kernel security level (securelevel = 0 always).
## This allows writing to /dev/mem, loading kernel modules while multi-user,
## and other insecurities good only for development work.  Do not use this
## option on a production machine.
options 	INSECURE

## `FDSCRIPTS' allows non-readable but executable scripts by providing a
## pre-opened opaque file to the script interpreter.  `SETUIDSCRIPTS',
## which implies FDSCRIPTS, allows scripts to be set-user-id using the same
## opaque file mechanism.  Perl calls this "secure setuid scripts."

#options 	FDSCRIPTS
#options 	SETUIDSCRIPTS

## Options for compatibility with previous releases foreign system binaries.
## In the cases of COMPAT_SUNOS and COMPAT_SVR4, you may need to set up
## additional user-level utilities or system configuration files. See
## compat_sunos(8) and compat_svr4(8).

options 	COMPAT_43	# 4.3BSD system interfaces
options 	COMPAT_09	# NetBSD 0.9 binary compatibility (SUNOS)
options 	COMPAT_10	# NetBSD 1.0 binary compatibility (SUNOS)
#options 	COMPAT_11	# NetBSD 1.1 binary compatibility
options 	COMPAT_12	# NetBSD 1.2 binary compatibility (SUNOS)
options 	COMPAT_13	# NetBSD 1.3 binary compatibility
options 	COMPAT_14	# NetBSD 1.4 binary compatibility
options 	COMPAT_16	# NetBSD 1.6 binary compatibility
options 	COMPAT_20	# NetBSD 2.0 binary compatibility
#options 	COMPAT_SVR4_32	# SunOS 5.x 32-bit binary compatibility -- 64-bit only
#options 	COMPAT_NETBSD32	# NetBSD/sparc binary compatibility -- 64-bit only
#options 	COMPAT_SUNOS	# SunOS 4.x binary compatibility
#options 	COMPAT_SVR4	# SunOS 5.x binary compatibility
#options 	EXEC_ELF32	# Exec module for 32 bit ELF binaries.
#options 	EXEC_AOUT	# execve(2) support for a.out binaries

#options 	TCP_COMPAT_42	# 4.2BSD TCP/IP bug compat. Not recommended.
options		COMPAT_BSDPTY	# /dev/[pt]ty?? ptys.

## File systems.  You probably need at least one of FFS or NFS.
file-system	FFS		# Berkeley Fast Filesystem
file-system	NFS		# Sun NFS-compatible filesystem client
file-system	KERNFS		# kernel data-structure filesystem
file-system	NULLFS		# NULL layered filesystem
file-system	MFS		# memory-based filesystem
#file-system	FDESC		# user file descriptor filesystem
#file-system	UMAPFS		# uid/gid remapping filesystem
#file-system	LFS		# Log-based filesystem (still experimental)
#file-system	PORTAL		# portal filesystem (still experimental)
#file-system	PROCFS		# /proc
file-system	CD9660		# ISO 9660 + Rock Ridge file system
#file-system	UNION		# union file system
file-system	MSDOSFS		# MS-DOS FAT filesystem(s).

## File system options.
options 	NFSSERVER	# Sun NFS-compatible filesystem server
#options 	QUOTA		# FFS quotas
options 	FFS_EI		# FFS Endian Independent support
#options 	SOFTDEP		# FFS soft updates support.
#options 	FFS_NO_SNAPSHOT	# ffs snapshots

## Network protocol support.  In most environments, INET is required.
options 	INET		# IP (Internet Protocol) v4
#options 	INET6		# IPV6
#options 	IPSEC		# IP security
#options 	IPSEC_ESP	# IP security (encryption part; define w/IPSEC)
#options 	IPSEC_NAT_T	# IPsec NAT traversal (NAT-T)
#options 	IPSEC_DEBUG	# debug for IP security
#options 	GATEWAY		# packet forwarding ("router switch")
#options 	MROUTING	# packet forwarding of multicast packets
#options 	PIM		# Protocol Independent Multicast
#options 	DIRECTED_BROADCAST	# allow broadcasts through routers
#options 	NS		# Xerox NS networking
#options 	NSIP		# Xerox NS tunneling over IP
#options 	ISO,TPIP	# OSI networking
#options 	EON		# OSI tunneling over IP
#options 	CCITT,LLC,HDLC	# X.25 packet switched protocol
#options 	NETATALK	# AppleTalk (over Ethernet) protocol
options 	NTP		# Network Time Protocol in-kernel support
#options 	PPS_SYNC	# Add serial line synchronization for NTP
options 	PFIL_HOOKS	# Add pfil(9) hooks, intended for custom LKMs.
options 	IPFILTER_LOG	# Add ipmon(8) logging for ipfilter device
#options 	PPP_BSDCOMP	# Add BSD compression to ppp device
#options 	PPP_DEFLATE	# Add deflate (libz) compression to ppp device
#options 	PPP_FILTER	# Add active filters for ppp (via bpf)
#options 	TCP_DEBUG	# Record last TCP_NDEBUG packets with SO_DEBUG


#### Main bus and CPU .. all systems.
mainbus0 at root
cpu0	at mainbus0

#### Bus types found on SPARC systems.

#sbus0	at mainbus0				# Ultra 1
psycho*	at mainbus0				# Darwin, Ultra5
pci*	at psycho?
pci*	at ppb?
ppb*	at pci? dev ? function ?		# `APB' support.
ebus*	at pci?					# ebus devices
# XXX 'puc's aren't really bridges, but there's no better place for them here
#puc*	at pci? dev ? function ?		# PCI "universal" comm. cards

#### Standard system devices -- all required for a given architecture

## PROM console driver -- if all else fails
pcons0	at mainbus0				# PROM console

## Auxiliary system registers -- We use the OBP for power management
#auxio*	at sbus? slot ? offset ?		# auxio registers
auxio0	at ebus?				# auxio registers

# We also need:
# bpp0 at sbus0					# parallel port
lpt*	at ebus?				# parallel port

## Mostek clock found on 4/300, sun4c, sun4m and sun4u systems.
## The Mostek clock NVRAM is the "eeprom" on sun4/300 systems.
#clock0	at sbus0 slot ? offset ?
clock0	at ebus?

## Timer chip found on 4/300, sun4c, sun4m and sun4u systems.
timer0	at mainbus0				# sun4c

#### Serial port configuration

## Sbus machines:
##	Zilog 8530 serial chips.  Each has two-channels.
##	zs0 is ttya and ttyb.  zs1 is the keyboard and mouse.
#zs0	at sbus0 slot ? offset ?
#zstty0	at zs0 channel 0	# ttya
#zstty1	at zs0 channel 1	# ttyb
#kbd0	at zstty?
#ms0	at zstty?

## PCI machines:
##	Siemens SAB82532 controller: ttya and ttyb (sab)
##	Part of NS PC87332VLJ Super I/O controller: kbd/mouse (com)

## The SAB82532 controller has two serial ports
sab*	at ebus?				# ttya/ttyb
sabtty*	at sab? channel ?

## Part of a PC87332VLJ
com*	at ebus?				# `com' driver for `su'
kbd0	at com?					# keyboard
ms0	at com?					# mouse

#### Disk controllers and disks

## The following flags may be set for the NCR53c94 based esp driver:
##	bits 0-7:  disable disconnect/reselect for the corresponding target
##	bits 8-15: disable synchronous negotiation for target [bit-8]

## sun4/300, sun4c, sun4m and sun4u on-board SCSI, and FSBE/S SBus SCSI cards.
## Both `dma' and `esp' are needed in all cases.
## Two kinds of additional SBus SCSI interfaces are available.  One uses
## "esp at sbus" like the sun4c on-board; the other uses "esp at dma".

## sun4/300 SCSI - an NCR53c94 or equivalent behind
## an LSI Logic DMA controller

#dma0	at sbus0 slot ? offset ?			# sun4c/sun4m/sun4u
#esp0	at dma0 flags 0x0000				# sun4m/sun4u
#scsibus* at esp?

# FSBE/S SCSI
#dma*	at sbus? slot ? offset ?			# SBus
#esp*	at dma? flags 0x0000				# SBus

#scsibus* at esp?

## Qlogic ISP SBus or PCI SCSI Card
#isp*	at sbus? slot ? offset ?
#isp*	at pci?
#scsibus* at isp?

## FAS support missing
#fas*	at sbus? slot ? offset ?
#scsibus* at fas?

## GLM support
#siop*	at pci? dev ? function ?			# 53C875 "glm" compatible
#scsibus* at siop?

# PCI SCSI cards
#ahc*	at pci? dev ? function ?
#scsibus* at ahc?

## These entries find devices on all SCSI busses and assign
## unit numbers dynamically.
#sd*	at scsibus? target ? lun ?		# SCSI disks
#st*	at scsibus? target ? lun ?		# SCSI tapes
#cd*	at scsibus? target ? lun ?		# SCSI CD-ROMs
#ch*	at scsibus? target ? lun ?		# SCSI changer devices
#ss*	at scsibus? target ? lun ?		# SCSI scanners
#ses*	at scsibus? target ? lun ?		# SCSI SES/SAF-TE devices
#uk*	at scsibus? target ? lun ?		# unknown SCSI

# PCI IDE.
pciide* at pci? dev ? function ? flags 0x0000	# GENERIC pciide driver
acardide* at pci? dev ? function ?	# Acard IDE controllers
aceride* at pci? dev ? function ?	# Acer Lab IDE controllers
artsata* at pci? dev ? function ?	# Intel i31244 SATA controller
cmdide* at pci? dev ? function ?	# CMD tech IDE controllers
cypide* at pci? dev ? function ?	# Cypress IDE controllers
hptide* at pci? dev ? function ?	# Triones/HighPoint IDE controllers
optiide* at pci? dev ? function ?	# Opti IDE controllers
pdcide* at pci? dev ? function ?	# Promise IDE controllers
pdcsata* at pci? dev ? function ?	# Promise SATA150 controllers
satalink* at pci? dev ? function ?	# SiI SATALink controllers
siside* at pci? dev ? function ?	# SiS IDE controllers
slide*  at pci? dev ? function ?	# Symphony Labs IDE controllers
viaide* at pci? dev ? function ?	# VIA/AMD/Nvidia IDE controllers
atabus* at ata?
wd*	at atabus? drive ? flags 0x0000
atapibus* at atapi?

cd*	at atapibus? drive ? flags 0x0000	# ATAPI CD-ROM drives
sd*	at atapibus? drive ? flags 0x0000	# ATAPI disk drives
uk*	at atapibus? drive ? flags 0x0000	# ATAPI unknown

## Floppy controller and drive found on SPARCstations.

# need share with the sparc...uses auxreg.  what is this on sparc64?
#fdc0	at sbus0 slot ? offset ?
#fdc0	at pci?					# `fdthree'
#fd*	at fdc0 	 			# the drive itself

## A disk-like interface to files.  Can be used to create floppy, CD,
## miniroot images, etc.

#pseudo-device	vnd	4

## Concatenated and striped disks; with this, you can create a software-based
## disk array similar to a "RAID 0" setup.  See ccd(4).

#pseudo-device	ccd	4

## RAIDframe disk driver: software RAID driver.  See raid(4).

pseudo-device	raid	4
options 	RAID_AUTOCONFIG		# auto-configuration of RAID components
# Options to enable various other RAIDframe RAID types.
# options 	RF_INCLUDE_EVENODD=1
# options 	RF_INCLUDE_RAID5_RS=1
# options 	RF_INCLUDE_PARITYLOGGING=1
# options 	RF_INCLUDE_CHAINDECLUSTER=1
# options 	RF_INCLUDE_INTERDECLUSTER=1
# options 	RF_INCLUDE_PARITY_DECLUSTERING=1
# options 	RF_INCLUDE_PARITY_DECLUSTERING_DS=1

## Memory disk device, used on boot floppies with compressed
## kernel-plus-root-disk images.

#pseudo-device	md	1


#### Network interfaces

## LANCE Ethernet - an AMD 7990 LANCE behind specialized DMA glue
## Three flavors of additional SBus ethernets are available.  One attaches
## directly like the sun4c on-board, one uses the ledma device like the
## sun4m on-board, and one uses the lebuffer device.

#ledma0		at sbus0 slot ? offset ?		# sun4m on-board
#le0		at ledma0				# sun4m on-board
#le*		at sbus? slot ? offset ?		# SBus
#ledma*		at sbus? slot ? offset ?		# SBus
#le*		at ledma?				# SBus
#lebuffer0	at sbus? slot ? offset ?		# SBus
#le0		at lebuffer?				# SBus
#lebuffer*	at sbus? slot ? offset ?		# SBus
#le*		at lebuffer?				# SBus

## Happy Meal Ethernet
#hme*		at sbus? slot ? offset ?
hme*		at pci?	dev ? function ?		# network "hme" compatible

## qec/be, qec/hme
#qec*		at sbus? slot ? offset ?
#be*		at qec?
#qe*		at qec?

# DEC Tulip
tlp*		at pci? dev ? function ?

# MII/PHY support
#exphy*	at mii? phy ?			# 3Com internal PHYs
#icsphy*	at mii? phy ?			# Integrated Circuit Systems ICS189x
#inphy*	at mii? phy ?			# Intel 82555 PHYs
#lxtphy*	at mii? phy ?			# Level One LXT-970 PHYs
nsphy*	at mii? phy ?			# NS83840 PHYs
#qsphy*	at mii? phy ?			# Quality Semiconductor QS6612 PHYs
#sqphy*	at mii? phy ?			# Seeq 80220/80221/80223 PHYs
#tlphy*	at mii? phy ?			# ThunderLAN PHYs
ukphy*	at mii? phy ?			# generic unknown PHYs

## Loopback network interface; required
pseudo-device	loop

## SLIP and CSLIP interfaces, for IP over a serial line.
#pseudo-device	sl		2

## PPP, the successor to SLIP.  See pppd(8).
#pseudo-device	ppp		2

## Starmode Radio IP, a special hardware network device.
#pseudo-device	strip		1

## Network "tunnel" device, allowing protocol stacks to run in the userland.
## This is used by the third-party user-mode "ppp" program, and others.
#pseudo-device	tun		4

## Generic L3 over IP tunnel
#pseudo-device	gre		2	# generic L3 over IP tunnel

## Berkeley Packet Filter, required to run RARPD.  A generic C-language
## interface that allows selective examining of incoming packets.
pseudo-device	bpfilter	8

## IP Filter, used in firewall and NAT applications.  See ipnat(8) for
## one example of the use of the IP Filter.
pseudo-device	ipfilter


#### Audio and video devices

## /dev/audio support (`audiocs' plus `audio')
##
#audiocs*	at sbus? slot ? offset ?		# SUNW,CS4231
audiocs*	at ebus?				# SUNW,CS4231
audio*		at audiocs?


## Sun "bwtwo" black and white framebuffer, found on sun4, sun4c, and sun4m
## systems.  If your sun4 system has a cgfour installed in the P4 slot,
## the P4 entries for "bwtwo" will attach to the overlay plane of the
## "cgfour".

#bwtwo0		at sbus0 slot ? offset ?		# sun4c and sun4m
#bwtwo*		at sbus? slot ? offset ?		#

## Sun "cgthree" Sbus color framebuffer
#cgthree0	at sbus? slot ? offset ?
#cgthree*	at sbus? slot ? offset ?

## Sun "cgsix" accelerated color framebuffer.
#cgsix0		at sbus? slot ? offset ?
#cgsix*		at sbus? slot ? offset ?

## Sun "tcx" accelerated color framebuffer.
#tcx0		at sbus? slot ? offset ?
#tcx*		at sbus? slot ? offset ?

# Sun "cgfourteen" accelerated 24-bit framebuffer.
#cgfourteen0	at obio0			# sun4m

#### Other device configuration

## Pseudo ttys, required for network logins and programs like screen.

pseudo-device	pty			# pseudo-ttys (for network, etc.)

## Random device, used to implement /dev/random (a source of random noise),
## and generate randomness for some kernel formulae.

pseudo-device	rnd

pseudo-device	clockctl		# user control of clock subsystem
