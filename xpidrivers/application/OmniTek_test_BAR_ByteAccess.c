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


#define TESTREG_BASE 20

int showusage();

int read_hw_buffer(int fd, unsigned char * buffer, int offset, int length);
int write_hw_buffer(int fd, unsigned char * buffer, int offset, int length);


int main(int argc, char * argv[])
{

  int pos = 0;

  if (argc != 2)
    return showusage();
  

  //we have 5 registers = 20 bytes space to play with
  //Allocate an array for this
  unsigned char * local_buffer = malloc(sizeof(unsigned char) * 20);

  unsigned char * read_buffer  = malloc(sizeof(unsigned char) * 20);
  unsigned char * write_buffer = malloc(sizeof(unsigned char) * 20);

  memset(local_buffer,0x55,20);
  memset(read_buffer,0xAA,20);
  memset(write_buffer,0x55,20);

	printf("Opening BAR: %s\n",argv[1]);
  int fd = open(argv[1],0);
  if (fd == -1)
  {
	  printf("Couldn't open file!\n");
    err(1,NULL);
    return 0;
  }   




  


  //We're going to do multiple interleaved reads/writes to these memory locations
  //in order to test the hardware

  if (write_hw_buffer(fd,write_buffer,0,20) != 20)
  {
    printf("Initial write failed!\n");
    return 0;
  }
  if (read_hw_buffer(fd,read_buffer,0,20) != 20)
  {
    printf("Initial read failed!\n");
    return 0;
  }
  //Check that worked ok

  if ((pos = memcmp(read_buffer,write_buffer,20)))
  {
    printf("Initial read/write compare failed at pos: %d!\n",pos);
    return 0;
  }

  int i = 0, j= 0, k = 0, l=0;
  int a,b;
  //Do a rolling loop.  We're going to make identical accesses to both memory buffers and
  //see if we get the same results

  printf(" \n\n\n\n\n\n");
  while (1)
  {
    i = (i+1);
    j = (j+5);
    k = (k+1013) ;
    
    b=i % 20 + 1  ;
    a=j % 21;


    //Use the lesser of i or j as the base address, and the greater as the end address
    //Write the value of k to the range of registers
    int offset = a < b ? a : b;
    int length = a < b ? b-a : a-b;
    
//    printf("i: %d, j: %d, a: %d, b: %d, offset: %d, length: %d\n",i,j,a,b,offset,length);

    if (a == b) continue;   //Skip zero length transfers

    //Perform the local memory write
    memset(local_buffer+offset,k & 0xFF,length);
    memset(write_buffer+offset,k & 0xFF,length);
    
    if ((pos = memcmp(local_buffer,write_buffer,20)))
    {
      printf("Buffer mismatch: %d!\n",pos);
      return 0;
    }


    //Write to hardware

    if (write_hw_buffer(fd,local_buffer+offset,offset,length) != length)
    {
      printf("Write failed!\n");
      return 0;
    }

    //Read back from hardware
    
    if (read_hw_buffer(fd,read_buffer,0,20) != 20)
    {
      printf("Read failed!\n");
      return 0;
    }
    //Check that worked ok

    //Print a dump of what's happening.
    printf("\r\033[A\033[A\033[A\033[A\033[A\033[A");
    printf("k:0x%02x\t offset: %d\t length: %d\n",k&0xFF,offset,length);

    for (l = 0; l < 5; l++)
      printf("%02x %02x %02x %02x \t %02x %02x %02x %02x\n",(unsigned char)local_buffer[4*l+3],
                                                            (unsigned char)local_buffer[4*l+2],
                                                            (unsigned char)local_buffer[4*l+1],
                                                            (unsigned char)local_buffer[4*l+0],
                                                            (unsigned char)read_buffer[4*l+3],
                                                            (unsigned char)read_buffer[4*l+2],
                                                            (unsigned char)read_buffer[4*l+1],
                                                            (unsigned char)read_buffer[4*l+0]);


    if ((pos = memcmp(read_buffer,local_buffer,20)))
    {
      printf("Read/write compare failed at pos: %d!\n",pos);

      if (read_hw_buffer(fd,read_buffer,0,20) != 20)
      {
        printf("Read failed!\n");
        return 0;
      }
      if ((pos = memcmp(read_buffer,local_buffer,20)))
      {
        printf("Read/write compare failed again at pos: %d!\n",pos);
      }

      return 0;


    }


  }



}

int showusage()
{
  printf("OmniTek_test_BAR_ByteAccess DEVICE\n");
  return 0;
}

int write_hw_buffer(int fd, unsigned char * buffer, int offset, int length)
{
  OmniTekBARIoctlBuffer ioc_buff;
  int err = 0;
  

  //Initial clear
  ioc_buff.Offset = TESTREG_BASE + offset;
  ioc_buff.Size   = length;
  ioc_buff.Buffer = buffer;

  err = ioctl(fd, OMNITEK_BAR_IOCQRAWWRITEBYTERANGE, &ioc_buff);
  if (err) return err;
  
  return ioc_buff.Value;

}


int read_hw_buffer(int fd, unsigned char * buffer, int offset, int length)
{
  OmniTekBARIoctlBuffer ioc_buff;
  int err = 0;
  


  //Initial clear
  ioc_buff.Offset = TESTREG_BASE + offset;
  ioc_buff.Size   = length;
  ioc_buff.Buffer = buffer;

  err=ioctl(fd, OMNITEK_BAR_IOCQRAWREADBYTERANGE, &ioc_buff);
  if (err) return err;
  
  return ioc_buff.Value;

}
