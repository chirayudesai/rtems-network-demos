/*
 * clientUdp.c : generate an executable to test the UDP Sockets.
 * the RTEMS target must be configured as server.
 * Don't forget to change the IP address.
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

#define DEFAULT_IP "194.2.81.61" 
#define DEFAULT_LISTEN_IP "194.2.81.126"


int main(int argc, char** argv)
{
  int     sd;
  struct  sockaddr_in server;
  int i;
  char	*tabChar;
  char *server_name= "localhost";
  unsigned short port;
  unsigned long	lenChar;
  struct timespec timeToWait, timeRem;

  
  printf("main begin...\n");
  port = DEFAULT_PORT_SERVER;
  server_name = DEFAULT_IP;
  
  bzero(&server, sizeof(server));
  
  sd = socket (AF_INET,SOCK_DGRAM,0);
  if (inet_aton (server_name, (struct in_addr*)&(server.sin_addr.s_addr)) == 0) {
    printf("%s : IP address: bad format\n",
	   argv[0]);
  } 

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  
  printf("server : %X  port : %X\n", server.sin_addr.s_addr, server.sin_port);
    
  while(1) {
      
    tabChar = BuildBuffer();
    lenChar = *((unsigned long *)(tabChar));
    i = sendto(sd, tabChar, lenChar, 0, (struct sockaddr *)&server, sizeof(server));
		
    if (i == -1) {
      fprintf(stderr,"sendto() failed : %d\n", errno);
      return -1;
    }

    timeToWait.tv_sec = 1;
    timeToWait.tv_nsec = 0;
    nanosleep(&timeToWait, &timeRem);
      
    FreeBuffer(tabChar);
  }
  return 0;
}

