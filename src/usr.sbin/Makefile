#	$NetBSD: Makefile,v 1.199.2.1 2005/04/20 11:15:13 tron Exp $
#	from: @(#)Makefile	5.20 (Berkeley) 6/12/93

.include <bsd.own.mk>

SUBDIR=	ac accton altq amd apm apmd arp bad144 bind bootp catman \
	chown chroot chrtbl cnwctl config cron dev_mkdb \
	dhcp diskpart dumpfs dumplfs edquota eeprom \
	envstat eshconfig etcupdate fssconfig grfconfig \
	grfinfo gspa hilinfo ifwatchd inetd installboot \
	iopctl iostat ipwctl irdaattach isdn iteconfig iwictl\
	kgmon lastlogin link lpr mailwrapper makefs \
	map-mbone mdconfig memswitch mlxctl mopd mountd moused \
	mrinfo mrouted mscdlabel mtrace \
	mtree ndbootd netgroup_mkdb nfsd ntp pcictl pkg_install pppd \
	pstat pwd_mkdb postinstall powerd quot quotacheck quotaon \
	rarpd rbootd rdate \
	repquota rmt rpc.bootparamd rpc.lockd rpc.pcnfsd \
	rpc.statd rpcbind rwhod sa screenblank sesd \
	sliplogin slstats sntp \
	spray sunlabel sup sushi syslogd tadpolectl tcpdchk \
	tcpdmatch tcpdump timed tpctl traceroute trpt trsp unlink \
	usbdevs user videomode vipw vnconfig wiconfig wlanctl wsconscfg \
	wsfontload wsmoused wsmuxctl zdump zic

.if (${MKKERBEROS} != "no")
SUBDIR+= hprop kadmin kdc kstash ktutil
.endif

.if (${MKYP} != "no")
SUBDIR+= rpc.yppasswdd ypbind yppoll ypserv ypset
.endif

.if ${TOOLCHAIN_MISSING} != "no"
SUBDIR+= mdsetimage
.endif

.if (${MKINET6} != no)
SUBDIR+=faithd ifmcstat mld6query ndp \
	rip6query route6d rtadvd traceroute6 rtsold
.endif

# ATM PVC
SUBDIR+=pvcsif pvctxctl

.if (${MKCRYPTO} != "no")
SUBDIR+= racoon racoonctl
.endif

# IP Filter
.if (${MKIPFILTER} != "no")
SUBDIR+=ipf
.endif

# PF
.if (${MKPF} != "no")
SUBDIR+=pf
.endif

.include <bsd.subdir.mk>
