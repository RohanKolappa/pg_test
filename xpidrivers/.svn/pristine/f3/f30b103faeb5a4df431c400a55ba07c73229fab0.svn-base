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
#include <libaio.h>
#include <time.h>
#include "../include/OmniTekIoctl_linux.h"

int main (int argc, char* argv[])
{
  int i = 0, j=0;

  int read_mode = 1;

  if (argc == 2)
  {
    if (strncmp(argv[1],"read",4) == 0)
    {
      read_mode = 1;
    }      
    else if (strncmp(argv[1],"write",5) == 0)
    {
      read_mode = 0;
    }
  }


	//Open the ioctl_test file
	int fd;
  char * filename;

#define BUFFER_SIZE (1<<19)   //1/2MBish buffer - the sg_alloc_table routine in the ARM borks otherwise,...
  if (read_mode == 1) filename = "/dev/OmniTekFDMA_READ00";
  else                filename = "/dev/OmniTekFDMA_WRITE00";

#define NIOEVENTS (5)

	printf("Opening file: %s\n",filename);
  fd = open(filename,O_RDWR);

  if (fd == -1)
  { 
	  printf("Fopen for FDMA %s returned %d\n",filename,errno);
    err(1,NULL);
    return 0;
  } 



	printf("files opened\n");

  //Set FDMA Channel to manual mode
  ioctl(fd,OMNITEK_FDMA_SET_AUTO,0);


/* Create Buffers - fill with a count*/
  unsigned char * buffer[NIOEVENTS];

  for (i = 0; i < NIOEVENTS; i++)
  {
    buffer[i] = malloc(BUFFER_SIZE*sizeof(unsigned char));
    for (j = 0; j<BUFFER_SIZE; j++)
    {
      *(buffer[i] + j) = j;
    }
  }


/* SETUP AIO */
	struct io_context * ctx = 0;
  int result;

	printf("Creating AIO Context\n");
	result = io_setup(NIOEVENTS, &ctx);
	if (result != 0)
	{
		printf("Failed, error: %d\n",result);
		printf("%s\n",strerror(-result));
		return result;
	}

	struct iocb * iocbs[NIOEVENTS];
	struct io_event events[NIOEVENTS];


  for (i = 0; i < NIOEVENTS; i++)
  {
    iocbs[i] = malloc(sizeof(struct iocb));
    if (read_mode)
      io_prep_pread(iocbs[i],fd,buffer[i],BUFFER_SIZE,0);		//Initialize iocb
    else
      io_prep_pwrite(iocbs[i],fd,buffer[i],BUFFER_SIZE,0);		//Initialize iocb
  }

  /* Initial submission */
	result = io_submit(ctx,NIOEVENTS,iocbs);
	if ((result < 0) || (result != NIOEVENTS))
	{
		printf("Failed, error: %d\n",result);
		printf("%s\n",strerror(-result));        
		return result;
	}
  else if (result < NIOEVENTS)
  {
		printf("Only %d out of %d requests submitted\n",result,NIOEVENTS);
  }
	else
		printf("IO Submitted OK\n");

  //Start FDMA channel
  
  clock_t timer = clock();
  clock_t lasttime = timer;
  printf("Start time: %d\n",timer/(CLOCKS_PER_SEC/1000));
  ioctl(fd,OMNITEK_FDMA_START,1);

  
  struct timespec timeout = {
    .tv_sec = 10,
    .tv_nsec = 0
  };

  //Main Loop.
  int state = 0;
  int totalEvents = 0;
  while(1)
  {
    
    //Wait for an event
		int nevents = io_getevents(ctx, 1, NIOEVENTS, events,&timeout);



		if (nevents < 0)
		{
			printf("Failure: %d?\n",i);
			printf("%s\n",strerror(-result));
			return -1;	
		}
		else if (nevents >= 1)
		{
#if 0
      timer = clock();
      printf("Event time: %d\n",(timer-lasttime)/(CLOCKS_PER_SEC/1000));
      lasttime=timer;

#else
#ifndef ARM
      switch (++state & 0x3)  {
      case 0:
        printf("\b-");
        break;
      case 1:
        printf("\b\\");
        break;
      case 2:
        printf("\b|");
        break;
      default:
        printf("\b/");
        break;
      }
#else
//      printf (".");
      printf("%d",nevents);

#endif
      fflush(stdout);
#endif
      //Re-submit this request...



      for (i = 0; i< nevents; i++)
      {
        if (events[i].res != 0)
        {
          printf("Event completed with result: %lx\n",events[0].res);
          return -1;
        }

        struct iocb * cur_iocb = (struct iocb *)(events[i].obj);
//        io_submit(ctx,1,&cur_iocb);
        iocbs[i] = cur_iocb;
      }
      io_submit(ctx,i,iocbs);
      totalEvents+=nevents;
      
		}
    else if (nevents == 0)
    {
      printf("Timeout after %d events?  Cancelling events\n",totalEvents);
      int cancelled_events = 0;
      int result = 0;


/*    'Nice' cancel - stop a request then see if the others come back
      if ((result = io_cancel(ctx,iocbs[0],events)))
        printf("Cancel returned error: %08x %s\n",result, strerror(-result));
      else
        cancelled_events++;
      
      //Cancelled first event.  Others should cancel automagically
      printf("Waiting on further events:\n");
      while (cancelled_events < NIOEVENTS)
      {
        result = io_getevents(ctx, 1, NIOEVENTS, events, &timeout);
        if (result == 0)
          printf("Timed out waiting for more events to cancel\n");
        else if (result > 0)
        {
          cancelled_events += result;
          printf("%d events cancelled, %d total\n",result,cancelled_events);
        }
        else if (result < 0)
        {
          printf("Getevents returned error: %08x %s\n",result,strerror(-result));
        }
      }

      if (cancelled_events == NIOEVENTS)
        printf("All requests cancelled successfully!!!\n");
*/   
      printf("Destroy Context\n");
      io_destroy(ctx);    //Let's just kill the lot of it...
      printf("Close file descriptor\n");
      //Set FDMA Channel to manual mode
      ioctl(fd,OMNITEK_FDMA_SET_AUTO,1);

      close(fd);

    
      printf("Exit\n");



      return 0;

    }
    else
    {
      printf("Got unexpected number of events back: %d",nevents);
    }
  }


  if (fd != -1)
  {
      //Set FDMA Channel to manual mode
      ioctl(fd,OMNITEK_FDMA_SET_AUTO,1);
      close(fd);
  }


  return 0;

}
