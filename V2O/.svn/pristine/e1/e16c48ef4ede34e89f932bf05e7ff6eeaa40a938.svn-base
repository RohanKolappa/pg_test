/*
 * cli.c
 *
 *  Created on: Oct 10, 2013
 *      Author: rkale
 */

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CUSTOM_MENU_DEFS

#include "vlog.h"
#include "vutils.h"
#include "nargv.h"

#define READ_END      0
#define WRITE_END     1

#define COMMAND_NOT_FOUND     -100

static int pipes[2];
static int oldStdOut;
static int oldStdErr;

// Grab cli output from read end of the pipe
static int get_cli_output(char ** output_buffer)
{
    static char * cli_output_buffer = NULL;
    static int cli_output_buffer_len = 0;

    // On first call, preallocate static buffer
    if (cli_output_buffer == NULL) {
        cli_output_buffer = (char *) malloc(1024);
        cli_output_buffer_len = 1024;
    }

    strcpy(cli_output_buffer, "");

    int buffer_len = 1024;
    char buffer[buffer_len];
    fcntl(pipes[READ_END], F_SETFL, O_NONBLOCK);
    int cli_output_buffer_offset = 0;
    int bytes_read = read(pipes[READ_END], buffer, buffer_len - 1 );
    while (bytes_read > 0) {
        if (cli_output_buffer_len - cli_output_buffer_offset <= bytes_read) {
            // Need to grow buffer
            cli_output_buffer_len = cli_output_buffer_offset + bytes_read + 1024;
            char *newbuffer = realloc(cli_output_buffer, cli_output_buffer_len);
            cli_output_buffer = newbuffer;
        }
        memcpy(cli_output_buffer + cli_output_buffer_offset, buffer, bytes_read);
        cli_output_buffer_offset += bytes_read;
        bytes_read = read(pipes[READ_END], buffer, buffer_len - 1);
    }

    cli_output_buffer[cli_output_buffer_offset] = 0;
    *output_buffer = cli_output_buffer;
    return cli_output_buffer_offset;
}

// Redirect stdout and stderr into the write end of our pipe
// and then execute the cli command entry point.
// The command string is first parsed into arc/argv array
static int run_cli_command(const char * command) {
    int rc = 0;

    // Parse command string into arc and argv
    NARGV *nargv = nargv_parse(command);
    if (nargv->error_code || nargv->argc < 1) {
        nargv_free(nargv);
        return COMMAND_NOT_FOUND;
    }

//    int arg;
//    for (arg = 0; arg < nargv->argc; arg++) {
//        printf("argument %i: %s\n", arg, nargv->argv[arg]);
//    }

    // Make stdout and stderr go to our pipe
    dup2(pipes[WRITE_END], fileno(stdout));
    dup2(pipes[WRITE_END], fileno(stderr));

    // Reset global optind variable so that the getopt() flavors that any
    // cli command may use can be repeated
    optind = 0;

    // Find and execute command
    util_entry_point util_main = find_cli_util(nargv->argv[0]);
    if (util_main != NULL)
        rc = util_main(nargv->argc, nargv->argv);
    else
        rc = COMMAND_NOT_FOUND;


    // Now restore std and stderr
    dup2(oldStdOut, fileno(stdout));
    dup2(oldStdErr, fileno(stderr));

    // Free up command parser
    nargv_free(nargv);

    return rc;
}

// Execute cli command and grab its output
static int execute_cli_command(char * command, char ** output_buffer, int * output_length) {
    int rc = run_cli_command(command);
    if (rc == COMMAND_NOT_FOUND)
        return rc;
    *output_length = get_cli_output(output_buffer);
    return rc;
}

static unsigned long long getTimeInMicroSec(void)
{
    struct timeval tNow;
    gettimeofday(&tNow, NULL);
    return ((unsigned long long)tNow.tv_sec) * 1000000 + (tNow.tv_usec);
}

