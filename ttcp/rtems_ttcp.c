/*
 * A collection of hacks, glue, and patches to
 * provide a `UNIX-like' environment to ttcp.
 *
 * Some of the code here should migrate to the libc
 * support routines some day.  Some of the more sleazy
 * hacks should never make it outside this file!
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
 */

#include <stdio.h>
#include <rtems.h>
#include <rtems_ka9q.h>
#include <rtems/error.h>
#include <socket.h>
#include <sockaddr.h>
#include <netuser.h>
#include <sys/time.h>

/*
 * Glue between UNIX-style ttcp code and RTEMS
 */
int rtems_ttcp_main (int argc, char **argv);

#define ENOBUFS	2000

struct  hostent {
	char    *h_name;        /* official name of host */
	char    **h_aliases;    /* alias list */
	int     h_addrtype;     /* host address type */
	int     h_length;       /* length of address */
	char    **h_addr_list;  /* list of addresses from name server */
	#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
};

#define SOL_SOCKET	0
#define SO_DEBUG	0

static struct hostent *
gethostbyname (const char *cp)
{
	rtems_panic ("gethostbyname()");
}

static int
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	rtems_panic ("select()");
}

static void
(*signal(int sig, void (*func)()))()
{
	return 0;;
}

static char *
rtems_inet_ntoa (struct in_addr in)
{
	return inet_ntoa (in.s_addr);
}
#define inet_ntoa rtems_inet_ntoa
#define inet_addr(cp)	resolve(cp)

int
gettimeofday (struct timeval *tp, struct timezone *tzp)
{
	rtems_clock_time_value now;

	rtems_clock_get (RTEMS_CLOCK_GET_TIME_VALUE, &now);
	tp->tv_sec = now.seconds;
	tp->tv_usec = now.microseconds;
	return 0;
}

#define _SYS_RESOURCE_H_
#define	RUSAGE_SELF	0		/* calling process */
#define	RUSAGE_CHILDREN	-1		/* terminated child processes */
struct rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	int ru_maxrss;		/* maximum resident set size */
	int ru_ixrss;		/* currently 0 */
	int ru_idrss;		/* integral resident set size */
	int ru_isrss;		/* currently 0 */
	int ru_minflt;		/* page faults not requiring physical I/O */
	int ru_majflt;		/* page faults requiring physical I/O */
	int ru_nswap;		/* swaps */
	int ru_inblock;		/* block input operations */
	int ru_oublock;		/* block output operations */
	int ru_msgsnd;		/* messages sent */
	int ru_msgrcv;		/* messages received */
	int ru_nsignals;	/* signals received */
	int ru_nvcsw;		/* voluntary context switches */
	int ru_nivcsw;		/* involuntary context switches */
};
int
getrusage(int ignored, struct rusage *ru)
{
	rtems_clock_time_value now;
	static struct rusage nullUsage;

	rtems_clock_get (RTEMS_CLOCK_GET_TIME_VALUE, &now);
	*ru = nullUsage;
	ru->ru_stime.tv_sec  = now.seconds;
	ru->ru_stime.tv_usec = now.microseconds;
	ru->ru_utime.tv_sec  = 0;
	ru->ru_utime.tv_usec = 0;
	return 0;
}

void show_ka9q_tables (void);

static void
rtems_ttcp_exit (int code)
{
	rtems_interval ticksPerSecond;

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
	rtems_task_wake_after (ticksPerSecond * 2);
	show_ka9q_tables ();
	exit (code);
}

extern volatile int ttcp_running;

/*
 * Task to run UNIX ttcp command
 */
char *__progname;
static void
ttcpTask (rtems_task_argument arg)
{
	int code;
	int argc;
	char arg0[10];
	char *argv[20];
	char linebuf[200];

	for (;;) {
		char *cp;

		/*
		 * Set up first argument
		 */
		argc = 1;
		strcpy (arg0, "ttcp");
		argv[0] = __progname = arg0;

		/*
		 * Read a line
		 */
		printf (">>> %s ", argv[0]);
		fflush (stdout);
		fgets (linebuf, sizeof linebuf, stdin);

		/*
		 * Break line into arguments
		 */
		cp = linebuf;
		for (;;) {
			while (isspace (*cp))
				*cp++ = '\0';
			if (*cp == '\0')
				break;
			if (argc >= ((sizeof argv / sizeof argv[0]) - 1)) {
				printf ("Too many arguments.\n");
				argc = 0;
				break;
			}
			argv[argc++] = cp;
			while (!isspace (*cp)) {
				if (*cp == '\0')
					break;
				cp++;
			}
		}
		if (argc > 1) {
			argv[argc] = NULL;
			break;
		}
		printf ("You must give some arguments!\n");
		printf ("At the very least, you must provide\n");
		printf ("         -r\n");
		printf ("or\n");
		printf ("         -t destination.internet.address\n");
	}
        ttcp_running = 1;
	code = rtems_ttcp_main (argc, argv);
	rtems_ttcp_exit (code);
}

static int
rtems_ttcp_bind (int s, struct sockaddr *name, int namelen)
{
	struct sockaddr_in *in = (struct sockaddr_in *)name;
	/*
	 * KA9Q doesn't like 0 port numbers
	 */
	if (in->sin_port == 0)
		in->sin_port = 2662;
	return bind (s, name, namelen);
}

/*
 * Test network throughput
 */
void
test_network (void)
{
	rtems_id tid;
	rtems_status_code sc;
	rtems_time_of_day now;
	rtems_task_priority my_priority;

	/*
	 * Set up time-of-day clock
	 */
	now.year = 1997;
	now.month = 1;
	now.day = 1;
	now.hour = 0;
	now.minute = 0;
	now.second = 0;
	now.ticks = 0;
	sc = rtems_clock_set (&now);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't set date/time; %s\n", rtems_status_text (sc));
		return;
	}

	/*
	 * Spawn test task
	 */
	rtems_task_set_priority (RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &my_priority);
	sc = rtems_task_create (rtems_build_name ('T', 'T', 'C', 'P'),
			my_priority,
			32*1024,
			RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&tid);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't create task; %s\n", rtems_status_text (sc));
		return;
	}
	sc = rtems_task_start (tid, ttcpTask, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		printf ("Can't start task; %s\n", rtems_status_text (sc));
		return;
	}
	/* rtems_task_suspend (RTEMS_SELF); */
}

#define main		rtems_ttcp_main
#define exit(code)	close(fd),rtems_ttcp_exit(code)
#define bind		rtems_ttcp_bind
#define read_timer	rtems_read_timer

/*
 * RTEMS/KA9Q code expects port numbers in host byte order!
 */
#define htons(x)	(x)

#undef delay

#include "ttcp_orig/ttcp.c"
