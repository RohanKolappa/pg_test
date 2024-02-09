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


static int fd = 0;
static int faction=0;

int main(int argc, char **argv)
{
	int lstate = 0;
	int pid=0;

	/* exactly one argument must be given */
	if (argc != 2)
		exit(1);
	/* open device */
	fd = open(argv[1], O_RDWR | O_NDELAY);
	if (fd < 0) {
		fprintf(stderr, "ERROR: can not open \"%s\"\n", argv[1]);
		exit(2);
	}
	pid = fork();
	if (pid)
		exit(0);
	ioctl(fd, TIOCMSET, &lstate);
	while (1) {
		lstate=TIOCM_DTR |TIOCM_RTS;
	        ioctl(fd, TIOCMSET, &lstate);
		ioctl(fd, TIOCMGET, &lstate);
		if (lstate & TIOCM_CAR) {
			printf ("Shutdown Button has been pushed\n");
			faction = open ("/tmp/shutdown", O_CREAT,  0666);
			close(faction);
		}
		if (lstate & TIOCM_DSR) {
			printf ("Reset Button has been pushed\n");
			faction = open ("/tmp/reset", O_CREAT,  0666);
			close(faction);
		}
		sleep(2);
	}
	return 0;
}
