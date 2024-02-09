/* 
 * sii7172.c - based on the SiI7189_7172 Starter Kit
 *             SiI-SW-1018_SRC_v00.01_7172 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/videodev2.h>
#include <linux/workqueue.h>
#include <linux/math64.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>
#include <media/platform-netvizxpi.h>

#include "v4l2-chip-ident.h"
#include "v4l2-hdmi.h"
#include "v4l2-dv-bt-timings.h"
#include "netvizxpi.h"
#include "sii7172.h"
#include "../OmniTek_linux.h"
#define   unlocked_ioctl ioctl

MODULE_DESCRIPTION("Netviz XPI sii7172 iTMDS transmitter driver");
MODULE_AUTHOR("Barco IP Video Systems");
MODULE_LICENSE("GPL");

static int debug = 0;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "debug level, default is 0 (no debug messages)");

struct sii7172_state {
    struct v4l2_subdev sd;
    struct sii7172_platform_data *pdata;
    unsigned long long pixelclock;
    int colordepth;
    int tmdsclock;
    int duallink;

    /* i2c clients */
    struct i2c_client *i2c_slave;
};

/*----------------------------------------------------------------------------*/

static inline struct sii7172_state *to_state(struct v4l2_subdev *sd)
{
    return container_of(sd, struct sii7172_state, sd);
}
                                                                                
/* ----------------------------------------------------------------------- */

static s32 sii_smbus_read_byte_data_check(struct i2c_client *client,
                                          u8 command,
                                          bool check)
{
    union i2c_smbus_data data;
    int err;

    /* error if no client or no platform data */
    if (!client)
        return -EIO;

    /* read byte data from device */
    if (!(err = i2c_smbus_xfer(client->adapter, 
                              client->addr, 
                              client->flags,
                              I2C_SMBUS_READ, 
                              command, 
                              I2C_SMBUS_BYTE_DATA, 
                              &data))) {
        return data.byte;
    }
    if (check) {
        v4l_err(client, "SII7172: %s: error %d reading %02x, %02x\n", 
                __func__, err, client->addr, command);
    }
    return -EIO;
}

static s32 sii_smbus_read_byte_data(struct i2c_client *client, 
                                    u8 command)
{
    return sii_smbus_read_byte_data_check(client, command, false);
}

static s32 sii_smbus_write_byte_data(struct i2c_client *client,
                                     u8 command,
                                     u8 value)
{
    union i2c_smbus_data data;
    int err;
    int retries;

    /* error if no client or no platform data */
    if (!client)
        return -EIO;

    /* write byte data to device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) {
        err = i2c_smbus_xfer(client->adapter, 
                             client->addr, 
                             client->flags,
                             I2C_SMBUS_WRITE, 
                             command, 
                             I2C_SMBUS_BYTE_DATA, 
                             &data);
        if (!err)
                break;
    }
    if (err < 0) {
        v4l_err(client, "SII7172: %s: error %d writing %02x, %02x, %02x\n", 
                __func__, err, client->addr, command, value);
    }
    return err;
}

/* ----------------------------------------------------------------------- */

static inline int master_read(struct v4l2_subdev *sd, u8 reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
   
    return sii_smbus_read_byte_data_check(client, reg, 1); 
}

static inline int master_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
   
    return sii_smbus_write_byte_data(client, reg, val);
}

static inline int master_write_and_or(struct v4l2_subdev *sd, 
                                      u8 reg, 
                                      u8 mask,
                                      u8 val)
{
    return master_write(sd, reg, (master_read(sd, reg) & mask) | val);
}

static inline int slave_read(struct v4l2_subdev *sd, u8 reg)
{
    struct sii7172_state *state = to_state(sd);

    if (!state->i2c_slave)
        return -1;
   
    return sii_smbus_read_byte_data_check(state->i2c_slave, reg, 1); 
}

static inline int slave_write(struct v4l2_subdev *sd, u8 reg, u8 val)
{
    struct sii7172_state *state = to_state(sd);
    
    if (!state->i2c_slave)
        return -1;

    return sii_smbus_write_byte_data(state->i2c_slave, reg, val);
}

