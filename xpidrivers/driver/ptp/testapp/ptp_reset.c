#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "commontypes.h"
#include "../labx_ptp_defs.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int main(int argc, char *argv[])
{
    int dev = 0;
    int ret = 0;
 //   unsigned arg1 = 0;
 //   unsigned arg2 = 0;


    if (argc != 1)
    {
        printf("PTP testapp\n"
               "Usage: testapp\n");
        return EXIT_SUCCESS;
    }
    else
    {
//        sscanf(argv[1], "%u", &arg1);
//        sscanf(argv[2], "%u", &arg2);

        if ((dev = open("/dev/ptp0", O_RDWR | O_SYNC)) == -1)
        {
            printf("ERROR opening device -> %s (%d)\n", strerror(errno), errno);
            return EXIT_FAILURE;
        }


        // set time
        {
            PtpTime t = {0};
            t.secondsUpper = 0xAA;
            t.secondsLower = 0x55;
            t.nanoseconds = 0xEE;
            ret = ioctl ( dev, IOC_PTP_SET_TIME, &t);
            if ( ret < 0)
            {
                printf("ERROR ioctl device -> %s (%d) ret=%d\n", strerror(errno), errno,ret);
                close(dev);
                return EXIT_FAILURE;
            }
            else
            {
                printf ("\tPTP set time\n");
                printf ("\t\tsecondsUpper: %u\n",t.secondsUpper);
                printf ("\t\tsecondsLower: %u\n",t.secondsLower);
                printf ("\t\tnanoseconds: %u\n",t.nanoseconds);
            }
        }



         close(dev);
    }

    return EXIT_SUCCESS;
}
