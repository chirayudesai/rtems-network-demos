/*
 *  $Id$
 */

#ifndef __USER_CFG_h
#define __USER_CFG_h

/*
 * Board ethernet address
 * REPLACE THIS WITH YOUR OWN VALUE BEFORE TRYING ANY OF THESE PROGRAMS!!!
 */
#define MY_ETHERNET_ADDRESS "00:80:7F:22:61:77"

/*
 * Some board support packages let the network driver
 * get the Ethernet address from the bootstrap PROM.
 */
/*
#define MY_ETHERNET_ADDRESS "prom"
*/


/*
 * Define this if we should use BOOTP to get information about the target?
 */
/* #define USE_BOOTP    1 */

/*
 *  If you don't use BOOTP, then you must define MY_IP_ADDRESS.
 *  CHOOSE A VALUE APPROPRIATE TO YOUR NETWORK!
 */

#define MY_IP_ADDRESS   "192.168.1.100"


/*
 *  Define this if you want to see debugging messages.
 */

#define TRACE_NETWORK_DRIVER 1

/*
 *  Do some consistency checking.
 */

#ifndef MY_ETHERNET_ADDRESS
#error "Need to define a hardware Ethernet address"
#endif

#if !defined(USE_BOOTP) && !defined(MY_IP_ADDRESS)
#error "MY_IP_ADDRESS is not defined"
#endif

#endif
/* end of include file */