static inline int slave_write_and_or(struct v4l2_subdev *sd, 
                                     u8 reg, 
                                     u8 mask,
                                     u8 val)
{
    struct sii7172_state *state = to_state(sd);

    if (!state->i2c_slave)
        return -1;

    return slave_write(sd, reg, (slave_read(sd, reg) & mask) | val);
}

/* ----------------------------------------------------------------------- */

static inline bool receiver_connected(struct v4l2_subdev *sd)
{
    /* powered on receiver connected to the transmitter outputs */
    return master_read(sd, SII7172_TX_STAT) & 0x04; 
}

static inline bool in_range(__u64 llval, int lbound, int ubound)
{
    u32 val = div_u64(llval, 1000000);

    if (val >= lbound && val <= ubound)
        return true;  
    else
        return false;
}

/* 
 * See Table 1. PLL and Color Depth Register Settings (SiI-AN-1011-B, 
 * dated 12 Jan 2010). 
 */

static int get_tmdsclk(int colordepth, int *pixperclk)
{
    int rc = TMDSCLK_INVALID;

    switch (colordepth) {
    case 8:
        if (*pixperclk == 1) {
            rc = TMDSCLK_1;
        }
        else {
            *pixperclk = 2;
            rc = TMDSCLK_2;
        }
        break;
    case 10:		
        if (*pixperclk == 1) {
            rc = TMDSCLK_1P25;
        }
        else {
            *pixperclk = 2;
            rc = TMDSCLK_2P5;
        }
        break;
    case 12:
        if (*pixperclk == 1) {
            rc = TMDSCLK_1P5;
        }
        else {
            *pixperclk = 2;
            rc = TMDSCLK_3;
        }
        break;
    case 16:
        if (*pixperclk == 1) {
            rc = TMDSCLK_2;
        }
        else {
            *pixperclk = 2;
            rc = TMDSCLK_4;
        }
        break;
    default:
        break;
    }
    return rc;
}

static void set_pll_and_color_depth(struct v4l2_subdev *sd, 
                                    __u64 pixelclock,
                                    __u32 colordepth,
                                    __u32 tmdsclk, int master)
{
    int (*write_func)(struct v4l2_subdev *, u8, u8) ;
    int (*write_func_and_or)(struct v4l2_subdev *, u8, u8, u8) ;

    v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", __func__,
            (unsigned long long) pixelclock, colordepth);

    if (pixelclock > 225000000) {
        v4l2_err(sd, "%s: unsupported pixelclock: %lld\n",   
                __func__, (unsigned long long) pixelclock);
    }

    if (master) {
        write_func = master_write;
        write_func_and_or = master_write_and_or;
    }
    else {
        write_func = slave_write;
        write_func_and_or = slave_write_and_or;
    }

    switch (colordepth) {
    case 8:
        if (tmdsclk == TMDSCLK_1) {
            if (in_range(pixelclock, 25, 65)) {
                write_func(sd, SII7172_TX_ITPLL, 0x06);
                write_func(sd, SII7172_TX_PLL_CNT, 0x30);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x32);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x00);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x05);
            }
            else if (in_range(pixelclock, 66, 112)) { 
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x31);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x11);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x00);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x05);
            }
            else if (in_range(pixelclock, 113, 225)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x33);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x05);
            }
        }
        else if (tmdsclk == TMDSCLK_2) {
            if (in_range(pixelclock, 25, 65)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x30);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x15);
            }
            else if (in_range(pixelclock, 65, 225)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x31);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x15);
            }
        }
        else {
            v4l2_err(sd, "%s: unsupported tmdsclk %d\n", __func__, tmdsclk); 
        }
        break;
    case 10:		
        if (tmdsclk == TMDSCLK_1P25) {
            if (in_range(pixelclock, 25, 53)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x40);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x30);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x03);
            }
            else if (in_range(pixelclock, 54, 104)) { 
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x41);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x00);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x03);
            }
            else if (in_range(pixelclock, 105, 203)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x43);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x03);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else if (tmdsclk == TMDSCLK_2P5) {
            if (in_range(pixelclock, 25, 53)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x40);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x13);
           }
           else if (in_range(pixelclock, 54, 94)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x41);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x13);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else {
            v4l2_err(sd, "%s: unsupported tmdsclk %d\n", __func__, tmdsclk); 
        }
        break;
    case 12:
        if (tmdsclk == TMDSCLK_1P5) {
            if (in_range(pixelclock, 25, 44)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x50);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x30);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x06);
            }
            else if (in_range(pixelclock, 54, 68)) { 
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x51);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x06);
            }
            else if (in_range(pixelclock, 69, 168)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x53);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x06);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else if (tmdsclk == TMDSCLK_3) {
            if (in_range(pixelclock, 25, 44)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x50);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x30);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x16);
            }
            else if (in_range(pixelclock, 45, 68)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x51);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x16);
            }
            else if (in_range(pixelclock, 69, 168)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x53);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x16);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else {
            v4l2_err(sd, "%s: unsupported tmdsclk %d\n", __func__, tmdsclk); 
        }
        break;
    case 16:
        if (tmdsclk == TMDSCLK_2) {
            if (in_range(pixelclock, 25, 64)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x30);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x10);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x02);
            }
            else if (in_range(pixelclock, 65, 114)) { 
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x31);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x02);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else if (tmdsclk == TMDSCLK_4) {
            if (in_range(pixelclock, 25, 54)) {
                write_func(sd, SII7172_TX_ITPLL, 0x03);
                write_func(sd, SII7172_TX_PLL_CNT, 0x30);
                write_func(sd, SII7172_TX_PLL_COUNT, 0x00);
                write_func(sd, SII7172_TX_TFRCOUNT, 0x10);
                write_func_and_or(sd, SII7172_TX_COLOR, 0xec, 0x12);
            }
            else {
                v4l2_dbg(2, debug, sd, "%s: pixelclock %llu, colordepth %u\n", 
                        __func__, (unsigned long long) pixelclock, colordepth);
            }
        }
        else {
            v4l2_err(sd, "%s: unsupported tmdsclk %d\n", __func__, tmdsclk); 
        }
        break;
    default:
        v4l2_err(sd, "%s: unsupported colordepth %d\n", __func__, colordepth);
        break;
    }
}

