#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include "../include/OmniTekIoctl_linux.h"

#define NUM_MDMA 1



int main (int argc, char* argv[])
{
	int i = 0, j=0;
	//Open the ioctl_test file
	int fd[NUM_MDMA];
  int fd_rand;
  for (i= 0 ; i < NUM_MDMA; i++)
  {
    char filename[20];
    sprintf(filename,"%s%02d", "/dev/OmniTekMDMA", i);

  	printf("Opening file: %s\n",filename);
    fd[i] = open(filename,O_RDWR);

	  if (fd[i] == -1)
	  {
		  printf("Fopen for bar %d returned %d\n",i,errno);
      err(1,NULL);

	  } else
      j++;
  }
#define BUFFER_SIZE (32768/1)

  unsigned int buf_size = BUFFER_SIZE;
  unsigned int buf_offset = 0;

  if (argc == 2)
  {
    if (strncmp(argv[1],"upper",5) == 0)
    {
      buf_size = BUFFER_SIZE/2;
      buf_offset = BUFFER_SIZE/2;
    }      
    else if (strncmp(argv[1],"lower",5) == 0)
    {
      buf_size = BUFFER_SIZE/2;
      buf_offset = 0;
    }    
  }

  fd_rand = open("/dev/urandom",0);
//  fd_rand = open("/dev/zero",0);

	printf("%d files opened\n",j);
  
//We have two 16kbit block rams, so approx 4kBytes memory


  unsigned long long transferred = 0;

  int result = 0;
  unsigned long long lastmb = 0;
  
  OmniTekMDMAIoctlBuffer ioc_buffer;

  while (result >= 0)
  {
    for (i=0; i< NUM_MDMA; i++)
    {
      char  rand_buffer[buf_size];
      char  read_buffer[buf_size];
      result = read(fd_rand,rand_buffer,buf_size);
      if (result < 0) {
        perror("Failure: read from random");
        break;
      }

      //lseek(fd[i],buf_offset,SEEK_SET);
      //result = write(fd[i],rand_buffer,buf_size);      
      ioc_buffer.buf = rand_buffer;
      ioc_buffer.count = buf_size;
      ioc_buffer.f_pos = buf_offset;
      result = ioctl(fd[i],OMNITEK_MDMA_WRITE_KERNEL,&ioc_buffer);
      
      if (result < 0) {
        perror("Failure: write to MDMA");
        break;
      }
      
      //memset(read_buffer,0,BUFFER_SIZE);
      //lseek(fd[i],buf_offset,SEEK_SET);
      //result = read(fd[i],read_buffer,buf_size);
      ioc_buffer.buf = read_buffer;
      ioc_buffer.count = buf_size;
      ioc_buffer.f_pos = buf_offset;
      result = ioctl(fd[i],OMNITEK_MDMA_READ_KERNEL,&ioc_buffer);

      if (result < 0) {
        perror("Failure: read from MDMA");
        break;
      }

      result = memcmp(read_buffer,rand_buffer,buf_size);

      if (result != 0) {
        perror("Failure: Read data didn't match written data");
        break;
      }

      transferred  += buf_size;

//#ifdef ARM
      if (lastmb != (transferred >> 20))
      {
//        printf("%lld\t%lld\n",transferred>>20,lastmb);
        lastmb = transferred >> 20;      
//#endif
        printf("\r\033[ATransferred: %lld B %lld KiB %lld MiB %lld GiB\n",transferred,   
                                                                transferred >> 10, 
                                                                transferred >> 20, 
                                                                transferred >> 30);      
//#ifdef ARM
      }
//#endif



    }
  }
  
  for (i= 0 ; i < NUM_MDMA; i++)
  {
    if (fd[i] != -1)
      close(fd[i]);
  }

  close(fd_rand);

  return 0;

}

