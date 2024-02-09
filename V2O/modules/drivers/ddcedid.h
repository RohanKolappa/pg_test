#ifndef __DDCEDID_H_INCLUDED__
#define __DDCEDID_H_INCLUDED__

#ifdef __KERNEL__

/* function prototypes */

int ddcedid_init(struct v2d_board *board);
int ddcedid_read (unsigned char start_addr, unsigned char *data, 
                  int number_of_bytes);

#endif /* __KERNEL__ */

#endif /* __DDCEDID_H_INCLUDED__ */
