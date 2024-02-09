#ifndef __AD9888VIF_H_INCLUDED__
#define __AD9888VIF_H_INCLUDED__


/* register offsets */

#define VIF_CHIP_REV        0x00
#define VIF_PLL_MSB         0x01
#define VIF_PLL_LSB         0x02
#define VIF_VCO_CPMP        0x03
#define VIF_PHASE_ADJ       0x04
#define VIF_CLAMP_PLACE     0x05
#define VIF_CLAMP_DUR       0x06
#define VIF_HSYNC_OUT       0x07
#define VIF_RED_GAIN        0x08
#define VIF_GRN_GAIN        0x09
#define VIF_BLU_GAIN        0x0A
#define VIF_RED_OFFSET      0x0B
#define VIF_GRN_OFFSET      0x0C
#define VIF_BLU_OFFSET      0x0D
#define VIF_SYNC_CNTL       0x0E
#define VIF_CLAMP_COAST     0x0F
#define VIF_SYNC_ONGRN      0x10
#define VIF_SYNC_SEPTHR     0x11
#define VIF_PRE_C0AST       0x12
#define VIF_POST_C0AST      0x13
#define VIF_SYNC_DETECT     0x14 
#define VIF_MODE_MUX        0x15
#define VIF_TEST1           0x16
#define VIF_TEST2           0x17
#define VIF_TEST3           0x18
#define VIF_TEST4           0x19

/* VIF_SYNC_CNTL Bits */
#define VIFBIT_SYNC_POLARITY_OVERRIDE   0x80
#define VIFBIT_HSYNC_INPUT_POLARITY     0x40
#define VIFBIT_HSYNC_OUTPUT_POLARITY    0x20
#define VIFBIT_HSYNC_OVERRIDE           0x10
#define VIFBIT_HSYNC_SELECT             0x08
#define VIFBIT_VSYNC_INVERT             0x04
#define VIFBIT_VSYNC_OVERRIDE           0x02
#define VIFBIT_VSYNC_SELECT             0x01

/* VIF_CLAMP_COAST Bits */
#define VIFBIT_COAST_SELECT             0x20
#define VIFBIT_IPOLARITY_OVERIDE        0x10
#define VIFBIT_IPOLARITY_SELECT         0x08
#define VIFBIT_NO_PWRDN                 0x01

/* VIF_SYNC_DETECT Bits */
#define VIFBIT_HSYNC_DETECT     0x80
#define VIFBIT_VSYNC_DETECT     0x10
#define VIFBIT_HSYNC_POLARITY   0x20
#define VIFBIT_VSYNC_POLARITY   0x04
#define VIFBIT_SOG_DETECT       0x02

#ifdef __KERNEL__
/* function prototypes */

int ad9888_init(struct v2d_board *board);
int ad9888_read (unsigned char start_addr, unsigned char *data, 
                 int number_of_bytes);
int ad9888_write(unsigned char start_addr, unsigned char *data, 
                 int number_of_bytes);


static inline int set_ad9888_val(int reg, unsigned char value) {
    unsigned char b[1];
    b[0] = value;
    return ad9888_write(reg, b, 1);
}

static inline unsigned char  get_ad9888_val(int reg) {
    unsigned char b[1];
    ad9888_read(reg, b, 1);
    return b[0];
}

#endif /* __KERNEL__ */

#endif /* __AD9888VIF_H_INCLUDED__ */
