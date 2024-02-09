#ifndef __KBM_CONTROL_H_INCLUDED__
#define __KBM_CONTROL_H_INCLUDED__

#define MS_ID_PS2               0
#define MS_ID_IMPS2             3
#define MS_TYPE_PS2             MS_ID_PS2
#define MS_TYPE_IMPS2           MS_ID_IMPS2
#define MS_PS2_PKTSIZE          6
#define MS_IMPS2_PKTSIZE        8
#define KB_TYPE_PC              2
#define KB_TYPE_SGI             3

#define KBM_DATA_SIZE           32

#ifdef  __KERNEL__
struct kbm_device {
    struct semaphore kbm_sem;           /* mutual exclusion semaphore */
    wait_queue_head_t kbm_inq, kbm_outq;/* queue of processes waiting to
                                           read and write to device */
    spinlock_t  kbm_lock;               /* controls access to kbm_buffer */
    char kbm_buffer[KBM_DATA_SIZE + 1]; /* buffers to be read or written */
    char *kbm_wp;                       /* next byte to write to in buffer */
    char *kbm_rp;                       /* next byte to read from buffer */
    int  kbm_bytecount;                 /* count of bytes in buffer */
    int  kbm_flushcount;                /* count of # times mouse input was
                                           flushed */ 
    unsigned long kbm_rbytes;           /* total bytes read from device */
    unsigned long kbm_wbytes;           /* total bytes written to PS2 */
    unsigned long kbm_usb_wbytes;       /* total bytes written to USB */
    unsigned long kbm_rdropped;         /* total read bytes dropped so far */
    unsigned long kbm_wdropped;         /* total write bytes dropped so far */
    int kbm_type;                       /* 0 or 3 for MS, 2 or 3 for KB */
    unsigned long kbm_bad_interrupts;
    unsigned long kbm_errors;
    unsigned long kbm_long;
    unsigned long kbm_toolong;
    unsigned long kbm_flush_mouse;
    int kbm_mouse_pktsize;
};

extern int kbm_control_init(struct v2d_board *board);
extern int kbm_control_init2(struct v2d_board *board);
extern void kbm_control_exit(struct v2d_board *board);
extern int kbm_control_ioctl( struct v2d_board *board, unsigned int cmd, unsigned long data);

extern void flush_kbm_queues(struct v2d_board *board);
extern void flush_mouse_inqueue(struct v2d_board *board);
#endif


#endif /* __KBM_CONTROL_H_INCLUDED__ */
