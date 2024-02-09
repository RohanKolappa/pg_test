#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/io.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <asm/io.h>
#include <termios.h>
#include "633_linux/typedefs.h"
#include "633_linux/serial.h"
#include "633_linux/633_packet.h"
#include "633_linux/show_packet.h"
#include "supermicro_lcd/libsmc.h"

#define 	PAFILE		"/tmp/PAOverride"
#define 	PADEV		"/dev/ttyS0"
#define		PAOVERRIDEON	1
#define		PAOVERRIDEOFF	0
#define 	ENCFILE		"/home/onair/encoder.control"

#define		LED_SERIAL		0
#define		LED_MULTISERIAL		1
#define		LED_PARALLEL		2
#define		LCD_PANEL		3


#define 	RESETDEV	"/dev/ttyS1"
#define 	STATUSDEV	"/dev/ttyS2"
#define 	LPTDEV		0x378
#define 	LCDUSBDEV	"/dev/ttyUSB0"
#define 	LCDUSBBAUD	115200
#define 	LCDDEV		"/dev/ttyS1"
#define 	LCDBAUD		19200

#define         LCDMODEL_631               1
#define         LCDMODEL_633               2
#define         LCDMODEL_SUPERMICRO        3

//#define DEBUG_FRONTPANEL

#ifdef DEBUG_FRONTPANEL
#define LOG	printf
#else 
#define LOG(...)	syslog(LOG_LOCAL7 | LOG_INFO,__VA_ARGS__)
#endif

extern int GetCode(void);


typedef struct statedisplay
{
	int  l_code;
	char l_cmd[80];
	char l1_pattern[80];
	char l2_pattern[80];
	int  l_blinkrate;
	char  l_message[80];
} sysstate_t;
#define 	MAXCMDS			18

#define 	STATUS_NONE		0
#define 	STATUS_INIT		1
#define 	STATUS_RESET		3
#define 	STATUS_RESET_SENT	4
#define 	STATUS_RESET_COMPLETE	5
#define 	STATUS_STOP 		6
#define 	STATUS_STOP_SENT	7
#define 	STATUS_STOP_COMPLETE	8
#define 	STATUS_OS_OK		11
#define 	STATUS_APPIMAGE_OK	12
#define 	STATUS_APPBOOTFILE_OK	13
#define 	STATUS_APPBOOT_OK	14
#define 	STATUS_APPCONFIG_OK	15
#define 	STATUS_ENCODER_OK	16
#define 	STATUS_DISK_OK		17
#define 	STATUS_STREAM_OK	18
#define 	STATUS_LCD_TEST		19

#define RED "00"
#define GREEN "11"
#define BLINK "01"
#define FAST 100000
#define SLOW 500000

sysstate_t StatusCmd[] = {
    
    {STATUS_NONE, "none", RED, RED, 0, "none"},
    {STATUS_RESET, "reset", BLINK, RED, FAST, "RESET"},
    {STATUS_RESET_SENT, "resetSent", BLINK, RED, SLOW, "RESET Sent"},
    {STATUS_RESET_COMPLETE, "resetComplete", GREEN, RED, 0, "RESET Complete"},
    {STATUS_STOP, "stop", BLINK, RED, FAST, "STOP"},
    {STATUS_STOP_SENT, "stopSent", BLINK, RED, SLOW, "STOP Sent"},
    {STATUS_STOP_COMPLETE, "stopComplete", RED, RED, 0, "STOP Complete"},
    {STATUS_INIT, "init", RED, BLINK, SLOW, "INIT"},
    { STATUS_OS_OK, "1", 		RED, BLINK, FAST, 
      "CHECK Software"},
    { STATUS_APPIMAGE_OK, "2", 	RED, GREEN, 0, 
      "CHECK Parameters"},
    { STATUS_APPBOOTFILE_OK, "3", 	BLINK, RED, FAST, 
      "Initializing"},
    { STATUS_APPBOOT_OK, "4", 	BLINK, BLINK, FAST, 
      "CHECK Config"},
    { STATUS_APPCONFIG_OK, "5", 	BLINK, GREEN, FAST, 
      "CHECK Encoders"},
    { STATUS_ENCODER_OK, "6",	GREEN, RED, 0, 
      "CHECK Disk(s)"},
    { STATUS_DISK_OK, "7", 	GREEN, BLINK, FAST, 
      "CHECK Streams"},
    { STATUS_STREAM_OK, "8", 	GREEN, GREEN, 0, 
      "READY"},
    { STATUS_LCD_TEST, "9", 	GREEN, GREEN, 0, 
      "TEST"},
};

