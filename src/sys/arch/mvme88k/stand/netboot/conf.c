/*	$OpenBSD: conf.c,v 1.3 1999/09/27 19:30:01 smurph Exp $ */

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>

#include <stand.h>
#include <nfs.h>
#include <dev_net.h>

struct fs_ops file_system[] = {
	{ nfs_open, nfs_close, nfs_read, nfs_write, nfs_seek, nfs_stat },
};
int nfsys = sizeof(file_system) / sizeof(file_system[0]);

struct devsw devsw[] = {
	{ "net",  net_strategy,  net_open,  net_close,  net_ioctl },
};
int	ndevs = sizeof(devsw) / sizeof(devsw[0]);
/*
extern struct netif_driver le_driver;
*/
extern struct netif_driver ie_driver;

struct netif_driver *netif_drivers[] = {
   &ie_driver,
/*	
   &le_driver,
*/
};
int n_netif_drivers = sizeof(netif_drivers) / sizeof(netif_drivers[0]);


/* XXX */
int netif_debug;
int debug;
int errno;
int try_bootp = 1;  /* dev_net.c */
