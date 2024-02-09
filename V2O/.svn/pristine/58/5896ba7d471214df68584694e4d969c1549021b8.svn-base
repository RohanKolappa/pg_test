#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/apic.h>
#include <linux/sched.h>
#include <linux/semaphore.h>

#include "fpga.h"
#include "log_control.h"
#include "legacyv2d.h"
#include "v4_fpga.h"
#include "fpga_ioctl.h"
#include "plx9056pciif.h"
#include "flow_control.h"


#define FC_TIMER_FREQ  10

static inline void enqueue_packet(struct sk_buff_head *list, struct sk_buff *pskb)
{
    spin_lock_bh(&list->lock);
    __skb_queue_tail(list, pskb);
    spin_unlock_bh(&list->lock);
}


static inline struct sk_buff *dequeue_packet(struct sk_buff_head *list)
{
    struct sk_buff *pskb;
    spin_lock_bh(&list->lock);
    pskb = __skb_dequeue(list);
    spin_unlock_bh(&list->lock);
    return pskb;
}

static inline void push_packet_to_pool(struct v2d_board *board,
                                       struct sk_buff *pskb)
{
    pskb->len = 0;
    pskb->tail = pskb->data = pskb->head;
    enqueue_packet(&board->v_poolq, pskb);
}

static inline struct sk_buff *pop_packet_from_pool(struct v2d_board *board) {
    return dequeue_packet(&board->v_poolq);
}

// Retrieve device structure from device type
static inline struct av_device * get_av_device(struct v2d_board *board,
                                               int device_type)
{
    if (device_type == AUDIO_DEVICE)
        return (struct av_device *) board->v_audiodev;
    else
        return (struct av_device *) board->v_videodev;
}

// Retrieve device structure from minor number of the device file pointer
static inline struct av_device * retrieve_av_device(struct v2d_board *board,
                                                    struct file *filp) {
    int device_type = MINOR(filp->f_dentry->d_inode->i_rdev) % V2D_CHAR_DEVICES;
    return get_av_device(board, device_type);
}

static ssize_t av_read(struct file *filp, char __user *buff,
                                    size_t count, loff_t *offp);
static ssize_t av_write(struct file *filp, const char __user *buff,
                                    size_t count, loff_t *offp);
static unsigned int av_poll(struct file *filp, poll_table *wait);


static int audio_procfile_read(char *page, char **start, off_t off,
                                    int count, int *eof, void *data) ;
static int video_procfile_read(char *page, char **start, off_t off,
                                    int count, int *eof, void *data) ;

static void v4_request_dma_read_transfer(struct v2d_board *board, int type);
static void v2_request_dma_read_transfer(struct v2d_board *board, int type);

static void v2_dma_read_handler(unsigned long data);

static void v4_request_dma_write_transfer(struct v2d_board *board, int type);
static void v2_request_dma_write_transfer(struct v2d_board *board, int type);

static void v4_dma_read_handler(unsigned long data);
static void dma_read_handler_bh(unsigned long data);
static void v4_dma_write_handler(unsigned long data);
static void v4_dma_write_handler_bh(unsigned long data);

static void v2_drop_fifo_empty(unsigned long data);

static void add_video_fifo_has_something(unsigned long data);
static void add_audio_fifo_has_something(unsigned long data);
static void drop_video_fifo_empty(unsigned long data);
static void drop_audio_fifo_empty(unsigned long data);

/**********************************************************************
 * On video change flush addside video buffers and reset flow control *
 **********************************************************************/
void flow_control_addside_video_reset(struct v2d_board *board) {
    struct sk_buff *pskb=NULL;
    struct av_device *hDevice;

    hDevice = board->v_videodev;
    if (hDevice == NULL)
        return;

    while ((pskb = dequeue_packet(&hDevice->av_rq)) != NULL) {
        push_packet_to_pool(board, pskb);
    }

    // Need to reset our copy of frame/slice counters since FPGA also
    // resets its copy during addside reset
    hDevice->av_last_rframe_cnt = 0;
    hDevice->av_last_rslice_cnt = 0;

    hDevice = board->v_audiodev;
    while ((pskb = dequeue_packet(&hDevice->av_rq)) != NULL) {
        push_packet_to_pool(board, pskb);
    }

    board->v_video_changed = 1;

    return;
}

void flow_control_dropside_video_reset(struct v2d_board *board) {
    struct av_device *hVideo;

    // Need to reset our copy of frame/slice counters since FPGA also
    // resets its copy during dropside reset
    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;
    hVideo->av_last_wframe_cnt = 0;
    hVideo->av_last_wslice_cnt = 0;
}

static void set_fpga_blocksize_registers(struct v2d_board *board) {
    struct av_device *hVideo;

    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;
    set_fpga_val(board, FPGA_AXBLOCKREG, hVideo->av_Xblocks);
    set_fpga_val(board, FPGA_ANOBREG, 32);
    set_fpga_val(board, FPGA_ARSLICEREG, hVideo->av_Rlow);
}

/**
 * END Utilities that need to move to a separate file
 */

/**
 * File Operations
 */
static ssize_t av_read(struct file *filp, char __user *buff,
                               size_t count, loff_t *offp) {
    struct v2d_board *board = (struct v2d_board *) filp->private_data;
    struct av_device *hDevice;
    struct sk_buff *pskb;
    int len = 0;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED) ||
            !access_ok(VERIFY_WRITE, buff, count)) {
        return -EFAULT;
    }

    // Retrieve board and device stuctures
    hDevice = retrieve_av_device(board, filp);
    if (hDevice == NULL)
        return -EFAULT;

    // If video changes send EOF to application
    if ((hDevice->av_type == VIDEO_DEVICE) && board->v_video_changed) {
        board->v_video_changed = 0;
        return 0;
    }

    // Acquire the device semaphore. Allows exclusive use of the IO queues
    if (down_interruptible(&hDevice->av_sem))
        return -ERESTARTSYS;

    hDevice->av_read_requests++;

    // Check if we have data
    do {
        pskb = dequeue_packet(&hDevice->av_rq);

        // If we have a packet to serve, no need to wait
        if (pskb != NULL)  {
            break;
        }

        // Release the device semaphore
        up(&hDevice->av_sem);

        // Request DMA transfer
        if (is_plx_device(board))
            v4_request_dma_read_transfer(board, hDevice->av_type);
        else
            v2_request_dma_read_transfer(board, hDevice->av_type);

        if (filp->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }

        // Wait for data to show up
        if (wait_event_interruptible(hDevice->av_inq,
                                     (skb_queue_len(&hDevice->av_rq) != 0))) {
            return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
        }

        /* Loop, but first reacquire the device semaphore */
        if (down_interruptible(&hDevice->av_sem))
            return -ERESTARTSYS;

    } while (pskb == NULL);


    if (pskb != NULL) {
        // Check if it is a non zero packet
        if (pskb->len <= 0) {
            hDevice->av_read_empty++;
            push_packet_to_pool(board, pskb);
            up(&hDevice->av_sem);
            return -EFAULT;
        }

        // Copy the packet to the user buffer
        len = pskb->len;
        len = min(pskb->len, (unsigned int) count);
        if (copy_to_user(buff, pskb->data, len) != 0) {
            push_packet_to_pool(board, pskb);
            up(&hDevice->av_sem);
            return -EFAULT;
        }

        // Update stats
        hDevice->av_read_bytes += len;
        hDevice->av_read_count++;

        // Recover skb
        push_packet_to_pool(board, pskb);

        // Request next DMA transfer
        if (is_plx_device(board))
            v4_request_dma_read_transfer(board, hDevice->av_type);
        else
            v2_request_dma_read_transfer(board, hDevice->av_type);

    }

    // Release semaphore
    up(&hDevice->av_sem);

    return len;
}

static ssize_t av_write(struct file *filp, const char __user *buff,
                                size_t count, loff_t *offp) {
    struct v2d_board *board = (struct v2d_board *) filp->private_data;
    struct av_device *hDevice;
    struct sk_buff *pskb;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED) ||
            !access_ok(VERIFY_WRITE, buff, count))
        return -EFAULT;

    // Refuse to write more than QUANT_SIZE at time
    if (count > QUANT_SIZE)
        count = QUANT_SIZE;
    if (count == 0) return 0;

    // Retrieve board and device stuctures
    hDevice = retrieve_av_device(board, filp);
    if (hDevice == NULL)
        return -EFAULT;

    // Acquire the device semaphore.
    if (down_interruptible(&hDevice->av_sem)) {
        return -ERESTARTSYS;
    }
    // If the wq is above a threshold flush the write queue,
    // release the buffer to the pool
    // and increment the flush counter
    if (skb_queue_len(&hDevice->av_wq) > hDevice->av_write_queue_limit) {
        hDevice->av_write_queue_full++;
        while((pskb = dequeue_packet(&hDevice->av_wq)) != NULL) {
            push_packet_to_pool(board, pskb);
        }
    }

    // Acquire an skb from the pool
    pskb = pop_packet_from_pool(board);
    if (pskb == NULL) {
        hDevice->av_wbuf_dropped++;
        goto REQUEST_DMA; // Keep flushing the buffer
    }


    // Update stats
    hDevice->av_write_count++;
    hDevice->av_write_bytes += count;

    // Fill up the skbuff with user data.
    skb_put(pskb, count);
    if (copy_from_user(pskb->data, buff, count) == 0) {
        enqueue_packet(&hDevice->av_wq, pskb);
        hDevice->av_write_requests++;
    }
    else {
        push_packet_to_pool(board, pskb);
        hDevice->av_write_errors++;
    }