char *lcdmenu[]= {"CMD", "Shutdown", "Reset"};
int  lcdpos=0;
int  lcdconfirm=0;
int  lcdoptions=3;
int  lcdmodel = LCDMODEL_633;

#define 	STATUSFILE	"/tmp/status.led"
#define 	ALARMFILE	"/tmp/alarm.led"

int nLedType = LED_SERIAL;
int nLCDStatus = 0;

static SLHandle	*pSL = NULL;

int getCmdIndex(iCmd) {
    int i;
    int index = 0;
    for (i=0; i < MAXCMDS; i++) {
        if (iCmd == StatusCmd[i].l_code) {
            index = i;
            break;
        }
    }
    return index;
}

int SetMultiLed(char *pattern1, char *pattern2) {
    
    int clpt=0;
    
    if(!strcmp(pattern1, RED) && !strcmp(pattern2, RED) )
        clpt = 10;
    if(!strcmp(pattern1, RED) && !strcmp(pattern2, GREEN) )
        clpt = 6;
    if(!strcmp(pattern1, GREEN) && !strcmp(pattern2, RED) )
        clpt = 9;
    if(!strcmp(pattern1, GREEN) && !strcmp(pattern2, GREEN) )
        clpt = 5;
    
    if (ioperm(LPTDEV,1,1))
        return 0;
    
    outb(clpt, LPTDEV);
    
    return 1;
}
void GetIPaddress(char *buf)
{
    struct ifaddrs *ipadd;
    struct ifaddrs *ifa;

    strcpy(buf, "");
    getifaddrs(&ipadd);
  
    for (ifa = ipadd; ifa != NULL; ifa = ifa->ifa_next) {
        if ((ifa->ifa_addr->sa_family == AF_INET)
                &&(strcmp(ifa->ifa_name, "eth0") == 0)
                && (ifa->ifa_flags & 0x00000001)) {

            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr,
                              buf, INET_ADDRSTRLEN);
            break;
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
           inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr, 
                     buf,INET6_ADDRSTRLEN);
        }
    }
    freeifaddrs(ipadd);     
}

void GetSerialNumber(char *buf)
{

    FILE *fp;
    char *cp;
    strcpy(buf, "");

    fp = fopen("/etc/config/device_serial_number", "r");
    if (fp == NULL) 
      return ;
  

    fgets(buf, 255, fp);
    if ((cp = strchr(buf, '\n')) != 0)
        *cp = '\0';

    fclose(fp);

  //  LOG("serial address = %s\n", buf);
}

