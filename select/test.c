/*
 * Test RTEMS networking
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BASE_PORT	24742
#define CLIENT_COUNT	2

static int clientfd[CLIENT_COUNT];

static void
getClients (unsigned short port)
{
	int s, s1;
	struct sockaddr_in myAddr, farAddr;
	int addrlen;
	int clientCount;

	printf ("Create socket.\n");
	s = socket (AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		rtems_panic ("Can't create socket: %s", strerror (errno));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons (port);
	myAddr.sin_addr.s_addr = INADDR_ANY;
	memset (myAddr.sin_zero, '\0', sizeof myAddr.sin_zero);
	printf ("Bind socket.\n");
	if (bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
		rtems_panic ("Can't bind socket: %s", strerror (errno));
	printf ("Listen.\n");
	if (listen (s, 2) < 0)
		rtems_panic ("Can't listen on socket: %s", strerror (errno));

	/*
	 * Accumulate clients
	 */
	for (clientCount = 0 ; clientCount < CLIENT_COUNT ; clientCount++) {
		printf ("Accept.\n");
		addrlen = sizeof farAddr;
		s1 = accept (s, (struct sockaddr *)&farAddr, &addrlen);
		if (s1 < 0)
			rtems_panic ("Can't accept connection: %s", strerror (errno));
		else
			printf ("ACCEPTED:%lX\n", ntohl (farAddr.sin_addr.s_addr));
		clientfd[clientCount] = s1;
	}

	close (s);
}

static void
echoServer (unsigned short port)
{
	fd_set clientfdset;
	int clientCount;
	int topfd = 0;

	getClients (port);

	FD_ZERO (&clientfdset);

	for (clientCount = 0 ; clientCount < CLIENT_COUNT ; clientCount++) {
		int s1;

		s1 = clientfd[clientCount];
		FD_SET (s1, &clientfdset);
		if (s1 > topfd)
			topfd = s1;
	}

	/*
	 * Run clients
	 */
	for (;;) {
		fd_set readfdset;
		struct timeval tv;
		int n;
		int i;

		tv.tv_sec = 5;
		tv.tv_usec = 0;
		readfdset = clientfdset;
		n = select (topfd + 1, &readfdset, NULL, NULL, &tv);
		if (n < 0) {
			printf ("Select() error: %s\n", strerror (errno));
			return;
		}
		if (n == 0) {
			printf ("Timeout\n");
			continue;
		}
	
		printf ("Activity on %d file descriptor%s.\n", n, n == 1 ? "" : "s");
		for (i = 0 ; n && (i < CLIENT_COUNT) ; i++) {
			int fd = clientfd[i];
			if (FD_ISSET (fd, &readfdset)) {
				char buf[200];
				int nread;

				printf ("Activity on file descriptor %d.\n", fd);
				n--;
				nread = read (fd, buf, sizeof buf);
				if (nread < 0) {
					printf ("Read error %s.\n", strerror (errno));
					return;
				}
				if (nread == 0) {
					printf ("EOF\n");
					FD_CLR (fd, &clientfdset);
					close (fd);
					if (--clientCount == 0)
						return;
				}
				printf ("Read %d.\n", nread);
			}
		}
	}
}

static rtems_id tid;

static void
wakeup (struct socket *so, caddr_t arg)
{
	rtems_event_send (tid, RTEMS_EVENT_0 + (int) arg);
}

static void
echoServer2 (port)
{
	struct sockwakeup sw, sw2;
	int swlen;
	int clientCount;
	rtems_event_set clientEvents;

	getClients (port);

	sw.sw_pfn = &wakeup;
	clientEvents = 0;
	for (clientCount = 0 ; clientCount < CLIENT_COUNT ; clientCount++) {
		sw.sw_arg = (caddr_t) clientCount;
		if (setsockopt (clientfd[clientCount], SOL_SOCKET,
				SO_RCVWAKEUP, &sw, sizeof sw) < 0)
			rtems_panic ("setsockopt failed: %s",
				     strerror (errno));
		swlen = sizeof sw2;
		if (getsockopt (clientfd[clientCount], SOL_SOCKET,
				SO_RCVWAKEUP, &sw2, &swlen) < 0)
			rtems_panic ("getsockopt failed: %s",
				     strerror (errno));
		if (swlen != sizeof sw2
		    || sw2.sw_pfn != &wakeup
		    || (int) sw2.sw_arg != clientCount)
			rtems_panic ("getsockopt mismatch");

		clientEvents |= RTEMS_EVENT_0 + clientCount;
	}

	if (rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_LOCAL_NODE, &tid)
	    != RTEMS_SUCCESSFUL)
	  rtems_panic ("rtems_task_ident failed");

	for (;;) {
		rtems_event_set events;
		rtems_status_code status;
		int i;

		status = rtems_event_receive (clientEvents,
					      RTEMS_WAIT | RTEMS_EVENT_ANY,
					      RTEMS_MILLISECONDS_TO_TICKS (5000),
					      &events);

		if (status == RTEMS_TIMEOUT) {
			printf ("Timeout\n");
			continue;
		}

		for (i = 0; i < CLIENT_COUNT; ++i) {
			if (events == 0)
				break;
			if (events & (i + RTEMS_EVENT_0)) {
				int fd;
				char buf[200];
				int nread;

				fd = clientfd[i];
				printf ("Activity on file descriptor %d.\n", fd);
				events &= ~ (i + RTEMS_EVENT_0);
				nread = read (fd, buf, sizeof buf);
				if (nread < 0) {
					printf ("Read error %s.\n", strerror (errno));
					return;
				}
				if (nread == 0) {
					printf ("EOF\n");
					clientEvents &= ~ (i + RTEMS_EVENT_0);
					close (fd);
					if (--clientCount == 0)
						return;
				}
				printf ("Read %d.\n", nread);
			}
		}
	}
}

void
doSocket (void)
{
	echoServer (BASE_PORT);
}