REQUEST_DMA:
    // Request write DMA transfer
    if (is_plx_device(board))
        v4_request_dma_write_transfer(board, hDevice->av_type);
    else
        v2_request_dma_write_transfer(board, hDevice->av_type);

    up(&hDevice->av_sem);
    return count;
}

static unsigned int av_poll(struct file *filp, poll_table *wait) {
    unsigned int mask=0;
    struct v2d_board *board = (struct v2d_board *) filp->private_data;
    struct av_device *hDevice;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return mask;

    // Retrieve board and device stuctures
    board = (struct v2d_board *)filp->private_data;
    hDevice = retrieve_av_device(board, filp);
    if (hDevice == NULL)
        return -EFAULT;

    // Announce our wait queues
    poll_wait(filp, &hDevice->av_inq, wait);
    poll_wait(filp, &hDevice->av_outq, wait);

    // Request DMA if we have nothing to serve
    if (skb_queue_len(&hDevice->av_rq) != 0) {
        mask |= POLLIN | POLLRDNORM;
    }
    else {
        if (is_plx_device(board))
            v4_request_dma_read_transfer(board, hDevice->av_type);
        else
            v2_request_dma_read_transfer(board, hDevice->av_type);
    }

    // Currently, write is always ready if board is initialized
    // If queues are ever full, we will throw away packets
    mask |= POLLOUT | POLLWRNORM;

    return mask;
}

static int device_procfile_read(char *page, struct av_device *hDevice) {

    int len = 0;

    if (hDevice == NULL)
        return -EFAULT;

    len += sprintf(page + len, "%20s %10s %10s\n",
                   (hDevice->av_type == AUDIO_DEVICE) ? "Audio" : "Video",
                   "Input", "Output");
    len += sprintf(page + len, "%20s %10lu %10lu\n", "DMA Requests",
            hDevice->av_rdma_requests, hDevice->av_wdma_requests);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "DMA Busy",
            hDevice->av_rdma_busy, hDevice->av_wdma_busy);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "DMA Lock Busy",
                   hDevice->av_rdma_lock_busy, hDevice->av_wdma_lock_busy);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "DMA Errors",
            hDevice->av_rdma_errors, hDevice->av_wdma_errors);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "DMA Grants",
            hDevice->av_rdma_grants, hDevice->av_wdma_grants);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FIFO Empty",
            hDevice->av_read_fifo_empty, hDevice->av_write_fifo_empty);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FIFO Full",
            hDevice->av_read_fifo_full, hDevice->av_write_fifo_full);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FOPS Requests",
            hDevice->av_read_requests, hDevice->av_write_requests);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FOPS Success",
            hDevice->av_read_count, hDevice->av_write_count);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FOPS Errors",
            hDevice->av_read_errors, hDevice->av_write_errors);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FOPS Bytes",
            hDevice->av_read_bytes, hDevice->av_write_bytes);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Sequence",
            hDevice->av_rseq, 0UL);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "FIFO reset",
            hDevice->av_read_fifo_reset, hDevice->av_write_fifo_reset);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Queue Length",
                   (unsigned long) skb_queue_len(&hDevice->av_rq),
                   (unsigned long) skb_queue_len(&hDevice->av_wq));
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Queue Full",
            hDevice->av_read_queue_full, hDevice->av_write_queue_full);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Queue Empty",
            hDevice->av_read_queue_empty, hDevice->av_write_queue_empty);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Queue Limit",
            hDevice->av_read_queue_limit, hDevice->av_write_queue_limit);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Dropped",
            hDevice->av_rbuf_dropped, hDevice->av_wbuf_dropped);
    len += sprintf(page + len, "%20s %10lu %10lu\n", "Buffer Pool",
                   (unsigned long) skb_queue_len(&(hDevice->av_board->v_poolq)),
                   0UL);

    return len;

}

static void get_av_read_device_stats(struct fc_av_device_stats *device_stat,
        struct av_device *hDevice)
{
    if (device_stat == NULL || hDevice == NULL)
        return;

    device_stat->dma_requests = hDevice->av_rdma_requests;
    device_stat->dma_busy = hDevice->av_rdma_busy;
    device_stat->dma_lock_busy = hDevice->av_rdma_lock_busy;
    device_stat->dma_errors = hDevice->av_rdma_errors;
    device_stat->dma_grants = hDevice->av_rdma_grants;
    device_stat->fifo_empty = hDevice->av_read_fifo_empty;
    device_stat->fifo_full = hDevice->av_read_fifo_full;
    device_stat->fops_requests = hDevice->av_read_requests;
    device_stat->fops_success = hDevice->av_read_count;
    device_stat->fops_errors = hDevice->av_read_errors;
    device_stat->fops_bytes = hDevice->av_read_bytes;
    device_stat->sequence = hDevice->av_rseq;
    device_stat->queue_length = (unsigned long) skb_queue_len(&hDevice->av_rq);
    device_stat->queue_full = hDevice->av_read_queue_full;
    device_stat->queue_empty = hDevice->av_read_queue_empty;
    device_stat->queue_limit = hDevice->av_read_queue_limit;
    device_stat->fifo_reset = hDevice->av_read_fifo_reset;
    device_stat->buffer_drops = hDevice->av_rbuf_dropped;
    device_stat->buffer_pool =
        (unsigned long) skb_queue_len(&(hDevice->av_board->v_poolq));

}

static void get_av_write_device_stats(struct fc_av_device_stats *device_stat,
        struct av_device *hDevice)
{
    if (device_stat == NULL || hDevice == NULL)
        return;

    device_stat->dma_requests = hDevice->av_wdma_requests;
    device_stat->dma_busy = hDevice->av_wdma_busy;
    device_stat->dma_lock_busy = hDevice->av_wdma_lock_busy;
    device_stat->dma_errors = hDevice->av_wdma_errors;
    device_stat->dma_grants = hDevice->av_wdma_grants;
    device_stat->fifo_full = hDevice->av_write_fifo_full;
    device_stat->fifo_empty = hDevice->av_write_fifo_empty;
    device_stat->fops_requests = hDevice->av_write_requests;
    device_stat->fops_success = hDevice->av_write_count;
    device_stat->fops_errors = hDevice->av_write_errors;
    device_stat->fops_bytes = hDevice->av_write_bytes;
    device_stat->sequence = 0;
    device_stat->queue_length = (unsigned long) skb_queue_len(&hDevice->av_wq);
    device_stat->queue_full = hDevice->av_write_queue_full;
    device_stat->queue_empty = hDevice->av_write_queue_empty;
    device_stat->queue_limit = hDevice->av_write_queue_limit;
    device_stat->fifo_reset = hDevice->av_write_fifo_reset;
    device_stat->buffer_drops = hDevice->av_wbuf_dropped;
    device_stat->buffer_pool = 0;
}

static int audio_procfile_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data) {
    struct v2d_board *board = (struct v2d_board*) data;

    return device_procfile_read(page, board->v_audiodev);
}

static int video_procfile_read(char *page, char **start, off_t off,
                               int count, int *eof, void *data) {
    struct v2d_board *board = (struct v2d_board*) data;

    return device_procfile_read(page, board->v_videodev);
}


