/* vim: set sw=8 ts=8 si : */
/*
 * Serial line daemon program for shutdown of your server.
 * A blinking LED indicates the server is running.
 * The program uses the serial interface lines
 * DTR (output: data terminal ready), RTS (output:request to send)  
 * and CD (input: carrier detect)
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License.
 * See http://www.gnu.org/copyleft/ for details.
 *
 * Written by Guido Socher <guido@linuxfocus.org> 
 *
 * Some ideas were taken from an earlier progam called 
 * cpanel written by Mirko Dölle <dg2fer@qsl.net>
 */
#define VERINFO "version 0.1"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

char *cmd[] = {
		"reset",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8"};
char *dtrcode[] = {
			"000000000000000000000000000000000",
			"100",
			"101",
			"110",
			"111",
			"011",
			"010",
			"001",
			"000",
		};
#define MAXCODES	9

static int fd = 0;
static int faction=0;

int main(int argc, char **argv)
{
	int lstate = 0;
	int pid=0;
	char *pattern=NULL;
	char *code=NULL;
	int i=0;

	/* exactly one argument must be given */
	if (argc != 3)
		exit(1);
	/* open device */
	code = (char *)strdup(argv[2]);
	fd = open(argv[1], O_RDWR | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "ERROR: can not open \"%s\"\n", argv[1]);
		exit(2);
	}
        if (strcmp (code, "clock") == 0) {
                while(1) {
                        lstate=TIOCM_RTS;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);
                        lstate=TIOCM_RTS|TIOCM_DTR;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);
                }
        }

        if (strcmp (code, "data") == 0) {
                while(1) {
                        lstate=TIOCM_DTR;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);
                        lstate=TIOCM_RTS|TIOCM_DTR;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);
                }
        }

        if (strcmp (code, "alternate") == 0) {
                lstate=TIOCM_RTS|TIOCM_DTR;
                ioctl(fd, TIOCMSET, &lstate);
                usleep(1);
                while(1) {
                        lstate=TIOCM_RTS;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);

                        lstate=0;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);

                        lstate=TIOCM_DTR;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);

                        lstate=0;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);

                        lstate=TIOCM_RTS;
                        ioctl(fd, TIOCMSET, &lstate);
                        usleep(1);

                	lstate=TIOCM_RTS|TIOCM_DTR;
                	ioctl(fd, TIOCMSET, &lstate);
                	usleep(1);

                }
        }

	for (i=0; i < MAXCODES; i++) {
		if (strcmp (cmd[i], code) == 0 ) {
			pattern = dtrcode[i];
			break;
		}
	}
	if (pattern == NULL) {
		printf ("Could not find code %s\n", code);
		exit(0);
	}
	for (i=0; i < 20; i++) {
		lstate=TIOCM_RTS;
	        ioctl(fd, TIOCMSET, &lstate);
		usleep(1);
		lstate=TIOCM_RTS|TIOCM_DTR;
	        ioctl(fd, TIOCMSET, &lstate);
		usleep(1);
	}	

	printf ("Sending %s\n", pattern);

	for (i=0; i < strlen(pattern); i++) {
		lstate=TIOCM_DTR|TIOCM_RTS;
		ioctl(fd, TIOCMSET, &lstate);
		usleep(1);
		switch(pattern[i]) {
			case '1':
				lstate=TIOCM_DTR;
				ioctl(fd, TIOCMSET, &lstate);
				break;
			default:
				ioctl(fd, TIOCMSET, &lstate);
				break;
		}
		usleep(1);
		lstate^= TIOCM_DTR;
	        ioctl(fd, TIOCMSET, &lstate);
		usleep(1);
	}
	lstate=TIOCM_DTR|TIOCM_RTS;
	ioctl(fd, TIOCMSET, &lstate);

	lstate=TIOCM_RTS;
	ioctl(fd, TIOCMSET, &lstate);
	return;
	printf ("Press any key to exit\n");
	getchar();
	return 0;
}
