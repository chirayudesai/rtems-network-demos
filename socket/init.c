/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Don't forget to change the IP addresses
 *
 *  $Id$
 */

#define TEST_INIT

#define CONFIGURE_EXECUTIVE_RAM_SIZE	(512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES	20
#define CONFIGURE_MAXIMUM_TASKS		20
#define CONFIGURE_MAXIMUM_TIMERS	10
#define CONFIGURE_MAXIMUM_PERIODS	1

#include "system.h"
 
#include <rtems/error.h>
#include <ka9q/rtems_ka9q.h>
#include <ka9q/netuser.h>
#include <ka9q/socket.h>
#include <netinet/in.h>
#include <time.h>

#define ARGUMENT 0

/* define and include  for socket test */
/*#define PINKFLOYD */

/*#define SERVER*/
#define CLIENT

/*#define UDP*/    /* if enabled, testing UDP socket else TCP socket */

#include "buffer.h"


#define DEFAULT_IP_SERVER "194.2.81.61"
#define DEFAULT_LISTEN_IP_SERVER "194.2.81.126"
#define DEFAULT_IP_CLIENT "194.2.81.126"
#define DEFAULT_LISTEN_IP_CLIENT "194.2.81.61"

rtems_task Init(
  rtems_task_argument argument
)
{
  unsigned int oldPri;
  rtems_name        task_name;
  rtems_id          tid;
  rtems_status_code status;



  printf("\n\n*** SAMPLE SINGLE PROCESSOR APPLICATION ***\n\r" );
  printf("Creating and starting an application task\n\r" );
  fflush(stdout);

  rtems_task_set_priority (RTEMS_SELF, 30, &oldPri);

  rtems_ka9q_start (10);
  
#ifdef PINKFLOYD
  if (rtems_ka9q_execute_command("attach rtems "
				 "broadcast n "
				 "ip 194.2.81.127 "
				 "irno 5 "
				 "bpar 0xd0000 "
				 "port 0x240 "
				 "ether 00:E0:29:12:3f:85 "))
#else
    if (rtems_ka9q_execute_command("attach rtems "
				 "broadcast y "
				 "ip 194.2.81.61 "
				 "irno 3 "
				 "bpar 0xcc000 "
				 "port 0x260 "
				 "ether 00:E0:29:12:40:77 "))
#endif
      rtems_panic ("Can't attach Ethernet driver.");

  if (rtems_ka9q_execute_command("ifconfig rtems "
				 "broadcast 255.255.255.255"))
    rtems_panic ("Can't configure Ethernet driver.");

  if (rtems_ka9q_execute_command ("arp add 255.255.255.255 "
				  "ether FF:FF:FF:FF:FF:FF"))
    rtems_panic ("Can't add broadcast entry to ARP table.");

  if (rtems_ka9q_execute_command ("ifconfig rtems "
				  "netmask 255.255.255.0"))
    rtems_panic ("Can't set netmask.");

  if (rtems_ka9q_execute_command ("route add default rtems"))
    rtems_panic ("Can't add default route.");

  rtems_ka9q_execute_command ("route");

  if (rtems_ka9q_execute_command ("arp gratuitous rtems"))
    rtems_panic ("Can't send gratuitous ARP.\n");

  printf("End of INIT\n\r");
  fflush(stdout);

#ifdef SERVER
  printf("Creation of SERVER SocketTask\n\r");
  fflush(stdout);

  task_name = rtems_build_name( 'T', 'A', '1', ' ' );
  if ((status = rtems_task_create( task_name, 20, RTEMS_MINIMUM_STACK_SIZE,
				  RTEMS_INTERRUPT_LEVEL(0),
				  RTEMS_DEFAULT_ATTRIBUTES, &tid ))
      != RTEMS_SUCCESSFUL){
    printf("status = %d\n",status);
    rtems_panic ("Can't create task.\n");
  }

#ifdef UDP
  status = rtems_task_start(tid, testUdpServerSocket, 0);
#else  
  status = rtems_task_start(tid, testTcpServerSocket, 0);
#endif

#endif  /* SERVER */
  
#ifdef CLIENT
  printf("Creation of CLIENT SocketTask\n\r");
  fflush(stdout);

  task_name = rtems_build_name( 'T', 'A', '2', ' ' );
  if ((status = rtems_task_create( task_name, 20, RTEMS_MINIMUM_STACK_SIZE,
				  RTEMS_INTERRUPT_LEVEL(0),
				  RTEMS_DEFAULT_ATTRIBUTES, &tid ))
      != RTEMS_SUCCESSFUL){
    printf("status = %d\n",status);
    rtems_panic ("Can't create task.\n");
  }

#ifdef UDP
  status = rtems_task_start(tid, testUdpClientSocket, 0);
#else  
  status = rtems_task_start(tid, testTcpClientSocket, 0);
#endif
  
#endif /* CLIENT */
  
  status = rtems_task_delete( RTEMS_SELF );
}