static void v4_request_dma_read_transfer(struct v2d_board *board, int type) {
    struct av_device *hDevice;
    unsigned long locaddr;
    int flags;
    struct sk_buff *pskb;
    unsigned long fifolevel=0;

    // Bail if board not initialized
    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    // Get device structure
    hDevice = get_av_device(board, type);
    if (hDevice == NULL)
        return;

    // Bail if DMA is busy
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ) {
        hDevice->av_rdma_pending = 1;
        hDevice->av_rdma_busy++;
        return;
    }

    // Make the rest of this function non-reentrant for this board
    if (spin_trylock_bh(&board->dma_channel[DMA_RCHAN].lock) == 0) {
        hDevice->av_rdma_pending = 1;
        hDevice->av_rdma_lock_busy++;
        return;
    }

    // Check DMA busy flag again (double checked locking)
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ) {
        hDevice->av_rdma_busy++;
        goto EXIT_LABEL;
    }

    // Check queue length
    if (skb_queue_len(&hDevice->av_rq) >= hDevice->av_read_queue_limit) {
        hDevice->av_read_queue_full++;
        goto EXIT_LABEL;
    }

    // Check FIFO level
    if (type == AUDIO_DEVICE) {
        fifolevel = get_fpga_val(board, FPGA_ALFLREG);
        fifolevel = (fifolevel & 0xffff0000) >> 16;
    }
    else {
        fifolevel = get_fpga_val(board, FPGA_ALFLREG);
        fifolevel &= 0x0000ffff;
    }

    if (type == VIDEO_DEVICE && fifolevel < ADD_VIDEO_FIFO_THRES) {
        if (video_available(board)) {
            hDevice->av_read_fifo_empty++;
            hDevice->av_rdma_pending = 1;
            if (!fpga_is_interrupt_enabled(board, INT_20_ADD_VIDEO_FIFO))
                fpga_interrupt_enable(board, INT_20_ADD_VIDEO_FIFO);
            goto EXIT_LABEL;
        }
    }
    else if (type == AUDIO_DEVICE && fifolevel < ADD_AUDIO_FIFO_THRES) {
        hDevice->av_read_fifo_empty++;
        hDevice->av_rdma_pending = 1;
        if (!fpga_is_interrupt_enabled(board, INT_22_ADD_AUDIO_FIFO))
            fpga_interrupt_enable(board, INT_22_ADD_AUDIO_FIFO);
        goto EXIT_LABEL;
    }

    // Sanity check: The dma skb pointer is expected to be NULL now
    // Unless the previous dma request has been handled we should not have
    // gotten this far into the function
    if (hDevice->av_rdma_skb != NULL) {
        hDevice->av_rdma_errors++;
        goto EXIT_LABEL;
    }

    if (skb_queue_len(&hDevice->av_rq) == 0) {
        hDevice->av_read_queue_empty++;
    }

    // Get a new sk buff for DMA from our pool
    pskb = pop_packet_from_pool(board);
    if (pskb == NULL) {
        hDevice->av_rbuf_unavailable++;
        goto EXIT_LABEL;
    }

    // Set the flag marking an audio or video dma in progress
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ) {
        hDevice->av_rdma_errors++;
    }
    flags = (type == AUDIO_DEVICE) ?
        DMA_FLAG_AUDIO_READ : DMA_FLAG_VIDEO_READ;
    board->dma_channel[DMA_RCHAN].flags |= flags;

    // Clear pending flag
    hDevice->av_rdma_pending = 0;

    // Setup the skb for dma
    skb_reserve(pskb, HEAD_ROOM);
    skb_put(pskb, QUANT_SIZE);
    hDevice->av_rdma_skb = pskb;

    // If no video available, schedule bottom half directly to send
    // dummy data
    if (!video_available(board) && type == VIDEO_DEVICE) {
        tasklet_schedule(&hDevice->av_read_tasklet);
        goto EXIT_LABEL;
    }

    // Setup DMA memory
    hDevice->av_rbusaddr = pci_map_single(board->v_pci_dev,
                                          hDevice->av_rdma_skb->data,
                                          QUANT_SIZE, PCI_DMA_FROMDEVICE);

    // Start Open Channel
    if (plx_block_dma_channel_open(board, DMA_RCHAN) != OK) {
        hDevice->av_rdma_errors++;
    }

    // Start DMA transfer
    if (type == AUDIO_DEVICE)
        locaddr = board->v_fpga_base | V4_DMA_READ_AUDIO_ADDR;
    else
        locaddr = board->v_fpga_base | V4_DMA_READ_VIDEO_ADDR;
    if (plx_block_dma_transfer(board,
                               DMA_RCHAN,    /* read channel */
                               (unsigned long) hDevice->av_rbusaddr,
                               locaddr,      /* local bus addr */
                               QUANT_SIZE,   /* transfer size */
                               DMA_DIR_FROM_LOCAL) != OK) {

        board->dma_channel[DMA_RCHAN].flags &= ~flags;
        plx_block_dma_channel_close(board, DMA_RCHAN);
        pci_unmap_single(board->v_pci_dev, hDevice->av_rbusaddr,
                         QUANT_SIZE, PCI_DMA_FROMDEVICE);
        push_packet_to_pool(board, hDevice->av_rdma_skb);
        hDevice->av_rdma_skb = NULL;
        hDevice->av_rdma_errors++;
     }
    else {
        hDevice->av_rdma_requests++;
    }

EXIT_LABEL:
    spin_unlock_bh(&board->dma_channel[DMA_RCHAN].lock);

    return;
}


static void v2_request_dma_read_transfer(struct v2d_board *board, int type) {
    struct av_device *hDevice, *hVideo;
    int flags;
    struct sk_buff *pskb;
    unsigned short addrhigh, addrlow, retries;
    int oldval;

    // Bail if board not initialized
    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    // Get device structure
    hDevice = get_av_device(board, type);
    if (hDevice == NULL)
        return;

    // Bail if DMA is busy
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ) {
        hDevice->av_rdma_busy++;
        return;
    }

    // Make the rest of this function non-reentrant for this board
    if (spin_trylock_bh(&board->dma_channel[DMA_RCHAN].lock) == 0) {
        hDevice->av_rdma_lock_busy++;
        return;
    }

    // Allowing DMA request for audio is tricky on I50 since the fpga
    // always gives us audio if it available else it will give us
    // video. We only have one DMA channel which serves both audio and
    // video. There is no way to ask for "just audio". A regular video
    // DMA request will give us all the audio we need.

    // We use hVideo->av_rdma_skb for both audio and video
    hVideo = board->v_videodev;

    // Sanity check: The dma skb pointer is expected to be NULL now
    // Unless the previous dma request has been handled we should not have
    // gotten this far into the function
    if (hVideo->av_rdma_skb != NULL) {
        hDevice->av_rdma_errors++;
        goto EXIT_LABEL;
    }

    pskb = NULL;
    if (type == AUDIO_DEVICE) {
        struct av_device * hAudio = board->v_audiodev;
        // On Tx, do not allow audio if no video available
        if (is_tx_board(board) && !video_available(board)) {
            hDevice->av_read_errors++;
            goto EXIT_LABEL;
        }

        // Allow Audio only if room in both queues
        if ((skb_queue_len(&hVideo->av_rq) < hVideo->av_read_queue_limit) &&
            (skb_queue_len(&hAudio->av_rq) < hAudio->av_read_queue_limit)) {
            pskb = pop_packet_from_pool(board);
        }
        else {
            hDevice->av_read_queue_full++;
            goto EXIT_LABEL;
        }
    }
    else if (type == VIDEO_DEVICE) {
        // On Rx, a video req. should not be made.
        if (is_rx_board(board)) {
            hDevice->av_read_errors++;
            goto EXIT_LABEL;
        }
        // Check Video queue limit
        if (skb_queue_len(&hVideo->av_rq) < hVideo->av_read_queue_limit) {
            pskb = pop_packet_from_pool(board);
        }
        else {
            hDevice->av_read_queue_full++;
            goto EXIT_LABEL;
        }
    }

    if (pskb == NULL) {
        hDevice->av_rbuf_unavailable++;
        goto EXIT_LABEL;
    }

    if (skb_queue_len(&hDevice->av_rq) == 0) {
        hDevice->av_read_queue_empty++;
    }

    // Set the flag marking an audio or video dma in progress
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ) {
        hDevice->av_rdma_errors++;
    }
    flags = (type == AUDIO_DEVICE) ?
        DMA_FLAG_AUDIO_READ : DMA_FLAG_VIDEO_READ;
    board->dma_channel[DMA_RCHAN].flags |= flags;

    // Setup the skb for dma
    skb_reserve(pskb, HEAD_ROOM);
    skb_put(pskb, QUANT_SIZE);

    // Note: Use of hVideo is intentional
    hVideo->av_rdma_skb = pskb;

    // If no video available, schedule bottom half directly to send
    // dummy data
    if (type == VIDEO_DEVICE && !video_available(board)) {
        tasklet_schedule(&hDevice->av_read_tasklet);
        goto EXIT_LABEL;
    }

    // Setup DMA memory
    hVideo->av_rbusaddr = pci_map_single(board->v_pci_dev,
                                          hVideo->av_rdma_skb->data,
                                          QUANT_SIZE, PCI_DMA_FROMDEVICE);

    addrhigh = (unsigned short)((hVideo->av_rbusaddr & 0xffff0000) >> 16);
    addrlow = (unsigned short)(hVideo->av_rbusaddr & 0xFFFF);

    /* Writing to FPGA DMA address registers is not *
     * reliable for some reason. So we retry.       */
    for (retries = 0; retries < 10 ; retries ++) {
        set_fpga_val(board, FPGA_DMA_ADDRHIGH, addrhigh);
        set_fpga_val(board, FPGA_DMA_ADDRLOW, addrlow);

        if ((get_fpga_val(board, FPGA_DMA_ADDRHIGH) == addrhigh) &&
            (get_fpga_val (board, FPGA_DMA_ADDRLOW) == addrlow))
            break;
    }
    if (retries == 10) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
                 "Failed to write to FPGA DMA address registers!\n");
        // Undo DMA reqest
        board->dma_channel[DMA_RCHAN].flags &= ~flags;
        pci_unmap_single(board->v_pci_dev, hVideo->av_rbusaddr,
                         QUANT_SIZE, PCI_DMA_FROMDEVICE);
        push_packet_to_pool(board, hVideo->av_rdma_skb);
        hVideo->av_rdma_skb = NULL;
        hDevice->av_rdma_errors++;
        goto EXIT_LABEL;
    }

    // Make the DMA request
    oldval = get_fpga_val(board, FPGA_DMA_COMMAND);
    oldval &= ~DMA_REQLEN_MASK;
    oldval |= (DMA_REQLEN_MASK & (QUANT_SIZE >> hVideo->av_dma_read_size));
    oldval &= ~DMA_COMMAND_GO;
    set_fpga_val(board, FPGA_DMA_COMMAND, oldval);
    set_fpga_val(board, FPGA_DMA_COMMAND, oldval | DMA_COMMAND_GO);
    hDevice->av_rdma_requests++;

EXIT_LABEL:
    spin_unlock_bh(&board->dma_channel[DMA_RCHAN].lock);

    return;
}

