/*
 * serverTcp.c : generate an executable to test the TCP Sockets.
 * the RTEMS target must be configured as client.
 * Don't forget to change the IP address.
 *
 *  $Id$
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#include "../buffer.h"

#define DEFAULT_IP "194.2.81.126" 
#define DEFAULT_LISTEN_IP "194.2.81.61"

int main(int argc, char** argv)
{
  int   sd, psd;
  struct   sockaddr_in server;
  struct sockaddr_in from;
  int fromlen;
  char *tabChar;
  int rc;
  int i;
  char *server_name= "localhost";
  unsigned short port;

  
  printf("main begin...\n");
  port = DEFAULT_PORT_CLIENT;
  server_name = DEFAULT_IP;

  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  
  if (inet_aton (server_name, (struct in_addr*)&(server.sin_addr.s_addr)) == 0) {
    printf("Usage : %s IP address in ascii format : xxx.xxx.xxx.xxx\n",
	   argv[0]);
    return 1;
  }

  server.sin_port = htons(port);
  
  sd = socket (PF_INET,SOCK_STREAM,0);
  printf("socket() returns <%d>\n", sd);

  if (sd<0) {
    printf("socket() call failure: opening stream socket\n");
    exit(-1);
  }	

  rc = bind( sd, &server, sizeof(server) ) ;
  if (rc == -1) {
    printf("%s : bind failed errno = %d\n", argv[0], errno);
    close(sd);
    return 1;
  }
  printf("bind() returns <%d>\n", rc);

  printf("Server Port is: %d\n", ntohs(server.sin_port));
  rc = listen(sd,5);
  printf("listen() returns <%d>\n", rc);
  fromlen = sizeof(from);
    
  tabChar = AllocBuffer();
  if(!tabChar) {
    printf("malloc failed ...\n");
    return 1;
  }

  psd  = accept(sd, &from, &fromlen);
  printf("accept() returns <%d>\n", psd);


   printf("TCP Server : ready to receive messages...\n");
   
  for(;;){
    rc = recv(psd, tabChar, SIZE_MAX_RCV_BUFFER, 0 );

    if (rc == -1) {
      printf("%s : recv() failed errno = %d\n", argv[0], errno);
      close(sd);
      return 1;
    }

    if (rc == 0) {
      continue;
    }
    if (CheckBuffer(tabChar) == 0){
      printf("Received %d bytes, data [%s] from client\n",rc,tabChar);
      printf("Packet received:<%d> bytes\n", rc);
      printf("Received:<");
      tabChar += 8;
      for (i=0; i<rc ; i++) printf("%c",tabChar[i]);
      printf(">\n");
    }
  }

  FreeBuffer(tabChar);

}	

