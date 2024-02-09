#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/syslog.h>
#include <time.h>
#include <errno.h>

/***************************
 * Log utility from vlog.c *
 ***************************/
#define BUFLEN 256

static int debug_message(const char *fmt, ... ) {

   char buff[BUFLEN];
   va_list ap;
   int len;

   len = 0;

   va_start(ap, fmt);
   len = vsnprintf(buff + len, BUFLEN-len, fmt, ap);
   va_end(ap);

   /* print the error message to syslog */
   syslog(LOG_INFO, buff);
   fflush(stderr);

   return len;
}

/************************
 * Test Routine for CSU *
 ************************/
static struct timeval lasttime = {0,0};

static void print_tester_help(void) {
    printf("\nUsage: tester [-s millisecs]\n\n");
}

int main(int argc, char **argv) {
    int opt, csufd, rc, stime = 1, count = 0, pid = getpid();

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "s:")) > 0) {

	switch (opt) {
	case 's': 
            stime = atoi(optarg);
            break;
	case '?': 
	case 'h': 
	    print_tester_help();
	    return -1;
	}   
    }

    if ((csufd = open("/dev/csu", O_RDONLY)) == -1) {
        printf("Unable to open /dev/csu. %m.\n");
        return -1;
    }

    if (daemon(0, 0) < 0) {
        printf("Cannot run Tester as daemon.\n");
    }
    
    while (1) {
        struct timeval now;
        unsigned long elapsed_usecs;
        
        gettimeofday(&now, (struct timezone *)0); 
        if (now.tv_usec < lasttime.tv_usec) {
            now.tv_usec += 1000000;
            now.tv_sec--;
        }
        elapsed_usecs = now.tv_usec - lasttime.tv_usec +
			(now.tv_sec - lasttime.tv_sec) * 1000000;

        if (elapsed_usecs >= 1000000) {
            debug_message("[%d] sleep time = %d ms, count = %d.", pid, stime, count);
            lasttime.tv_sec = now.tv_sec;
            lasttime.tv_usec = now.tv_usec;
            if (lasttime.tv_usec > 1000000) {
                lasttime.tv_sec++;
                lasttime.tv_usec -= 1000000;
            }
            count = 0;
        }
        if ((rc = read(csufd, NULL, stime)) < 0)
            debug_message("read error %d: %m.", rc); 
        count++;
    }

    return 0;
}


