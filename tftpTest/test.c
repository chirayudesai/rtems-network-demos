/*
 * Test RTEMS/KA9Q TFTP device driver
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
#include <rtems/error.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <bootp.h>
#include <netuser.h>

#include "../usercfg.h"

static char cbuf[1024];
static char *fullname;
static rtems_interval then, now;

static void
showRate (unsigned long totalRead)
{
	int elapsed;

	printf ("Read %lu bytes", totalRead);
	elapsed = now - then;
	if (elapsed) {
		rtems_interval ticksPerSecond;
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
		printf (" (%ld bytes/sec)",
				(long)(((long long)totalRead * ticksPerSecond)
								/ elapsed));
	}
	printf (".\n");
rtems_ka9q_execute_command ("ifconfig rtems");
}

static void
testRawRead (void)
{
	int fd;
	int nread;
	unsigned long totalRead = 0;

	fd = open (fullname, O_RDONLY);
	if (fd < 0) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (;;) {
		nread = read (fd, cbuf, sizeof cbuf);
		if (nread < 0) {
			printf ("Read failed: %s\n", strerror (errno));
			close (fd);
			return;
		}
		if (nread == 0)
			break;
		totalRead += nread;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	close (fd);
	showRate (totalRead);
}

static void
testFread (void)
{
	FILE *fp;
	int nread;
	unsigned long totalRead = 0;

	fp = fopen (fullname, "r");
	if (fp == NULL) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (;;) {
		nread = fread (cbuf, sizeof cbuf[0], sizeof cbuf, fp);
		if (nread < 0) {
			printf ("Read failed: %s\n", strerror (errno));
			fclose (fp);
			return;
		}
		if (nread == 0)
			break;
		totalRead += nread;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	fclose (fp);
	showRate (totalRead);
}

static int
makeFullname (rtems_unsigned32 host, const char *file)
{
	const char *hostname;
#if !defined(USE_BOOTP)
	static char name[16];
#endif

	printf( "makeFullname(0x08%x, %s)\n", host, file);

#if (defined (USE_BOOTP))
	if (host) {
		hostname = rtems_hostname_for_address (BootpHost.s_addr, 0);
		if (hostname == NULL) {
			printf ("No host to read from!\n");
			return 0;
		}
	} else {
		hostname = "";
	}
#else
	if (host) {
		sprintf( name, "%d.%d.%d.%d", host >> 24, (host >> 16) & 0xff, 
			(host >> 8) & 0xff, host & 0xff
		);
		hostname = name;
	} else {
		hostname = "";
	}
#endif
	fullname = realloc (fullname, 8 + strlen (file) + strlen (hostname));
	sprintf (fullname, "/TFTP/%s/%s", hostname, file);
	printf ("Read `%s'.\n", fullname);
	return 1;
}

void
testTFTP (void)
{
	/*
	 * Check that invalid file names are reported as such
	 */
	if (makeFullname (0, "")) {
		testRawRead ();
		testFread ();
	}

	/*
	 * Check that non-existent files are reported as such
	 */
	if (makeFullname (0, "BAD-FILE-NAME")) {
		testRawRead ();
		testFread ();
	}

	/*
	 * Check read speed
	 */
#if (defined (USE_BOOTP))
	if (BootpFileName == NULL) {
		printf ("Nothing to read!\n");
		return;
	}
	if (makeFullname (BootpHost.s_addr, BootpFileName)) {
#else
	if (makeFullname (aton (DATA_SOURCE_HOST), DATA_SOURCE_FILE)) {
#endif

		testRawRead ();
		testFread ();
	}
}