rtems_task testUdpServerSocket(
  rtems_task_argument argument
  ){

  int   sd;
  struct   sockaddr_in server, from;
  int fromlen;
  char *tabChar;
  int rc;
  unsigned long msgId;
  char *server_name= "localhost";
  unsigned short port;

  /****************************************/
  /* Now, we are going to test the socket */
  /****************************************/

   printf("Task testServerSocket : created\n\r");
   fflush(stdout);

   port = DEFAULT_PORT_SERVER;
   server_name = DEFAULT_IP_SERVER;

   bzero((void *)(&server), sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = aton (server_name);

   server.sin_port = htons(port); 

   sd = socket(AF_INET,SOCK_DGRAM, 0);
   if (sd == -1) {
     printf(" socket failed errno = %d\n", errno);
     return ;
   }
  
   rc = bind(sd, (struct sockaddr *)&server, sizeof(server)) ;
   if (rc == -1) {
     printf(" bind failed errno = %d\n", errno);
     return ;
   }
        
   tabChar = AllocBuffer();

   if(!tabChar) {
     printf("malloc failed ...\n");
     return ;
     }
           
   bzero((void *)(&from), sizeof(from));
   from.sin_family = AF_INET;
   from.sin_addr.s_addr = aton (DEFAULT_LISTEN_IP_SERVER);
   fromlen = sizeof(from);

   msgId = 0;
   
   for(;;){
     rc=recvfrom(sd, tabChar, SIZE_MAX_RCV_BUFFER, 0, (struct sockaddr *)&from, &fromlen);
     
      if (rc < 0) {
	printf("\n Recvfrom error %d\n\n",errno);
	fflush(stdout);
      }
      else{
	msgId = CheckBuffer(tabChar);
      }
      if (msgId == 0)
	break ;
   }

   FreeBuffer(tabChar);
   
}  


rtems_task testUdpClientSocket(
  rtems_task_argument argument
  ){

  int   sd;
  struct   sockaddr_in server;
  char *tabChar;
  int i;
  char *server_name= "localhost";
  unsigned short port;
  unsigned long	lenChar;
  struct timespec timeToWait, timeRem;

  /****************************************/
  /* Now, we are going to test the socket */
  /****************************************/

   printf("Task testClientSocket : created\n\r");
   fflush(stdout);

   port = DEFAULT_PORT_CLIENT;
   server_name = DEFAULT_IP_CLIENT;

   bzero((void *)(&server), sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = aton (server_name);

   server.sin_port = htons(port); 

   sd = socket(AF_INET,SOCK_DGRAM, 0);
   if (sd == -1) {
     printf(" socket failed errno = %d\n", errno);
     return ;
   }
  
   while(1) {
     tabChar = BuildBuffer();
     lenChar = *((unsigned long *)(tabChar));
     i = sendto(sd, tabChar, lenChar, 0, (struct sockaddr *)&server, sizeof(server));
		
     if (i == -1) {
       fprintf(stderr,"sendto() failed (at least local errors): %d\n", errno);
	    return ;
     }

     timeToWait.tv_sec = 1;
     timeToWait.tv_nsec = 0;
     nanosleep(&timeToWait, &timeRem);
     FreeBuffer(tabChar);
   }

}  

rtems_task testTcpServerSocket(
  rtems_task_argument argument
  ){
  int   sd, psd;
  struct   sockaddr_in server;
  struct sockaddr_in from;
  int fromlen;
  char *tabChar;
  int rc;
  int i;
  char *server_name= "localhost";
  unsigned short port;

  
  /****************************************/
  /* Now, we are going to test the socket */
  /****************************************/

  printf("Task testServerSocket : created\n\r");
  fflush(stdout);

  port = DEFAULT_PORT_SERVER;
  server_name = DEFAULT_IP_SERVER;

  bzero((void *)(&server), sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = aton (server_name);

  server.sin_port = htons(port);
  
  sd = socket (AF_INET,SOCK_STREAM,0);
  printf("socket() returns <%d>\n", sd);
  fflush(stdout);

  if (sd<0) {
    printf("socket() call failure: opening stream socket\n");
    fflush(stdout);
    exit(-1);
  }	

  rc = bind( sd, (struct sockaddr *)&server, sizeof(server) ) ;
  if (rc == -1) {
    printf("bind failed errno = %d\n", errno);
    fflush(stdout);
    close_s(sd);
    return;
  }
  printf("bind() returns <%d>\n", rc);
  fflush(stdout);

  printf("Server Port is: %d\n", ntohs(server.sin_port));
  fflush(stdout);
  rc = listen(sd,5);
  printf("listen() returns <%d>\n", rc);
  fflush(stdout);
  fromlen = sizeof(from);
    
  tabChar = AllocBuffer();
  if(!tabChar) {
    printf("malloc failed ...\n");
    fflush(stdout);
    return;
  }

  psd  = accept(sd,(struct sockaddr *)&from, &fromlen);
  printf("accept() returns <%d>\n", psd);
  fflush(stdout);

  for(;;){
    rc = recv(psd, tabChar, SIZE_MAX_RCV_BUFFER, 0 );

    if (rc == -1) {
      printf("recv() failed errno = %d\n", errno);
      fflush(stdout);
      close_s(sd);
      return;
    }

    if (rc == 0) {
      continue;
    }	

    if (CheckBuffer(tabChar) == 0){
      printf("Received %d bytes, data [%s] from client\n",rc,tabChar);
      printf("Packet received:<%d> bytes\n", rc);
      printf("Received:<");
      fflush(stdout);
      tabChar += 8;
      for (i=0; i<rc ; i++) printf("%c",tabChar[i]);
      printf(">\n");
    }
  }

  FreeBuffer(tabChar);
   
}  


rtems_task testTcpClientSocket(
  rtems_task_argument argument
  ){
  int   sd;
  struct   sockaddr_in server;
  /*struct  hostent *hp;*/
  struct sockaddr_in from;
  int fromlen;
  int i,retval;
  char	*tabChar;
  char *server_name= "localhost";
  unsigned short port;
  unsigned long	lenChar;
  struct timespec timeToWait, timeRem;

  /****************************************/
  /* Now, we are going to test the socket */
  /****************************************/

  printf("Task testServerSocket : created\n\r");
  fflush(stdout);

  port = DEFAULT_PORT_CLIENT;
  server_name = DEFAULT_IP_CLIENT;

  bzero((void *)(&server), sizeof(server));

  sd = socket (AF_INET,SOCK_STREAM,0);
  printf("socket() result:<%d>\n", sd);
  fflush(stdout);

  if (sd<0) {
    printf("opening stream socket\n");
    fflush(stdout);
    return;
  }
  	
  server.sin_addr.s_addr = aton (server_name);

  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  /* Connect to TCP/SERVER */
  if ( (retval = connect(sd, (struct sockaddr *)&server, sizeof(server))) < 0 ) {
    close_s(sd);
    perror("connecting stream socket");
    fflush(stdout);
    exit(0);
  }
  printf("connect() result:<%d>\n", retval);
  fflush(stdout);
  fromlen = sizeof(from);
  if ( (retval = getpeername(sd,(struct sockaddr *)&from,&fromlen)) <0){
    perror("could't get peername\n");
    fflush(stdout);
    exit(1);
  }
  printf("getpeername() result:<%d>\n", retval);
  printf("Connected to TCP/Server:");
  printf("%s:%d\n", inet_ntoa((int32)(from.sin_addr.s_addr)), ntohs(from.sin_port));
  fflush(stdout);

  printf("Now, let's send packets...");
  fflush(stdout);
  
  while(1) {
	
    tabChar = BuildBuffer();
    lenChar = *((unsigned long *)(tabChar));
    
    i = send(sd, tabChar, lenChar, 0);
    if (i == -1) 
      {
	fprintf(stderr,"send() failed (at least locally detected errors)\n");
	fflush(stdout);
	return;
      }

    timeToWait.tv_sec = 1;
    timeToWait.tv_nsec = 0;
    nanosleep(&timeToWait, &timeRem);
    FreeBuffer(tabChar);
   
  }
  
  close_s(sd);
}  

