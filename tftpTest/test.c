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
 *
 *  $Id$
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <rtems.h>
#include <rtems/error.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <malloc.h>

static char cbuf[1024];
static char *readName, *writeName;
static rtems_interval then, now;

static void
showRate (unsigned long byteCount)
{
	int elapsed;

	printf ("Transferred %lu bytes", byteCount);
	elapsed = now - then;
	if (elapsed) {
		rtems_interval ticksPerSecond;
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
		printf (" (%ld bytes/sec)",
				(long)(((long long)byteCount * ticksPerSecond)
								/ elapsed));
	}
	printf (".\n");
}

static void
testRawRead (void)
{
	int fd;
	int nread;
	unsigned long totalRead = 0;

	fd = open (readName, O_RDONLY);
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

	fp = fopen (readName, "r");
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

static void
testRawWrite (void)
{
	int fd;
	int nwrite;
	unsigned long totalWrite = 0;

	fd = open (writeName, O_WRONLY|O_CREAT);
	if (fd < 0) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	while (totalWrite < (100 * 1024)) {
		nwrite = write (fd, cbuf, sizeof cbuf);
		if (nwrite != sizeof cbuf) {
			printf ("Write failed: %s\n", strerror (errno));
			close (fd);
			return;
		}
		totalWrite += nwrite;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	close (fd);
	showRate (totalWrite);
}

static void
testFwrite (void)
{
	FILE *fp;
	int nwrite;
	unsigned long totalWrite = 0;

	fp = fopen (writeName, "w");
	if (fp == NULL) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	while (totalWrite < (100 * 1024)) {
		nwrite = fwrite (cbuf, sizeof cbuf[0], sizeof cbuf, fp);
		if (nwrite != sizeof cbuf) {
			printf ("Write failed: %s\n", strerror (errno));
			fclose (fp);
			return;
		}
		totalWrite += nwrite;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	fclose (fp);
	showRate (totalWrite);
}

static void
testFcopy (void)
{
	FILE *fin, *fout;
	int nread, nwrite;
	unsigned long totalRead = 0;

	fin = fopen (readName, "r");
	if (fin == NULL) {
		printf ("Open (%s) failed: %s\n", readName, strerror (errno));
		return;
	}
	fout = fopen (writeName, "w");
	if (fout == NULL) {
		printf ("Open (%s) failed: %s\n", writeName, strerror (errno));
		fclose (fin);
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (;;) {
		nread = fread (cbuf, sizeof cbuf[0], sizeof cbuf, fin);
		if (nread < 0) {
			printf ("Read failed: %s\n", strerror (errno));
			fclose (fin);
			fclose (fout);
			return;
		}
		if (nread == 0)
			break;
		nwrite = fwrite (cbuf, sizeof cbuf[0], nread, fout);
		if (nwrite != nread) {
			printf ("Write failed: %s\n", strerror (errno));
			fclose (fin);
			fclose (fout);
			return;
		}
		totalRead += nread;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	fclose (fin);
	fclose (fout);
	showRate (totalRead);
}

static void
makeFullName (const char *hostname, const char *file, int flags)
{
	if (hostname == NULL)
		hostname = "";
	if ((flags == O_RDONLY) || (flags == O_RDWR)) {
		readName = realloc (readName, 8 + strlen (file) + strlen (hostname));
		sprintf (readName, "/TFTP/%s/%s", hostname, file);
		printf ("Read `%s'.\n", readName);
	}
	if ((flags == O_WRONLY) || (flags == O_RDWR)) {
		writeName = realloc (writeName, 12 + strlen (file) + strlen (hostname));
		sprintf (writeName, "/TFTP/%s/%s.tmp", hostname, file);
		printf ("Write `%s'.\n", writeName);
	}
}

void
testTFTP (const char *hostname, const char *filename)
{
	printf ("*** Check that invalid file names are reported as such.\n");
	makeFullName (hostname, "", O_RDONLY);
	testRawRead ();
	testFread ();

	printf ("*** Check that non-existent files are reported as such.\n");
	makeFullName (hostname, "BAD-FILE-NAME", O_RDONLY);
	testRawRead ();
	testFread ();

	printf ("*** Check that file read operations work.\n");
	makeFullName (hostname, filename, O_RDONLY);
	testRawRead ();
	testFread ();

	printf ("*** Check that file write operations work.\n");
	makeFullName (hostname, filename, O_WRONLY);
	testRawWrite ();
	testFwrite ();

	printf ("*** Check that file copy operations work.\n");
	makeFullName (hostname, filename, O_RDWR);
	testFcopy ();
}