static void set_duallink(struct v4l2_subdev *sd, bool duallink)
{
    if (duallink) {
        slave_write(sd, SII7172_TX_DUAL_LINK, 0x30);
        master_write(sd, SII7172_TX_DUAL_LINK, 0x20);
    }
    else {
        slave_write(sd, SII7172_TX_DUAL_LINK, 0x00);
        master_write(sd, SII7172_TX_DUAL_LINK, 0x00);
    }
}

/* 
 * See "Dual Link Firmware Guidelines" (SiI-AN-1011-B, dated 12 Jan 2010). 
 */
static void sii7172_chip_setup(struct v4l2_subdev *sd,
                               unsigned long long pixelclock,
                               int colordepth,
                               int tmdsclock)
{
    struct sii7172_state *state = to_state(sd);

    if (state->pdata == NULL) {
        v4l2_err(sd, "%s: null platform data!\n", __func__);
        return;
    }

    if (state->pdata->flags & SII7172_FLAG_VERBOSE)
        printk("%s entered. pixelclock %llu, colordepth %d, tmdsclock %d\n", 
                __func__, pixelclock, colordepth, tmdsclock);

    state->colordepth = colordepth;
    state->tmdsclock = tmdsclock;
    state->duallink = (pixelclock > PCR_DUALLINK_DVI * 1000000) ? 1 : 0;

    if (state->duallink) 
        state->pixelclock = pixelclock / 2;
    else 
        state->pixelclock = pixelclock;

    /* power up or down slave depending on duallink or not */
    if (!state->duallink) {
        set_gpio_pin(GPIO_DVI_TXS_PDN, "GPIO_DVI_TXS_PDN", 0);
    }
    else {
        set_gpio_pin(GPIO_DVI_TXS_PDN, "GPIO_DVI_TXS_PDN", 1);
    }
    mdelay(1);

    /* toggle IRST pin to reset primary and secondary chips */
    set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 1);
    mdelay(1);
    set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 0);
    mdelay(1);

    master_write(sd, SII7172_TX_STERMOF, 0x18);    
    slave_write(sd, SII7172_TX_STERMOF, 0x08);    

    /* initialize the devices to use I2C register control */
    slave_write(sd, SII7172_TX_STRAP_SELECT, 0x00);    
    master_write(sd, SII7172_TX_STRAP_SELECT, 0x00);    

    /* setup the PLL */
    slave_write(sd, SII7172_TX_FPLLEN, 0x03);
    master_write(sd, SII7172_TX_FPLLEN, 0x03);

    /* set pll and color depth */
    /*for slave */
    set_pll_and_color_depth(sd, state->pixelclock, 
            state->colordepth, 
            state->tmdsclock,0); 
    /* for master */
    set_pll_and_color_depth(sd, state->pixelclock, 
            state->colordepth, 
            state->tmdsclock,1); 

    /* reset the FIFO (DVI mode)*/
    slave_write(sd, SII7172_TX_FIFO_RESET, 0x24);
    master_write(sd, SII7172_TX_FIFO_RESET, 0x24);

    /* setup dual-link mode */ 
    set_duallink(sd, state->duallink);

    /* set clock edge and normal operation mode */ 
    slave_write(sd, SII7172_TX_SYS_CTRL, 0x33);
    master_write(sd, SII7172_TX_SYS_CTRL, 0x33);

    /* clear FIFO reset (DVI mode) */ 
    slave_write(sd, SII7172_TX_FIFO_RESET, 0x20);
    master_write(sd, SII7172_TX_FIFO_RESET, 0x20);

    /* sync bit */
    slave_write(sd, SII7172_TX_DUAL_LINK, 0x38);
    master_write(sd, SII7172_TX_DUAL_LINK, 0x28);

    /* clear MDI interrupt for master (or with 0x09 */
    /* if MDI is generated by monitoring HTPLG)     */
    master_write_and_or(sd, SII7172_TX_STAT, 0xfe, 0x01);

    /* reset primary and secondary chips (?) */
    master_write(sd, SII7172_TX_STERMOF, 0x38);    
    slave_write(sd, SII7172_TX_STERMOF, 0x28);    

    if (!state->duallink) {
        set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 0);
        mdelay(1);
        set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 1);
        mdelay(1);
        set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 0);
    }

    return;
}

