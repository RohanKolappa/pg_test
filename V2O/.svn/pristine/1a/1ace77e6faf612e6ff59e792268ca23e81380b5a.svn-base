#ifndef __FLOW_CONTROL_H_INCLUDED__
#define __FLOW_CONTROL_H_INCLUDED__

int flow_control_init(struct v2d_board *board);
int flow_control_init2(struct v2d_board *board);
int flow_control_init3(struct v2d_board *board);
void flow_control_exit2(struct v2d_board *board);
void flow_control_exit(struct v2d_board *board);
int flow_control_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data);
void flow_control_addside_video_reset(struct v2d_board *board);
void flow_control_dropside_video_reset(struct v2d_board *board);

#define VIDEO_DEVICE    0
#define AUDIO_DEVICE    3

#define QUANT_SIZE      1024
#define HEAD_ROOM       128

#define SKB_POOL_SIZE       4096
#define SKB_POOL_THRESHOLD  4000

#define VIDEO_READ_QUEUE_LIMIT      16
#define AUDIO_READ_QUEUE_LIMIT      2
#define VIDEO_WRITE_QUEUE_LIMIT     4000
#define AUDIO_WRITE_QUEUE_LIMIT     16 

#define ADD_VIDEO_FIFO_THRES        0x0120
#define ADD_AUDIO_FIFO_THRES        0x0120
#define DROP_VIDEO_FIFO_THRES_HI    0x3e00
#define DROP_VIDEO_FIFO_THRES_LO    0x0120
#define DROP_AUDIO_FIFO_THRES_HI    0x0600
#define DROP_AUDIO_FIFO_THRES_LO    0x0120

#define Y_LOW           0
#define Y_HIGH          1

#define R_LOW_STILL     0
#define R_LOW_BUSY      1
#define R_HIGH          2

#define AVG_SLICERATE_SHIFT   2
#define AVG_SLICERATE_WINDOW   (1 << AVG_SLICERATE_SHIFT)

struct pc_params {
    unsigned long prev_frame_count;
    unsigned long prev_slice_count;
    int Ystate;
    int Rstate;
    unsigned  int slice_rate_array[AVG_SLICERATE_WINDOW];
    unsigned long sra_cnt;
    int rcnt, ycnt, fcnt, scnt;
    unsigned long slice_rate;
};

struct av_device {
    unsigned char       av_type;
    unsigned char       av_flags;

    struct semaphore    av_sem; // Mutex for IO operations

    wait_queue_head_t   av_inq, av_outq;
    struct sk_buff_head av_rq; // Read Queue
    struct sk_buff_head av_wq; // Write Queue

    struct sk_buff      *av_rdma_skb;
    unsigned long       av_rdma_requests;
    unsigned long       av_rdma_busy;
    unsigned long       av_rdma_lock_busy;
    unsigned long       av_rdma_errors;
    unsigned long       av_rdma_grants;
    unsigned long       av_rdma_pending;
    unsigned long       av_rbuf_unavailable;
    unsigned long       av_rbuf_dropped;
    unsigned long       av_read_requests;
    unsigned long       av_read_empty;
    unsigned long       av_read_bytes;
    unsigned long       av_read_count;
    unsigned long       av_read_errors;
    unsigned long       av_read_fifo_empty;
    unsigned long       av_read_fifo_full;
    unsigned long       av_read_fifo_reset;
    unsigned long       av_read_queue_empty;
    unsigned long       av_read_queue_limit;
    unsigned long       av_read_queue_full;
    dma_addr_t          av_rbusaddr;
    unsigned long       av_rseq;

    spinlock_t          av_wlock; /* controls access to curr_skb */
    struct sk_buff      *av_wdma_skb;
    unsigned long       av_wdma_requests;
    unsigned long       av_wdma_busy;
    unsigned long       av_wdma_lock_busy;
    unsigned long       av_wdma_errors;
    unsigned long       av_wdma_grants;
    unsigned long       av_wbuf_unavailable;
    unsigned long       av_wbuf_dropped;
    unsigned long       av_write_requests;
    unsigned long       av_write_empty;
    unsigned long       av_write_bytes;
    unsigned long       av_write_count;
    unsigned long       av_write_errors;
    unsigned long       av_write_fifo_full;
    unsigned long       av_write_fifo_empty;
    unsigned long       av_write_fifo_reset;
    unsigned long       av_write_queue_empty;
    unsigned long       av_write_queue_limit;
    unsigned long       av_write_queue_full;


    dma_addr_t          av_wbusaddr;
    int                 av_dma_read_size;
    struct tasklet_struct av_read_tasklet;
    struct tasklet_struct av_write_tasklet;
    struct v2d_board *  av_board;

    // These are Progressive Compression parameters only relevant for Video
    int av_Xblocks;
    int av_Yhigh, av_Ylow;
    int av_Rhigh, av_Rlow;

    struct pc_params av_pc;

    // These are Statistics only relevant for Video
    unsigned long av_rframe_cnt;
    unsigned long av_rslice_cnt;
    unsigned short av_last_rframe_cnt;
    unsigned short av_last_rslice_cnt;

    unsigned long av_wframe_cnt;
    unsigned long av_wslice_cnt;
    unsigned short av_last_wframe_cnt;
    unsigned short av_last_wslice_cnt;


};
#endif /* __FLOW_CONTROL_H_INCLUDED__ */
