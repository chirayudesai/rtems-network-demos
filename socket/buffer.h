/*
 * buffer.h : generate an executable to test the sockets.
 * contains some useful declarations
 *
 *  $Id$
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdio.h>


#define SIZE_MAX_IP_HEADER	20	
#define SIZE_MAX_TCP_HEADER    	20	
#define SIZE_MAX_UDP_HEADER    	8	
#define SIZE_MAX_BUFFER	       	5*1024 - SIZE_MAX_IP_HEADER - SIZE_MAX_TCP_HEADER 

/*
 *The receive buffer size max shall be able to receive 
 *the payload and the headers (IP and TCP/UDP):
 *- max IP header = 0xf ulong (ulong = 4bytes)	
 *- max TCP header = 0xf ulong (ulong = 4bytes)	
 *- max UDP header = 8 bytes)
 */
#define SIZE_MAX_RCV_BUFFER		SIZE_MAX_BUFFER + (2*4*0xf)

#define DEFAULT_PORT_SERVER 12345
#define DEFAULT_PORT_CLIENT 54321
#define DEF_MAXLOOP		-1


unsigned char * BuildBuffer();
void FreeBuffer(unsigned char * p_Buffer_puc);
unsigned char * AllocBuffer();
unsigned long CheckBuffer(unsigned char * p_Buffer_puc);

#endif /* _BUFFER_H */