/*----------------------------------------------------------------------------*/

static int sii7172_s_dv_timings(struct v4l2_subdev *sd,
                                struct v4l2_dv_timings *timings)
{
    int tmdsclock = TMDSCLK_INVALID, pixperclk = 1;
    struct sii7172_state *state = to_state(sd);

    tmdsclock = get_tmdsclk(state->colordepth, &pixperclk);
    sii7172_chip_setup(sd, timings->bt.pixelclock, state->colordepth, tmdsclock);
    return 0;
}

static void sii7172_inv_register(struct v4l2_subdev *sd)
{
    v4l2_info(sd, "0x000-0x0ff: Main Map\n");
    v4l2_info(sd, "0x100-0x1ff: Slave Map\n");
}

static int sii7172_g_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg) 
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    int rc = 0;

    if (reg == NULL)
        return -EFAULT;
    if (!v4l2_chip_match_i2c_client(client, &reg->match))
        return -EINVAL;
    if (!capable(CAP_SYS_ADMIN))
        return -EPERM;
    switch (reg->reg >> 8) {
    case 0:
        reg->val = master_read(sd, reg->reg & 0xff);
        break;
    case 1:
        reg->val = slave_read(sd, reg->reg & 0xff);
        break;
    default:
        v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
        sii7172_inv_register(sd);
        rc = -EINVAL;
        break;
    }
    return rc;
}

static int sii7172_s_register(struct v4l2_subdev *sd,
                              struct v4l2_dbg_register *reg)
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    int rc = 0;

    if (reg == NULL) {
        return -EFAULT;
    }
    if (!v4l2_chip_match_i2c_client(client, &reg->match)) {
        return -EINVAL;
    }
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM;
    }
    switch (reg->reg >> 8) {
    case 0: {
        master_write(sd, reg->reg & 0xff, reg->val & 0xff);
        break;
    }
    case 1:
        slave_write(sd, reg->reg & 0xff, reg->val & 0xff);
        break;
    default:
        v4l2_info(sd, "Register %03llx not supported\n", reg->reg);
        rc = -EINVAL;
        sii7172_inv_register(sd);
        break;
    }
    return rc;
}

