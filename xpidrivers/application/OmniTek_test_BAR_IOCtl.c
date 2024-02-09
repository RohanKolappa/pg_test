#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

#include "../include/OmniTekIoctl_linux.h"

#define NUM_BARS 2

int main (int argc, char* argv[])
{
	OmniTekBARIoctlBuffer buffer;
	int i = 0, j=0;
	//Open the ioctl_test file
	int fd[NUM_BARS];
  for (i= 0 ; i < NUM_BARS; i++)
  {
    char filename[20];
    sprintf(filename,"/dev/OmniTekBAR%d",i);
  	printf("Opening file: %s\n",filename);
    fd[i] = open(filename,0);

	  if (fd[i] == -1)
	  {
		  printf("Fopen for bar %d returned %d\n",i,errno);
      err(1,NULL);

	  } else
      j++;
  }
	printf("Files %d opened\n",j);

  if (j== NUM_BARS)
  {
    
    for (i = 0; i <NUM_BARS; i++)
    {
      for (j=0; j<100;j++)
      {
        buffer.Offset = j;
        if (ioctl(fd[i], OMNITEK_BAR_IOCQREADREGISTER, &buffer))
          printf("Read from bar: %d, offset: %d returned error!\n",i,j);
        else
          printf("Read from bar: %d, offset: %d returned result: 0x%x\n",i,j,buffer.Value);
      }
    }
  }
  
  for (i= 0 ; i < NUM_BARS; i++)
  {
    if (fd[i] != -1)
      close(fd[i]);
  }

  return 0;

}