int SetStatusLed(int fd, int iCmd) {
    char *pattern1=NULL;
    char *pattern2=NULL;
    char *p1=NULL;
    char *p2=NULL;
    int i=0;
    int lstate=0;
    int clpt=0;

    int index = getCmdIndex(iCmd);
       
    if (nLedType == LED_SERIAL) {
        pattern1 = StatusCmd[index].l1_pattern;
        pattern2 = StatusCmd[index].l2_pattern;
        for(i = 0; i < 2; i++) {
            lstate = 0;
            clpt=0;
            if (pattern1[i] == '1') {
                lstate |= TIOCM_RTS;
                clpt |= 1;
            }
            // TBD Seems to be some reversal of pins
            // '1' gives red and '0' gives green so reverse
            // if (pattern2[i] == '1')
            if (pattern2[i] == '0') {
                lstate |= TIOCM_DTR;
                clpt |= 1;
            }
            ioctl(fd, TIOCMSET, &lstate);
            if (!ioperm(LPTDEV,1,1)) {
                outb(clpt, LPTDEV);
            }
            usleep(StatusCmd[index].l_blinkrate);
        }
    }
    else if (nLedType == LCD_PANEL) {
        if (nLCDStatus != index) {

            if (lcdmodel == LCDMODEL_SUPERMICRO) {
                char buf[32];
                sprintf(buf,"%16s", StatusCmd[index].l_message);
                sl_mvprintf(pSL, 0, 1, buf);
            }
            else {
                //Send line 2 to the 633 using command 31
                outgoing_command.command = 31;
                outgoing_command.data[0]=0; //col
                outgoing_command.data[1]=0; //row
                sprintf((char *)&outgoing_command.data[2],"%16s",
                        StatusCmd[index].l_message);
                outgoing_command.data_length = 18;
                send_packet();
            }
            nLCDStatus=index;
            LOG("%s %d %d\n", 
                StatusCmd[index].l_message, index, iCmd);
        }
    }
    else {
        pattern1 = StatusCmd[index].l1_pattern;
        pattern2 = StatusCmd[index].l2_pattern;
        
        if(!strcmp(pattern1, BLINK))
            p1 = RED;
        else
            p1 = pattern1;
        
        if(!strcmp(pattern2, BLINK))
            p2 = RED;
        else
            p2 = pattern2;
        SetMultiLed(p1, p2);
        
        if(StatusCmd[index].l_blinkrate != 0) {
            usleep(StatusCmd[index].l_blinkrate);
            if(!strcmp(pattern1, BLINK))
                p1 = GREEN;
            if(!strcmp(pattern2, BLINK))
                p2 = GREEN;
            SetMultiLed(p1, p2);
        }
        
    }
    return 0;
}


int PollPAOverride(int fd) {
    int lstate=0;
    int retval = 0;
    FILE *FPAFile=NULL;
    char sPACommand[20];

    FPAFile = fopen (PAFILE, "r");
    if (FPAFile != NULL) {
        fscanf (FPAFile, "%s", sPACommand);
        fclose(FPAFile);
        if (strcmp (sPACommand, "ON") == 0) {
            retval = 1;
        }
        else if (strcmp (sPACommand, "OFF") == 0) {
            retval = 0;
        }
        else if (strcmp (sPACommand, "DELETE") == 0) {
            unlink(PAFILE);
        }
    }
    else {
        lstate=TIOCM_DTR;
        ioctl(fd, TIOCMSET, &lstate);
        ioctl(fd, TIOCMGET, &lstate);
        if (lstate & TIOCM_DSR) {
            retval = 1;
        }
    }

    return retval;
}

