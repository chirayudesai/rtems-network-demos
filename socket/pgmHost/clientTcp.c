/*
 * clientTcp.c : generate an executable to test the TCP Sockets.
 * the RTEMS target must be configured as server.
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

#define DEFAULT_IP "194.2.81.61" 
#define DEFAULT_LISTEN_IP "194.2.81.126"

int main(int argc, char** argv )
{
  int   sd;
  struct   sockaddr_in server;
  struct  hostent *hp;
  struct sockaddr_in from;
  int fromlen;
  int i,retval;
  char	*tabChar;
  char *server_name= "localhost";
  unsigned short port;
  unsigned long	lenChar;
  struct timespec timeToWait, timeRem;

  printf("main begin...\n");
  port = DEFAULT_PORT_SERVER;
  server_name = DEFAULT_IP;

  bzero(&server, sizeof(server));

  sd = socket (AF_INET,SOCK_STREAM,0);
  printf("socket() result:<%d>\n", sd);

  if (sd<0) {
    printf("opening stream socket\n");
    exit(-1);
  }
  	
  if (inet_aton (server_name, (struct in_addr*)&(server.sin_addr.s_addr)) == 0) {
    printf("%s : IP address: bad format\n",
	   argv[0]);
  } 

  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  /* Connect to TCP/SERVER */
  if ( (retval = connect(sd, &server, sizeof(server))) < 0 ) {
    close(sd);
    perror("connecting stream socket");
    exit(0);
  }
  printf("connect() result:<%d>\n", retval);
  fromlen = sizeof(from);
  if ( (retval = getpeername(sd,&from,&fromlen)) <0){
    perror("could't get peername\n");
    exit(1);
  }
  printf("getpeername() result:<%d>\n", retval);
  printf("Connected to TCP/Server:");
  printf("%s:%d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));

  if ((hp = gethostbyaddr((const char *)(&from.sin_addr.s_addr),
			  sizeof(from.sin_addr.s_addr),AF_INET)) == NULL)
    fprintf(stderr, "Can't find host %s\n", inet_ntoa(from.sin_addr));
  else
    printf("(Name is : %s)\n", hp->h_name);

  printf("Now, let's send packets...");

  while(1) {
	
    tabChar = BuildBuffer();
    lenChar = *((unsigned long *)(tabChar));
    
    i = send(sd, tabChar, lenChar, 0);
    if (i == -1) 
      {
	fprintf(stderr,"send() failed (at least locally detected errors)\n");
	return -1;
      }
    timeToWait.tv_sec = 1;
    timeToWait.tv_nsec = 0;
    nanosleep(&timeToWait, &timeRem);
          
    FreeBuffer(tabChar);
   
  }
  
  close(sd);
  exit (0);
}	


