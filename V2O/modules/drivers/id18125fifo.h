#ifndef __ID18125FIFO_H_INCLUDED__
#define __ID18125FIFO_H_INCLUDED__

/* function prototypes */

#ifdef __KERNEL__
int id18125_init(struct v2d_board *board);
int id18125_write(int empty_offset, int full_offset);
int id18125_read(int *empty_offset, int *full_offset);
#endif

#endif /* __ID18125FIFO_H_INCLUDED__ */
