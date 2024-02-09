#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

#include "../include/OmniTekIoctl_linux.h"

#define NUM_BARS (2)

typedef int (*cmd_call)(int argc, char * argv[]);

int debug_cmd_rr(int argc, char * argv[]);
int debug_cmd_rw(int argc, char * argv[]);
int debug_cmd_help(int argc, char * argv[]);

int debug_cmd_mr(int argc, char * argv[]);
int debug_cmd_mw(int argc, char * argv[]);


struct debug_command {
  char *    cmd;
  char *    description;
  char *    usage;
  int       arg_count;
  cmd_call  callback;
};

struct debug_command commands[] = 
{
  { 
    .cmd          = "rr",
    .description  = "register read",
    .usage        = "BAR 0xADDR",
    .arg_count    = 2,
    .callback     = &debug_cmd_rr
  },
  
  { 
    .cmd          = "rw",
    .description  = "register write",
    .usage        = "BAR 0xADDR 0xDATA",
    .arg_count    = 3,
    .callback     = &debug_cmd_rw
  },
  {
    .cmd          = "?",
    .description  = "Display help",
    .usage        = "N/A",
    .arg_count    = 0,
    .callback     = &debug_cmd_help
  },
  {
    .cmd          = "mr",
    .description  = "MDMA Read",
    .usage        = "dma_dev 0xADDR 0xLENGTH <filename>",
    .arg_count    = 2,
    .callback     = &debug_cmd_mr
  },
  {
    .cmd          = "mw",
    .description  = "MDMA Write",
    .usage        = "dma_dev 0xADDR 0xLENGTH filename",
    .arg_count    = 3,  
    .callback     = &debug_cmd_mw
  },
  {
    .callback     = 0
  }
};
  

int main(int argc, char * argv[])
{

  //Decode command argument  

  if (argc <= 1)
  {
    printf("No command specified, use ? for list\n");
    return 0;
  }

  struct debug_command * current = commands;
  while (current->callback)
  {
    if (!strncmp(current->cmd,argv[1],2))
    {
      printf("Command: %s\n",current->description);
      if (argc < current->arg_count+2)
      {
        printf("Usage: %s\n",current->usage);
        return 0;
      }
      return (current->callback(argc,argv));
    }
    current++;
  }
  printf("Unrecognized Command\n");

return 0;
}

int openbar(int barno)
{

  //Open BAR device
  char filename[32];
  sprintf(filename,"/dev/OmniTekBAR%d",barno);
  int fd = open(filename,0);
  if (fd == -1)
  {
	  printf("Fopen for bar %d returned %d\n",barno,errno);
    err(1,NULL);
  }   
  return fd;


}


