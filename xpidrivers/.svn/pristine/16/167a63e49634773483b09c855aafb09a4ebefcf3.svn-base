#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>


#include "../include/OmniTekIoctl_linux.h"

#define NUM_BARS 6
#define DEBUG 1

int showusage()
{

  printf("OmniTek BAR Register access application:\n\n");
  printf("This provides basic debug access directly to the device BARs.  Command usage is as follows:\n\n");
  printf("OmniTek_BAR_RegAccess -r barno offset\n");
  printf("\tRead from the specified BAR and offset (hex)\n\n");
  printf("OmniTek_BAR_RegAccess -w barno offset val\n");
  printf("\tWrite val (hex) to the specified BAR and offset (hex)\n\n");
  printf("OmniTek_BAR_RegAccess -R barno offset\n");
  printf("\tRead from the specified BAR and offset (hex) - no bounds checking (for BARs without an OmniTek Capability structure) is performed, potentially unstable!\n\n");
  printf("OmniTek_BAR_RegAccess -W barno offset val\n");
  printf("\tWrite val (hex) to the specified BAR and offset (hex) - no bounds checking (for BARs without an OmniTek Capability structure) is performed, potentially unstable!\n\n");
  printf("OmniTek_BAR_RegAccess -br barno offset\n");
  printf("\tRead byte from specified BAR and offset (hex)\n\n");
  printf("OmniTek_BAR_RegAccess -bw barno offset val\n");
  printf("\tWrite val (hex) to specified BAR and offset (hex)\n\n");
  printf("OmniTek_BAR_RegAccess -mr barno offset length\n");
  printf("\tRead byte range from specified BARk offset and length (hex)\n\n");
  printf("OmniTek_BAR_RegAccess -mw barno offset val length\n");
  printf("\tWrite value to byte range specified BAR, offset and length (hex)\n\n");


  return 0;
}

  
  

int main (int argc, char* argv[])
{

  int barno = -1;
  long int offset = -1;
  unsigned long int val = -1;
  long int len = -1;
  int cmd = 0;
  

  //Check args:
  if (argc < 2)
    return showusage();
  
  if (strlen(argv[1]) > 3)
    return showusage();
  
  if (!strncmp(argv[1],"-r",2) & !strncmp(argv[1],"-R",2) & !strncmp(argv[1],"-w",2) & !strncmp(argv[1],"-W",2))
    return showusage();

  if (!strncmp(argv[1],"-r",2)) cmd = 1; 

  if (!strncmp(argv[1],"-R",2)) cmd = 2;

  if (!strncmp(argv[1],"-w",2)) cmd = 3; 

  if (!strncmp(argv[1],"-W",2)) cmd = 4;

  if (!strncmp(argv[1],"-br",3)) cmd = 5; 

  if (!strncmp(argv[1],"-bw",3)) cmd = 6;

  if (!strncmp(argv[1],"-mr",3)) cmd = 7; 

  if (!strncmp(argv[1],"-mw",3)) cmd = 8;


  //Check args by command
  if ((cmd == 0) ||
      ((cmd == 1) && (argc != 4)) ||
      ((cmd == 2) && (argc != 4)) ||
      ((cmd == 3) && (argc != 5)) ||
      ((cmd == 4) && (argc != 5)) ||
      ((cmd == 5) && (argc != 4)) ||
      ((cmd == 6) && (argc != 5)) ||
      ((cmd == 7) && (argc != 5)) ||
      ((cmd == 8) && (argc != 6)))
    return showusage();
  
  
  barno = strtol(argv[2],0,0);
  if ((barno < 0) | (barno > NUM_BARS))
    return showusage();
  
  offset = strtol(argv[3],0,16);
  if ((offset < 0) | (offset > 0xFFFFFFF))
    return showusage();
  
  if (argc == 5)
  {
    val = strtoul(argv[4],0,16);
   }

  if (argc == 6)
  {
    len = strtol(argv[5],0,16);
    if ((len < 0) | (len > 0xFFFFFFFF))
      return showusage();
  }
  else if ((argc == 5) && (cmd == 7))
  {
    len = strtol(argv[4],0,16);
    if ((len < 0) | (len > 0xFFFFFFFF))
      return showusage();
  }
  //So everything should be ok... let's open the BAR Device if we can...
  char filename[32];
  sprintf(filename,"/dev/OmniTekBAR%d",barno);
	if (DEBUG) printf("Opening BAR: %s\n",filename);
  int fd = open(filename,0);
  if (fd == -1)
  {
	  printf("Fopen for bar %d returned %d\n",barno,errno);
    err(1,NULL);
    return 0;
  }   
  
	OmniTekBARIoctlBuffer buffer;
  buffer.Offset = offset;
  buffer.Value = val;
  if ((cmd == 7) || (cmd == 8))
  {
    buffer.Buffer = malloc(len); 
    buffer.Size   = len;
    memset(buffer.Buffer,val,len);
  }
  else
  {
    buffer.Buffer = 0;
    buffer.Size   = 0;
  }


  int err = 0;

  switch (cmd)  {
    case 1:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQREADREGISTER, &buffer)))
          printf("Read from bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Read from bar: %d, offset: %d returned result: 0x%x\n",barno,buffer.Offset,buffer.Value);
        return 0;
    case 2:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWREADREGISTER, &buffer)))
          printf("RAW Read from bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("RAW Read from bar: %d, offset: %d returned result: 0x%x\n",barno,buffer.Offset,buffer.Value);
        return 0;
    case 3:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQWRITEREGISTER, &buffer)))
          printf("Write to bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Write bar: %d, offset: %d OK\n",barno,buffer.Offset);
        return 0;
    case 4:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEREGISTER, &buffer)))
          printf("RAW Write to bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("RAW Write bar: %d, offset: %d OK\n",barno,buffer.Offset);
        return 0;
    case 5:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWREADBYTE, &buffer)))
          printf("Byte Read from bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Byte Read from bar: %d, offset: %d returned result: 0x%x\n",barno,buffer.Offset,buffer.Value);
        return 0;
    case 6:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEBYTE, &buffer)))
          printf("Byte Write to bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Byte Write bar: %d, offset: %d OK\n",barno,buffer.Offset);
        return 0;
    case 7:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWREADBYTERANGE, &buffer)))
          printf("Byte Range Read from bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Byte Range Read from bar: %d, offset: %d returned result: 0x%x\n",barno,buffer.Offset,buffer.Value);

        //Dump Buffer
        for (buffer.Size = 0; buffer.Size < buffer.Value; buffer.Size++)
          printf("%02x ",buffer.Buffer[buffer.Size]);
  
        if (buffer.Buffer) free(buffer.Buffer);

        return 0;
    case 8:
        if ((err = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEBYTERANGE, &buffer)))
          printf("Byte Range Write to bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,err);
        else
          printf("Byte Range Write bar: %d, offset: %d OK\n",barno,buffer.Offset);

        if (buffer.Buffer) free(buffer.Buffer);

        return 0;



    default:
      printf("Something went wrong here!\n");
  };
  return 0;
}

      

      