static void v4_request_dma_write_transfer(struct v2d_board *board, int type) {
    struct av_device *hDevice;
    unsigned long locaddr;
    int flags;
    struct sk_buff *pskb;
    unsigned long fifolevel=0;

    // Bail if board not initialized
    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    // Get device structure
    hDevice = get_av_device(board, type);
    if (hDevice == NULL)
        return;

    // Bail if DMA is busy
    if (board->dma_channel[DMA_WCHAN].flags & DMA_FLAG_WRITE) {
        hDevice->av_wdma_busy++;
        return;
    }

    // Make the rest of this function non-reentrant for this board
    if (spin_trylock_bh(&board->dma_channel[DMA_WCHAN].lock) == 0) {
        hDevice->av_wdma_lock_busy++;
        return;
    }

    // Check DMA busy flag again (double checked locking)
    if (board->dma_channel[DMA_WCHAN].flags & DMA_FLAG_WRITE) {
        hDevice->av_wdma_busy++;
        goto EXIT_LABEL;
    }

    fifolevel = get_fpga_val(board, FPGA_DLFLREG);

    // Don't write video packets if there is audio waiting
    if (type == VIDEO_DEVICE) {
        struct av_device * hAudio = board->v_audiodev;
        unsigned short audiolevel = (fifolevel & 0xffff0000) >> 16;
        if (audiolevel < DROP_AUDIO_FIFO_THRES_LO) {
            if (skb_queue_len(&hAudio->av_wq) > 0)
                goto EXIT_LABEL;
       }
    }

    // Check FIFO level
    if (type == AUDIO_DEVICE) {
        fifolevel = (fifolevel & 0xffff0000) >> 16;
        if (fifolevel > DROP_AUDIO_FIFO_THRES_HI) {
            // Reset dropside audio
            and_in_fpga_val(board, FPGA_DCREG, ~DCREG_AUD_ENABLE);
            or_in_fpga_val(board, FPGA_DAUDCREG, DAUDCREG_RESET);
            hDevice->av_write_fifo_full++;
            fpga_interrupt_enable(board, INT_27_DROP_AUDIO_FIFO);
            goto EXIT_LABEL;
        }
    }
    else {
        fifolevel &= 0x0000ffff;
        if (fifolevel > DROP_VIDEO_FIFO_THRES_HI) {
            hDevice->av_write_fifo_full++;
            fpga_interrupt_enable(board, INT_25_DROP_VIDEO_FIFO);
            goto EXIT_LABEL;
        }
    }

    // Get an skb from the write queue for DMA
    pskb = dequeue_packet(&hDevice->av_wq);
    if (pskb == NULL) {
        // Nothing to do
        goto EXIT_LABEL;
    }


    // Set the flag marking an audio or video dma in progress
    flags = (type == AUDIO_DEVICE) ?
        DMA_FLAG_AUDIO_WRITE : DMA_FLAG_VIDEO_WRITE;
    board->dma_channel[DMA_WCHAN].flags |= flags;

    // Setup DMA memory
    hDevice->av_wdma_skb = pskb;
    hDevice->av_wbusaddr = pci_map_single(board->v_pci_dev, pskb->data,
                                          QUANT_SIZE, PCI_DMA_TODEVICE);

    // Start Open Channel
    if (plx_block_dma_channel_open(board, DMA_WCHAN) != OK) {
        hDevice->av_wdma_errors++;
    }

    // Start DMA transfer
    if (type == AUDIO_DEVICE)
        locaddr = board->v_fpga_base | V4_DMA_WRITE_AUDIO_ADDR;
    else
        locaddr = board->v_fpga_base|V4_DMA_WRITE_VIDEO_ADDR;
    if (plx_block_dma_transfer(board,
                               DMA_WCHAN,    /* write channel */
                               (unsigned long) hDevice->av_wbusaddr,
                               locaddr,      /* local bus addr */
                               QUANT_SIZE,   /* transfer size */
                               DMA_DIR_TO_LOCAL) != OK) {
        // Undo DMA request
        board->dma_channel[DMA_WCHAN].flags &= ~flags;
        plx_block_dma_channel_close(board, DMA_WCHAN);
        pci_unmap_single(board->v_pci_dev, hDevice->av_wbusaddr,
                         QUANT_SIZE, PCI_DMA_TODEVICE);
        push_packet_to_pool(board, pskb);
        hDevice->av_wdma_skb = NULL;
        hDevice->av_wdma_errors++;
     }
    else {
        hDevice->av_wdma_requests++;
    }

EXIT_LABEL:
    spin_unlock_bh(&board->dma_channel[DMA_WCHAN].lock);
    return;
}

static void v2_request_dma_write_transfer(struct v2d_board *board, int type) {
    struct av_device *hDevice;
    struct sk_buff *pskb;

    // Bail if board not initialized
    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    // Get device structure
    hDevice = get_av_device(board, type);
    if (hDevice == NULL)
        return;

    // Make the rest of this function non-reentrant for this board
    if (spin_trylock_bh(&board->dma_channel[DMA_WCHAN].lock) == 0) {
        hDevice->av_wdma_lock_busy++;
        return;
    }


    // Don't write video packets if there is audio waiting
    if (type == VIDEO_DEVICE) {
        struct av_device * hAudio = board->v_audiodev;
        if (skb_queue_len(&hAudio->av_wq) == 1) {
            hDevice->av_wdma_busy++;
            goto EXIT_LABEL;
        }
    }

    // Keep writing to the FPGA till its buffers fill up or we run out
    // of data to write
    hDevice->av_wdma_requests++;
    while (1) {

        if (skb_queue_len(&hDevice->av_wq) == 0) {
            break;
        }

        // Bail if there is no room in FPGA buffers
        if (board->v_hardware_detected) {
            int tmp = QUANT_SIZE / 4;

            if (((type == AUDIO_DEVICE) &&
                 (1024 - (int)get_fpga_val(board,FPGA_DNAUDBFRS)*64 < tmp)) ||
                ((type == VIDEO_DEVICE) &&
                 (16384 - (int)get_fpga_val(board, FPGA_ALFLREG) < (tmp + 16)))) {

                hDevice->av_write_fifo_full++;
                fpga_interrupt_enable(board, INT_2_ADD_FIFO_EMPTY);
                break;
            }
        }

        // Get packet from write queue
        if ((pskb = dequeue_packet(&hDevice->av_wq)) == NULL) {
            break;
        }

        // Write directly to FPGA.
        // No DMA available on I50 for this.
        if (board->v_hardware_detected) {
            memcpy_toio((void *)board->v_AddMemBase, pskb->data, pskb->len);
        }
        else {
            // Dump packet
            hDevice->av_wbuf_dropped++;
        }

        hDevice->av_wdma_grants++;

        // Recover packet
        push_packet_to_pool(board, pskb);
    }

EXIT_LABEL:
    spin_unlock_bh(&board->dma_channel[DMA_WCHAN].lock);
    return;
}

void v2_drop_fifo_empty(unsigned long data) {
    struct v2d_board *board = (struct v2d_board*) data;
    struct av_device *hAudio;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    fpga_interrupt_disable(board, INT_2_ADD_FIFO_EMPTY);

    // Flush out pending writes. Audio first.
    hAudio = board->v_audiodev;
    if (hAudio == NULL)
        return;

    if (skb_queue_len(&hAudio->av_wq) > 0) {
        v2_request_dma_write_transfer(board, AUDIO_DEVICE);
    }
    else {
        v2_request_dma_write_transfer(board, VIDEO_DEVICE);
    }
}

/**
 * Interrupt Handlers
 */
void v2_dma_read_handler(unsigned long data) {
    struct v2d_board *board= (struct v2d_board*) data;
    struct av_device *hDevice, *hVideo, *hAudio;
    unsigned long *pSequence;
    unsigned long *pAltSequence;
    struct av_device *hAltDevice;
    struct sk_buff *pskb=NULL;
    int wlen;
    int len;
    int queue_limit;

    /******************************
     * Check if board initialized *
     ******************************/
    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    /***********************************
     * Check if DMA has been requested *
     ***********************************/
    if (!(board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ)) {
        ((struct av_device *)(board->v_videodev))->av_rdma_errors++;
        return;
    }

    /**********************************************************
     * Bit DMA_DEVTYPE_BIT of FPGA_DMA_WLEN indicates whether *
     * data is audio or video. Lower 12 bits of FPGA_DMA_WLEN *
     * contain the number of 32-bit words transferred.        *
     **********************************************************/
    hVideo = board->v_videodev;
    hAudio = board->v_audiodev;

    if (hVideo == NULL || hAudio == NULL)
        return;

    wlen = get_fpga_val(board, FPGA_DMA_WLEN);
    len = wlen & 0xfff;

    if (wlen & DMA_DEVTYPE_BIT) {
        hDevice = hAudio;
        queue_limit = hAudio->av_read_queue_limit;
    }
    else {
        hDevice = hVideo;
        queue_limit = hVideo->av_read_queue_limit;
    }

    // Note: Use of hVideo instead of hDevice below is intentional
    if (hVideo->av_rdma_skb != NULL) {
        hDevice->av_rdma_grants++;
        pci_unmap_single(board->v_pci_dev, hVideo->av_rbusaddr,
                         QUANT_SIZE, PCI_DMA_FROMDEVICE);
        pskb = hVideo->av_rdma_skb;

        /* Plug in sequence numbers */
        skb_push(pskb, 16);
        if (hDevice->av_type == VIDEO_DEVICE)
            hAltDevice = board->v_audiodev;
        else
            hAltDevice = board->v_videodev;
        pSequence = (unsigned long *) (pskb->data + 8);
        pAltSequence = (unsigned long *) pskb->data;
        *pSequence = hDevice->av_rseq++;
        *pAltSequence = hAltDevice->av_rseq;

        /* Trim down to the written length */
        skb_trim(pskb, 16 + QUANT_SIZE);

        /* Enqueue packet and signal reader */
        if (skb_queue_len(&hDevice->av_rq) < queue_limit) {
            enqueue_packet(&hDevice->av_rq, pskb);
            wake_up_interruptible(&hDevice->av_inq);
        }
        else {
            /* Recover packet */
            hDevice->av_rbuf_dropped++;
            push_packet_to_pool(board, pskb);
        }

        // Done with the dma skb
        hVideo->av_rdma_skb = NULL;
    }

    // What device made the request that caused this interrupt?
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_AUDIO_READ)
        hDevice = hAudio;
    else
        hDevice = hVideo;

    // Clear the DMA busy flag
    board->dma_channel[DMA_RCHAN].flags &= ~DMA_FLAG_READ;

    // Request another DMA transfer for the device that made this request
    v2_request_dma_read_transfer(board, hDevice->av_type);

    return;
}


