#ifndef __SII7172_H_INCLUDED__
#define __SII7172_H_INCLUDED__

/* 
 * Flags bits for the ADV7604 driver
 */
#define SII7172_FLAG_VERBOSE        0x01

/* 
 * SII7172 I2C switch port 
 */

#define SII7172_I2CSW_PORT          (SII7172_I2C_BUS - CONFIG_MV_I2C_NUM)
#define SII7172_I2CSW_SLAVE_PORT    (SII7172_SLAVE_I2C_BUS - CONFIG_MV_I2C_NUM)

/* 
 * SII7172 I2C registers (see SiI-AN-1011-B dated Jan 12, 2012 and 
 * SiI7172 I2C register map)
 */

#define SII7172_VENDOR_LO		0x00	/* vendor id low byte (0x01) */
#define SII7172_VENDOR_HI		0x01	/* vendor id high byte (0x00) */
#define SII7172_DEVICE_LO		0x02	/* device id low byte (0x06) */
#define SII7172_DEVICE_HI		0x03	/* device id high byte (0x00) */
#define SII7172_DEVICE_REV		0x04	/* device revision (0x00) */
#define SII7172_TX_FREQ_LOW		0x06	/* low freq limit @ 1 p/c mode*/
#define SII7172_TX_FREQ_HIGH	0x07	/* high freq limit @ 1 p/c mode */
#define SII7172_TX_SYS_CTRL		0x08	/* power down and edge select */
#define SII7172_TX_STAT		    0x09	/* monitor detect */
#define SII7172_TX_FPLLEN		0x0c	/* PLL settings */
#define SII7172_TX_STERMOF		0x0e	/* level bias & src termination */
#define SII7172_TX_HDCP_CTRL	0x0f	/* HDCP registers */
#define SII7172_TX_BKSV			0x10	/* 5 bytes */
#define SII7172_TX_AN			0x15	/* random number, 8 bytes */
#define SII7172_TX_AKSV			0x1d	/* 5 bytes */
#define SII7172_TX_SI			0x22	/* 2 bytes */
#define SII7172_TX_COLOR		0x27	/* color depth and LCM enable */
#define SII7172_TX_FIFO_RESET	0x28	/* s/w reset for FIFO & packer */
#define SII7172_TX_CLOCK		0x31	/* set bit2 according to AN-1101 */
#define SII7172_TX_STRAP_SELECT	0x33	/* strap select */ 
#define SII7172_TX_ITPLL		0x35	/* PLL settings */
#define SII7172_TX_PLL_CNT 		0x36    /* PLL settings */ 
#define SII7172_TX_PLL_COUNT	0x38	/* PLL settings */ 
#define SII7172_TX_DUAL_LINK	0x39	/* singla or dual link mode */
#define SII7172_TX_TFRCOUNT		0x3a    /* see SII7172 register map */

/* SII7172_TX_STAT bits */
#define SII7172_TX_STAT_RSEN    0x04    /* (in DC-coupled systems) 1 if */
                                        /* connected to transmitter output */

#define TMDSCLK_INVALID			0	/* invalid */
#define TMDSCLK_1			    1	/* equal */
#define TMDSCLK_1P25			2	/* 1.25 x input clock */
#define TMDSCLK_1P5			    3	/* 1.5 x input clock */
#define TMDSCLK_2			    4	/* 2 x input clock */
#define TMDSCLK_2P5			    5	/* 2.5 x input clock */
#define TMDSCLK_3			    6	/* 3 x input clock */
#define TMDSCLK_4			    7	/* 4 x input clock */

#define DUALLINK_OFF			0	/* single link mode */
#define DUALLINK_ON			    1	/* w/dual_link_sync bit on */

#ifdef __KERNEL__

#ifndef _NETVIZXPI_H_
#include <media/platform-netvizxpi.h>
#endif

struct sii7172_platform_data {
    struct v4l2_dv_timings timings;
    int colordepth;
    int tmdsclock;
    struct i2c_adapter  slave_adapter;
    int flags;
};
#endif
#endif          /* __SII7172_H_INCLUDED__ */
