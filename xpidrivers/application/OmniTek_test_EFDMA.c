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
#include "../driver/efdma/test_efdma_ioctl.h"


int main (int argc, char* argv[])
{

  //Open a file descriptor for the EFDMA test    

  int fd = open("/dev/OmniTek_EFDMA_TEST",O_RDWR);

  if (fd == -1)
  {
    printf("Couldn't open EFDMA device\n");
    return 0;
  }

  struct efdma_packet_iobuf read;
  struct efdma_packet_iobuf write;

  write.buffer = (char *)malloc(TEST_EFDMA_MAX_PACKET_SIZE);
  read.buffer = (char *)malloc(TEST_EFDMA_MAX_PACKET_SIZE);

  size_t length = 123;
  short count = 0;
  int result = 0;
  int i = 0;
  int j = 100000;
  short * pos;

  //Dump any outstanding packets
  while (result == 0)
  {
    result = ioctl(fd,OMNITEK_BAR_IOCEFDMAREADPACKET,&read);
    if (result == 0)
    {
      printf("Dumped packet\n",result);  
    }
  }


start:

  j = 100000;
  printf("Testing %d variable sized packets\n",j);

  while (j--)
  {
    //Create the write packet
//    for (i = 0; i<length;i+=2)
//    {
//      *((short *)(write.buffer+i)) = count++;
//    } 

    for (pos = (short *)write.buffer; pos < (short *)(write.buffer + length); pos++)
    {
      *pos = count++;
    }

    write.length = length;
    
    //Write the packet:
    result = ioctl(fd,OMNITEK_BAR_IOCEFDMAWRITEPACKET,&write);
    if (result <0)
    {
      printf("Write packet ioctl failed with %d\n",result);
      return 0;
    }
      
    //set length  for read
    read.length = TEST_EFDMA_MAX_PACKET_SIZE; 
    //Read packet
    result = ioctl(fd,OMNITEK_BAR_IOCEFDMAREADPACKET,&read);
    if (result <0)
    {
      printf("read packet ioctl failed with %d\n",result);
      return 0;
    }
  
    //Check length
    if (read.length != write.length)
    {
      printf("Got unexpected packet size back: write was %zd, read is %zd\n",write.length,read.length);
      return 0;
    }
  
    //Check data
    if (memcmp(read.buffer,write.buffer,read.length))
    {
      off_t location = 0;
      for (location = 0; location < read.length; location++)
      {
        if (read.buffer[location] != write.buffer[location])
          break;
      }

      printf("Buffers didn't match at offset %p!\n", location);
      return 0;
    }
  
    //Update size
    length += 123; 
    length %= TEST_EFDMA_MAX_PACKET_SIZE; 
    if (length <= 16) length += 123;


  
  }

  printf("Successfully transferred variable sized packets.  Now performing packet bursts\n");

  length = 64;
  //Create the write packet
//  for (i = 0; i<length;i++)
//  {
//    write.buffer[i] = count++;
//  }

  for (pos = (short *)write.buffer; pos < (short *)(write.buffer + length); pos++)
  {
    *pos = count++;
  }

  write.length = length;

  //set length  for read
  read.length = TEST_EFDMA_MAX_PACKET_SIZE; 


  j = 0;
  while (length <= 1024)
  {
    while (j <= 10)
    {

      int burst = 1 << j++;
      printf("Burst length %d, packet size %d\n",burst,length);    

      //Write the packet:

      for (i = 0; i < burst; i++)
      {
        *((unsigned int *)write.buffer) = burst << 16 | count++ & 0xFFFF;  //Store packet number  
        result = ioctl(fd,OMNITEK_BAR_IOCEFDMAWRITEPACKET,&write);
        if (result <0)
        {
          printf("Write packet ioctl failed with %d\n",result);
          return 0;
        }
      }
        
      //Read packet
      for (i = 0; i < burst; i++)
      {
        result = ioctl(fd,OMNITEK_BAR_IOCEFDMAREADPACKET,&read);
        if (result <0)
        {
          printf("read packet ioctl failed with %d\n",result);
          return 0;
        }
    
        //Check length
        if (read.length != write.length)
        {
          printf("Got unexpected packet size back: write was %zd, read is %zd\n",write.length,read.length);
          return 0;
        }
      }
    
      //Check data in last packet...
      if (memcmp(read.buffer,write.buffer,read.length))
      {
        off_t location = 0;
        for (location = 0; location < read.length; location++)
        {
          if (read.buffer[location] != write.buffer[location])
            break;
        }

        printf("Buffers didn't match at offset %p!\n", location);
        return 0;
      }
    

    
    }
    length <<= 1;
  }

  goto start;

  return 0;
}