/***************************************
 * Top half DMA read interrupt handler *
 ***************************************/
void v4_dma_read_handler(unsigned long data) {
    struct v2d_board *board= (struct v2d_board*) data;
    struct av_device *hDevice;
    unsigned long regval;

    /******************************
     * Check if board initialized *
     ******************************/
    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    /***********************
     * Clear DMA interrupt *
     ***********************/
    regval = get_plx_val(board, PCI9056_DMA_COMMAND_STAT);
    if ((regval & (1 << 4)) == 0) {
        ((struct av_device *)(board->v_videodev))->av_rdma_errors++;
        return;
    }
    set_plx_val(board, PCI9056_DMA_COMMAND_STAT, regval | (1 << 3));

    /***********************************
     * Check if DMA has been requested *
     ***********************************/
    if (!(board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ)) {
        ((struct av_device *)(board->v_videodev))->av_rdma_errors++;
        return;
    }

    /**********************
     * Close DMA channel. *
     **********************/
    plx_block_dma_channel_close(board, DMA_RCHAN);

    /*************************************
     * Check whether it's video or audio *
     *************************************/
    if (board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_AUDIO_READ) {
        hDevice = board->v_audiodev;
    }
    else  {
        hDevice = board->v_videodev;
    }
    if (hDevice == NULL)
        return;

    /***************************
     * Retrieve the DMA buffer *
     ***************************/
    if (hDevice->av_rdma_skb != NULL) {
        hDevice->av_rdma_grants++;
        pci_unmap_single(board->v_pci_dev, hDevice->av_rbusaddr, QUANT_SIZE,
                         PCI_DMA_FROMDEVICE);
    }
    else {
        hDevice->av_rdma_errors++;
    }

    /********************************************
     * Schedule a bottom half to handle the skb *
     ********************************************/
    tasklet_schedule(&hDevice->av_read_tasklet);

    // Comment this in if this function registered as bottom half
    //plx_interrupt_enable(board, PLX_INT_TYPE_DMA_0);
    return;
}

// This function serves as the bottom half read handler for PLX boards
// as well as to send out packets when no video available
void dma_read_handler_bh(unsigned long data) {
    struct av_device *hDevice = (struct av_device *) data;
    struct v2d_board *board;
    unsigned long *pSequence;
    unsigned long *pAltSequence;
    struct av_device *hAltDevice;

    if (hDevice == NULL)
        return;

    board = hDevice->av_board;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    if (hDevice->av_rdma_skb != NULL) {
        /* Plug in sequence numbers */
        skb_push(hDevice->av_rdma_skb, 16);
        if (hDevice->av_type == VIDEO_DEVICE)
            hAltDevice = board->v_audiodev;
        else
            hAltDevice = board->v_videodev;
        pSequence = (unsigned long *) (hDevice->av_rdma_skb->data + 8);
        pAltSequence = (unsigned long *) hDevice->av_rdma_skb->data;

        *pSequence = hDevice->av_rseq++;
        *pAltSequence = hAltDevice->av_rseq;

        /* Trim down to the written length */
        skb_trim(hDevice->av_rdma_skb, 16 + QUANT_SIZE);

        /* Put the packet into the input queue for the device */
        enqueue_packet(&hDevice->av_rq, hDevice->av_rdma_skb);
        hDevice->av_rdma_skb = NULL;
        wake_up_interruptible(&hDevice->av_inq);
    }
    else {
        hDevice->av_rdma_errors++;
    }

    /* Clear the DMA busy flag */
    board->dma_channel[DMA_RCHAN].flags &= ~DMA_FLAG_READ;

    /* Issue another dma transfer */
    if (is_plx_device(board)) {
        // Always fire a Audio read
        v4_request_dma_read_transfer(board, AUDIO_DEVICE);
        // If DMA is not busy fire the video read
        if (!(board->dma_channel[DMA_RCHAN].flags & DMA_FLAG_READ))
            v4_request_dma_read_transfer(board, VIDEO_DEVICE);
    }
    else {
        v2_request_dma_read_transfer(board, hDevice->av_type);
    }
}

void v4_dma_write_handler(unsigned long data) {
    struct v2d_board *board= (struct v2d_board*) data;
    struct av_device *hDevice, *hVideo, *hAudio;
    unsigned long regval;

    /******************************
     * Check if board initialized *
     ******************************/
    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    /***********************
     * Clear DMA interrupt *
     ***********************/
    regval = get_plx_val(board, PCI9056_DMA_COMMAND_STAT);
    if ((regval & (1 << 12)) == 0) {
        ((struct av_device *)(board->v_videodev))->av_wdma_errors++;
        return;
    }
    set_plx_val(board, PCI9056_DMA_COMMAND_STAT, regval | (1 << 11));

    /***********************************
     * Check if DMA has been requested *
     ***********************************/
    if (!(board->dma_channel[DMA_WCHAN].flags & DMA_FLAG_WRITE)) {
        ((struct av_device *)(board->v_videodev))->av_wdma_errors++;
        return;
    }

    /**********************
     * Close DMA channel. *
     **********************/
    plx_block_dma_channel_close(board, DMA_WCHAN);

    /*************************************
     * Check whether it's video or audio *
     *************************************/
    hVideo = board->v_videodev;
    hAudio = board->v_audiodev;

    if (board->dma_channel[DMA_WCHAN].flags & DMA_FLAG_AUDIO_WRITE) {
        hDevice = hAudio;
    }
    else  {
        hDevice = hVideo;
    }
    if (hDevice == NULL)
        return;

    /****************************************************
     * If the DMA buffer was set up, process the packet *
     ****************************************************/

    if (hDevice->av_wdma_skb != NULL) {
        hDevice->av_wdma_grants++;
        pci_unmap_single(board->v_pci_dev, hDevice->av_wbusaddr,
                         QUANT_SIZE, PCI_DMA_TODEVICE);

    }
    else {
        hDevice->av_wdma_errors++;
    }

    /********************************************
     * Schedule a bottom half to handle the skb *
     ********************************************/
    tasklet_schedule(&hDevice->av_write_tasklet);

    // Comment this in if this function registered as bottom half
    //plx_interrupt_enable(board, PLX_INT_TYPE_DMA_1);
    return;

}

void v4_dma_write_handler_bh(unsigned long data) {

    struct av_device *hDevice = (struct av_device *) data;
    struct v2d_board *board;

    if (hDevice == NULL)
        return;

    board = hDevice->av_board;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    /* Put the packet into the pool queue for the device */
    if (hDevice->av_wdma_skb != NULL) {
        push_packet_to_pool(board, hDevice->av_wdma_skb);
        hDevice->av_wdma_skb = NULL;
    }
    else {
        hDevice->av_wdma_errors++;
    }

    /* Clear the DMA busy flag */
    board->dma_channel[DMA_WCHAN].flags &= ~DMA_FLAG_WRITE;

    /* Request another write transfer */
    v4_request_dma_write_transfer(board, hDevice->av_type);

}

void add_video_fifo_has_something(unsigned long data) {
    struct v2d_board *board= (struct v2d_board*) data;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    fpga_interrupt_disable(board, INT_20_ADD_VIDEO_FIFO);

    plx_interrupt_enable(board, PLX_INT_TYPE_LOCAL);
    v4_request_dma_read_transfer(board, VIDEO_DEVICE);
}

void add_audio_fifo_has_something(unsigned long data) {
    struct v2d_board *board= (struct v2d_board*) data;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    fpga_interrupt_disable(board, INT_22_ADD_AUDIO_FIFO);

    plx_interrupt_enable(board, PLX_INT_TYPE_LOCAL);
    v4_request_dma_read_transfer(board, AUDIO_DEVICE);

}

void drop_video_fifo_empty (unsigned long data){
    struct v2d_board *board= (struct v2d_board*) data;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    fpga_interrupt_disable(board, INT_25_DROP_VIDEO_FIFO);

    plx_interrupt_enable(board, PLX_INT_TYPE_LOCAL);
    v4_request_dma_write_transfer(board, VIDEO_DEVICE);
}

void drop_audio_fifo_empty (unsigned long data){
    struct v2d_board *board= (struct v2d_board*) data;
    struct av_device *hDevice;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED)) {
        return;
    }

    fpga_interrupt_disable(board, INT_27_DROP_AUDIO_FIFO);

    // Get device structure
    hDevice = get_av_device(board, AUDIO_DEVICE);
    if (hDevice == NULL)
        return;

    // Take dropside audio out of reset
    and_in_fpga_val(board, FPGA_DAUDCREG, ~DAUDCREG_RESET);
    or_in_fpga_val(board, FPGA_DCREG, DCREG_AUD_ENABLE);

    plx_interrupt_enable(board, PLX_INT_TYPE_LOCAL);
    v4_request_dma_write_transfer(board, VIDEO_DEVICE);
}

static void set_refresh_slice_per_frame(struct v2d_board *board, unsigned short val) {
    unsigned short dreg = 0;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    if (!is_plx_device(board) &&
        (get_fpga_val(board, FPGA_IDREG) & 0x00ff) <= 5) {
        dreg = get_fpga_val(board, FPGA_AISLICEREG);
        dreg &= ~0xf000;
        set_fpga_val(board, FPGA_AISLICEREG, (dreg | (val << 12)));
    }
    else {
        set_fpga_val(board, FPGA_ARSLICEREG, val);
    }
}

static void progressive_compression_init(struct v2d_board *board)
{
    struct av_device *hVideo;
    struct pc_params *pc;

    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;

    hVideo->av_Xblocks = 5;
    hVideo->av_Rlow = 1;
    hVideo->av_Rhigh = 15;
    hVideo->av_Ylow = 0;
    hVideo->av_Yhigh = 15;

    set_fpga_blocksize_registers(board);

    pc = &hVideo->av_pc;
    pc->Ystate = Y_LOW;
    pc->Rstate = R_LOW_STILL;
    pc->rcnt = 0;
    pc->ycnt = 0;
    pc->fcnt = 0;
    pc->slice_rate = 100;
    pc->sra_cnt = 0;

}

