/*
 * serverUdp.c : generate an executable to test the UDP Sockets.
 * the RTEMS target must be configured as client.
 * Don't forget to change the IP address.
 *
 *  $Id$
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "../buffer.h"

#define DEFAULT_IP "194.2.81.126" 
#define DEFAULT_LISTEN_IP "194.2.81.61"

int main(int argc, char** argv)
{

  int   sd;
  struct   sockaddr_in server, from;
  int fromlen;
  char *tabChar;
  int rc;
  unsigned long msgId;
  char *server_name= "localhost";
  unsigned short port;

   port = DEFAULT_PORT_CLIENT;
   server_name = DEFAULT_IP;

   bzero(&server, sizeof(server));
   server.sin_family = AF_INET;
   inet_aton (server_name, (struct in_addr*)&(server.sin_addr.s_addr));

   server.sin_port = htons(port);

   sd = socket(AF_INET,SOCK_DGRAM, 0);
   if (sd == -1) {
     printf(" socket failed errno = %d\n", errno);
     return 0;
   }
  
   rc = bind(sd, (struct sockaddr *)&server, sizeof(server)) ;
   if (rc == -1) {
     printf(" bind failed errno = %d\n", errno);
     return 0;
   }

   tabChar = AllocBuffer();

   if(!tabChar) {
     printf("malloc failed ...\n");
     return 0;
     }
           
   bzero(&from, sizeof(from));
   from.sin_family = AF_INET;
   inet_aton (DEFAULT_LISTEN_IP, (struct in_addr*)&(from.sin_addr.s_addr));
   fromlen = sizeof(from);

   printf("UDP Server : ready to receive messages...\n");

   msgId = 0; 
   
   for(;;){
     rc=recvfrom(sd, tabChar, SIZE_MAX_RCV_BUFFER, 0, (struct sockaddr *)&from, &fromlen);

     if (rc < 0) {
       printf("\n Recvfrom error %d\n\n",errno);
     }
     else{
       msgId = CheckBuffer(tabChar);
     }
     if (msgId == 0)
       break ;
   }

   FreeBuffer(tabChar);
   return 0;
}  

