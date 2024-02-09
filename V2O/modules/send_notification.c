#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "vutils.h"
#include "commonutils.h" 
#include "remoteConfig.h" 

static void print_help(void) {
    printf("Usage:  send_notification [OPTIONS] [List of name value pairs]\n");
    printf("Both OPTIONS below are mandatory.\n");
    printf("\t-s \tEvent Source\n");
    printf("\t-t \tEvent Type\n");
} 

int send_notification_main(int argc, char **argv) {

    int opt;
    int arg_count;
    char *event_source = NULL;
    char *event_type = NULL;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "s:t:h")) > 0) {
        switch (opt) {
        case 's':
            event_source = strdup(optarg);
            break;
        case 't':
            event_type = strdup(optarg);
            break;
        case 'h':
        case '?':
            print_help();
            return 1;
        default:
            break;
        }
    }

    if (event_source == NULL ||
        event_type == NULL) {
        print_help();
        return 1;
    }

    arg_count = argc - optind;


    if (arg_count % 2 != 0) {
        printf("ERROR: Odd number of arguments.\n\n");
        print_help();
        return 1;
    }

    remote_notification_start(event_source, event_type);

    if (arg_count > 0) {
        int i;
        for (i = 0; i < arg_count; i += 2) {
            printf("%s = %s\n", argv[optind + i], argv[optind + i + 1]);
            remote_notification_add_data(argv[optind + i], argv[optind + i + 1]);
        }
    }

    remote_notification_send();
    return 0;

}