static void progressive_compression(struct v2d_board *board)
{
    struct av_device *hVideo;
    int i;
    unsigned long frame_diff, slice_diff;
    unsigned short dreg = 0;
    struct pc_params *pc;

    if (board->v_ctl == NULL)
        return;

    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;
    pc = &hVideo->av_pc;

    slice_diff = hVideo->av_rslice_cnt - pc->prev_slice_count;

    /* Compute average slice rate over the last
       AVG_SLICERATE_WINDOW frames */
    if ((frame_diff = hVideo->av_rframe_cnt - pc->prev_frame_count) > 0) {
        pc->slice_rate_array[pc->sra_cnt++%AVG_SLICERATE_WINDOW] =
            slice_diff/frame_diff;
        for (pc->slice_rate = 0, i = 0; i < AVG_SLICERATE_WINDOW; i++)
            pc->slice_rate += pc->slice_rate_array[i];
        pc->slice_rate = pc->slice_rate >> AVG_SLICERATE_SHIFT;

        pc->prev_frame_count = hVideo->av_rframe_cnt;
        pc->prev_slice_count = hVideo->av_rslice_cnt;
    }

    /* There're 4 additional slices when latency optimization on xp200 is on */
    if (is_plx_device(board)) {
        if (get_fpga_val(board, FPGA_ALATCONTROL) & 0x01) {
            /* TODO: slice rate should never be < 4 for this case here! */
            if (pc->slice_rate > 4)
                pc->slice_rate -= 4;
        }
    }

    /* If the last measured slice rate < (Yhigh + Rlow)
       the video is changing from dynamic to static so we
       should use Max Slices as the compression threshold. If
       the average slice rate is > (Yhigh + Rlow) the
       video is changing from static to dynamic so we should
       use Min Slices as the compression threshold. */
    if ((pc->Ystate == Y_LOW) &&
        (pc->slice_rate < (hVideo->av_Yhigh + hVideo->av_Rlow)) &&
        (++pc->ycnt >= 1) &&
        (pc->Rstate != R_HIGH)) {

        pc->Ystate = Y_HIGH;
        pc->ycnt = 0;
        dreg = get_fpga_val(board, FPGA_AISLICEREG);
        dreg &= ~0xfff;

        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
            "Switch Channel %d from Y_LOW to Y_HIGH "
            "(slice rate %ld, AISLICEREG %x)\n",
            board->v_id+1, pc->slice_rate,
            dreg | (hVideo->av_Yhigh & 0xfff));

        set_fpga_val(board, FPGA_AISLICEREG, (dreg | (hVideo->av_Yhigh & 0xfff)));
        /* Set block threshold again since after the video
           moves FPGA seems to lose this setting. */
        set_fpga_val(board, FPGA_AXBLOCKREG, hVideo->av_Xblocks);
    }
    else if ((pc->Ystate == Y_HIGH) &&
             (frame_diff > 0) &&
             (slice_diff > (hVideo->av_Yhigh + (hVideo->av_Rlow))) &&
             (pc->Rstate != R_HIGH)) {

        pc->Ystate = Y_LOW;
        dreg = get_fpga_val(board,FPGA_AISLICEREG);
        dreg &= ~0xfff;

        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
            "Switch Channel %d from Y_HIGH to Y_LOW "
            "(slice rate %ld AISLICEREG 0x%x)\n",
            board->v_id+1, pc->slice_rate,
            dreg | (hVideo->av_Ylow & 0xfff));

        set_fpga_val(board,FPGA_AISLICEREG, (dreg | (hVideo->av_Ylow & 0xfff)));
    }

    /* Number of refresh slices per frame is set to Max Refresh
       for one frame when video changes from dynamic video to
       static video (R_HIGH), otherwise it is set to Min Refresh
       (R_LOW_STILL i.e. static or R_LOW_BUSY i.e. dynamic). */
    if (pc->Rstate == R_LOW_STILL) {
        if ((frame_diff > 0) && (++pc->scnt >= AVG_SLICERATE_WINDOW)) {
            if (pc->slice_rate > (hVideo->av_Yhigh + hVideo->av_Rlow)) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                    "Switch Channel %d from R_LOW_STILL to R_LOW_BUSY "
                    "slice rate %ld (> %d)\n",
                    board->v_id+1, pc->slice_rate,
                    hVideo->av_Yhigh + hVideo->av_Rlow);
                pc->Rstate = R_LOW_BUSY;
                pc->scnt = 0;
            }
        }
    }
    else if (pc->Rstate == R_LOW_BUSY) {
        if ((frame_diff > 0) && (++pc->rcnt >= AVG_SLICERATE_WINDOW)) {
            if (pc->slice_rate < (hVideo->av_Yhigh + hVideo->av_Rlow)) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                    "Switch Channel %d from R_LOW_BUSY to R_HIGH "
                    "slice rate %ld (< %d)\n",
                    board->v_id+1, pc->slice_rate,
                    hVideo->av_Yhigh + hVideo->av_Rlow);
                pc->Rstate = R_HIGH;
                pc->rcnt = 0;
                set_refresh_slice_per_frame(board, hVideo->av_Rhigh);
            }
        }
    }
    else if (pc->Rstate == R_HIGH) {
        if ((pc->fcnt += frame_diff) >
            (board->v_ctl->num_slices_per_frame + hVideo->av_Rhigh - 1) /
            ((hVideo->av_Rhigh > 0) ? hVideo->av_Rhigh : 1)) {

            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                "Switch Channel %d from R_HIGH to R_LOW_STILL fcnt %d (> %d)\n",
                board->v_id+1, pc->fcnt,
                (board->v_ctl->num_slices_per_frame + hVideo->av_Rhigh - 1) /
                ((hVideo->av_Rhigh > 0) ? hVideo->av_Rhigh : 1));

            pc->Rstate = R_LOW_STILL;
            pc->fcnt = 0;

            set_refresh_slice_per_frame(board, hVideo->av_Rlow);
        }
        else if ((frame_diff > 0) &&
                 (pc->slice_rate > (hVideo->av_Rhigh + hVideo->av_Yhigh))) {

             LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Switch Channel %d from R_HIGH to R_LOW_BUSY "
                 "slice rate %ld (> %d)\n",
                 board->v_id+1, pc->slice_rate,
                 hVideo->av_Rhigh + hVideo->av_Yhigh);

            pc->Rstate = R_LOW_BUSY;
            pc->fcnt = 0;

            set_refresh_slice_per_frame(board, hVideo->av_Rlow);
        }
    }
}

static void collect_video_encoder_stats(struct v2d_board *board) {

    unsigned short cnt;
    struct av_device *hVideo;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;

    // Convert frame count to 32 bits
    cnt = get_fpga_val(board, FPGA_AFRMCNTREG);
    if (cnt < hVideo->av_last_rframe_cnt) {
        hVideo->av_rframe_cnt += cnt + (0x10000 - hVideo->av_last_rframe_cnt);
    }
    else {
        hVideo->av_rframe_cnt += cnt - hVideo->av_last_rframe_cnt;
    }
    hVideo->av_last_rframe_cnt = cnt;

    // Convert slice count to 32 bits
    cnt = get_fpga_val(board, FPGA_ASLICECNTREG);
    if(cnt < hVideo->av_last_rslice_cnt) {
        hVideo->av_rslice_cnt += cnt + (0x10000 - hVideo->av_last_rslice_cnt);
    }
    else {
        hVideo->av_rslice_cnt += cnt - hVideo->av_last_rslice_cnt;
    }
    hVideo->av_last_rslice_cnt = cnt;

}

static void collect_video_decoder_stats(struct v2d_board *board) {

    unsigned short cnt;
    struct av_device *hVideo;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED))
        return;

    hVideo = board->v_videodev;
    if (hVideo == NULL)
        return;

    // Convert frame count to 32 bits
    cnt = get_fpga_val(board, FPGA_DFRMCNTREG);
    if (cnt < hVideo->av_last_wframe_cnt) {
        hVideo->av_wframe_cnt += cnt + (0x10000 - hVideo->av_last_wframe_cnt);
    }
    else {
        hVideo->av_wframe_cnt += cnt - hVideo->av_last_wframe_cnt;
    }
    hVideo->av_last_wframe_cnt = cnt;

    // Convert slice count to 32 bits
    cnt = get_fpga_val(board, FPGA_DSLICECNTREG);
    if(cnt < hVideo->av_last_wslice_cnt) {
        hVideo->av_wslice_cnt += cnt + (0x10000 - hVideo->av_last_wslice_cnt);
    }
    else {
        hVideo->av_wslice_cnt += cnt - hVideo->av_last_wslice_cnt;
    }
    hVideo->av_last_wslice_cnt = cnt;
}