int PollResetSwitch(int fd) {
    int lstate = 0;
    int retval = 0;
    char c=0;
    char d=0;
    int bUpdateLCD=0;
    int bExecuteCmd=0;


    if (nLedType == LED_SERIAL) {
        write (fd, &c, 1);
        if (read (fd, &d, 1) == 1) {
            retval = 1;
            while (read (fd, &d, 1) == 1);
        }
    }
    else if (nLedType == LCD_PANEL) {
        if (lcdmodel == LCDMODEL_SUPERMICRO) {
            /* Map codes returned by SuperMicro LCD to equivalent
               keycode returned by _633 LCD Panel */
            unsigned int rc  = sl_get_key (pSL, 100); // block for upto 100 ms

            switch (rc & 0x00ff) {
            case SLK_UP:
                if ((rc & 0xff00) == 0)
                    c = 7;
                break;
            case SLK_DOWN:
                if ((rc & 0xff00) == 0)
                    c = 8;
                break;
            case SLK_ENTER:
                if ((rc & 0xff00) == 0)
                    c = 11;
                break;
            case SLK_CANCEL:
                if ((rc & 0xff00) == 0)
                    c = 12;
                break;
            case SLK_RIGHT:
            case SLK_LEFT:
                c = 0;
                break;
            default:
                retval = rc;
                c = 0;
            }
            
        } else {
            /* 633 or 631 serial LCD */
            if(check_for_packet()) {
                c = GetCode();
            }
        }

        if(c) {
            if (lcdpos < 0) {
                lcdpos = 0;
                bUpdateLCD=1;
            }
            switch (c) {
            case 7:
            case 13:
                if (!lcdconfirm) {
                    bUpdateLCD=1;
                    lcdpos++;
                    lcdpos = lcdpos % lcdoptions;
                }
                break;
            case 8:
            case 15:
                if (!lcdconfirm) {
                    bUpdateLCD=1;
                    lcdpos+= lcdoptions -1;
                    lcdpos = lcdpos % lcdoptions;
                }
                break;
            case 11:
            case 14:
                bUpdateLCD=1;
                if (lcdconfirm != 0) {
                    bExecuteCmd=1;
                }
                else {
                    if (lcdpos != 0)
                        lcdconfirm=1;
                }
                break;
            case 12:
            case 16:
                bUpdateLCD=1;
                lcdconfirm=0;
                bExecuteCmd=0;
                break;
                            
            }
            if (bUpdateLCD) {
                if (lcdmodel == LCDMODEL_SUPERMICRO) {
                    char buf[32];
                    if (bExecuteCmd) {
                        sprintf(buf, "%12s ...", lcdmenu[lcdpos]);
                    }
                    else if (lcdconfirm) {
                        sprintf(buf, "%12s OK?", lcdmenu[lcdpos]);
                    }
                    else {
                        sprintf(buf, "%16s", lcdmenu[lcdpos]);
                    }
                    sl_mvprintf(pSL, 0, 1, buf);
                }
                else {
                    outgoing_command.command = 31;
                    outgoing_command.data[0]=0; //col
                    outgoing_command.data[1]=1; //row
                    outgoing_command.data_length = 18;
                    if (bExecuteCmd) {
                        sprintf((char *) &outgoing_command.data[2],
                                "%12s ...", lcdmenu[lcdpos]);
                    }
                    else if (lcdconfirm) {
                        sprintf((char *) &outgoing_command.data[2],
                                "%12s OK?", lcdmenu[lcdpos]);
                    }
                    else {
                        sprintf((char *) &outgoing_command.data[2],
                                "%16s", lcdmenu[lcdpos]);
                    }
                    send_packet();
                }
            }
            if (bExecuteCmd) {
                switch (lcdpos) {
                case 1: 
                    //shutdown
                    LOG ("Shutting Down\n");
                    system("/V2O/bin/shutdown_system");
                    break;
                case 2:
                    //reset
                    LOG ("Reset in Progress\n"); 
                    system("/V2O/bin/reboot_system");
                    break;
                }
            }
        }
    }
    else {
        lstate=TIOCM_DTR |TIOCM_RTS;
        ioctl(fd, TIOCMSET, &lstate);
        ioctl(fd, TIOCMGET, &lstate);
        if (lstate & TIOCM_DSR) {
            retval = 1;
        }
    }
  
    return retval;
}

int PollStatusLed() {
    struct stat fileStat;
    FILE *fpStatus;
    char sStatus[80];
    int iCode=0;
    int i=0;
    bzero(sStatus, 80);
    if (!stat (STATUSFILE, &fileStat)) {
            if (fileStat.st_mtime >= fileStat.st_atime) {
               fpStatus = fopen(STATUSFILE, "r");
               if (fpStatus != NULL) {
                fscanf (fpStatus, "%s", sStatus);
               }
               fclose(fpStatus);
            }
    }
    for (i=0; i < MAXCMDS; i++) {
        if (strcasecmp(StatusCmd[i].l_cmd, sStatus) == 0) {
            iCode = StatusCmd[i].l_code;
            break;
        }
    }
    return iCode;
}

