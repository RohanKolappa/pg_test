
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "633_linux/typedefs.h"
#include "633_linux/serial.h"
#include "633_linux/633_packet.h"
#include "633_linux/show_packet.h"

static void wait_for_response() {
    int k;
    int timed_out = 1; //default timed_out is true

    //CFA-631 / CFA-633 communications protocol only allows
    //one outstanding packet at a time. Wait for the response
    //packet from the CFA-631 / CFA-633 before sending another
    //packet.
    for(k=0;k<=100000;k++) {
        if(check_for_packet())
        {
            ShowReceivedPacket();
            timed_out = 0; //set timed_out to false
            break;
        }
    }
    if (timed_out) {
        printf("Timed out waiting for a response.\n");
    }
}

//============================================================================
int main(int argc, char* argv[])
{
    char * device_string;
    char device_string_buffer[32];
    int offset;

    // Ensure atleast 3 parameters are entered, 
    // prompt for the missing parameter(s)
    if(argc < 4)
    {

        printf("Burn Reset Image to LCD.\n");
        printf("Usage:\n");
        printf("%s PORT BAUD DEVICENAME [burn]\n",argv[0]);
        printf("PORT is something like \"/dev/ttyS0\" or \"/dev/ttyUSB0\"\n");
        printf("BAUD is 19200 or 115200\n");
        printf("DEVICENAME is string you want to write as the device name\n");
        printf("Last argument of \"burn\" actually writes to make\n");
        printf("the display permanent\n");

        printf("\nMISSING A PARAMETER\n");
        printf("Enter PORT, BAUD, DEVICENAME and optionally \"burn\"\n\n");
        return(0);
    }
    
    // Check for optional "burn" parameter and set flag if found
    int burnimage = 0;
    if ((argc > 4) && (!strcmp(argv[4],"burn"))) 
        burnimage = 1;
    
    int baud;

    // Determine baud rate
    if (strcmp(argv[2],"115200") == 0) {
        baud=115200;
    }
    else if (strcmp(argv[2],"19200") == 0) {
        baud=19200;
    }
    else {
        printf("BAUD should be one of 19200 or 115200\n");
        exit(1);
    }

    // Create device string
    device_string = strdup(argv[3]);
    if (strlen(device_string) > 20) {
        printf("Device string too long.\n");
        exit(1);
    }

    strcpy(device_string_buffer, "                    ");
    offset = (20 - strlen(device_string))/2;
    memcpy(device_string_buffer + offset, device_string, 
           strlen(device_string));

    if(Serial_Init(argv[1],baud))
    {
        printf("Could not open port \"%s\" at \"%d\" baud.\n",argv[1],baud);
        return(1);
    }
    else {
        printf("\"%s\" opened at \"%d\" baud.\n\n",argv[1], baud);
    }


    //For some reason, Linux seems to buffer up data from the LCD, and they are sometimes
    //dumped at the start of the program. Clear the serial buffer.
    while(BytesAvail())
        GetByte();
    
    
    /* First clear screen */
    outgoing_command.command = 6;
    outgoing_command.data_length = 0;
    send_packet();
    wait_for_response();

    /* Clear legend */
    outgoing_command.command = 32;
    outgoing_command.data[0] = 0; // Disable
    outgoing_command.data_length = 1;
    send_packet();
    wait_for_response();

    //Send line 1 to the 633 using command 31
    outgoing_command.command = 31;
    outgoing_command.data[0]=0; //col
    outgoing_command.data[1]=0; //row
    memcpy(&outgoing_command.data[2], "     BARCO INC      ", 20);
    outgoing_command.data_length = 20;
    send_packet();
    wait_for_response();
    

    //Send line 2 to the 633 using command 31
    outgoing_command.command = 31;
    outgoing_command.data[0]=0; //col
    outgoing_command.data[1]=1; //row
    memcpy(&outgoing_command.data[2], device_string_buffer, 20);
    outgoing_command.data_length = 20;
    send_packet();
    
    wait_for_response();

    if (burnimage) {
        outgoing_command.command = 4;
        outgoing_command.data_length = 0;
        send_packet();
        wait_for_response();
        printf("\nDisplay burned to LCD.\n\n");
    }
    else {
        printf("\nTest run. Display not burned yet.\n\n");
    }

    printf("Done.\n\n");
    Uninit_Serial();
    return 0;
}