static void flow_control_timer_proc( unsigned long data) {
    struct v2d_board *board = (struct v2d_board*) data;
    static int audio_fifo_hi = 0;
    unsigned long fifolevel;

    if (!(board->v_status & FPGA_BOARD_INITIALIZED) ||
            board->v_ctl == NULL)
        return;

    // Setup timer for next round
    mod_timer(&board->v_flow_control_timer,
              jiffies + msecs_to_jiffies(FC_TIMER_FREQ));

    if (is_tx_board(board)) {
        collect_video_encoder_stats(board);
        if (board->v_ctl->progressive_compression)
            progressive_compression(board);
        if (is_plx_device(board))
            v4_request_dma_read_transfer(board, AUDIO_DEVICE);
    }
    if (is_rx_board(board)) {
        collect_video_decoder_stats(board);
#define MAX_FIFO_HI_INTERVAL  10
        // reset drop audio fifo if fifo level is high 10 times in a row
        fifolevel = get_fpga_val(board, FPGA_DLFLREG);
        fifolevel = (fifolevel & 0xffff0000) >> 16;
        if (fifolevel > DROP_AUDIO_FIFO_THRES_HI - 0x120) {
            if (audio_fifo_hi >= MAX_FIFO_HI_INTERVAL) {
                struct av_device *hDevice = board->v_audiodev;
                hDevice->av_write_fifo_reset++;
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                        "Channel %d audio fifo reset \n", board->v_id+1);
                and_in_fpga_val(board, FPGA_DCREG, ~DCREG_AUD_ENABLE);
                or_in_fpga_val(board, FPGA_DCREG, DCREG_AUD_ENABLE);
                audio_fifo_hi = 0;
            }
            else {
                audio_fifo_hi++;
            }
        }
        else {
            audio_fifo_hi = 0;
        }
    }
}

int flow_control_init(struct v2d_board *board) {
    int rc=0;
    struct devlist *v2dcdev;
    struct file_operations *fops;
    struct proc_dir_entry *pdr;
    struct av_device *hVideo, *hAudio;
    struct sk_buff *pskb=NULL;
    int i=0;

    /**
     * Video Device
     */
    v2dcdev = board->v_devlist + VIDEO_DEV_MINOR;
    fops = &v2dcdev->d_fops;
    fops->read = av_read;
    fops->write = av_write;
    fops->poll = av_poll;
    v2dcdev->d_pcdev->ops = fops;

    /**
     * Audio Device
     */
    v2dcdev = board->v_devlist + AUDIO_DEV_MINOR;
    fops = &v2dcdev->d_fops;
    fops->read = av_read;
    fops->write = av_write;
    fops->poll = av_poll;
    v2dcdev->d_pcdev->ops = fops;

    /* Initialize common structures */
    spin_lock_init(&board->dma_channel[DMA_RCHAN].lock);
    spin_lock_init(&board->dma_channel[DMA_WCHAN].lock);

    skb_queue_head_init(&board->v_poolq);
    for (i=0; i < SKB_POOL_SIZE; i++) {
        pskb = dev_alloc_skb(QUANT_SIZE + 2*HEAD_ROOM);
        if (pskb == NULL) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL,
                    "Could not allocate Video Pool\n");
            return -1;
        }
        else {
            push_packet_to_pool(board, pskb);
        }
    }

    // Initialize timer
    init_timer(&board->v_flow_control_timer);
    board->v_flow_control_timer.function = flow_control_timer_proc;
    board->v_flow_control_timer.data = (long)board;
    board->v_flow_control_timer.expires = jiffies + msecs_to_jiffies(FC_TIMER_FREQ);


    /**
     * Initialize the AV devices
     */
    hVideo = kzalloc (sizeof(struct av_device), GFP_KERNEL);
    board->v_videodev = hVideo;
    hVideo->av_type = VIDEO_DEVICE;
    hVideo->av_dma_read_size = 2;
    init_waitqueue_head (&hVideo->av_inq);
    init_waitqueue_head (&hVideo->av_outq);
    sema_init(&hVideo->av_sem, 1);

    skb_queue_head_init(&hVideo->av_rq);
    skb_queue_head_init(&hVideo->av_wq);

    tasklet_init(&hVideo->av_read_tasklet,
                 dma_read_handler_bh, (unsigned long) hVideo);
    tasklet_init(&hVideo->av_write_tasklet,
                 v4_dma_write_handler_bh, (unsigned long) hVideo);
    hVideo->av_read_queue_limit = VIDEO_READ_QUEUE_LIMIT;
    hVideo->av_write_queue_limit = VIDEO_WRITE_QUEUE_LIMIT;

    hVideo->av_board = board; // Back pointer to board structure.

    // Progressive Compression parameters
    progressive_compression_init(board);


    hAudio = kzalloc (sizeof(struct av_device), GFP_KERNEL);
    board->v_audiodev = hAudio;
    hAudio->av_type = AUDIO_DEVICE;
    hAudio->av_dma_read_size = 2;
    init_waitqueue_head (&hAudio->av_inq);
    init_waitqueue_head (&hAudio->av_outq);
    sema_init(&hAudio->av_sem, 1);

    skb_queue_head_init(&hAudio->av_rq);
    skb_queue_head_init(&hAudio->av_wq);


    tasklet_init(&hAudio->av_read_tasklet,
                  dma_read_handler_bh, (unsigned long) hAudio);
    tasklet_init(&hAudio->av_write_tasklet,
                  v4_dma_write_handler_bh, (unsigned long) hAudio);
    hAudio->av_read_queue_limit = AUDIO_READ_QUEUE_LIMIT;
    hAudio->av_write_queue_limit = AUDIO_WRITE_QUEUE_LIMIT;

    hAudio->av_board = board;


    /**********************************
     * Create proc files              *
     **********************************/
    if ((pdr = create_proc_read_entry("video_stats", 0, board->v_pdr,
                video_procfile_read, board)) == NULL) {
        printk("Could not create fpga video stats proc file\n");
        return -1;
    }
    if ((pdr = create_proc_read_entry("audio_stats", 0, board->v_pdr,
                audio_procfile_read, board)) == NULL) {
        printk("Could not create fpga audio stats proc file\n");
        return -1;
    }

    return rc;
}


int flow_control_init2(struct v2d_board *board) {
    int rc=0;
    /// TODO
    add_timer(&board->v_flow_control_timer);
    switch (board->v_pci_dev_type) {
        case V2DI50_DEVICE:
            fpga_register_irq(board, INT_10_LAN_FIFO_HAS_SOMETHING,
                      v2_dma_read_handler);
            fpga_register_disabled_irq(board, INT_2_ADD_FIFO_EMPTY,
                                       v2_drop_fifo_empty);
            break;
        case PLX_DEVICE:
            fpga_register_tophalf_irq(board, INT_30_PLX_DMA_0, v4_dma_read_handler);
            fpga_register_tophalf_irq(board, INT_31_PLX_DMA_1, v4_dma_write_handler);
            fpga_register_disabled_irq(board, INT_20_ADD_VIDEO_FIFO,
                                        add_video_fifo_has_something);
            fpga_register_disabled_irq(board, INT_22_ADD_AUDIO_FIFO,
                                        add_audio_fifo_has_something);
            fpga_register_disabled_irq(board, INT_25_DROP_VIDEO_FIFO,
                                        drop_video_fifo_empty);
            fpga_register_disabled_irq(board, INT_27_DROP_AUDIO_FIFO,
                                        drop_audio_fifo_empty);
            break;
    }
    return rc;
}

void flow_control_exit2(struct v2d_board *board) {
    // Remove timer
    del_timer_sync(&board->v_flow_control_timer);
    return;
}

void flow_control_exit(struct v2d_board *board) {

    struct av_device *hVideo, *hAudio;
    struct sk_buff *pskb;

    hVideo = board->v_videodev;
    hAudio = board->v_audiodev;

    // Unregister interrupt handlers
    switch (board->v_pci_dev_type) {
        case V2DI50_DEVICE:
            fpga_unregister_irq(board, INT_10_LAN_FIFO_HAS_SOMETHING);
            fpga_unregister_irq(board, INT_2_ADD_FIFO_EMPTY);
            break;
        case PLX_DEVICE:
            fpga_unregister_irq(board, INT_31_PLX_DMA_1);
            fpga_unregister_irq(board, INT_30_PLX_DMA_0);
            fpga_unregister_irq(board, INT_20_ADD_VIDEO_FIFO);
            fpga_unregister_irq(board, INT_22_ADD_AUDIO_FIFO);
            fpga_unregister_irq(board, INT_25_DROP_VIDEO_FIFO);
            fpga_unregister_irq(board, INT_27_DROP_AUDIO_FIFO);
            break;
    }


    // Delete resources
    while((pskb = pop_packet_from_pool(board)) != NULL) {
        dev_kfree_skb(pskb);
    }

    while((pskb = dequeue_packet(&hVideo->av_rq)) != NULL) {
        dev_kfree_skb(pskb);
    }

    while((pskb = dequeue_packet(&hVideo->av_wq)) != NULL) {
        dev_kfree_skb(pskb);
    }

    if (hVideo->av_rdma_skb != NULL)
        dev_kfree_skb(hVideo->av_rdma_skb);

    kfree(hVideo);
    board->v_videodev = NULL;

    while((pskb = dequeue_packet(&hAudio->av_rq)) != NULL) {
        dev_kfree_skb(pskb);
    }

    while((pskb = dequeue_packet(&hAudio->av_wq)) != NULL) {
        dev_kfree_skb(pskb);
    }

    if (hAudio->av_rdma_skb != NULL)
        dev_kfree_skb(hAudio->av_rdma_skb);

    kfree(hAudio);
    board->v_audiodev = NULL;

    // Remove proc file entries
    remove_proc_entry("video_stats", board->v_pdr);
    remove_proc_entry("audio_stats", board->v_pdr);

    return;
}

/**
 * IOCTL Handler
 */