static int sii7172_g_chip_ident(struct v4l2_subdev *sd,
                                struct v4l2_dbg_chip_ident *chp) 
{
    struct i2c_client *client = v4l2_get_subdevdata(sd);
    return v4l2_chip_ident_i2c_client(client, chp, V4L2_IDENT_SII9022, 0);
}

static int sii7172_log_status(struct v4l2_subdev *sd)
{
    u8 reg_val = master_read(sd, SII7172_TX_STAT);

    if (reg_val & 0x04)  
        v4l2_info(sd, "Sink detected. id = 0x%02x\n", reg_val);
    else
        v4l2_info(sd, "Sink not detected. id = 0x%02x\n", reg_val);

    return 0;
}

static long sii7172_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
    int err = 0;

    switch(cmd) {
    case V4L2_G_REGISTER: 
        err = sii7172_g_register(sd, (struct v4l2_dbg_register *) arg);
        break;
    case V4L2_S_REGISTER:
        err = sii7172_s_register(sd, (struct v4l2_dbg_register *) arg);
        break;
    default:
        v4l2_err(sd, "%s: unknown ioctl 0x%08x\n", __func__, cmd);
        err = -EINVAL;
        break;
    }
    return err;
}

/*----------------------------------------------------------------------------*/

/* V4L2 core operation handlers */
static const struct v4l2_subdev_core_ops sii7172_core_ops = {
    .g_chip_ident = sii7172_g_chip_ident,
    .log_status   = sii7172_log_status,
    .ioctl        = sii7172_ioctl,
#ifdef CONFIG_VIDEO_ADV_DEBUG
    .g_register   = sii7172_g_register,
    .s_register   = sii7172_s_register,
#endif
};

/* Specific video subsystem operation handlers */
static const struct v4l2_subdev_video_ops sii7172_video_ops = {
    .s_dv_timings = sii7172_s_dv_timings,
};

/* V4L2 top level operation handlers */
static const struct v4l2_subdev_ops sii7172_ops = {
    .core = &sii7172_core_ops,
    .video = &sii7172_video_ops,
};

/*----------------------------------------------------------------------------*/

static void sii7172_unregister_clients(struct sii7172_state *state)
{
    if (state->i2c_slave)
        i2c_unregister_device(state->i2c_slave);
}

static int sii7172_core_init(struct v4l2_subdev *sd)
{
    struct sii7172_state *state = to_state(sd);
    const struct sii7172_platform_data *pdata = state->pdata;
    
    if (state->pdata == NULL) {
        v4l2_err(sd, "%s: null platform data!\n", __func__);
        return -1;
    }
    
    /* reset the chips (?) */
    v4l2_subdev_notify(sd, SII7172_RESET, NULL);

    /* initialize the chips */
    sii7172_chip_setup(sd, pdata->timings.bt.pixelclock, pdata->colordepth,
		pdata->tmdsclock);
    
    /* set termination control (?) */
    //if (state->i2c_slave)
    //    slave_write(sd, SII7172_TX_STERMOF, 0x38);
    //master_write(sd, SII7172_TX_STERMOF, 0x28);

    return 0;
}

static int sii7172_chip_revision(struct i2c_client *client)
{
    unsigned char vnd_idl, vnd_idh, dev_idl, dev_idh, dev_rev;
    int err = 0;

    vnd_idl = sii_smbus_read_byte_data(client, SII7172_VENDOR_LO);
    vnd_idh = sii_smbus_read_byte_data(client, SII7172_VENDOR_HI);
    dev_idl = sii_smbus_read_byte_data(client, SII7172_DEVICE_LO);
    dev_idh = sii_smbus_read_byte_data(client, SII7172_DEVICE_HI);
    dev_rev = sii_smbus_read_byte_data(client, SII7172_DEVICE_REV);

    if (vnd_idl != 0x01 || vnd_idh != 0x00 || dev_idl != 0x06 || 
		dev_idh != 0x00) {
        v4l_err(client, "not an sii7172 on address 0x%x.\n", client->addr);
        err = -ENODEV;
    }
    else {
        v4l_info(client, "SII7172 revision (%d) detected at address 0x%x.\n", 
            dev_rev, client->addr);
    }
    return err;
}