void UpdateDisplay(int lcdmodel, int col, 
                   int row, char *IPAdd) {
    char buff[255];
    if (lcdmodel == LCDMODEL_SUPERMICRO) {
        sprintf((char *) &buff, "%16s", IPAdd);
        sl_mvprintf (pSL, col, row, buff);
    } else {  
        outgoing_command.command = 31;
        outgoing_command.data[0] = col;
        outgoing_command.data[1] = row;
        sprintf((char *) &outgoing_command.data[2],
                   "%16s", IPAdd);
                
        outgoing_command.data_length = 18;
        send_packet();

    }

}
void UpdateSerialNum(int lcdmodel, char *SerialNum) {

    GetSerialNumber(SerialNum);
    lcdmenu[0] = SerialNum;
    UpdateDisplay(lcdmodel, 0, 1, SerialNum);
}

int CheckForCorrectLcdPanel()
{
    FILE *fp;
    char *cp;
    char DeviceType[255];
    int retval = 0;
    strcpy(DeviceType, "");

    fp = fopen("/etc/config/hw_profile_dms.txt", "r");
    if (fp == NULL) 
       return 0;
  
    fgets(DeviceType, 255, fp);
    if ((cp = strchr(DeviceType, '\n')) != 0)
        *cp = '\0';

    fclose(fp);
    
    /* here we are checking for supermicro LCD Panel*/
    if (!strcmp(DeviceType, "DMS-200-Device") || !strcmp(DeviceType, "DMS-100-Device")) {
        retval = 1;/* we are assuming it is supermicro LCD panel for device type*/
    }

    return retval;

}

int CheckDeviceDetection(int *lcdmodel, int *iSwitchInterval) {

    int ret = 1;
    if (pSL != NULL) {
        sl_close(pSL);
        pSL = NULL;

    }
    // First check for Serial over USB device 
    if (Serial_Init(LCDUSBDEV, LCDUSBBAUD) == 0) {
        LOG("Four Button Serial-USB LCD Panel device detected.\n");
        *lcdmodel = LCDMODEL_631;
        
    }
    else if ((pSL = sl_init()) != NULL) {
         LOG("Six Button SuperMicro LCD Panel device detected\n");
         *lcdmodel = LCDMODEL_SUPERMICRO;
         *iSwitchInterval = 1; // No polling, we do a blocking getkey call
    }
    // then check for direct serial LCD panel
    else if (Serial_Init(LCDDEV, LCDBAUD) == 0) {
         if (CheckForCorrectLcdPanel()) {
            ret = 0;
            Uninit_Serial();
         } else {
           LOG("Assuming  Serial LCD Panel\n");
           *lcdmodel = LCDMODEL_633;
         }
    }
    else {
         LOG("LCD Panel device not found\n");
         ret = 0;
    }
   
    return ret;
}

/* Reinilization of SuperMicro lCD panel when the bazel is removed 
   and put back */
void ReInitSuperMicroLCDPanel(int *lcdmodel, char *IpChange, 
                    char *SerialNum, int *iSwitchInterval) {

    FILE *fp;
    int rc = 0;
    /*if bzel is removed /proc/bus/usb/busNo/device address will be removed,
      if this device path still exists and retuns -1 error, it is a fake bzel detached code*/
    fp = fopen(pSL->name, "rb");
    if (fp == NULL) {
       if (pSL != NULL) {
          sl_close(pSL);
          pSL = NULL;
       }
    } else {
      fclose(fp);
      return;
    }
       
    /*when the device path is not exisiting, close the pre-existing 
      init and wait here till the bzel is attched again */
    while (!rc) {
        rc = CheckDeviceDetection(lcdmodel, iSwitchInterval);
        sleep(1);
    }

    sl_set_backlight (pSL, SL_ON); /* turn on backlight */
    sl_clear(pSL); /* Clear screen */
    sl_home(pSL); /* Go to 'Home' position */
    sl_set_cursor (pSL, SLCT_OFF); /* Set cursor to not blink */
    GetIPaddress(IpChange);
    UpdateDisplay(*lcdmodel, 0, 0, IpChange);
    UpdateSerialNum(*lcdmodel, SerialNum);

}