static int create_network_listener(int port)
{
    int sockFD;

    // Create Socket
    if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }

    // Bind to port
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    memset(sa.sin_zero, 0, sizeof (sa.sin_zero));
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockFD, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        close(sockFD);
        return -1;
    }

    // Start Listening
    if (listen (sockFD, 5) == -1) {
        close(sockFD);
        return -1;
    }

    return sockFD;
}

#define MAX_REQUEST_LEN 512

static void run_network_loop(int port)
{
    int listenFD = create_network_listener(port);
    if (listenFD == -1) {
        printf("Error: Unable to listen on port %d\n", port);
        return;
    }

    printf("Running CLI server on port %d\n", port);

    // Daemonize
    daemon(1, 0);

    struct sockaddr_in peerAddress;
    socklen_t peerAddressLen = sizeof(peerAddress);
    int connectionFD;
    char request_buffer[MAX_REQUEST_LEN];
    int rcvLen;
    char *cp;
    char * outputbuffer;
    int outputlen;

    // Accept and handle one connection at a time
    while (1) {
        peerAddressLen = sizeof(peerAddress);
        connectionFD = accept(listenFD, (struct sockaddr *)&peerAddress, &peerAddressLen);

        if (connectionFD < 0)
            continue;

        rcvLen = recv(connectionFD, request_buffer, MAX_REQUEST_LEN - 1, 0);
        if (rcvLen <= 0) {
            close(connectionFD);
            continue;
        }

        // Terminate received input at first new line
        request_buffer[rcvLen] = '\0';
        if ((cp = strchr(request_buffer, '\n')) != NULL) {
            *cp = '\0';
        }
        else if ((cp = strchr(request_buffer, '\r')) != NULL) {
            *cp = '\0';
        }


        int rc = execute_cli_command(request_buffer, &outputbuffer, &outputlen);
        if (rc == COMMAND_NOT_FOUND) {
            char * message = "Command not found.\n";
            send(connectionFD, message, strlen(message), 0);
        }
        else {
            send(connectionFD, outputbuffer, outputlen, 0);
        }

        close(connectionFD);

    }
}

static void run_cli_loop(void) {
    char * prompt_string = "CLI > ";
    char line_in[4096];
    char * outputbuffer;
    int outputlen;

    unsigned long long prevTime = getTimeInMicroSec();

    while (1) {

        printf("[%llu] %s", getTimeInMicroSec() - prevTime, prompt_string);

        if (fgets(line_in, 4096, stdin) == line_in) {
            if (strncmp(line_in, "quit", 4) == 0)
                break;
            if (strncmp(line_in, "exit", 4) == 0)
                break;

            prevTime = getTimeInMicroSec();

            int rc = execute_cli_command(line_in, &outputbuffer, &outputlen);
            if (rc == COMMAND_NOT_FOUND) {
                printf("Command not found.\n");
            }
            else {
                printf("%s", outputbuffer);
            }
        }
    }

}

static void print_help(void) {
    printf("\t-p port\tServe cli commands at given TCP port\n");
    printf("\t-h \tPrint this message\n");
}

int cli_main(int argc, char **argv)
{
    int opt;
    int port = 0;

    // parse command line options
    while ((opt = getopt(argc, argv, "hp:")) > 0) {
        switch (opt) {
        case 'p':
            port = atoi(optarg);
            break;
        case 'h':
        case '?':
            print_help();
            return -1;
        default:
            break;
        }
    }

    pipes[READ_END] = 0;
    pipes[WRITE_END] = 0;

    // Create a pair of pipes
    if (pipe(pipes) == -1 ) {
        printf("Error Failed to create pipe pair\n");
        return -1;
    }


    // Save off the current stdout and stderr so that we can restore later
    oldStdOut = dup(fileno(stdout));
    oldStdErr = dup(fileno(stderr));

    // Set stdout and stderror to be unbuffered.
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (port > 0)
        run_network_loop(port);
    else
        run_cli_loop();

    return 0;
}