static int sii7172_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    struct sii7172_state *state;
    struct sii7172_platform_data *pdata;
    struct v4l2_subdev *sd;
    int err = 0;

    v4l_info(client, "%s: probing device at address 0x%x (%s)\n",
        SII7172_DRIVER_NAME, client->addr, client->adapter->name);

    /* Check if the adapter supports the needed features */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
        v4l_err(client, "%s: failed functionaliy check\n", __func__);
        err = -EIO;
        goto err_out;
    }

    /* platform data */ 
    pdata = client->dev.platform_data;
    if (pdata == NULL) {
        v4l_err(client, "no platform data!\n");
        err = -ENODEV;
        goto err_out;
    }

    if ((err = sii7172_chip_revision(client)) < 0)
        goto err_out;

    state = kzalloc(sizeof(struct sii7172_state), GFP_KERNEL);
    if (state == NULL) {
        v4l_err(client, "%s: failed to allocate memory\n", __func__);
        err = -ENOMEM;
        goto err_out;
    }

    sd = &state->sd;
    v4l2_i2c_subdev_init(sd, client, &sii7172_ops);

    state->pdata = pdata;

    state->i2c_slave = i2c_new_dummy(&pdata->slave_adapter, 
            SII7172_SLAVE_I2C_ADDR);

    if (state->i2c_slave == NULL) {
        v4l_err(client, "failed to create i2c slave client\n");
    }

    v4l_info(client, "%s: probing device at address 0x%x (%s)\n",
        SII7172_DRIVER_NAME, state->i2c_slave->addr, 
        pdata->slave_adapter.name);

    /* Check if the slave adapter supports the needed features */
    if (!i2c_check_functionality(&pdata->slave_adapter, I2C_FUNC_SMBUS_BYTE)) {
        v4l_err(client, "%s: slave failed functionaliy check\n", __func__);
        err = -EIO;
    }
    else if (sii7172_chip_revision(state->i2c_slave) < 0)
        state->i2c_slave = NULL;

    err = sii7172_core_init(sd);
    if (err) {
        v4l_err(client, "error initializing SII7172!\n");
        goto err_free_clients;
    }

    v4l2_info(sd, "SII7172 initialized.\n");

    return 0;

err_free_clients:
    sii7172_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

err_out:
    v4l_dbg(1, debug, client, "failed to initialize SII7172. err %d\n", err);

    return err;
}

static int sii7172_remove(struct i2c_client *client)
{
    struct v4l2_subdev *sd = i2c_get_clientdata(client);
    struct sii7172_state *state = to_state(sd);

    v4l_info(client, "%s: removing %s on address 0x%x (%s)\n",
            SII7172_DRIVER_NAME, client->name, client->addr << 1,
            client->adapter->name);
    sii7172_unregister_clients(state);
    v4l2_device_unregister_subdev(sd);
    kfree(state);

    v4l_info(client, "%s removed.\n", __func__);

    return 0;
}

/*----------------------------------------------------------------------------*/

/*
 * i2c device id table
 */
const struct i2c_device_id sii7172_id[] = {
    {SII7172_DRIVER_NAME, 0},
    {},
};
MODULE_DEVICE_TABLE(i2c, sii7172_id);

/*
 * i2c driver data 
 */
struct i2c_driver sii7172_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = SII7172_DRIVER_NAME,
    },
    .probe = sii7172_probe,
    .remove = sii7172_remove,
    .id_table = sii7172_id,
};

/*
 * sii7172_init - register the sii7172 driver 
 */
static int __init sii7172_init(void) 
{
    return i2c_add_driver(&sii7172_driver);
}

/*
 * sii7172_exit - remove driver via i2c interface
 */
static void __exit sii7172_exit(void) 
{
    i2c_del_driver(&sii7172_driver);
}

module_init(sii7172_init);
module_exit(sii7172_exit);
