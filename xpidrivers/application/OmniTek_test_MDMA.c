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
    sprintf(filename,"%s%02d","/dev/OmniTekMDMA",i);

  	printf("Opening file: %s\n",filename);
    fd[i] = open(filename,O_RDWR);

	  if (fd[i] == -1)
	  {
		  printf("Fopen for bar %d returned %d\n",i,errno);
      err(1,NULL);

	  } else
      j++;
  }
#define BUFFER_SIZE (32768/8)

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
#define RAND_SIZE_MULT 16

//Can be set to use aligned memory
#if 1
  char * rand_buffer = (char *)malloc(buf_size * RAND_SIZE_MULT);
#else
  void * vrand_buffer;
  char * rand_buffer;
  if (posix_memalign(&vrand_buffer, 1<<12, buf_size * RAND_SIZE_MULT))
  {
    printf("Couldn't allocate aligned memory\n");
    return 0;
  }
  else
  {
    printf("allocated aligned memory at %p\n",vrand_buffer);
    rand_buffer = (char*)vrand_buffer;
  }
#endif

  off_t  rand_offset = 12;

#define COUNT_RAND
#ifndef COUNT_RAND
  result = read(fd_rand,rand_buffer,buf_size * RAND_SIZE_MULT);
  if (result < 0) {
    perror("Failure: to read from random");
    return 0;
  }
#else
  for (i = 0 ; i < buf_size * RAND_SIZE_MULT; i+= sizeof(unsigned int))
    *((unsigned int *)(rand_buffer + i)) = 2*i / sizeof(unsigned int);
#endif



  while (result >= 0)
  {
    for (i=0; i< NUM_MDMA; i++)
    {
      char  read_buffer[buf_size];
#ifndef SKIP_DMA    //Enable this to skip the DMA transfer - just to check the basic function is correct
      lseek(fd[i],buf_offset,SEEK_SET);
      result = write(fd[i],rand_buffer + rand_offset,buf_size);
      if (result < 0) {
        perror("Failure: to write to MDMA");
        break;
      }
      
      //memset(read_buffer,0,BUFFER_SIZE);
      lseek(fd[i],buf_offset,SEEK_SET);

      result = read(fd[i],read_buffer,buf_size);
      if (result < 0) {
        perror("Failure: to read from MDMA");
        break;
      }
#else
      memcpy(read_buffer,rand_buffer + rand_offset,buf_size);
#endif            

      result = memcmp(read_buffer,rand_buffer + rand_offset,buf_size);

      if (result != 0) {
        perror("Failure: Read data didn't match written data");

        for ( j = 0 ; j < buf_size; j++)
        {
          if (read_buffer[j] != rand_buffer[rand_offset + j])
            printf("Difference at offset: 0x%08x - W: 0x%08x R: 0x%08x?\n",j, (*(unsigned int *)(rand_buffer + rand_offset + j)), (*(unsigned int *)(read_buffer + j)));
        }

        //Redo the buffer read and re-check the
        char  read_buffer_two[buf_size];
        lseek(fd[i],buf_offset,SEEK_SET);

        result = read(fd[i],read_buffer_two,buf_size);
        if (result < 0) {
          perror("Failure: to read from MDMA");
          break;
        }


        result = memcmp(read_buffer_two,rand_buffer + rand_offset,buf_size);
        if (result != 0) {
          perror("Retry Failure: Read data (still) didn't match written data");

          for ( j = 0 ; j < buf_size; j++)
          {
            if (read_buffer_two[j] != rand_buffer[rand_offset + j])
              printf("Difference at offset: 0x%08x - W: 0x%08x R: 0x%08x?\n",j, (*(unsigned int *)(rand_buffer + rand_offset + j)), (*(unsigned int *)(read_buffer_two + j)));
          }
        }        
        else
        {
          printf("Buffers match on retry\n");
        }
 
        goto out;
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
    
      rand_offset += 123;
      if (rand_offset > (buf_size * RAND_SIZE_MULT) - buf_size)
        rand_offset %= (buf_size * RAND_SIZE_MULT) - buf_size;
    
      rand_offset &= ~0x7;

    }

    if (argc == 2)
    {
      if (strncmp(argv[1],"once",4) == 0)
        break;

    }
  }
  
out:
  for (i= 0 ; i < NUM_MDMA; i++)
  {
    if (fd[i] != -1)
      close(fd[i]);
  }

  close(fd_rand);

  free(rand_buffer);

  return 0;

}