int flow_control_ioctl(struct v2d_board *board,
                       unsigned int cmd, unsigned long data) {
    int rc=0;
    struct av_device *hVideo, *hAudio;
    hVideo = board->v_videodev;
    hAudio = board->v_audiodev;

    if (hVideo == NULL || hAudio == NULL)
        return -EFAULT;

    /// TODO
    switch (cmd) {
        case FPGA_IOC_FC_WPARAM_GET:
        {
            struct fc_wparam_io fcwpio;
            struct fc_wparam_io *fcwpiop = (struct fc_wparam_io *) data;

            if ((fcwpiop == NULL) ||
                (copy_from_user(&fcwpio, fcwpiop, sizeof(struct fc_wparam_io))))
                return -EFAULT;

            fcwpio.wframe_cnt = hVideo->av_wframe_cnt;
            fcwpio.wslice_cnt = hVideo->av_wslice_cnt;

            if (copy_to_user((struct fc_wparam_io *)data,
                             &fcwpio, sizeof(struct fc_wparam_io)) != 0)
                return -EFAULT;
        }
        break;

        case FPGA_IOC_FC_PARAM_GET:
        {
            struct fc_param_io fcpio;
            struct fc_param_io *fcpiop = (struct fc_param_io *) data;

            if ((fcpiop == NULL) ||
                (copy_from_user(&fcpio, fcpiop, sizeof(struct fc_param_io))))
                return -EFAULT;

            fcpio.frame_cnt = hVideo->av_rframe_cnt;
            fcpio.slice_cnt = hVideo->av_rslice_cnt;

            if (copy_to_user((struct fc_param_io *)data,
                         &fcpio, sizeof(struct fc_param_io)) != 0)
            return -EFAULT;
        }
            // TODO
            break;
        case FPGA_IOC_FC_RSTAT_GET:
        {
            struct fc_stat_io fcstatio;

            if (((struct fc_param_io *)data) == NULL)
                return -EFAULT;

            memset(&fcstatio.video_stats, 0, sizeof(struct fc_av_device_stats));
            memset(&fcstatio.audio_stats, 0, sizeof(struct fc_av_device_stats));

            get_av_read_device_stats(&fcstatio.video_stats, hVideo);
            get_av_read_device_stats(&fcstatio.audio_stats, hAudio);

            if (copy_to_user((struct fc_stat_io *)data,
                    &fcstatio, sizeof(struct fc_stat_io)) != 0)
                return -EFAULT;
            break;
        }
        case FPGA_IOC_FC_WSTAT_GET:
        {
            struct fc_stat_io fcstatio;

            if (((struct fc_param_io *)data) == NULL)
                return -EFAULT;

            memset(&fcstatio.video_stats, 0, sizeof(struct fc_av_device_stats));
            memset(&fcstatio.audio_stats, 0, sizeof(struct fc_av_device_stats));

            get_av_write_device_stats(&fcstatio.video_stats, hVideo);
            get_av_write_device_stats(&fcstatio.audio_stats, hAudio);

            if (copy_to_user((struct fc_stat_io *)data,
                    &fcstatio, sizeof(struct fc_stat_io)) != 0)
                return -EFAULT;
            break;
        }
        case FPGA_IOC_FC_FLUSH:
            flow_control_addside_video_reset(board);
            break;

        case FPGA_IOC_FC_QUEUE_GET:
        {
            struct fc_queue_limits_io fcqlimitio;

            if (((struct fc_queue_limits_io *)data) == NULL)
                return -EFAULT;

            fcqlimitio.video_read_queue_limit = hVideo->av_read_queue_limit;
            fcqlimitio.audio_read_queue_limit = hAudio->av_read_queue_limit;
            fcqlimitio.video_write_queue_limit = hVideo->av_write_queue_limit;
            fcqlimitio.audio_write_queue_limit = hAudio->av_write_queue_limit;

            if (copy_to_user((struct fc_queue_limits_io *)data,
                    &fcqlimitio, sizeof(struct fc_queue_limits_io)) != 0)
                return -EFAULT;
            break;
        }
        case FPGA_IOC_FC_QUEUE_SET:
        {
            struct fc_queue_limits_io fcqlimitio;
            struct fc_queue_limits_io *fcqlimitiop = (struct fc_queue_limits_io *) data;

            if ((fcqlimitiop == NULL) ||
                (copy_from_user(&fcqlimitio, fcqlimitiop,
                                sizeof(struct fc_queue_limits_io))))
                return -EFAULT;

            if (fcqlimitio.video_read_queue_limit > 0)
                hVideo->av_read_queue_limit = fcqlimitio.video_read_queue_limit;
            if (fcqlimitio.audio_read_queue_limit > 0)
                hAudio->av_read_queue_limit = fcqlimitio.audio_read_queue_limit;
            if (fcqlimitio.video_write_queue_limit > 0)
                hVideo->av_write_queue_limit =
                    fcqlimitio.video_write_queue_limit;
            if (fcqlimitio.audio_write_queue_limit > 0)
                hAudio->av_write_queue_limit =
                    fcqlimitio.audio_write_queue_limit;

            break;
        }
        case FPGA_IOC_PC_PARAM_SET:
            {
                struct pc_params *pc = &hVideo->av_pc;
                struct pc_param_io pcpio;
                struct pc_param_io *pcpiop = (struct pc_param_io *) data;
                unsigned short nobreg;
                unsigned short yrlimit = 15;
                if ((pcpiop == NULL) ||
                    (copy_from_user(&pcpio, pcpiop,
                                    sizeof(struct pc_param_io))))
                    return -EFAULT;

                nobreg = get_fpga_val(board, FPGA_ANOBREG);

                if (board->v_pci_dev_type == PLX_DEVICE) {
                    yrlimit = 255;
                }
                else {
                    if ((get_fpga_val(board, FPGA_IDREG) & 0x00ff) >= 6)
                        yrlimit = 255;
                }

                if (pcpio.setmask & PC_SET_PARAM_XBLOCKS) {
                    if (pcpio.pc_xblocks > nobreg || pcpio.pc_xblocks <= 0)
                        hVideo->av_Xblocks = 5;
                    else
                        hVideo->av_Xblocks = pcpio.pc_xblocks;
                    set_fpga_val(board, FPGA_AXBLOCKREG, hVideo->av_Xblocks);
                }
                if (pcpio.setmask & PC_SET_PARAM_YLOW) {
                    if (pcpio.pc_ylow >= 0 && pcpio.pc_ylow <= yrlimit) {
                        hVideo->av_Ylow = pcpio.pc_ylow;
                    }
                    if (pc->Ystate == Y_LOW) {
                        unsigned short reg;
                        reg = get_fpga_val(board, FPGA_AISLICEREG);
                        set_fpga_val(board, FPGA_AISLICEREG,
                                (reg | (hVideo->av_Ylow & 0xfff)));
                    }
                }
                if (pcpio.setmask & PC_SET_PARAM_YHIGH) {
                    if (pcpio.pc_yhigh >= hVideo->av_Ylow &&
                        pcpio.pc_yhigh <= yrlimit) {
                        hVideo->av_Yhigh = pcpio.pc_yhigh;
                    }
                    if (pc->Ystate == Y_HIGH) {
                        unsigned short reg;
                        reg = get_fpga_val(board, FPGA_AISLICEREG);
                        set_fpga_val(board, FPGA_AISLICEREG,
                            (reg | (hVideo->av_Yhigh & 0xfff)));
                     }
                }
                if (pcpio.setmask & PC_SET_PARAM_RLOW) {
                    if (pcpio.pc_rlow >= 0 && pcpio.pc_rlow <= yrlimit) {
                        hVideo->av_Rlow = pcpio.pc_rlow;
                    }
                    if (pc->Rstate == R_LOW_STILL || pc->Rstate == R_LOW_BUSY) {
                        /* Make FPGA use the new refresh slice per frame */
                        set_fpga_val(board, FPGA_ARSLICEREG, hVideo->av_Rlow);
                    }
                }
                if (pcpio.setmask & PC_SET_PARAM_RHIGH) {
                    if (pcpio.pc_rhigh >= hVideo->av_Rlow &&
                        pcpio.pc_rhigh <= yrlimit) {
                        hVideo->av_Rhigh = pcpio.pc_rhigh;
                    }
                    if (pc->Rstate == R_HIGH) {
                        /* Make FPGA use the new refresh slice per frame */
                        set_fpga_val(board, FPGA_ARSLICEREG, hVideo->av_Rhigh);
                    }
                }
            }
            break;
        case FPGA_IOC_PC_PARAM_GET:
            {
                struct pc_param_io pcpio;
                struct pc_param_io *pcpiop = (struct pc_param_io *) data;

                if ((pcpiop == NULL) ||
                        (copy_from_user(&pcpio, pcpiop,
                                        sizeof(struct pc_param_io))))
                    return -EFAULT;
                pcpio.pc_xblocks = hVideo->av_Xblocks;
                pcpio.pc_ylow = hVideo->av_Ylow;
                pcpio.pc_yhigh = hVideo->av_Yhigh;
                pcpio.pc_rlow = hVideo->av_Rlow;
                pcpio.pc_rhigh = hVideo->av_Rhigh;
                if (copy_to_user((struct pc_param_io *)data,
                    &pcpio, sizeof(struct pc_param_io)) != 0)
                    return -EFAULT;
            }
            break;

        case FPGA_IOC_PROGCMPRS_ENABLE:
            {
                int ival;

                if ((int *) data == NULL)
                    return -EFAULT;
                if (copy_from_user(&ival, (int *)data, sizeof(int)) != 0)
                    return -EFAULT;
                if (board->v_ctl == NULL)
                    return -EFAULT;
                board->v_ctl->progressive_compression = (ival) ? 1 : 0;
            }
            break;
        case FPGA_IOC_GET_LATENCY:
        case FPGA_IOC_SET_FPGA_DROP:
        case FPGA_IOC_QAR_PARAM_GET:
        case FPGA_IOC_QAR_PARAM_SET:
            /// nop
            break;
        default:
            rc = -EINVAL;
            break;
    }
    return rc;
}
