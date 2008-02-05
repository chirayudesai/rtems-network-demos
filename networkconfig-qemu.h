/*
 * Network configuration -- QEMU NOT using DHCP
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *  networkconfig.h,v 1.8 2001/08/31 18:11:43 joel Exp
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_

/*
 *  The following will normally be set by the BSP if it supports
 *  a single network device driver.  In the event, it supports
 *  multiple network device drivers, then the user's default
 *  network device driver will have to be selected by a BSP
 *  specific mechanism.
 */

#undef RTEMS_BSP_NETWORK_DRIVER_NAME
#undef RTEMS_BSP_NETWORK_DRIVER_ATTACH
#define RTEMS_BSP_NETWORK_DRIVER_NAME    "ne1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH  rtems_ne_driver_attach

#ifdef RTEMS_USE_BOOTP
#undef RTEMS_USE_BOOTP
#endif

/*
 * If you use DHCP on the QEMU instance, there is less configuration
 */
// #define RTEMS_USE_BOOTP

#include <bsp.h>

/*
 * Define RTEMS_SET_ETHERNET_ADDRESS if you want to specify the
 * Ethernet address here.  If RTEMS_SET_ETHERNET_ADDRESS is not
 * defined the driver will choose an address.
 */
//#define RTEMS_SET_ETHERNET_ADDRESS
#ifdef RTEMS_SET_ETHERNET_ADDRESS
#undef RTEMS_SET_ETHERNET_ADDRESS
#endif

#if (defined (RTEMS_SET_ETHERNET_ADDRESS))
/* static char ethernet_address[6] = { 0x08, 0x00, 0x3e, 0x12, 0x28, 0xb1 }; */
static char ethernet_address[6] = { 0x00, 0x80, 0x7F, 0x22, 0x61, 0x77 };
#endif

#ifdef RTEMS_USE_LOOPBACK 
/*
 * Loopback interface
 */
extern void rtems_bsdnet_loopattach();
static struct rtems_bsdnet_ifconfig loopback_config = {
        "lo0",                          /* name */
        rtems_bsdnet_loopattach,        /* attach function */
        NULL,                           /* link to next interface */
        "127.0.0.1",                    /* IP address */
        "255.0.0.0",                    /* IP net mask */
};
#endif

/*
 * Default network interface
 */
static struct rtems_bsdnet_ifconfig netdriver_config = {
        RTEMS_BSP_NETWORK_DRIVER_NAME,          /* name */
        RTEMS_BSP_NETWORK_DRIVER_ATTACH,        /* attach function */

#ifdef RTEMS_USE_LOOPBACK 
        &loopback_config,               /* link to next interface */
#else
        NULL,                           /* No more interfaces */
#endif

#if (defined (RTEMS_USE_BOOTP))
        NULL,                           /* BOOTP supplies IP address */
        NULL,                           /* BOOTP supplies IP net mask */
#else
        "10.0.2.5",                     /* IP address */
        "255.255.255.0",                /* IP net mask */
#endif /* !RTEMS_USE_BOOTP */

#if (defined (RTEMS_SET_ETHERNET_ADDRESS))
        ethernet_address,               /* Ethernet hardware address */
#else
        NULL,                           /* Driver supplies hardware address */
#endif
        0,                              /* Use default driver parameters */
        0, /* mtu */
        0, /* rbuf_count */
        0, /* xbuf_count */
        0, /* port */
        9 /* irq */
};

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
        &netdriver_config,

#if (defined (RTEMS_USE_BOOTP))
        rtems_bsdnet_do_bootp,
#else
        NULL,
#endif
        0,                      /* Default network task priority */
        256 * 1024,             /* 256K - not default mbuf capacity */
        256 * 1024,             /* 256K - not default mbuf cluster capacity */
#if (!defined (RTEMS_USE_BOOTP))
        "rtems",        /* Host name */
        "",             /* Domain name */
        "10.0.2.1",     /* Gateway */
        "10.0.0.1",     /* Log host */
        {"10.0.2.3" },  /* Name server(s) */
        {"" },          /* NTP server(s) */
#endif /* !RTEMS_USE_BOOTP */

};

/*
 * For TFTP test application
 */
#if (defined (RTEMS_USE_BOOTP))
#define RTEMS_TFTP_TEST_HOST_NAME "BOOTP_HOST"
#define RTEMS_TFTP_TEST_FILE_NAME "BOOTP_FILE"
#else
#define RTEMS_TFTP_TEST_HOST_NAME "XXX.YYY.ZZZ.XYZ"
#define RTEMS_TFTP_TEST_FILE_NAME "tftptest"
#endif

/*
 * For NFS test application
 * 
 * NFS mount and a directory to ls once mounted
 */
#define RTEMS_NFS_SERVER      "192.168.1.210"
#define RTEMS_NFS_SERVER_PATH "/home"
#define RTEMS_NFS_LS_PATH     "/mnt/nfstest"

#endif /* _RTEMS_NETWORKCONFIG_H_ */