int debug_cmd_rr(int argc, char * argv[])
{
	OmniTekBARIoctlBuffer buffer;
  int result =0;
  int barno = -1;
  int fd = -1;

  barno = strtol(argv[2],0,0);
  if ((barno < 0) | (barno > NUM_BARS))
  {
    printf("BAR Must be between 0 and %d\n",NUM_BARS-1);
    return 0;
  }

  buffer.Offset = strtol(argv[3],0,16);

  fd = openbar(barno);
  if (fd == -1) 
  {
    err(1,NULL);  
    return 0;
  }

  if ((result = ioctl(fd, OMNITEK_BAR_IOCQREADREGISTER, &buffer)))
    printf("Read from bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,result);
  else
    printf("Read from bar: %d, offset: %d returned result: 0x%x\n",barno,buffer.Offset,buffer.Value);

  return 0;

}

int debug_cmd_rw(int argc, char * argv[])
{
	OmniTekBARIoctlBuffer buffer;
  int result =0;
  int barno = -1;
  int fd = -1;

  barno = strtol(argv[2],0,0);
  if ((barno < 0) | (barno > NUM_BARS))
  {
    printf("BAR Must be between 0 and %d\n",NUM_BARS-1);
    return 0;
  }

  buffer.Offset = strtol(argv[3],0,16);
  buffer.Value =  strtol(argv[4],0,16);

  fd = openbar(barno);
  if (fd == -1) return 0;

  if ((result = ioctl(fd, OMNITEK_BAR_IOCQWRITEREGISTER, &buffer)))
    printf("Write to bar: %d, offset: %d returned error: %d!\n",barno,buffer.Offset,result);
  else
    printf("Write bar: %d, offset: %d OK\n",barno,buffer.Offset);

  return 0;

}

int debug_cmd_help(int argc, char * argv[])
{
  struct debug_command * current = commands;
  
  printf("Usages:\n");

  while (current->callback)
  {
    printf("Command:\t %s\n",current->description);
    printf("Usage:\t\t omnitek_debug %s %s\n\n",current->cmd,current->usage);
    current++;
  }

  return 0;
}

int debug_mdma_open(int argc, char * argv[], loff_t * address, size_t * length, int * dma_fd, int *local_fd, const char ** buffer, int write)
{
  //Perform an IOCtl based MDMA Read

  //Decode the arguments:
  *dma_fd = open(argv[2],0);
  if (*dma_fd == -1) 
  {
    printf("Couldn't open MDMA Device file: %s\n",argv[2]);
    err(1,NULL);  
    return err;
  }

  //Get address
  *address = strtol(argv[3],0,16);
  *length  = strtol(argv[4],0,16);
  
  //Get local file if provided 
  if (argc==6)
  {
    printf("Opening local file: %s\n",argv[5]);
    *local_fd = open(argv[5],O_RDWR | O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
  }
  else if (write)
  {
    printf("Opening local file: %s\n","/dev/urandom");
    *local_fd = open("/dev/urandom",O_RDONLY);
  }
  else
  {
    *local_fd = -1;
  }
  
  if ((*local_fd == -1) && ((write) || ((!write && argc==6))))
  {
    printf("Couldn't open source data file: %s\n",argc == 6 ? argv[5] : "/dev/urandom");
    err(1,NULL);  
    return -ENOENT;
  }

  if ((*address < 0) /*|| (*address > OMNITEK_MAX_MDMA_ADDR)*/)
  {
    printf("Address: %x out of range!\n",*address);
    return -ERANGE;
  }
  if ((*length < 0) /*|| (*length > OMNITEK_MAX_MDMA_LENGTH)*/)
  {
    printf("Length: %x out of range!\n",*length);
    return -ERANGE;
  }
  
  *buffer = malloc(sizeof(char) * *length);
  return 0;
}

int debug_cmd_mr(int argc, char * argv[])
{
  int fd, output_fd,i;
  struct _OmniTekMDMAIoctlBuffer  dma_buffer;

  int result = debug_mdma_open(argc,argv,&dma_buffer.f_pos,&dma_buffer.count,&fd,&output_fd,&dma_buffer.buf, 0);
  if (result) return result;

  //Have everything we need to do the read

  //Read from the device
  if (result = ioctl(fd, OMNITEK_MDMA_READ, &dma_buffer) != dma_buffer.count)
    printf("Read from MDMA returned error: %d!\n",result);
  else
    printf("Read from MDMA Ok!\n");

  if (output_fd != -1)
  {
    result = write(output_fd,dma_buffer.buf,dma_buffer.count);
    if (result != dma_buffer.count)
    {
      printf("Failed!\n");
      return result;
    }
  }
  else
  {
    printf("Output:\n");
    for(i = 0; i < dma_buffer.count; i++)
    {       
      printf("%c",dma_buffer.buf[i]);
    }
    printf("\nDone\n");
  }

  return 0;
  

}

int debug_cmd_mw(int argc, char * argv[])
{
  int fd, input_fd;
  struct _OmniTekMDMAIoctlBuffer  dma_buffer;

  int result = debug_mdma_open(argc,argv,&dma_buffer.f_pos,&dma_buffer.count,&fd,&input_fd,&dma_buffer.buf, 1);
  if (result) return result;

  //Have everything we need to do the write
  //read from file
  result = read(input_fd,dma_buffer.buf,dma_buffer.count);
  if (result != dma_buffer.count)
  {
    printf("Couldn't read from input file, result: %d\n",result);
    return errno;
  }
  
  //Write to DMA
  if (result = ioctl(fd, OMNITEK_MDMA_WRITE, &dma_buffer) != dma_buffer.count)
    printf("Write to MDMA returned error: %d!\n",result);
  else
    printf("Write to MDMA Ok!\n");

  return 0;
}

