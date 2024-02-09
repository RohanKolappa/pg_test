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


  if (argc != 3)
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

  int oddmode = 0;
  if        (strncmp(argv[2],"odd",3)  == 0) 
  {
    oddmode = 1;
    printf("Operating in 'odd' mode\n");
  }
  else if   (strncmp(argv[2],"even",4) == 0) 
  {
    oddmode = 0;
    printf("Operating in 'even' mode\n");
  }
  else      return showusage();


  //We're going to do a series of reads and writes to various locations within the register space
  //Even mode gets registers 0,2,4 and odd mode 1,3,5

  //Initialise the register space:
  
  if (oddmode)
  {
    write_hw_buffer(fd, write_buffer, 4, 4)  ;
    write_hw_buffer(fd, write_buffer, 12, 4)  ;
  }
  else
  {
    write_hw_buffer(fd, write_buffer, 0, 4)  ;
    write_hw_buffer(fd, write_buffer, 8, 4)  ;
    write_hw_buffer(fd, write_buffer, 16,4)  ;
  }

  //We'll also do a range of byte-sized accesses, from 1 to 4 to each register
  int a,k,l;
  int byte_a, byte_b, cur_reg = oddmode;

  printf(" \n\n\n\n\n\n");
  while(1)
  {

    cur_reg+= 2;
    if ((oddmode)  && (cur_reg >= 5)) cur_reg = 1;
    if ((!oddmode) && (cur_reg >= 6)) cur_reg = 0;

    a += 3;

    if (++k >= 0xFF) k = 0;

    byte_a = a & 0x3;
    byte_b = (a >> 2) & 0x3;

//    int offset =cur_reg * 4 + (byte_a < byte_b ? byte_a : byte_b);
//    int length = ((byte_a < byte_b ? byte_b - byte_a : byte_a - byte_b) & 0x3) + 1;
    int offset =cur_reg * 4;
    int length = 4;



    //Perform the local memory write
    memset(local_buffer+offset,k & 0xFF,length);
    memset(write_buffer+offset,k & 0xFF,length);


    if (!length)
      continue; //Skip zero-length transfers

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


    //Print a dump of what's happening.
    printf("\r\033[A\033[A\033[A\033[A\033[A\033[A");
    printf("k:0x%02x\t offset: %d\t length: %d\n",k&0xFF,offset,length);

    for (l = 0; l < 5; l++)
      printf("%d:\t%02x %02x %02x %02x \t %02x %02x %02x %02x\n",l,
                                                            (unsigned char)local_buffer[4*l+3],
                                                            (unsigned char)local_buffer[4*l+2],
                                                            (unsigned char)local_buffer[4*l+1],
                                                            (unsigned char)local_buffer[4*l+0],
                                                            (unsigned char)read_buffer[4*l+3],
                                                            (unsigned char)read_buffer[4*l+2],
                                                            (unsigned char)read_buffer[4*l+1],
                                                            (unsigned char)read_buffer[4*l+0]);

    //Compare the buffers:
    if (oddmode)
    {
      if (memcmp(local_buffer+4,read_buffer+4,4) )
      {
        printf("Failed register 1\n");
        return 0;
      }
      else if (memcmp(local_buffer+12,read_buffer+12,4))
      {
        printf("Failed register 3\n");
        return 0;
      }

    }
    else
    {
      if (memcmp(local_buffer,read_buffer,4) )
      {
        printf("Failed register 0\n");
        return 0;
      }
      else if (memcmp(local_buffer+8,read_buffer+8,4))
      {
        printf("Failed register 2\n");
        return 0;
      }
      else if (memcmp(local_buffer+16,read_buffer+16,4))
      {
        printf("Failed register 4\n");
        return 0;
      }

    }

  }





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

int showusage()
{
  printf("OmniTek_test_BAR_MultiAccess DEVICE ODD|EVEN \n");
  return 0;
}
