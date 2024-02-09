#ifndef __V4_FPGA_INCLUDED__
#define __V4_FPGA_INCLUDED__

/********************************************
 * Function prototpyes for fpga_v4          *
 ********************************************/
int v4_fpga_hardware_init(struct v2d_board *board);
int v4_fpga_hardware_exit(struct v2d_board *board);
int v4_fpga_board_init(struct v2d_board *board);
int v4_fpga_board_exit(struct v2d_board *board);

#define PLX_DMA_IO_SIZE       1024
#define V4_DMA_READ_VIDEO_ADDR   0x00008000
#define V4_DMA_READ_AUDIO_ADDR   0x00010000
#define V4_DMA_WRITE_VIDEO_ADDR   0x00004000
#define V4_DMA_WRITE_AUDIO_ADDR   0x0000c000

#define PROD_DFF_TX     0xa4
#define PROD_DFF_RX     0xd4

struct plx_io {
    unsigned char       type;
    unsigned char       dir;
    unsigned short      addr_lo;
    unsigned short      addr_hi;
    unsigned int        wait_states;
    unsigned int        burst_mode;
    unsigned int        chan;
    unsigned int        numbytes;
    unsigned int        repeat;
    unsigned char       dat[PLX_DMA_IO_SIZE];
};

/***************************************************
 * PLX access functions                            *
 ***************************************************/
static inline void set_plx_val(struct v2d_board *board,
                               unsigned short reg,  
                               unsigned long val)
{
    if (!board->v_hardware_detected) return;
    *(volatile unsigned long *)(board->v_PlxMemBase + reg) = val;
    mb();
}

static inline unsigned long get_plx_val(struct v2d_board *board, unsigned short reg)
{
    if (!board->v_hardware_detected) return 0;
    return *(volatile unsigned long *)(board->v_PlxMemBase + reg);
}

static inline void set_plx_config_val(struct v2d_board *board, unsigned short reg, unsigned long val) 
{
    if (!board->v_hardware_detected) return;
    pci_write_config_dword(board->v_pci_dev, reg, val);
}

static inline unsigned long get_plx_config_val(struct v2d_board *board, unsigned short reg) 
{
    unsigned long rc;
    if (!board->v_hardware_detected) return 0;
    if (pci_read_config_dword(board->v_pci_dev, reg, (u32 *) &rc) != 0)
        return 0;
    return rc;
}


#endif /* __V4_FPGA_INCLUDED__ */
