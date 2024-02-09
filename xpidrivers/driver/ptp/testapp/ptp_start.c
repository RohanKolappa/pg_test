#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>


#include "commontypes.h"
#include "../labx_ptp_defs.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int main(int argc, char *argv[])
{
    int dev = 0;
    int ret = 0;
    unsigned int rtcChangesAllowed = 0;
    unsigned int delayTimestampEnable = 0;
    unsigned int enablePTP = 0;
    unsigned int b;





    if (argc != 4)
    {
        printf("PTP testapp\n"
	       "\tUsage: ptp_start <RTC changes allowed [0/1]> <delay timestamp enabled [0/1]> <disable/enable PTP[0/1]\n\n");
        return EXIT_SUCCESS;
    }
    else
    {
        sscanf(argv[1], "%u", &rtcChangesAllowed);
	sscanf(argv[2], "%u", &delayTimestampEnable);
	sscanf(argv[3], "%u", &enablePTP);

        if ((dev = open("/dev/ptp0", O_RDWR | O_SYNC)) == -1)
        {
            printf("ERROR opening device -> %s (%d)\n", strerror(errno), errno);
            return EXIT_FAILURE;
        }


        // set the service flags
        {
            PtpServiceFlags f = {0};
            f.delayTimestampEnable = delayTimestampEnable;
            f.rtcChangesAllowed = rtcChangesAllowed;
            ret = ioctl ( dev, IOC_PTP_SET_SERVICE_FLAGS, &f);
            if ( ret < 0)
            {
                printf("ERROR ioctl device -> %s (%d) ret=%d\n", strerror(errno), errno,ret);
                close(dev);
                return EXIT_FAILURE;
            }
            else
            {
                printf ("\tPTP service flags set:\n");
                printf ("\t\tdelayTimestampEnable: %u\n",f.delayTimestampEnable);
                printf ("\t\trtcChangesAllowed: %u\n",f.rtcChangesAllowed);
            }
        }

        // set the port properties --> set MAC address of the interface to use
        {
            struct ifreq ifr;
            int fd;
            int i;
            PtpPortProperties properties;
            bzero( &ifr, sizeof( ifr ) );
            strcpy( ifr.ifr_name, "eth1" );
            fd = socket( PF_INET, SOCK_DGRAM, 0);

            ioctl( fd, SIOCGIFHWADDR, &ifr );
            printf("MAC address = ");
            for (i =0; i < 6; i++)
            {
                printf("%02X:",ifr.ifr_hwaddr.sa_data[i]);
                properties.sourceMacAddress[i] = ifr.ifr_hwaddr.sa_data[i];
            }
            printf ("\n");
            properties.portNumber = 0;
            properties.stepsRemoved = 48822;

            ret = ioctl ( dev, IOC_PTP_SET_PORT_PROPERTIES, &properties);
            if ( ret < 0)
            {
                printf("ERROR ioctl device -> %s (%d) ret=%d\n", strerror(errno), errno,ret);
                close(dev);
                return EXIT_FAILURE;
            }


        }


	if (enablePTP)
	{
	    // start the PTP service
	    ret = ioctl ( dev, IOC_PTP_START_SERVICE, &b);
	    printf("PTP service enabled\n");
	}
	else
	{
	    // stop the PTP service
	    ret = ioctl ( dev, IOC_PTP_STOP_SERVICE, &b);
	    printf("PTP service disabled\n");
	}
	if ( ret < 0)
        {
	    printf("ERROR ioctl device -> %s (%d) ret=%d\n", strerror(errno), errno,ret);
	    close(dev);
            return EXIT_FAILURE;
        }
         else
        {
 //           printf ("\tPTP service started.\n");
        }

        close(dev);
    }

    return EXIT_SUCCESS;
}