int main()
{
    int iCmdCurr=STATUS_NONE;
    int iCmdPrev=STATUS_NONE;

    int iStatusInterval;
    int iSwitchInterval;
    int iPAInterval;
    int index;
    
    unsigned int llastStatus, lnow, llastSwitch, llastCmd, llastPA;
    struct timeval tnow;


    int fdreset, fdstatus;

    struct termios options;
    int lstate;
    int k=0;
    int timed_out=0;
    char ip[255];
    char IpChange[255];
    char SerialNum[25];
    bool bDisplayChanged = 0;
    int iTicksToDisplay = 0;
    int rc = 0;
    int ret = 0;
  
/*     int pid = fork(); */
/*     if (pid) */
/*         exit(0); */

#ifndef DEBUG_FRONTPANEL
    if (daemon(1, 0) == -1) {
        printf("Could not daemonize process\n");
        exit(1);
    }
#else
    printf("Debug Mode: Running as foregrond\n");
#endif

    llastCmd = 0;
    llastSwitch = 0;
    llastPA = 0;


    gettimeofday (&tnow, NULL);
    lnow = (tnow.tv_sec*1000) + (tnow.tv_usec/1000);
    llastStatus = 0L;
    iStatusInterval = 1000;
    iSwitchInterval = 500;
    iPAInterval = 500;

    strcpy(IpChange, "");
    strcpy(ip, "");
    strcpy(SerialNum, "");
    
    fdreset = open(RESETDEV, O_RDWR | O_NDELAY);
    if (fdreset < 0) {
        fprintf(stderr, "ERROR: can not open \"%s\"\n", RESETDEV);
        exit(2);
    }
    lstate=TIOCM_DTR |TIOCM_RTS;
    ioctl(fdreset, TIOCMSET, &lstate);
    ioctl(fdreset, TIOCMGET, &lstate);
    if (lstate & TIOCM_DSR) {
        LOG ("Single Serial Port LEDS\n");
        nLedType = LED_SERIAL;
    }
    else {
//		LOG ("Multiple Serial Port LEDS");
//		nLedType = LED_MULTISERIAL;
        LOG ("LCD Panel\n");
        nLedType = LCD_PANEL;
        close(fdreset);
        
        /* Poll here till LCD panel is connected & detected */
        while(!ret) {
            ret = CheckDeviceDetection(&lcdmodel, &iSwitchInterval);
            sleep(1);
        }
              
        if (lcdmodel == LCDMODEL_SUPERMICRO) {
            /* Initialize SuperMicro LCD panel */
            sl_set_backlight (pSL, SL_ON); /* turn on backlight */
            sl_clear(pSL); /* Clear screen */
            sl_home(pSL); /* Go to 'Home' position */
            sl_set_cursor (pSL, SLCT_OFF); /* Set cursor to not blink */
            sl_mvprintf (pSL, 0, 0, "     BARCO INC ");
        }
        else {
            while(BytesAvail()) 
                GetByte();
            
            outgoing_command.command = 6;
            outgoing_command.data_length = 0;
            send_packet();
                    
            //CFA-631 / CFA-633 communications protocol only allows
            //one outstanding packet at a time. Wait for the response
            //packet from the CFA-631 / CFA-633 before sending another
            //packet.
            timed_out = 1; //reset timed_out to default false value
            for(k=0;k<=1000;k++) {
                if(check_for_packet())
                {
                    ShowReceivedPacket(); 
                    timed_out = 0; //set timed_out to false
                    break;
                }
                usleep(10000);
            }
            if(timed_out) {
                printf("Timed out waiting for a response.\n");
                LOG("LCD Panel device initialization failed.\n");
                exit(2);
            }
            if (lcdmodel == LCDMODEL_631) {

               // Setup key legend for LCDMODEL_631
                outgoing_command.command = 32;
                outgoing_command.data[0] = 1; // Enable
                outgoing_command.data[1] = 3; // Up
                outgoing_command.data[2] = 2; // Check
                outgoing_command.data[3] = 4; // Down
                outgoing_command.data[4] = 1; // Cancel
                outgoing_command.data_length = 5;
                        
                send_packet();
            }
        }
    }
    switch (nLedType) {
    case LED_SERIAL:
        fdstatus = fdreset;
        break;
    case LED_MULTISERIAL:
        fdstatus = open(STATUSDEV, O_RDWR | O_NDELAY);
        break;
    case LCD_PANEL:
        break;
    }
    if (nLedType != LCD_PANEL) {
        /* get the current options */
        tcgetattr(fdreset, &options);
        /* set raw input, 1 second timeout */
        options.c_cflag &= ~CRTSCTS;
        options.c_cflag     |= (CLOCAL | CREAD);
        options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_oflag     &= ~OPOST;
        options.c_cc[VMIN]  = 0;
        options.c_cc[VTIME] = 10;


        tcsetattr(fdreset, TCSANOW, &options);
    }

    SetStatusLed(fdstatus, iCmdPrev);

    /* Start of main loop. Create a marker file to denote we are up and running */
    system("/bin/touch /tmp/frontpanel_started.txt");

    while(1) {

        gettimeofday (&tnow, NULL);
        lnow = (tnow.tv_sec*1000) + (tnow.tv_usec/1000);


        if (iStatusInterval && ((llastStatus > lnow) || 
                                (lnow - llastStatus > iStatusInterval))) {
            llastStatus = lnow;
            iCmdCurr = PollStatusLed();
            GetIPaddress(IpChange);
            if (strcmp(IpChange, ip) != 0) {
                iTicksToDisplay = 1;
                strcpy(ip, IpChange);
            }
            if (bDisplayChanged == 1) {
                iTicksToDisplay = 3;
                bDisplayChanged = 0;
            } else if (iTicksToDisplay > 0) {
                --iTicksToDisplay;
                if (iTicksToDisplay <= 0) {
                    GetIPaddress(IpChange);
                    UpdateDisplay(lcdmodel, 0, 0, IpChange);
                    UpdateSerialNum(lcdmodel, SerialNum);
                }
            }

        }
        if (iSwitchInterval && ((llastSwitch > lnow) || 
                                (lnow - llastSwitch > iSwitchInterval))) {
            llastSwitch = lnow;
            rc = PollResetSwitch(fdreset);
            if (rc == 1) {
                switch(iCmdPrev) {
                case STATUS_STOP:
                    iCmdCurr = STATUS_STOP_SENT;
                    LOG ("Shutting Down\n");
                    system("/V2O/bin/shutdown_system");
                    break;
                case STATUS_STOP_COMPLETE:
                    iCmdCurr = STATUS_RESET;
                    LOG ("Waiting for Confirmation\n");
                    break;
                case STATUS_RESET:
                    iCmdCurr = STATUS_RESET_SENT;
                    LOG ("Reset in Progress\n"); 
                    system("/V2O/bin/reboot_system");
                    break;
                case STATUS_STREAM_OK:
                    iCmdCurr = STATUS_STOP;
                    LOG ("Wait for Confirm\n");
                    break;
                }
            } else if (rc == -1 && (lcdmodel == LCDMODEL_SUPERMICRO)) {
                    ReInitSuperMicroLCDPanel(&lcdmodel, IpChange,
                                     SerialNum, &iSwitchInterval);
            }
        }

        if ((iCmdCurr != 0) && (iCmdCurr != iCmdPrev)) {
            LOG (" %d %s\n", iCmdCurr, StatusCmd[getCmdIndex(iCmdCurr)].l_cmd);
            iCmdPrev = iCmdCurr;
            SetStatusLed(fdstatus, iCmdCurr);
            bDisplayChanged = 1;

        }
        index = getCmdIndex(iCmdPrev);
        if (StatusCmd[index].l_blinkrate != 0) {
            usleep(StatusCmd[index].l_blinkrate);
            SetStatusLed(fdstatus, iCmdPrev);
        }
        else {
            if (lcdmodel != LCDMODEL_SUPERMICRO) {
                usleep(100000);
            }
            else {
                // no sleep for SuperMicro, the getkey calls blocks for 100 ms
            }
        }
        
        if (iCmdCurr == STATUS_LCD_TEST) {
            system("echo 8 > " STATUSFILE);
        }
    }
    return 0;
}
