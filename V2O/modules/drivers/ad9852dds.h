#ifndef __AD9852DDS_H_INCLUDED__
#define __AD9852DDS_H_INCLUDED__

#ifndef __KERNEL__

/* reference clock multiplication factor */
#define V2_DDSFFACTOR      297.0
#define V4_DDSFFACTOR      300.0

/* multiplication factors to compute AD9852 register values */
#define TWORAISED24         (double)16777216.000000
#define TWORAISED48         (double)281474976710656.000000
#endif

/* AD9852 base offset */
#define AD9852_REGBASE_OFFSET 0x400

/* register offsets (everything is on 4 byte offsets) */
#define DDS_PHASE_ADJ1_01   0x0000
#define DDS_PHASE_ADJ1_02   0x0004
#define DDS_PHASE_ADJ2_01   0x0008
#define DDS_PHASE_ADJ2_02   0x000C
#define DDS_FREQ_WORD1_01   0x0010
#define DDS_FREQ_WORD1_02   0x0014
#define DDS_FREQ_WORD1_03   0x0018
#define DDS_FREQ_WORD1_04   0x001C
#define DDS_FREQ_WORD1_05   0x0020
#define DDS_FREQ_WORD1_06   0x0024
#define DDS_FREQ_WORD2_01   0x0028
#define DDS_FREQ_WORD2_02   0x002C
#define DDS_FREQ_WORD2_03   0x0030
#define DDS_FREQ_WORD2_04   0x0034
#define DDS_FREQ_WORD2_05   0x0038
#define DDS_FREQ_WORD2_06   0x003C
#define DDS_DELTA_FREQ_01   0x0040
#define DDS_DELTA_FREQ_02   0x0044
#define DDS_DELTA_FREQ_03   0x0048
#define DDS_DELTA_FREQ_04   0x004C
#define DDS_DELTA_FREQ_05   0x0050
#define DDS_DELTA_FREQ_06   0x0054
#define DDS_UPDATE_CLK_01   0x0058
#define DDS_UPDATE_CLK_02   0x005C
#define DDS_UPDATE_CLK_03   0x0060
#define DDS_UPDATE_CLK_04   0x0064
#define DDS_RAMP_RATEC_01   0x0068
#define DDS_RAMP_RATEC_02   0x006C
#define DDS_RAMP_RATEC_03   0x0070
#define DDS_CNTL_REG_01     0x0074
#define DDS_CNTL_REG_02     0x0078
#define DDS_CNTL_REG_03     0x007C
#define DDS_CNTL_REG_04     0x0080
#define DDS_OSK_MULT_01     0x0084
#define DDS_OSK_MULT_02     0x0088
#define DDS_RESERVED_01     0x008C
#define DDS_RESERVED_02     0x0090
#define DDS_OSK_RAMP_01     0x0094
#define DDS_CNTL_DAC_01     0x0098
#define DDS_CNTL_DAC_02     0x009C

#ifdef __KERNEL__
static inline void set_ad9852_val(struct v2d_board *board, unsigned short reg,  unsigned char val)
{
    if (!board->v_hardware_detected)
        return;

    if(board->v_type == HARDWARE_V2D_TX)
        return;

    *(volatile unsigned char *)((board->v_ConfigMemBase + AD9852_REGBASE_OFFSET) | reg) = val;
    mb();

}

static inline unsigned char get_ad9852_val(struct v2d_board *board, unsigned short reg)
{
    if (!board->v_hardware_detected)
        return 0;

    if(board->v_type == HARDWARE_V2D_TX)
        return 0;

    return *(volatile unsigned char *) ((board->v_ConfigMemBase + AD9852_REGBASE_OFFSET) | reg) ;
}


/***********************
 * Function prototypes *
 ***********************/
extern int ad9852_init(struct v2d_board *board);
extern void ad9852_exit(struct v2d_board *board);
extern void reset_ad9852(struct v2d_board *board);
extern void tweak_frequency(struct v2d_board *board, 
                            int tweak_hi, 
                            int tweak_lo);
#endif /* __KERNEL__ */
#endif /* __AD9852DDS_H_INCLUDED__ */
