#ifndef __AD9887VIF_H_INCLUDED__
#define __AD9887VIF_H_INCLUDED__


/* register offsets */

#define VIF_9887_CHIP_REV        0x00
#define VIF_9887_PLL_MSB         0x01
#define VIF_9887_PLL_LSB         0x02
#define VIF_9887_VCO_CPMP        0x03
#define VIF_9887_PHASE_ADJ       0x04
#define VIF_9887_CLAMP_PLACE     0x05
#define VIF_9887_CLAMP_DUR       0x06
#define VIF_9887_HSYNC_OUT       0x07
#define VIF_9887_RED_GAIN        0x08
#define VIF_9887_GRN_GAIN        0x09
#define VIF_9887_BLU_GAIN        0x0A
#define VIF_9887_RED_OFFSET      0x0B
#define VIF_9887_GRN_OFFSET      0x0C
#define VIF_9887_BLU_OFFSET      0x0D
#define VIF_9887_MODE_CNTL1      0x0E
#define VIF_9887_CLAMP_COAST     0x0F
#define VIF_9887_MODE_CNTL2      0x10
#define VIF_9887_SYNC_DETECT     0x11
#define VIF_9887_ACTIVE_INTF     0x12
#define VIF_9887_SYNC_SEPTHR     0x13
#define VIF_9887_CNTL_BITS       0x14
#define VIF_9887_POL_STAT        0x15
#define VIF_9887_CNTL_BITS2      0x16
#define VIF_9887_PRE_C0AST       0x17
#define VIF_9887_POST_C0AST      0x18
#define VIF_9887_TEST1           0x19
#define VIF_9887_TEST2           0x1A
#define VIF_9887_TEST3           0x1B
#define VIF_9887_MODE_MUX        0x1C
#define VIF_9887_REG_1D          0x1D
#define VIF_9887_REG_1E          0x1E
#define VIF_9887_REG_1F          0x1F
#define VIF_9887_REG_20          0x20
#define VIF_9887_REG_21          0x21
#define VIF_9887_REG_22          0x22
#define VIF_9887_REG_23          0x23
#define VIF_9887_REG_24          0x24
#define VIF_9887_REG_25          0x25
#define VIF_9887_REG_26          0x26
#define VIF_9887_REG_27          0x27

/* VIF_9887_SYNC_DETECT Bits */
#define VIFBIT_9887_DIGITAL_INTF_DETECT         0x10
#define VIFBIT_9887_VSYNC_DETECT                0x20
#define VIFBIT_9887_HSYNC_DETECT                0x80
#define VIFBIT_9887_ANALOG_INTF_DETECT          0xe0

/* VIF_9887_ACTIVE_INTF Bits */
#define VIFBIT_9887_NO_PWRDN                    0x01
#define VIFBIT_9887_COAST_VSYNC_INPUT           0x02
#define VIFBIT_9887_VSYNC_SELECT                0x04
#define VIFBIT_9887_VSYNC_OVERRIDE              0x08
#define VIFBIT_9887_HSYNC_SELECT                0x10
#define VIFBIT_9887_HSYNC_OVERRIDE              0x20
#define VIFBIT_9887_DIGITAL_INTF_SELECT         0x40
#define VIFBIT_9887_ACTIVE_INTF_OVERRIDE        0x80

/* VIF_9887_SYNC_CTL Bits */
#define VIFBIT_9887_DIGITAL_INTF_DETECT         0x10
#define VIFBIT_9887_ANALOG_INTF_DETECT          0xe0

/* VIF_9887_CTL_BITS Bits */
#define VIFBIT_9887_HSYNC_POLARITY_OVERRIDE     0x01

/* VIF_9887_CLAMP_COAST Bits */
#define VIFBIT_9887_HSYNC_POLARITY_HIGH         0x80

#ifdef __KERNEL__
/* function prototypes */

int ad9887_init(struct v2d_board *board);
int ad9887_read (unsigned char start_addr, unsigned char *data, 
                 int number_of_bytes);
int ad9887_write(unsigned char start_addr, unsigned char *data, 
                 int number_of_bytes);
void ad9887_dvisetup(void);


static inline int set_ad9887_val(int reg, unsigned char value) {
    unsigned char b[1];
    b[0] = value;
    return ad9887_write(reg, b, 1);
}

static inline unsigned char  get_ad9887_val(int reg) {
    unsigned char b[1];
    ad9887_read(reg, b, 1);
    return b[0];
}

#endif /* __KERNEL__ */

#endif /* __AD9887VIF_H_INCLUDED__ */
