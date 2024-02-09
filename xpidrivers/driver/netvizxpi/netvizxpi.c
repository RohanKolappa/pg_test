/*
 * netvizxpi.c - Barco Netviz XPi FPGA driver
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c-mux.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/videodev2.h>
#include <linux/tty.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-device.h>
#include <media/platform-netvizxpi.h>


#include "../../include/OmniTekTypes_linux.h"
#include "../../include/OmniTek.h"
#include "../../include/OmniTek_Exports.h"
#include "../OmniTek_linux.h"
#undef ioctl

#include "v4l2-hdmi.h"
#include "v4l2-chip-ident.h"
#include "v4l2-dv-bt-timings.h"
#include "slice_size.h"
#include "adv7604.h"
#include "adv7612.h"
#include "sii7172.h"
#include "netvizxpi.h"
#include "lut_table.h"
#include "coeff_matrix.h"

#define DRIVER_NAME		    "xpi-v4l2"
#define DRIVER_DESCRIPTION	"Barco NGS-D200 XPi-v4l2"
#define BUS_NAME            "i2c"
#define DRIVER_VERSION      1
#define CLASS_NAME		    "xpi-v4l2"
#define DEVICE_NAME		    "xpi-v4l2"

#define ENABLE_EXPANSION_I2CMUX

MODULE_DESCRIPTION("Barco XPi Driver");
MODULE_AUTHOR("Barco SIL");
MODULE_SUPPORTED_DEVICE("Barco XPi");
MODULE_LICENSE("GPL");

static int debug = NETVIZXPI_DEBUG_INFO | NETVIZXPI_DEBUG_AUTOLUT;
static int flags = NETVIZXPI_FLAG_RESETFPGA;

extern const struct v4l2_dv_timings supported_timings[];
extern int supported_timings_max;

static inline struct netvizxpi_state *to_state(struct v4l2_device *vd);

#define xpi_err(state, fmt, arg...) \
    dev_err(&(state)->pdev->dev, fmt, ##arg);
#define xpi_warn(state, fmt, arg...) \
    dev_warn(&(state)->pdev->dev, fmt, ##arg);
#define xpi_info(state, fmt, arg...) \
    dev_info(&(state)->pdev->dev, fmt, ##arg);

static struct adv7604_platform_data netvizxpi_adv7604_platform_data = {
    .connector_hdmi        = 0,
    .hdmi_frun_en          = 0,
    .hdmi_frun_mode        = 0,
    .op_format_sel         = ADV7604_OP_FORMAT_SEL_SDR_444_24,
    .op_ch_sel             = ADV7604_OP_CH_SEL_RGB,
    .analog_hstart         = 0,
    .analog_vstart         = 0,
    .analog_sampling_phase = 0,
    .i2c_avlink            = ADV7604_AVLINK_ADDRESS,
    .i2c_cec               = ADV7604_CEC_ADDRESS,
    .i2c_infoframe         = ADV7604_INFOFRAME_ADDRESS,
    .i2c_esdp              = ADV7604_ESDP_ADDRESS,
    .i2c_dpp               = ADV7604_DPP_ADDRESS,
    .i2c_afe               = ADV7604_AFE_ADDRESS,
    .i2c_repeater          = ADV7604_REPEATER_ADDRESS,
    .i2c_edid              = ADV7604_EDID_ADDRESS,
    .i2c_hdmi		       = ADV7604_HDMI_ADDRESS,
    .i2c_test              = ADV7604_TEST_ADDRESS,
    .i2c_cp                = ADV7604_CP_ADDRESS,
    .i2c_vdp               = ADV7604_VDP_ADDRESS,
    .flags                 = ADV7604_FLAG_SKIP_STDI,
};

static struct adv7612_platform_data netvizxpi_adv7612_platform_data = {
    .inp_color_space       = ADV7612_INP_COLOR_SPACE_AUTO,
    .op_format_sel         = ADV7612_OP_FORMAT_SEL_SDR_444_24,
    .op_ch_sel             = ADV7612_OP_CH_SEL_RGB,
    .op_656_range          = 0,
    .alt_gamma             = 1,
    .rgb_out               = 1,
    .alt_data_sat          = 0,
    .data_blank_en         = 1,
    .avcode_insert_en      = 0,
    .repl_av_code          = 0,
    .op_swap_cbcr          = 0,
    .llc_dll_phase         = 3,
    .i2c_cec               = ADV7612_CEC_ADDRESS,
    .i2c_infoframe         = ADV7612_INFOFRAME_ADDRESS,
    .i2c_repeater          = ADV7612_REPEATER_ADDRESS,
    .i2c_hdmi              = ADV7612_HDMI_ADDRESS,
    .i2c_cp                = ADV7612_CP_ADDRESS,
    .i2c_edid              = ADV7612_EDID_ADDRESS,
    .flags                 = (ADV7612_LLC_PHASE << 4),
};

static struct sii7172_platform_data netvizxpi_sii7172_platform_data = {
    .timings               = V4L2_DV_BT_DMT_1920X1200P60_RB,
    .colordepth            = 8,
    .tmdsclock             = TMDSCLK_1,
    .flags                 = 0,
};

/* ----------------------------------sysfs----------------------------------- */

#ifdef CONFIG_SYSFS

/* sysfs interface definition:
 *
 * Upon loading the driver will create a sysfs directory under
 * /sys/devices/platform/xpi-v4l2
 * 
 * This directory exports the following interfaces.
 *
 * debug         :  debug level for netvizxpi 
 *                  bit 0 - info, bit 1 - trace, bit 2 - pps
 *                  bits 3,4 - vidin, bits 5,6 - vidout, bits 8,9,10 - lut 
 * flags         :  flag bits to control netvizxpi driver operations
 *                  bits 0 and 1 - resetfpga
 * 7604flags     :  flag bits to control adv7604 driver operations
 *                  bit 0 - verbose, 1 - skip STDI lock check
 * 7612flags     :  flag bits to control adv7612 driver operations
 *                  bit 0 - verbose, 4 to 8 - LLC phase 
 * 7172flags     :  flag bits to control sii7172 driver operations
 *                  bit 0 - verbose
 *
 * All attributes are displayed and stored as integers in hex format, i.e.
 *     echo 11 > debug sets debug to 0x11
 */

static ssize_t show_debug(struct device *d, 
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%x\n", debug);
}

static ssize_t store_debug(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;

    tmp = simple_strtoul(buf, NULL, 16);
    debug = tmp;
    return strnlen(buf, count);
}

static DEVICE_ATTR(debug, S_IRUGO|S_IWUSR, show_debug, store_debug);

static ssize_t show_flags(struct device *d, 
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%x\n", flags);
}

static ssize_t store_flags(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;

    tmp = simple_strtoul(buf, NULL, 16);
    flags = tmp;
    return strnlen(buf, count);
}

static DEVICE_ATTR(flags, S_IRUGO|S_IWUSR, show_flags, store_flags);

static ssize_t show_7604flags(struct device *d, 
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%x\n", netvizxpi_adv7604_platform_data.flags);
}

static ssize_t store_7604flags(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;

    tmp = simple_strtoul(buf, NULL, 16);
    netvizxpi_adv7604_platform_data.flags = tmp;
    return strnlen(buf, count);
}

static DEVICE_ATTR(adv7604flags, S_IRUGO|S_IWUSR, show_7604flags, store_7604flags);

static ssize_t show_7612flags(struct device *d, 
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%x\n", netvizxpi_adv7612_platform_data.flags);
}

static ssize_t store_7612flags(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;
    struct v4l2_device *v4l2dev = dev_get_drvdata(d);
    struct netvizxpi_state *state = to_state(v4l2dev);

    tmp = simple_strtoul(buf, NULL, 16);
    if (netvizxpi_adv7612_platform_data.flags != tmp) {
        netvizxpi_adv7612_platform_data.flags = tmp;
        v4l2_subdev_call(state->hdmi_receiver, video, s_routing, 
                ADV7612_MODE_HDMI_GR, 0, 0);
    }
    return strnlen(buf, count);
}

static DEVICE_ATTR(adv7612flags, S_IRUGO|S_IWUSR, show_7612flags, store_7612flags);

static ssize_t show_7172flags(struct device *d, 
        struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "%x\n", netvizxpi_sii7172_platform_data.flags);
}

static ssize_t store_7172flags(struct device *d, 
        struct device_attribute *attr, const char *buf, size_t count)
{
    int tmp;

    tmp = simple_strtoul(buf, NULL, 16);
    netvizxpi_sii7172_platform_data.flags = tmp;
    return strnlen(buf, count);
}

static DEVICE_ATTR(sii7172flags, S_IRUGO|S_IWUSR, show_7172flags, store_7172flags);

static struct attribute *netvizxpi_sysfs_entries[] = {
    &dev_attr_debug.attr,
    &dev_attr_flags.attr,
    &dev_attr_adv7604flags.attr,
    &dev_attr_adv7612flags.attr,
    &dev_attr_sii7172flags.attr,
    NULL
};

static struct attribute_group netvizxpi_attribute_group = {
    .name = NULL,   /* put in device directory */
    .attrs = netvizxpi_sysfs_entries,
};

#endif

/* ----------------------------------------------------------------------- */

static inline unsigned long get_fpga_val(struct netvizxpi_state *state,
                                         unsigned long reg)
{
    unsigned long val = 0;

    if (xpi_get_fpga_val(state->pExt, reg, &val) < 0)
        xpi_err(state, "%s: error read FPGA reg 0x%lx\n", __func__, reg);
    return val;
}

static inline void set_fpga_val(struct netvizxpi_state *state,
                                unsigned long reg,
                                unsigned long val)
{
    if (xpi_set_fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error write 0x%lx to FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void and_in_fpga_val(struct netvizxpi_state *state,
                                   unsigned long reg,
                                   unsigned long val)
{
    if (xpi_and_in_fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: anding 0x%lx to FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void or_in_fpga_val(struct netvizxpi_state *state,
                                  unsigned long reg,
                                  unsigned long val)
{
    if (xpi_or_in_fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error oring 0%lx to FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

/* ----------------------------------------------------------------------- */

static inline unsigned long get_omnitek_val(struct netvizxpi_state *state,
                                            int bar, 
                                            unsigned long reg) 
{
    unsigned long val = 0;

    if (xpi_get_omnitek_val(state->pExt, bar, reg, &val) < 0)
        xpi_err(state, "%s: error read Omnitek bar %d reg 0x%lx\n",
                __func__, bar, reg);
    return val;
}

static inline void set_omnitek_val(struct netvizxpi_state *state,
                                   int bar,
                                   unsigned long reg,
                                   unsigned long val)
{
    if (xpi_set_omnitek_val(state->pExt, bar, reg, val) < 0)
        xpi_err(state, "%s: error write 0x%lx to Omnitek bar %d reg 0x%lx\n",
                __func__, val, bar, reg);
}

static inline void and_in_omnitek_val(struct netvizxpi_state *state,
                                      int bar,
                                      unsigned long reg,
                                      unsigned long val)
{
    if (xpi_and_in_omnitek_val(state->pExt, bar, reg, val) < 0)
        xpi_err(state, "%s: error and 0x%lx to Omnitek bar %d reg 0x%lx\n",
                __func__, val, bar, reg);
}

static inline void or_in_omnitek_val(struct netvizxpi_state *state,
                                     int bar,
                                     unsigned long reg,
                                     unsigned long val)
{
    if (xpi_or_in_omnitek_val(state->pExt, bar, reg, val) < 0)
        xpi_err(state, "%s: error oring 0x%lx to Omnitek bar %d reg 0x%lx\n",
                __func__, val, bar, reg);
}

/* ----------------------------------------------------------------------- */

static inline unsigned long get_s6fpga_val(struct netvizxpi_state *state,
                                           unsigned long reg)
{
    unsigned long val = 0;

    if (xpi_get_s6fpga_val(state->pExt, reg, &val) < 0)
        xpi_err(state, "%s: error read S6 FPGA reg 0x%lx\n", __func__, reg);
    return val;
}

static inline void set_s6fpga_val(struct netvizxpi_state *state,
                                  unsigned long reg,
                                  unsigned long val)
{
    if (xpi_set_s6fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error write 0x%lx to S6 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void and_in_s6fpga_val(struct netvizxpi_state *state,
                                     unsigned long reg,
                                     unsigned long val)
{
    if (xpi_and_in_s6fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error anding 0x%lx to S6 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void or_in_s6fpga_val(struct netvizxpi_state *state,
                                    unsigned long reg,
                                    unsigned long val)
{
    if (xpi_or_in_s6fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error oring 0x%lx to S6 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

/* ------------------------------------------------------------------------- */

static inline unsigned long get_s3fpga_val(struct netvizxpi_state *state,
                                           unsigned long reg)
{
    unsigned long val = 0;

    if (xpi_get_s3fpga_val(state->pExt, reg, &val) < 0)
        xpi_err(state, "%s: error read S3 FPGA reg 0x%lx\n", __func__, reg);
    return val;
}

static inline void set_s3fpga_val(struct netvizxpi_state *state,
                                  unsigned long reg,
                                  unsigned long val)
{
    if (xpi_set_s3fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error write 0x%lx to S3 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void and_in_s3fpga_val(struct netvizxpi_state *state,
                                     unsigned long reg,
                                     unsigned long val)
{
    if (xpi_and_in_s3fpga_val(state->pExt, reg, val) < 0)
        xpi_err(state, "%s: error anding 0x%lx to S3 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

static inline void or_in_s3fpga_val(struct netvizxpi_state *state,
                                    unsigned long reg,
                                    unsigned long val)
{
    if (xpi_or_in_s3fpga_val(state->pExt, reg, val))
        xpi_err(state, "%s: error oring 0x%lx to S3 FPGA reg 0x%lx\n", 
                __func__, val, reg);
}

/* ------------------------------------------------------------------------- */

static inline unsigned long get_paged_mdio_val(struct netvizxpi_state *state, 
                                               int offset,
                                               int phyad,
                                               int page,
                                               unsigned long reg)
{
    int rc;

    /* always read from mdio_offset 0x2800, phyad 1 */
    rc = xpi_paged_mdio_read(state->pExt, XPI_MDIO_OFFSET, offset, phyad, 
            (u16) page, (u16) reg);
    return (unsigned long) rc;
}

static inline void set_paged_mdio_val(struct netvizxpi_state *state,
                                      int offset,
                                      int phyad,
                                      int page,
                                      unsigned long reg,
                                      unsigned long val)
{
    int rc;

    /* always write to mdio_offset 0x2800, phyad 1 */
    rc = xpi_paged_mdio_write(state->pExt, XPI_MDIO_OFFSET, offset, phyad, 
            (u16) page, (u16) reg, (u16) val);

    if (rc < 0)
        xpi_err(state, 
                "%s: error writing 0x%x to MDIO off 0x%x phyad 0x%x " 
                "page 0x%x reg 0x%x\n",
                __func__, (int) val, offset, phyad, (int) page, (int) reg);
}

static inline unsigned long get_mac_val(struct netvizxpi_state *state,
                                        unsigned long reg)
{
    int rc;

    /* always read from mdio_offset 0x2800 */
    rc = xpi_mac_read(state->pExt, XPI_MDIO_OFFSET, (int) reg);
    return (unsigned long) rc;
}

static inline void set_mac_val(struct netvizxpi_state *state,
                               unsigned long reg,
                               unsigned long val)
{
    int rc;

    /* always write to mdio_offset 0x2800 */
    rc = xpi_mac_write(state->pExt, XPI_MDIO_OFFSET, (int) reg, (int) val); 

    if (rc < 0) 
        xpi_err(state, 
                "%s: error writing 0x%x to MAC register 0x%x\n",
                __func__, (int) val, (int) reg);
}

/* ------------------------------------------------------------------------- */

static s32 netvizxpi_smbus_read_byte_check(struct netvizxpi_state *state, 
                                           u8 bus, 
                                           u8 addr,
                                           bool check)
{
    union i2c_smbus_data data;
    int err = -1;

    if (state->i2c_adapters[bus] == NULL)
        return err;

    /* receive byte data from device */
    if (!(err = i2c_smbus_xfer(state->i2c_adapters[bus], 
                               addr,
                               0,
                               I2C_SMBUS_READ,
                               0,
                               I2C_SMBUS_BYTE,
                               &data))) {
        return data.byte;
    }
    if (check)
        xpi_info(state, "error %d reading bus %d %02x\n", err, bus, addr);
    return -EIO;
}

static s32 netvizxpi_smbus_read_byte(struct netvizxpi_state *state, 
                                     u8 bus,
                                     u8 addr)
{
    return netvizxpi_smbus_read_byte_check(state, bus, addr, true);
}

static s32 netvizxpi_smbus_write_byte(struct netvizxpi_state *state, 
                                      u8 bus, 
                                      u8 addr,
                                      u8 value)
{
    union i2c_smbus_data data;
    int err = -1;
    int retries;

    if (state->i2c_adapters[bus] == NULL)
        return err;

    /* send byte data to device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) { 
        err = i2c_smbus_xfer(state->i2c_adapters[bus], 
                             addr, 
                             0, 
                             I2C_SMBUS_WRITE, 
                             value, 
                             I2C_SMBUS_BYTE, 
                             NULL);
        if (!err)
            break;
    }
    if (err < 0)
        xpi_info(state, "error %d writing bus %d %02x, %02x\n", 
                bus, err, addr, value);
    return err;
}

static s32 netvizxpi_smbus_read_byte_data_check(struct netvizxpi_state *state, 
                                                u8 bus, 
                                                u8 addr,
                                                u8 reg, 
                                                bool check)
{
    union i2c_smbus_data data;
    int err = -1;

    if (state->i2c_adapters[bus] == NULL)
        return err;

    /* read byte data from device */
    if (!(err = i2c_smbus_xfer(state->i2c_adapters[bus], 
                               addr,
                               0,
                               I2C_SMBUS_READ,
                               reg,
                               I2C_SMBUS_BYTE_DATA,
                               &data))) {
        return data.byte;
    }
    if (check)
        xpi_info(state, "error %d reading bus %d %02x, %02x\n", 
                err, bus, addr, reg);
    return -EIO;
}

static s32 netvizxpi_smbus_read_byte_data(struct netvizxpi_state *state, 
                                          u8 bus,
                                          u8 addr, 
                                          u8 reg)
{
    return netvizxpi_smbus_read_byte_data_check(state, bus, addr, reg, true);
}

static s32 netvizxpi_smbus_write_byte_data(struct netvizxpi_state *state, 
                                           u8 bus, 
                                           u8 addr,
                                           u8 reg, 
                                           u8 value)
{
    union i2c_smbus_data data;
    int err = -1;
    int retries;

    if (state->i2c_adapters[bus] == NULL)
        return err;

    /* write byte data to device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) { 
        err = i2c_smbus_xfer(state->i2c_adapters[bus], 
                             addr, 
                             0, 
                             I2C_SMBUS_WRITE, 
                             reg, 
                             I2C_SMBUS_BYTE_DATA, 
                             &data);
        if (!err)
            break;
    }
    if (err < 0)
        xpi_info(state, "error %d writing bus 0 %02x, %02x, %02x\n", err, 
                addr, reg, value);
    return err;
}

/* ----------------------------------------------------------------------- */

static inline int monedid_read(struct netvizxpi_state *state, u8 reg)
{
    return netvizxpi_smbus_read_byte_data(state, MONEDID_I2C_BUS, 
            MONEDID_I2C_ADDR, reg);
}

static inline int monedid_write(struct netvizxpi_state *state, u8 reg, u8 val)
{
    return netvizxpi_smbus_write_byte_data(state, MONEDID_I2C_BUS, 
            MONEDID_I2C_ADDR, reg, val);
}

static inline int si5338_read(struct netvizxpi_state *state, u8 reg)
{
    return netvizxpi_smbus_read_byte_data(state, SI5338_I2C_BUS, 
            SI5338_I2C_ADDR, reg);
}

static inline int si5338_write(struct netvizxpi_state *state, u8 reg, u8 val)
{
    return netvizxpi_smbus_write_byte_data(state, SI5338_I2C_BUS, 
            SI5338_I2C_ADDR, reg, val);
}

static inline int lm63_read(struct netvizxpi_state *state, u8 reg)
{
    return netvizxpi_smbus_read_byte_data(state, LM63_I2C_BUS, 
            LM63_I2C_ADDR, reg);
}

static inline int lm63_write(struct netvizxpi_state *state, u8 reg, u8 val)
{
    return netvizxpi_smbus_write_byte_data(state, LM63_I2C_BUS, 
            LM63_I2C_ADDR, reg, val);
}

static inline int ics9fg104_read(struct netvizxpi_state *state, u8 reg)
{
    return netvizxpi_smbus_read_byte_data(state, ICS9FG104_I2C_BUS, 
            ICS9FG104_I2C_ADDR, reg);
}

static inline int ics9fg104_write(struct netvizxpi_state *state, u8 reg, u8 val)
{
    return netvizxpi_smbus_write_byte_data(state, ICS9FG104_I2C_BUS, 
            ICS9FG104_I2C_ADDR, reg, val);
}

static inline int max6650_read(struct netvizxpi_state *state, u8 reg)
{
    return netvizxpi_smbus_read_byte_data(state, MAX6650_I2C_BUS, 
            MAX6650_I2C_ADDR, reg);
}

static inline int max6650_write(struct netvizxpi_state *state, u8 reg, u8 val)
{
    return netvizxpi_smbus_write_byte_data(state, MAX6650_I2C_BUS, 
            MAX6650_I2C_ADDR, reg, val);
}

static inline int expansionswitch_read(struct netvizxpi_state *state)
{
    return netvizxpi_smbus_read_byte(state, EXPANSIONSWITCH_I2C_BUS, 
            EXPANSIONSWITCH_I2C_ADDR);
}

static inline int expansionswitch_write(struct netvizxpi_state *state, u8 val)
{
    return netvizxpi_smbus_write_byte(state, EXPANSIONSWITCH_I2C_BUS, 
            EXPANSIONSWITCH_I2C_ADDR, val);
}

static inline int monedid_read_block(struct netvizxpi_state *state,
                                     unsigned len,
                                     u8 *val)
{
    u8 msgbuf0[1] = { 0 };
    u8 msgbuf1[256];
    struct i2c_msg msg[2] = {
        { MONEDID_I2C_ADDR, 0, 1, msgbuf0 },
        { MONEDID_I2C_ADDR, 0 | I2C_M_RD, len, msgbuf1 }
    };

    if (i2c_transfer(state->i2c_adapters[MONEDID_I2C_BUS], msg, 2) < 0)
        return -EIO;
    else
        memcpy(val, msgbuf1, len);

    return 0;
}

/* ----------------------------------------------------------------------- */

static s32 expansion_smbus_read_byte_data_check(struct netvizxpi_state *state, 
                                                u8 port, 
                                                u8 addr,
                                                u8 reg, 
                                                bool check)
{
    union i2c_smbus_data data;
    int err = -1;

#ifdef ENABLE_EXPANSION_I2CMUX
    if (state->i2c_expansion_adapters[port] == NULL)
        return err;

    /* read byte data from device */
    if (!(err = i2c_smbus_xfer(state->i2c_expansion_adapters[port], 
                               addr,
                               0,
                               I2C_SMBUS_READ,
                               reg,
                               I2C_SMBUS_BYTE_DATA,
                               &data))) {
        return data.byte;
    }
#else
    /* set switch to port for device */
    if (expansionswitch_read(state) != (1 << port)) {
        if (expansionswitch_write(state, (1 << port)))
            return -EIO;
    }

    /* read byte data from device */
    if (!(err = i2c_smbus_xfer(state->i2c_adapters[EXPANSIONSWITCH_I2C_BUS], 
                               addr, 
                               0,
		                       I2C_SMBUS_READ, 
                               reg, 
                               I2C_SMBUS_BYTE_DATA, 
                               &data))) {
        return data.byte;
    }
#endif
    if (check)
        xpi_info(state, "error %d reading expansion port %d %02x, %02x\n", 
                err, port, addr, reg);
    return -EIO;
}

static s32 expansion_smbus_read_byte_data(struct netvizxpi_state *state, 
                                          u8 port,
                                          u8 addr, 
                                          u8 reg)
{
    return expansion_smbus_read_byte_data_check(state, port, addr, reg, true);
}

static s32 expansion_smbus_write_byte_data(struct netvizxpi_state *state, 
                                           u8 port, 
                                           u8 addr,
                                           u8 reg, 
                                           u8 value)
{
    union i2c_smbus_data data;
    int err = -1;
    int retries;

#ifdef ENABLE_EXPANSION_I2CMUX
    if (state->i2c_expansion_adapters[port] == NULL)
        return err;

    /* write byte data to device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) { 
        err = i2c_smbus_xfer(state->i2c_expansion_adapters[port], 
                             addr, 
                             0, 
                             I2C_SMBUS_WRITE, 
                             reg, 
                             I2C_SMBUS_BYTE_DATA, 
                             &data);
        if (!err)
            break;
    }
#else
    /* set switch to port for device */
    if (expansionswitch_read(state) != (1 << port)) {
        if (expansionswitch_write(state, (1 << port)))
            return -EIO;
    }

    /* write byte data to device */
    data.byte = value;
    for (retries = 0; retries < 3; retries++) { 
        err = i2c_smbus_xfer(state->i2c_adapters[EXPANSIONSWITCH_I2C_BUS], 
                             addr, 
                             0, 
                             I2C_SMBUS_WRITE, 
                             reg, 
                             I2C_SMBUS_BYTE_DATA, 
                             &data);
        if (!err)
            break;
    }
#endif
    if (err < 0)
        xpi_info(state, "error %d writing expansion port %d %02x, %02x, %02x\n",
                err, port, addr, reg, value);
    return err;
}

/* ----------------------------------------------------------------------- */

#ifdef ENABLE_EXPANSION_I2CMUX

static struct i2c_board_info i2c_expansion_info = {
    I2C_BOARD_INFO("pca9548", 0x70),
};

static struct i2c_board_info i2c_audiocodec_info = {
    I2C_BOARD_INFO("tlv320aic32x4", 0x18),
};

static int netvizxpi_expansion_init(struct netvizxpi_state *state)
{
    struct i2c_adapter *adap = state->i2c_adapters[EXPANSIONSWITCH_I2C_BUS];
    int ret = -ENODEV;

    if (!i2c_check_functionality(adap, I2C_FUNC_SMBUS_BYTE)) {
        xpi_warn(state, 
            "%s: expansion I2C does not support needed features!\n", __func__);
        return ret;
    }
    state->i2c_expansion_client = 
        i2c_new_device(state->i2c_adapters[EXPANSIONSWITCH_I2C_BUS], 
                &i2c_expansion_info);
    if (state->i2c_expansion_client == NULL) {
        xpi_warn(state, "%s: error instantiating expansion I2C client\n",
                __func__);
        return ret;
    }

    i2c_set_clientdata(state->i2c_expansion_client, state);

    /* 
     * Write the mux register at addr to verify that the mux is in fact
     * present. This also initializes the mux to disconnected state 
     */
    if (expansionswitch_write(state, 0) < 0) {
        xpi_warn(state, "%s failed\n", __func__);
        return ret;
    }

    return 0;
}

static void netvizxpi_expansion_exit(struct netvizxpi_state *state)
{
    if (state->i2c_expansion_client)
        i2c_unregister_device(state->i2c_expansion_client);
    if (state->i2c_audiocodec_client)
        i2c_unregister_device(state->i2c_audiocodec_client);
}

/* ----------------------------------------------------------------------- */

static inline int audiocodec_write(struct netvizxpi_state *state, 
                                   u8 reg, 
                                   u8 val)
{
    return expansion_smbus_write_byte_data(state, TLV320AIC3254_I2C_PORT, 
            TLV320AIC3254_I2C_ADDR, reg, val); 
}

#ifdef INIT_AUDIOCODEC 
static void initialize_audiocodec(struct netvizxpi_state *state)
{
    if (state->i2c_audiocodec_client == NULL)
        return;

    audiocodec_write(state, 0x01, 0x01);    /* init device thru s/w reset */
    audiocodec_write(state, 0x00, 0x00);    /* set to page 0 */
    audiocodec_write(state, 0x12, 0x88);    /* power up NADC with value 8 */
    audiocodec_write(state, 0x13, 0x82);    /* power up MADC with value 2 */
    audiocodec_write(state, 0x14, 0x80);    /* program the OSR of ADC to 128 */
    audiocodec_write(state, 0x0b, 0x90);    /* set NDAC divider to 16 */
    audiocodec_write(state, 0x0c, 0x81);    /* set MDAC divider to 1 */
    audiocodec_write(state, 0x0d, 0x00);    /* program the OSR of DAC to */
    audiocodec_write(state, 0x0e, 0x80);    /*  128 */
    audiocodec_write(state, 0x1d, 0x06);    /* set BDIV_CLKIN mux input to */
                                            /*  ADC_CLK and disable loopback */
    audiocodec_write(state, 0x1e, 0x84);    /* power up N-divider w. value 4 */
    audiocodec_write(state, 0x21, 0x10);    /* config WCLK to come from */
                                            /*  generated ADC_FS */
    audiocodec_write(state, 0x1b, 0xcc);    /* config WCLK and BCLK as output */
                                            /*  wordlen 16 left justified */
    audiocodec_write(state, 0x1c, 0x01);    /* offset 1 bitclk */
    audiocodec_write(state, 0x00, 0x01);    /* disable internal crude AVdd in */
    audiocodec_write(state, 0x01, 0x08);    /*  presense of extAVdd or before */
                                            /*  powering up internal AVdd LDO */
    audiocodec_write(state, 0x02, 0x00);    /* enable master analog power */
    audiocodec_write(state, 0x00, 0x00);    /*  control */
}
#endif

#endif

/*----------------------------------------------------------------------- */

static inline int is_tx(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_TX) ? 1 : 0;
}

static inline int driver_enabled(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_DRIVER_ENABLE) ? 1 : 0;
}

static inline int video_available(struct netvizxpi_state *state) 
{
    if (state->current_interface & INTERFACE_ADDSIDE_VIDEO_UNAVAIL)
        return 0;
    else
        return (state->current_addside_lut != NULL) ? 1 : 0;
}

static inline int basic_enabled(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_VIDPROC_BASIC) ? 1 : 0;
}

static inline int duallink_enabled(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_VIDPROC_DUALLINK) ? 1 : 0;
}

static inline int stereo_enabled(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_VIDPROC_STEREO) ? 1 : 0;
}

static inline int ppscontrol_enabled(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_VIDPROC_PPSCONTROL) ? 1 : 0;
}

static inline int not_licensed(struct netvizxpi_state *state)
{
    return (state->flags & FLAG_XPI_LICENSE_VIOLATION) ? 1 : 0;
}

static inline void set_license_violation(struct netvizxpi_state *state)
{
    state->flags |= FLAG_XPI_LICENSE_VIOLATION;
}

static inline void clear_license_violation(struct netvizxpi_state *state)
{
    state->flags &= ~FLAG_XPI_LICENSE_VIOLATION;
}

static inline int input_digital(struct netvizxpi_state *state)
{
    return (state->current_interface & INTERFACE_DIGITAL) ? 1 : 0;
}

static inline int input_analog(struct netvizxpi_state *state)
{
    return (state->current_interface & INTERFACE_HV) ? 1 : 0;
}

static inline void set_input_digital(struct netvizxpi_state *state)
{
    if (!input_digital(state)) {
        state->current_interface &= ~INTERFACE_SRCMASK;
        state->current_interface |= INTERFACE_DIGITAL;
    }
}

static inline void set_input_analog(struct netvizxpi_state *state)
{
    if (!input_analog(state)) {
        state->current_interface &= ~INTERFACE_SRCMASK;
        state->current_interface |= INTERFACE_HV;
    }
}

static inline void set_input_none(struct netvizxpi_state *state)
{
    state->current_interface &= ~INTERFACE_SRCMASK;
}

static inline int interface_output_enable(struct netvizxpi_state *state)
{
    return (state->current_interface & INTERFACE_VIDEO_OUTPUT_ENABLE) ? 1 : 0;
}

static inline int interface_duallink(struct netvizxpi_state *state)
{
    return (state->current_interface & INTERFACE_DUALLINK) ? 1 : 0;
}

static inline int interface_stereo(struct netvizxpi_state *state)
{
    return (state->current_interface & INTERFACE_STEREO) ? 1 : 0;
}

static inline int local_mode(struct netvizxpi_state *state)
{
    return (state->oinfo.flags & OV_FLAG_SRC_LOCAL) ? 1 : 0;
}

static inline int remote_mode(struct netvizxpi_state *state)
{
    return (state->oinfo.flags & OV_FLAG_SRC_REMOTE) ? 1 : 0;
}

static inline int overlay_mode(struct netvizxpi_state *state)
{
    return (state->oinfo.flags & OV_FLAG_SRC_OVERLAY) ? 1 : 0;
}

static int bitvals[] = {
    0x0001, 0x0101, 0x0421, 0x1111, 
    0x1249, 0x5249, 0x2555, 0x5555, 
    0xb555, 0xb5b5, 0xbbb5, 0xbbbb, 
    0xfbbb, 0xfbfb, 0xfbff, 0xffff
};

static inline void enforce_ppscontrol(struct netvizxpi_state *state,
                                      struct LUT_struct *lutp)
{
    int fps, pixels, div, bitval = 0;

    if (lutp == NULL || !is_tx(state))
        return;

    pixels = lutp->a_orig_hres * lutp->a_orig_vres;
    fps = (state->pps_capacity * 10)/ pixels;
    div = (fps * 16 * 100) / lutp->a_lrefresh;
    div = (div < 1) ? 1 : ((div > 16) ? 16 : div);
    bitval = bitvals[div - 1];

    if (debug & NETVIZXPI_DEBUG_PPS) {
        xpi_info(state, 
                "Setting divisor to 0x%x (div %d lrefresh %ld, fps %d)\n",
                bitval, div, lutp->a_lrefresh, fps);
    }
    set_fpga_val(state, FPGA_ASKIPCNT, bitval);
    set_fpga_val(state, FPGA_ASKIPCNTBITS, MAX_ASKIPCNTBITS);
}

static inline void remove_ppscontrol(struct netvizxpi_state *state)
{
    /* set frame rate divider to 1 */
    if (debug & NETVIZXPI_DEBUG_PPS) {
        xpi_info(state, "Setting framerate divider to 1\n");
    }
    set_fpga_val(state, FPGA_ASKIPCNT, 0x01);
    set_fpga_val(state, FPGA_ASKIPCNTBITS, 0x00);
}

#define MAX_RETRIES         3

static inline int addside_dcm_locked(struct netvizxpi_state *state, int mode)
{
    unsigned long lockbits = S6FPGA_ASTATUS_PIX_CLKB_DCM_LOCKED;

    if (mode == NETVIZXPI_MODE_HDMI)
        lockbits |= S6FPGA_ASTATUS_PIX_CLKA_DCM_LOCKED;

    if ((get_s6fpga_val(state, S6FPGA_ASTATUSREG) & lockbits) == lockbits) 
        return 1;

    return 0;
}

static inline int reset_addside_dcm(struct netvizxpi_state *state, int mode)
{
    int retries, rc = 0;

    for (retries = 0; retries < MAX_RETRIES; retries++) {

        /* reset addside DCM */
        set_s6fpga_val(state, S6FPGA_CDCMRREG, 0x2);
        mdelay(5);

        /* take addside DCM out of reset */
        set_s6fpga_val(state, S6FPGA_CDCMRREG, 0);
        mdelay(5);

        /* check if pixel clock is locked  */
        if (addside_dcm_locked(state, mode))
            break;
    }

    if (retries == MAX_RETRIES) {
        xpi_info(state, "%s: addside DCM unlocked.\n", __func__);
        rc = -1;
    }
    return rc;
}

static inline int check_videolink_stable(struct netvizxpi_state *state)
{
    if (state->current_interface & INTERFACE_ADDSIDE_VIDEO_ENABLE) {
        unsigned long reg6e0_value = get_fpga_val(state, FPGA_REG6E0);
        if (reg6e0_value != state->reg6e0_value) {
            state->reg6e0_value = reg6e0_value;
            return 0;
        }
    }
    return 1;
}

static inline void reset_addside_link_enable(struct netvizxpi_state *state)
{
    or_in_fpga_val(state, FPGA_ACREG, ACREG_ADD_ENABLE);
    mdelay(1);
    and_in_fpga_val(state, FPGA_ACREG, ~ACREG_ADD_ENABLE);
}

static inline int get_slave_hres(struct netvizxpi_state *state)
{
    return (int) (get_s6fpga_val(state, S6FPGA_DE2HSYNCCNT) & 0xffff);
}

static inline int check_slave_hres_stable(struct netvizxpi_state *state,
                                          struct v4l2_dv_timings *timings)
{
    return (get_slave_hres(state) == timings->bt.width) ? 1 : 0;
}

static inline int check_addside_lut_change(struct netvizxpi_state *state,
                                           struct LUT_struct *lutp)
{
    int changed = 1;

    if (state->current_addside_lut == NULL) {
        changed = (lutp == NULL) ? 0 : 1;
    }
    else if (lutp && 
            (state->current_addside_lut->a_stereo == lutp->a_stereo &&
             state->current_addside_lut->a_orig_hres == lutp->a_orig_hres &&
             state->current_addside_lut->a_orig_vres == lutp->a_orig_vres &&
             state->current_addside_lut->a_hfp == lutp->a_hfp && 
             state->current_addside_lut->a_hsync == lutp->a_hsync && 
             state->current_addside_lut->a_hbp == lutp->a_hbp && 
             state->current_addside_lut->a_vfp == lutp->a_vfp && 
             state->current_addside_lut->a_vsync == lutp->a_vsync && 
             state->current_addside_lut->a_vbp == lutp->a_vbp && 
             state->current_addside_lut->a_lrefresh >= 
                                 (lutp->a_lrefresh - RFRSHMARGIN) && 
             state->current_addside_lut->a_lrefresh <= 
                                 (lutp->a_lrefresh + RFRSHMARGIN))) {
        changed = 0;
    }
    return changed;
}

static inline void set_addside_configuration(struct netvizxpi_state *state)
{
    set_fpga_val(state, FPGA_ACREG, ACREG_IFRAME);
    if (state->current_interface & INTERFACE_COMPMODE_444) 
        or_in_fpga_val(state, FPGA_ACREG, ACREG_444_SEL);

    if (state->current_interface & INTERFACE_COMPMODE_MOTION444)
        or_in_fpga_val(state, FPGA_ACREG, (ACREG_MOTION_444_SEL |
                                           ACREG_444_SEL));
    if (state->current_interface & (INTERFACE_COMPMODE_444 |
                                    INTERFACE_COMPMODE_MOTION444))
        or_in_fpga_val(state, FPGA_422AVGMODE, FPGA_422AVGMODE_BIT);
    else
        and_in_fpga_val(state, FPGA_422AVGMODE, ~FPGA_422AVGMODE_BIT);

    if (interface_stereo(state))
        or_in_fpga_val(state, FPGA_ACREG, ACREG_STEREO);

    if (state->current_interface & INTERFACE_ADDSIDE_VIDEO_ENABLE)
        or_in_fpga_val(state, FPGA_ACREG, ACREG_VID_ENABLE);

    if (state->current_interface & INTERFACE_ADDSIDE_AUDIO_ENABLE) 
        or_in_fpga_val(state, FPGA_ACREG, ACREG_AUD_ENABLE);
}

static inline void clear_addside_configuration(struct netvizxpi_state *state)
{
    and_in_fpga_val(state, FPGA_ACREG, ~(ACREG_VID_ENABLE |
                                         ACREG_AUD_ENABLE |
                                         ACREG_LAN_ENABLE |
                                         ACREG_444_SEL |
                                         ACREG_MOTION_444_SEL |
                                         ACREG_STEREO |
                                         ACREG_IFRAME));
}

static inline void set_dropside_configuration(struct netvizxpi_state *state)
{
    if (state->current_interface & 
            (INTERFACE_COMPMODE_444 | INTERFACE_COMPMODE_MOTION444))
        or_in_fpga_val(state, FPGA_DCREG, DCREG_444_SEL);

    if (state->current_interface & INTERFACE_STEREO)
        or_in_fpga_val(state, FPGA_DCREG, DCREG_STEREO);

    /* dropside video is enabled from CLI or menu */
    if (state->current_interface & INTERFACE_VIDEO_OUTPUT_ENABLE)
        or_in_fpga_val(state, FPGA_DCREG, DCREG_VID_ENABLE);

    /* dropside audio is always enabled */
    or_in_fpga_val(state, FPGA_DCREG, DCREG_AUD_ENABLE);
}

static inline void clear_dropside_configuration(struct netvizxpi_state *state)
{
    and_in_fpga_val(state, FPGA_DCREG, ~(DCREG_VID_ENABLE | 
                                         DCREG_AUD_ENABLE |
                                         DCREG_STEREO |
                                         DCREG_444_SEL));
}

static inline void set_s6fpga_local_video_timings(struct netvizxpi_state *state,
                                                  struct LUT_struct *lutp)
{
    if (lutp == NULL)
        return;
    set_s6fpga_val(state, S6FPGA_LVHAREG, lutp->a_orig_hres);
    set_s6fpga_val(state, S6FPGA_LVHFPREG, lutp->a_hfp);
    set_s6fpga_val(state, S6FPGA_LVHSREG, lutp->a_hsync);
    set_s6fpga_val(state, S6FPGA_LVHBPREG, lutp->a_hbp);
    set_s6fpga_val(state, S6FPGA_LVVAREG, lutp->a_orig_vres);
    set_s6fpga_val(state, S6FPGA_LVVFPREG, lutp->a_vfp);
    set_s6fpga_val(state, S6FPGA_LVVSREG, lutp->a_vsync);
    set_s6fpga_val(state, S6FPGA_LVVBPREG, lutp->a_vbp);
}

static void compute_actual_overlay_params(struct netvizxpi_state *state,
                                          struct ov_param_io *a_cfg,
                                          struct ov_param_io *cfg) {

    int srchres, srcvres, newhres, newvres, monhres, monvres;

    memcpy(a_cfg, cfg, sizeof(struct ov_param_io));

    /* Check for valid dropside and display LUTs */
    if (state->current_dropside_lut == NULL ||
            state->current_display_lut == NULL) {
        return;
    }

    /* Set actual overlay source to remote if no current addside LUT */
    if (state->current_addside_lut == NULL && 
            cfg->flags & OV_FLAG_SRC_OVERLAY) {
       a_cfg->flags &= ~OV_FLAG_SRCMASK;
       a_cfg->flags |= OV_FLAG_SRC_REMOTE;
    }

    /* Compute actual overlay H/V scale for current display LUT */
    if (a_cfg->flags & OV_FLAG_SRC_LOCAL) {
        a_cfg->hscale = a_cfg->vscale = RX_MAX_SCALE_VAL;
        a_cfg->hoffset = a_cfg->voffset = 0;
    }
    else {
        srchres = state->current_dropside_lut->a_hres;
        srcvres = state->current_dropside_lut->a_vres;
        if (state->oinfo.flags & OV_FLAG_CROP) {
            if (srchres > state->oinfo.hcropleft + state->oinfo.hcropright)
                srchres -= (state->oinfo.hcropleft + state->oinfo.hcropright);
            if (srcvres > state->oinfo.vcroptop + state->oinfo.vcropbottom)
                srcvres -= (state->oinfo.vcroptop + state->oinfo.vcropbottom);
        }
        monhres = state->current_display_lut->a_hres;
        monvres = state->current_display_lut->a_vres;

        if (a_cfg->flags & OV_FLAG_HRES)
            a_cfg->hscale = (a_cfg->hres * RX_MAX_SCALE_VAL)/srchres;
        else if (a_cfg->flags & OV_FLAG_HSCALEDISPLAY)
            a_cfg->hscale = (cfg->hscale * monhres)/srchres;
        if (a_cfg->hscale > RX_MAX_SCALE_VAL)
            a_cfg->hscale = RX_MAX_SCALE_VAL;
        if (a_cfg->hscale < RX_MIN_SCALE_VAL)
            a_cfg->hscale = RX_MIN_SCALE_VAL;

        if (a_cfg->flags & OV_FLAG_VRES)
            a_cfg->vscale = (a_cfg->vres * RX_MAX_SCALE_VAL)/srcvres;
        else if (a_cfg->flags & OV_FLAG_VSCALEDISPLAY)
            a_cfg->vscale = (cfg->vscale * monvres)/srcvres;
        if (a_cfg->vscale > RX_MAX_SCALE_VAL)
            a_cfg->vscale = RX_MAX_SCALE_VAL;
        if (a_cfg->vscale < RX_MIN_SCALE_VAL)
            a_cfg->vscale = RX_MIN_SCALE_VAL;

        newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        while ((newhres > monhres) && a_cfg->hscale > RX_MIN_SCALE_VAL) {
            a_cfg->hscale--;
            newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        }

        newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;
        while ((newvres > monvres) && a_cfg->vscale > RX_MIN_SCALE_VAL) {
            a_cfg->vscale--;
            newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;
        }

        if (a_cfg->flags & OV_FLAG_KEEPASPECT) {
            if (a_cfg->hscale > a_cfg->vscale)
                a_cfg->hscale = a_cfg->vscale;
            if (a_cfg->vscale > a_cfg->hscale)
                a_cfg->vscale = a_cfg->hscale;
        }
        newhres = srchres * a_cfg->hscale / RX_MAX_SCALE_VAL;
        newvres = srcvres * a_cfg->vscale / RX_MAX_SCALE_VAL;

        if (a_cfg->flags & OV_FLAG_HPOSLOCATION) {
            if (a_cfg->hlocation == OV_RIGHT_ADJUSTED)
                a_cfg->hoffset = monhres - newhres;
            else if (a_cfg->hlocation == OV_LEFT_ADJUSTED)
                a_cfg->hoffset = 0;
            else
                a_cfg->hoffset = (monhres - newhres) / 2;
        }
        if (a_cfg->flags & OV_FLAG_VPOSLOCATION) {
            if (a_cfg->vlocation == OV_BOTTOM_ADJUSTED)
                a_cfg->voffset = monvres - newvres;
            else if (a_cfg->vlocation == OV_TOP_ADJUSTED)
                a_cfg->voffset = 0;
            else
                a_cfg->voffset = (monvres - newvres) / 2;
        }
    }
    return;
}

static void compute_cropping_parameters(struct netvizxpi_state *state,
                                        struct LUT_struct *lutp,
                                        struct cropping_param_st *cparm)
{
    int newhres, newvres;

    if (lutp == NULL || cparm == NULL)
        return;

    memset(cparm, 0, sizeof(struct cropping_param_st));

    /* cropping is not relevant in local mode */
    if (state->oinfo.flags & OV_FLAG_SRC_LOCAL) 
        return;

    newhres = lutp->a_hres * state->oinfo.hscale / RX_MAX_SCALE_VAL;
    newvres = lutp->a_vres * state->oinfo.vscale / RX_MAX_SCALE_VAL;

    /* compute dummy pixels for fine cropping */
    if (state->oinfo.hscale != RX_MAX_SCALE_VAL ||
            state->oinfo.vscale != RX_MAX_SCALE_VAL) {
        cparm->dummy_pixels = ((newhres % 8) == 0) ? 0 : 8 - (newhres % 8);
        if (((newhres + cparm->dummy_pixels) / 8) % state->oinfo.hscale == 1)
            cparm->dummy_pixels += 8;
    }
    cparm->addr_in_line = (newhres + cparm->dummy_pixels) / 8;

    /* compute cropping parameters */
    if (lutp && state->oinfo.flags & OV_FLAG_CROP) {
        if (state->oinfo.hcropleft > 0 &&
                state->oinfo.hcropleft < lutp->a_orig_hres) {
            cparm->hcropleft = (state->oinfo.hcropleft * 
                state->oinfo.hscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            cparm->hcrop = 1;
        }
        if (state->oinfo.hcropright > 0 &&
                state->oinfo.hcropright < lutp->a_orig_hres) {
            cparm->hcropright = (state->oinfo.hcropright * 
                state->oinfo.hscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            cparm->hcrop = 1;
        }
        if (state->oinfo.vcroptop > 0 &&
            state->oinfo.vcroptop < lutp->a_orig_vres) {
            cparm->vcroptop = (state->oinfo.vcroptop * 
                state->oinfo.vscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            cparm->vcrop = 1;
        }
        if (state->oinfo.vcropbottom > 0 &&
                state->oinfo.vcropbottom < lutp->a_orig_vres) {
            cparm->vcropbottom = (state->oinfo.vcropbottom * 
                state->oinfo.vscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
            cparm->vcrop = 1;
        }
    }
    if (lutp->a_hres != lutp->a_orig_hres) {
        cparm->hcropright += ((lutp->a_hres - lutp->a_orig_hres) *
                state->oinfo.hscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
        cparm->hcrop = 1;
    }
    if (lutp->a_vres != lutp->a_orig_vres) {
        cparm->vcropbottom += ((lutp->a_vres - lutp->a_orig_vres) *
                state->oinfo.vscale + RX_MAX_SCALE_VAL - 1) / RX_MAX_SCALE_VAL;
        cparm->vcrop = 1;
    }
}

static void configure_s6splash_registers(struct netvizxpi_state *state,
                                         struct LUT_struct *lutp)
{
    if (lutp == NULL) 
        return;

    /* set resolution and delay registers */
    set_s6fpga_val(state, S6FPGA_DVHAREG, lutp->a_hres);
    set_s6fpga_val(state, S6FPGA_DVHFPREG, lutp->a_hfp);
    set_s6fpga_val(state, S6FPGA_DVHSREG, lutp->a_hsync);
    set_s6fpga_val(state, S6FPGA_DVHBPREG, lutp->a_hbp);
    set_s6fpga_val(state, S6FPGA_DVVAREG, lutp->a_vres);
    set_s6fpga_val(state, S6FPGA_DVVFPREG, lutp->a_vfp);
    set_s6fpga_val(state, S6FPGA_DVVSREG, lutp->a_vsync);
    set_s6fpga_val(state, S6FPGA_DVVBPREG, lutp->a_vbp);

    /* set video configuration register */
    if (state->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            S6FPGA_VIDCFG_REMOTE_VID_ABSENT | S6FPGA_VIDCFG_LOCAL_VID_SEL);
    }
    else if (state->oinfo.flags & OV_FLAG_SRC_OVERLAY) {
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, S6FPGA_VIDCFG_LOCAL_VID_SEL);
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            ~S6FPGA_VIDCFG_REMOTE_VID_ABSENT);
    }
    else {
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            ~(S6FPGA_VIDCFG_REMOTE_VID_ABSENT | S6FPGA_VIDCFG_LOCAL_VID_SEL));
    }
}

static struct LUT_struct *netvizxpi_get_lut_from_dv_timings(
        struct netvizxpi_state *state, 
        struct v4l2_dv_timings *dv, int stereo, int verbose, int autolut) 
{
    struct LUT_struct *lutp;

    if (autolut) {
        lutp = add_dv_timings_lut_entry(state, dv, stereo, verbose);
        if (debug & NETVIZXPI_DEBUG_AUTOLUT) {
            if (lutp == NULL) {
                xpi_info(state, "%s: returning NULL\n", __func__);
            }
            else {
                xpi_info(state, 
                    "%s: returning LUT %dx%d (%dx%d) rfrsh %ld pxclk %ld\n", 
                    __func__, lutp->a_orig_hres, lutp->a_orig_vres, 
                    lutp->a_hres, lutp->a_orig_vres, lutp->a_lrefresh, 
                    lutp->a_lpcr);
            }
        }
    }
    else
        lutp = get_lut_from_dv_timings(state, dv, stereo, verbose);

    return lutp;
}

/* ------------------------------------------------------------------------- */

static void netvizxpi_delayed_work_sii7172(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    struct netvizxpi_state *state;
    struct v4l2_dv_timings timings;
    struct LUT_struct *lutp;

    state = container_of(dwork, struct netvizxpi_state, delayed_work_sii7172);
    if (state->flags & FLAG_XPI_SPLASH_LOOPBACK) 
        lutp = state->current_splash_lut;
    else if (!is_tx(state) && remote_mode(state))
        lutp = state->current_display_lut;
    else
        lutp = state->current_addside_lut;

    if (lutp == NULL)
        return;

    lut2dv_timings(lutp, &timings);
    v4l2_subdev_call(state->hdmi_transmitter, video, s_dv_timings, &timings); 
}

/* ------------------------------------------------------------------------- */

/* must be called with 'lock' held */
static void configure_s6input_registers(struct netvizxpi_state *state,
                                        struct LUT_struct *lutp)
{
    if (lutp == NULL)
        return;

    /* set resolution and delay registers */
    set_s6fpga_local_video_timings(state, lutp);

    /* set source horiz and vert resolution (padded, not orig) */
    set_s6fpga_val(state, S6FPGA_SRCVHAREG, lutp->a_hres);
    set_s6fpga_val(state, S6FPGA_SRCVVAREG, lutp->a_vres);

    if (debug & NETVIZXPI_DEBUG_VIDIN)
        xpi_info(state, "%s: padded source hres %d vres %d\n", __func__, 
                lutp->a_hres, lutp->a_vres);

    /* set stereo */
    if (state->current_interface & INTERFACE_STEREO)
        or_in_s6fpga_val(state, S6FPGA_ACREG, S6FPGA_ACREG_STEREO);
    else
        and_in_s6fpga_val(state, S6FPGA_ACREG, ~S6FPGA_ACREG_STEREO);

    if (interface_duallink(state)) 
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, S6FPGA_VIDCFG_DUALLINK_DVI);
    else 
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, ~S6FPGA_VIDCFG_DUALLINK_DVI);
}

/* must be called with 'lock' held */
static void configure_v6encode_registers(struct netvizxpi_state *state,
                                           struct LUT_struct *lutp) 
{
    unsigned long nval, vact, thres0;
    int encodedwidth, encodedheight;
    int slicesize = DEFAULT_NOBPSLICE, num_slices_per_frame;
    int avhareg, avvareg;

    if (lutp == NULL)
        return;
    /* set noise registers */
    nval = ((lutp->a_noise & LUT_ACNOISE_MASK) >> LUT_ACNOISE_SHIFT) 
                                                    << ACNOISE_SHIFT;
    set_fpga_val(state, FPGA_ACNOISET, nval);
    nval = ((lutp->a_noise & LUT_DCNOISE_MASK) >> LUT_DCNOISE_SHIFT) 
                                                    << DCNOISE_SHIFT;
    set_fpga_val(state, FPGA_DCNOISET, nval);

    /* Set horizontal and vertical resolution (padded, not orig) */
    avhareg = lutp->a_hres;
    avvareg = lutp->a_vres;
    set_fpga_val(state, FPGA_AVHAREG, avhareg);
    udelay(1);
    set_fpga_val(state, FPGA_AVVAREG, avvareg);
    udelay(1);

    /* set horizontal and vertical resolution (orig, not padded) */
    set_fpga_val(state, FPGA_SRCVHAREG, lutp->a_orig_hres);
    set_fpga_val(state, FPGA_SRCVVAREG, lutp->a_orig_vres);

    /* figure out slice size and blocks per slice */
    compute_slice_size(avhareg, avvareg, &slicesize, &encodedwidth, 
            &encodedheight);

    if (debug & NETVIZXPI_DEBUG_VIDIN)
        xpi_info(state, "%s: padded %d/%d, slicesize %d, encoded %d/%d\n", 
                __func__, avhareg, avvareg, slicesize, encodedwidth, 
                encodedheight);

    set_fpga_val(state, FPGA_ANOBREG, slicesize);
    num_slices_per_frame = (avhareg * avvareg) / (64 * slicesize);
    set_fpga_val(state, FPGA_ANSLICEREG, num_slices_per_frame);

    /* set addside buffer threshold and latency control registers */
    vact = (lutp->a_vres / 16) * 16;
    thres0 = (lutp->a_hres * vact) / 8 / 2 - 1;
    set_fpga_val(state, FPGA_ALATTHRES0, thres0);
    set_fpga_val(state, FPGA_ALATCONTROL, 0);
    set_fpga_val(state, FPGA_ALATSLICE3, 0);
    set_fpga_val(state, FPGA_ALATSLICE4, 0);
    set_fpga_val(state, FPGA_ALATSLICE5, 0);

    if (state->current_interface & INTERFACE_ADDSIDE_LATENCY) {
        set_fpga_val(state, FPGA_ALATSLICE0, num_slices_per_frame / 4 - 1);
        set_fpga_val(state, FPGA_ALATSLICE1, num_slices_per_frame / 2 - 1);
        set_fpga_val(state, FPGA_ALATSLICE2, num_slices_per_frame * 3 / 4 - 1);
        set_fpga_val(state, FPGA_ALATCONTROL, 0x00ff);
    }
    else {
        set_fpga_val(state, FPGA_ALATSLICE0, 0);
        set_fpga_val(state, FPGA_ALATSLICE1, 0);
        set_fpga_val(state, FPGA_ALATSLICE2, 0);
    }

    /* set duallink */
    if (interface_duallink(state)) 
        and_in_fpga_val(state, FPGA_ATESTREG, ~FPGA_XPI_SGLLINK);
    else
        or_in_fpga_val(state, FPGA_ATESTREG, FPGA_XPI_SGLLINK);
}

/* must be called with 'lock' held */
static void configure_encode_and_config_registers(struct netvizxpi_state *state)
{
    if (state->current_addside_lut == NULL) 
        return;

    /* clear addside config flags */
    clear_addside_configuration(state);

    /* configure V6 encode section registers */
    configure_v6encode_registers(state, state->current_addside_lut);

    /* set addside config flags */
    set_addside_configuration(state);

    /* set dropside config for reverse audio */
    if (state->current_interface & INTERFACE_DROPSIDE_AUDIO_ENABLE)
        or_in_fpga_val(state, FPGA_DCREG, DCREG_AUD_ENABLE);
    else
        and_in_fpga_val(state, FPGA_DCREG, ~DCREG_AUD_ENABLE);

    /* set the new value for register 0x6e0 */
    state->reg6e0_value = get_fpga_val(state, FPGA_REG6E0);
}

/* must be called with 'lock' held */
static void configure_output_registers(struct netvizxpi_state *state, 
                                       int duallink,
                                       int splashloopback)
{
    if (debug & NETVIZXPI_DEBUG_VIDOUT_TRACE)
        xpi_info(state, "%s: duallink %d, splashloopback %d\n", __func__,
                duallink, splashloopback);

    /* set S6 video configuration register for duallink */
    if (duallink) 
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, S6FPGA_VIDCFG_DUALLINK_DVI);
    else 
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, ~S6FPGA_VIDCFG_DUALLINK_DVI);

    /* set video configuration register */
    if (state->current_interface & INTERFACE_VIDEO_OUTPUT_ENABLE) 
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, ~S6FPGA_VIDCFG_PWRDN_SYNCS);
    else
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, S6FPGA_VIDCFG_PWRDN_SYNCS);

    /* configure the sii7172 */
    if (state->hdmi_transmitter) {
        struct v4l2_dv_timings timings;
        struct LUT_struct *lutp;
        if (splashloopback) {
            state->flags |= FLAG_XPI_SPLASH_LOOPBACK;
            lutp = state->current_splash_lut;
        }
        else {
            state->flags &= ~FLAG_XPI_SPLASH_LOOPBACK;
            lutp = (!is_tx(state) && remote_mode(state)) ? 
                    state->current_display_lut : state->current_addside_lut;
        }
        if (debug & NETVIZXPI_DEBUG_VIDOUT) {
            xpi_info(state, "%s: duallink %d, splashloopback %d, "
                    "LUT %dx%d@%lu, pixelclock %lu\n", __func__, duallink,
                    splashloopback, lutp->a_orig_hres, lutp->a_orig_vres,
                    lutp->a_lrefresh / 1000, lutp->a_lpcr);
        }
        if (interface_duallink(state)) {
            if (state->work_queues) {
                queue_delayed_work(state->work_queues, 
                        &state->delayed_work_sii7172, HZ);
            }
        }
        else {
            lut2dv_timings(lutp, &timings);
            v4l2_subdev_call(state->hdmi_transmitter, video, s_dv_timings, 
                    &timings); 
        }
    }
    if (debug & NETVIZXPI_DEBUG_VIDOUT_TRACE)
        xpi_info(state, "%s: returning\n", __func__);
}

/* must be called with 'lock' held */
static void configure_addside(struct netvizxpi_state *state)
{
    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: entered\n", __func__);

    if (state->current_addside_lut == NULL)
        return;
    if (is_tx(state) || local_mode(state) || overlay_mode(state)) {
        /* configure the input section registers */
        configure_s6input_registers(state, state->current_addside_lut);

        /* configure the encode section and add/drop config registers */
        if (is_tx(state)) {
            configure_encode_and_config_registers(state);
        }

        /* configure the output section registers */
        configure_output_registers(state, interface_duallink(state), 0);
    }
    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: returning\n", __func__);
}

/* must be called with 'lock' held */
static void configure_addside_splash(struct netvizxpi_state *state,
                                     struct LUT_struct *lutp)
{
    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: LUT %dx%d@%lu. pixelclock %lu\n", __func__,
                lutp->a_orig_hres, lutp->a_orig_vres, lutp->a_lrefresh / 1000,
                lutp->a_lpcr);

    if (lutp == NULL)
        return;

    /* clear addside config flags */
    clear_addside_configuration(state);

    /* configure V6 encode section registers */
    configure_v6encode_registers(state, lutp);

    /* configure S6 splash video and pattern registers */
    configure_s6splash_registers(state, lutp);

    /* reset DCM (TODO: may not be needed) */
    if (reset_addside_dcm(state, NETVIZXPI_MODE_HDMI) < 0) {
        xpi_info(state, "%s: failed to reset addside DCM. status = 0x%lx\n",
                __func__, get_s6fpga_val(state, S6FPGA_ASTATUSREG));
    }

    /* enable splash */
    or_in_s6fpga_val(state, S6FPGA_ASPLASHCTRL, ASPLASHCTRL_SPLASH);

    /* configure s6 output and Sii7172 registers */
    if (state->current_splash_lut == NULL)
        state->current_splash_lut =
            kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
    memcpy(state->current_splash_lut, lutp, sizeof(struct LUT_struct));
    configure_output_registers(state, 
                (lutp->a_lpcr > PCR_DUALLINK_DVI * 1000) ? 1 : 0, 1);

    /* set addside config flags */
    set_addside_configuration(state);

    /* set dropside config for reverse audio */
    if (state->current_interface & INTERFACE_DROPSIDE_AUDIO_ENABLE)
        or_in_fpga_val(state, FPGA_DCREG, DCREG_AUD_ENABLE);
    else
        and_in_fpga_val(state, FPGA_DCREG, ~DCREG_AUD_ENABLE);

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: returning\n", __func__);
}

/* must be called with 'lock' held */
static void configure_v6decode_registers(struct netvizxpi_state *state, 
                                         struct LUT_struct *lutp,
                                         struct cropping_param_st *cparm)
{
    int encodedwidth, encodedheight, slicesize, num_slices_per_frame;
    int blocksinbank, newhres, newvres, srchres, srcvres, end_address_rem;
    unsigned long end_address;

    if (lutp == NULL || cparm == NULL || state->current_display_lut == NULL) 
        return;

    if ((state->oinfo.flags & OV_FLAG_SRC_REMOTE) && lutp == NULL) 
            return;

    /* set dropside slices per frame */
    compute_slice_size((int) lutp->a_hres, (int) lutp->a_vres, &slicesize, 
            &encodedwidth, &encodedheight); 
    if (slicesize == 0) {
        xpi_err(state, "%s: invalid slicesize %d\n", __func__, slicesize);
        return;
    }
    set_fpga_val(state, FPGA_DNOBREG, slicesize);
    num_slices_per_frame = (lutp->a_hres * lutp->a_vres) / (64 * slicesize);
    set_fpga_val(state, FPGA_DNSLICEREG, num_slices_per_frame);
    blocksinbank = slicesize * (num_slices_per_frame / 4);

    if (debug & NETVIZXPI_DEBUG_VIDIN)
        xpi_info(state, "%s: slicesize %d num_slices_per_frame %d\n", 
                __func__, slicesize, num_slices_per_frame);

    /* compute dropside video register values */
    if (state->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        srchres = newhres = state->current_display_lut->a_hres;
        srcvres = newvres = state->current_display_lut->a_vres;
        state->oinfo.hscale = state->oinfo.vscale = RX_MAX_SCALE_VAL;
        state->oinfo.hoffset = state->oinfo.voffset = 0;
    }
    else {
        srchres = (lutp) ? lutp->a_hres : 0;
        srcvres = (lutp) ? lutp->a_vres : 0;
        newhres = srchres * state->oinfo.hscale / RX_MAX_SCALE_VAL;
        newvres = srcvres * state->oinfo.vscale / RX_MAX_SCALE_VAL;
    }

    /* set encoded horiz and vert resolution */
    set_fpga_val(state, FPGA_DSRCHRES, srchres); 
    set_fpga_val(state, FPGA_DSRCVRES, srcvres); 

    /* set scaling registers */ 
    set_fpga_val(state, FPGA_DHSCALE, state->oinfo.hscale);
    set_fpga_val(state, FPGA_DVSCALE, state->oinfo.vscale);

    /* set scaled down horiz and vert resolution */
    set_fpga_val(state, FPGA_DNHRES, newhres); 
    set_fpga_val(state, FPGA_DNVRES, newvres); 

    if (debug & NETVIZXPI_DEBUG_VIDIN)
        xpi_info(state, "%s: encoded hres %d vres %d scaled hres %d vres %d\n", 
                __func__, srchres, srcvres, newhres, newvres);

    /* set scaling control register */
    if (state->oinfo.hscale == RX_MAX_SCALE_VAL &&
            state->oinfo.vscale == RX_MAX_SCALE_VAL) {
        set_fpga_val(state, FPGA_DSCALECTL, 0);
    }
    else {
        int k, hs = state->oinfo.hscale, vs = state->oinfo.vscale;
        unsigned long addr = DSCALE_COEFF_RAM_ADDR;

        set_fpga_val(state, FPGA_DSCALECTL, 1);

        /* load the coefficient ram */
        for (k = 0; k < 64; k++) {
            set_fpga_val(state, addr, *(unsigned long *)&c[hs - 1][vs - 1][k]);
            addr += sizeof(unsigned long);
            udelay(1);
        }
        for (k = 0; k < 64; k++) {
            set_fpga_val(state, addr, *(unsigned long *)&ct[hs - 1][vs - 1][k]);
            addr += sizeof(unsigned long);
            udelay(1);
        }
    }

    /* adjust scale control for cropping */
    or_in_fpga_val(state, FPGA_DSCALECTL, (cparm->dummy_pixels << 4));

    /* set DDR registers */
    set_fpga_val(state, FPGA_DDR2RDSTART, 
            cparm->vcroptop * cparm->addr_in_line + cparm->hcropleft / 8 + 1);

    if (cparm->hcrop) {
        end_address = (newvres - cparm->vcropbottom) * cparm->addr_in_line - 
            (cparm->hcropright + cparm->dummy_pixels) / 8 - 1;
    }
    else {
        end_address = (newvres - cparm->vcropbottom) *cparm->addr_in_line - 1;
    }

    end_address_rem = (end_address+1)%32 ;

    if (end_address_rem)
        end_address = (end_address + (64 - end_address_rem)) ;

        set_fpga_val(state, FPGA_DDR2RDEND, end_address );
    /* set crop registers */
    if (cparm->hcrop || cparm->vcrop) {
        set_fpga_val(state, FPGA_DCROPVRES, newvres - cparm->vcroptop - 
                cparm->vcropbottom);
        set_fpga_val(state, FPGA_DCROPHSTART, (cparm->vcroptop + 1) * 
                cparm->addr_in_line + cparm->hcropleft / 8);
        if (cparm->hcrop)
            set_fpga_val(state, FPGA_DCROPHEND, (cparm->vcroptop + 1) * 
                    cparm->addr_in_line - 
                    (cparm->hcropright + cparm->dummy_pixels) / 8 - 2);
        else
            set_fpga_val(state, FPGA_DCROPHEND, 
                    (cparm->vcroptop + 1) * cparm->addr_in_line - 2);
        set_fpga_val(state, FPGA_DCROPHRES, newhres - 
                cparm->hcropleft - cparm->hcropright);
        set_fpga_val(state, FPGA_DCROPHLEFT, cparm->hcropleft);
        set_fpga_val(state, FPGA_DCROPHRIGHT, cparm->hcropright);
    }

    /* set V6 TESTREG for cropping */
    if (cparm->vcrop) 
        or_in_fpga_val(state, FPGA_ATESTREG, VCROP_ENABLE);
    else 
        and_in_fpga_val(state, FPGA_ATESTREG, ~VCROP_ENABLE);
    if (cparm->hcrop) 
        or_in_fpga_val(state, FPGA_ATESTREG, HCROP_ENABLE);
    else 
        and_in_fpga_val(state, FPGA_ATESTREG, ~HCROP_ENABLE);
}

/* must be called with 'lock' held */
static void configure_s6decode_registers(struct netvizxpi_state *state, 
                                         struct LUT_struct *lutp,
                                         struct cropping_param_st *cparm)
{
    int hres, vres, hfp, hsync, hbp, vfp, vsync, vbp, hds, vds;
    int backgroundcolor = 0;
    int oalpha, srchres, srcvres, newhres, newvres;

    if (lutp == NULL || cparm == NULL || state->current_display_lut == NULL) 
        return;

    /* clear all flags initialially */
    and_in_s6fpga_val(state, S6FPGA_DCREG, ~DCREG_STEREO);

    /* set dropside horiz and vert res and porches */
    hres = state->current_display_lut->a_hres;
    hfp = state->current_display_lut->a_hfp;
    hsync = state->current_display_lut->a_hsync;
    hbp = state->current_display_lut->a_hbp;
    vres = state->current_display_lut->a_vres;
    vfp = state->current_display_lut->a_vfp;
    vsync = state->current_display_lut->a_vsync;
    vbp = state->current_display_lut->a_vbp;

    if (debug & NETVIZXPI_DEBUG_VIDOUT)
        xpi_info(state, "%s: dropside hres %d (%d/%d/%d) vres %d (%d/%d/%d)\n", 
                __func__, hres, hfp, hsync, hbp, vres, vfp, vsync, vbp);

    if ((state->oinfo.flags & OV_FLAG_SRC_REMOTE) ||
            (state->current_interface & INTERFACE_DIGITAL)) {
        hds = vds = 0;
    }
    else {
        // TODO: fix H/V shift for analog 
        hds = vds = 0;
    }

    // set display timing registers
    set_s6fpga_val(state, S6FPGA_DVHAREG, state->current_display_lut->a_orig_hres);
    set_s6fpga_val(state, S6FPGA_DVVAREG, state->current_display_lut->a_orig_vres);
    set_s6fpga_val(state, S6FPGA_DVHFPREG, hfp + hds);
    set_s6fpga_val(state, S6FPGA_DVHSREG, hsync);
    set_s6fpga_val(state, S6FPGA_DVHBPREG, hbp - hds);
    set_s6fpga_val(state, S6FPGA_DVVFPREG, vfp + vds);
    set_s6fpga_val(state, S6FPGA_DVVSREG, vsync);
    set_s6fpga_val(state, S6FPGA_DVVBPREG, vbp - vds);

    if (debug & NETVIZXPI_DEBUG_VIDOUT)
        xpi_info(state, "%s: display hres %d (%d/%d/%d) vres %d (%d/%d/%d)\n", 
                __func__, state->current_display_lut->a_orig_hres, hfp + hds, 
                hsync, hbp - hds, state->current_display_lut->a_orig_vres, 
                vfp + vds, vsync, vbp - vds);

    /* set dropside blank color registers */
    if ((state->oinfo.flags & OV_FLAG_SRC_REMOTE) &&
            (state->current_display_lut->d_ov_control & DOVCONTROL_ENABLE)) {
        backgroundcolor = DOVCONTROL_ENABLE |
            (state->current_display_lut->d_ov_control & 0xff);
    }

    set_s6fpga_val(state, S6FPGA_DOVCONTROL, backgroundcolor);

    /* compute dropside video register values */
    if (state->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        srchres = newhres = hres;
        srcvres = newvres = vres;
        state->oinfo.hoffset = state->oinfo.voffset = 0;
    }
    else {
        srchres = (lutp) ? lutp->a_hres : 0;
        srcvres = (lutp) ? lutp->a_vres : 0;
        newhres = srchres * state->oinfo.hscale / RX_MAX_SCALE_VAL;
        newvres = srcvres * state->oinfo.vscale / RX_MAX_SCALE_VAL;
    }

    /* set encoded horiz and vert resolution */
    set_s6fpga_val(state, S6FPGA_DSRCHRES, srchres); 
    set_s6fpga_val(state, S6FPGA_DSRCVRES, srcvres); 

    /* set horiz and vert positioning */
    set_s6fpga_val(state, S6FPGA_DHSHIFT, state->oinfo.hoffset); 
    set_s6fpga_val(state, S6FPGA_DVSHIFT, state->oinfo.voffset); 

    /* set scaled down horiz and vert resolution */
    set_s6fpga_val(state, S6FPGA_DNHRES, newhres); 
    set_s6fpga_val(state, S6FPGA_DNVRES, newvres); 

    /* set scaling control register */
    if (state->oinfo.hscale == RX_MAX_SCALE_VAL &&
            state->oinfo.vscale == RX_MAX_SCALE_VAL) {
        set_s6fpga_val(state, S6FPGA_DSCALECTL, 0);
    }
    else {
        set_s6fpga_val(state, S6FPGA_DSCALECTL, 1);
    }

    /* program alpha channel registers */
    if (state->oinfo.flags & OV_FLAG_SRC_REMOTE)
        oalpha = 0;
    else if (state->oinfo.flags & OV_FLAG_SRC_LOCAL)
        oalpha = 1024;
    else
        oalpha = (1024 * state->oinfo.oalpha) / 100;

    set_s6fpga_val(state, S6FPGA_DOVLPALPHA1, 1024 - oalpha);
    set_s6fpga_val(state, S6FPGA_DOVLPALPHA2, oalpha);
    set_s6fpga_val(state, S6FPGA_DNOVLPALPHA1, 0);
    set_s6fpga_val(state, S6FPGA_DNOVLPALPHA2, 1024);

    /*31:6 upper thershold 15:0 lower thershold */
    set_s6fpga_val(state, S6FPGA_RDATAFIFOREG, 0x020001f0);

    /* adjust scale control for cropping */
    or_in_s6fpga_val(state, S6FPGA_DSCALECTL, (cparm->dummy_pixels << 4));

    /* set video cropping regisers */
    if (cparm->hcrop || cparm->vcrop) {
        set_s6fpga_val(state, S6FPGA_DCROPVRES, newvres - cparm->vcroptop -
                cparm->vcropbottom);
        set_s6fpga_val(state, S6FPGA_DCROPHRES, newhres - cparm->hcropleft -
                cparm->hcropright);
        set_s6fpga_val(state, S6FPGA_DCROPHLEFT, cparm->hcropleft);
        set_s6fpga_val(state, S6FPGA_DCROPHRIGHT, cparm->hcropright);
    }

    /* set dropside control register for stereo */
    if (state->current_interface & INTERFACE_STEREO) 
        or_in_s6fpga_val(state, S6FPGA_DCREG, S6FPGA_DCREG_STEREO);
    else
        and_in_s6fpga_val(state, S6FPGA_DCREG, ~S6FPGA_DCREG_STEREO);

    /* TODO: genlock */

    /* TODO: pixel adjust */
 
    /* set local (input) video resolution registers */
    set_s6fpga_local_video_timings(state, state->current_addside_lut);

    /* TODO: set dropside control register for sync polarity */

    /* set video configuration register for local, overlay or remote mode */
    if (state->oinfo.flags & OV_FLAG_SRC_LOCAL) {
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            S6FPGA_VIDCFG_REMOTE_VID_ABSENT | S6FPGA_VIDCFG_LOCAL_VID_SEL);
    }
    else if (state->oinfo.flags & OV_FLAG_SRC_OVERLAY) {
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, S6FPGA_VIDCFG_LOCAL_VID_SEL);
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            ~S6FPGA_VIDCFG_REMOTE_VID_ABSENT);
    }
    else {
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, 
            ~(S6FPGA_VIDCFG_REMOTE_VID_ABSENT | S6FPGA_VIDCFG_LOCAL_VID_SEL));
    }

    /* set S6 video configuration register for cropping */
    if (cparm->vcrop) 
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, VCROP_ENABLE);
    else 
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, ~VCROP_ENABLE);
    if (cparm->hcrop) 
        or_in_s6fpga_val(state, S6FPGA_VIDCFGREG, HCROP_ENABLE);
    else 
        and_in_s6fpga_val(state, S6FPGA_VIDCFGREG, ~HCROP_ENABLE);
}

/* must be called with 'lock' held */
static void configure_dropside(struct netvizxpi_state *state)
{
    struct cropping_param_st cparm;

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: entered\n", __func__);

    if (is_tx(state) || state->current_display_lut == NULL)
        return;

    /* update current interface based on display LUT */
    if (state->current_display_lut->a_lpcr <= PCR_DUALLINK_XPI) 
        state->current_interface &= ~INTERFACE_DUALLINK;
    else
        state->current_interface |= INTERFACE_DUALLINK;
    if (state->current_display_lut->a_stereo == LUT_STEREO_YES) 
        state->current_interface |= INTERFACE_STEREO;
    else
        state->current_interface &= ~INTERFACE_STEREO;

    /* clear dropside config flags */
    clear_dropside_configuration(state);

    /* determine whether cropping is needed & by how much */
    compute_cropping_parameters(state, state->current_dropside_lut, &cparm);

    /* configure V6 decode section registers */
    configure_v6decode_registers(state, state->current_dropside_lut, &cparm);

    /* configure S6 decode section registers */
    configure_s6decode_registers(state, state->current_dropside_lut, &cparm);

    /* set dropside config flags */
    set_dropside_configuration(state);

    /* set addside config for reverse audio */
    if (state->current_interface & INTERFACE_ADDSIDE_AUDIO_ENABLE)
        or_in_fpga_val(state, FPGA_ACREG, ACREG_AUD_ENABLE);
    else
        and_in_fpga_val(state, FPGA_ACREG, ~ACREG_AUD_ENABLE);

    /* configure s6 output and Sii7172 registers if in remote mode */
    if (remote_mode(state)) 
        configure_output_registers(state, interface_duallink(state), 0);

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: returning\n", __func__);
}

/* must be called with 'lock' held */
static void configure_dropside_splash(struct netvizxpi_state *state,
                                      struct LUT_struct *lutp)
{
    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: entered\n", __func__);

    if (lutp == NULL || state->current_display_lut == NULL)
        return;

    /* set dropside LUT */
    if (state->current_dropside_lut == NULL)
        state->current_dropside_lut =
            kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
    if (state->current_dropside_lut == NULL) 
        return;

    memcpy(state->current_dropside_lut, lutp, sizeof(struct LUT_struct));

    /* enable splash */
    or_in_fpga_val(state, FPGA_DSPLASHCTRL, DSPLASHCTRL_SPLASH);

    /* configure dropside registers */
    configure_dropside(state);

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        xpi_info(state, "%s: returning\n", __func__);
}

/* must be called with 'lock' held */
static void force_free_run(struct netvizxpi_state *state, int lutid)
{
    if (lutid) {
        state->current_addside_lut = get_cp_frun_lut(lutid);
        state->flags |= FLAG_XPI_FREE_RUNNING;
    }
    else {
        state->flags |= FLAG_XPI_REFRESH_LUT;
        state->flags &= ~FLAG_XPI_FREE_RUNNING;
    }
    adv7604_set_free_run(state->hdmi_adc_receiver, lutid);
    configure_addside(state);
}

static int set_interface(struct netvizxpi_state *state, int intf)
{
    int src = intf & INTERFACE_SRCMASK;
    int changedbits;

    /* configure ADV7604 for digital or analog input */
    if (src != (state->current_interface & INTERFACE_SRCMASK)) { 
        u32 input;
        struct v4l2_subdev *sd = state->hdmi_adc_receiver;
        if (sd == NULL) 
            return -ENODEV;
        else if (!(intf & (INTERFACE_DIGITAL|INTERFACE_HV))) 
            return -EINVAL;
        input = (intf & INTERFACE_DIGITAL) ? NETVIZXPI_MODE_HDMI : 
            ((intf & INTERFACE_HV) ? NETVIZXPI_MODE_GR : 0);
        v4l2_subdev_call(state->hdmi_adc_receiver, video, s_routing, 
                (intf & INTERFACE_DIGITAL) ? NETVIZXPI_MODE_HDMI :
                NETVIZXPI_MODE_GR, 0, 0);
    }

    changedbits = (state->current_interface ^ intf) & ~INTERFACE_SRCMASK;

    if (changedbits == 0)
        return 0;

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;

    state->current_interface = intf;

    if (changedbits & INTERFACE_XPI_RESET_BITS) {
        configure_addside(state);
        if (!is_tx(state))
            configure_dropside(state);
    }
    else {
        if (changedbits & (INTERFACE_ADDSIDE_VIDEO_ENABLE |
                           INTERFACE_ADDSIDE_AUDIO_ENABLE)) {
            unsigned long acreg = get_fpga_val(state, FPGA_ACREG);
            if (intf & INTERFACE_ADDSIDE_VIDEO_ENABLE)
                acreg |= ACREG_VID_ENABLE;
            else
                acreg &= ~ACREG_VID_ENABLE;
            if (intf & INTERFACE_ADDSIDE_AUDIO_ENABLE)
                acreg |= ACREG_AUD_ENABLE;
            else
                acreg &= ~ACREG_AUD_ENABLE;
            set_fpga_val(state, FPGA_ACREG, acreg);

            if (is_tx(state) && (intf & INTERFACE_ADDSIDE_VIDEO_ENABLE)) {
                mdelay(1);    
                state->reg6e0_value = get_fpga_val(state, FPGA_REG6E0);
            }
        }
        if (changedbits & (INTERFACE_DROPSIDE_AUDIO_ENABLE |
                           INTERFACE_GENLOCKMASTER)) {
            unsigned long dcreg = get_fpga_val(state, FPGA_DCREG);
            if (intf & INTERFACE_DROPSIDE_AUDIO_ENABLE)
                dcreg |= DCREG_AUD_ENABLE;
            else
                dcreg &= ~DCREG_AUD_ENABLE;
            if (intf & INTERFACE_GENLOCKMASTER)
                dcreg |= DCREG_GENLOCK_MODE;
            else
                dcreg &= ~DCREG_GENLOCK_MODE;
            set_fpga_val(state, FPGA_DCREG, dcreg);
        }
        if (changedbits & (INTERFACE_HSYNC_INVERT |
                           INTERFACE_VSYNC_INVERT |
                           INTERFACE_LRSYNC_INVERT)) {
            unsigned long s6dcreg = get_s6fpga_val(state, S6FPGA_DCREG);
            if (intf & INTERFACE_HSYNC_INVERT)
                s6dcreg |= S6FPGA_DCREG_HSYNC_POL;
            else
                s6dcreg &= ~S6FPGA_DCREG_HSYNC_POL;
            if (intf & INTERFACE_VSYNC_INVERT)
                s6dcreg |= S6FPGA_DCREG_VSYNC_POL;
            else
                s6dcreg &= ~S6FPGA_DCREG_VSYNC_POL;
            if (intf & INTERFACE_LRSYNC_INVERT)
                s6dcreg |= S6FPGA_DCREG_LRSYNC_POL;
            else
                s6dcreg &= ~S6FPGA_DCREG_LRSYNC_POL;
            set_s6fpga_val(state, S6FPGA_DCREG, s6dcreg);
        }
        if (changedbits & INTERFACE_VIDEO_OUTPUT_ENABLE) {
            unsigned long vidcfgreg = get_s6fpga_val(state, S6FPGA_VIDCFGREG);
            if (intf & INTERFACE_VIDEO_OUTPUT_ENABLE) 
                vidcfgreg &= ~S6FPGA_VIDCFG_PWRDN_SYNCS;
            else
                vidcfgreg |= S6FPGA_VIDCFG_PWRDN_SYNCS;
            set_s6fpga_val(state, S6FPGA_VIDCFGREG, vidcfgreg);
        }
    }

    mutex_unlock(&state->lock);

    return 0;
}

/* ----------------------------------------------------------------------- */

static void netvizxpi_work(struct work_struct *work)
{
    struct netvizxpi_state *state;
    struct v4l2_dv_timings mastertimings;
    struct LUT_struct *lutp = NULL;
    int rc, duallink = 0, stereo = 0, inputmode = 0;
    struct stdi_readback stdi;
    static struct v4l2_dv_timings oldmastertimings;
    static int oldstereo, oldinputmode, found_lut = 0;
    unsigned long get_network_bw, check_av_enable;
    int autolut = flags & NETVIZXPI_FLAG_AUTOLUT;

    state = container_of(work, struct netvizxpi_state, work);

    /* do nothing if forcing addside */
    if (state->current_interface & INTERFACE_FORCE_ADDSIDE)  
        return;

    if (flags & NETVIZXPI_FLAG_RESETFPGA) {
        /* FPGA network deadlock issue - TBD Fix in next FPGA version and 
         * remove this code*/
        //check if video is enabled
        check_av_enable = is_tx(state) && 
            (get_fpga_val(state, FPGA_ACREG) & ACREG_VID_ENABLE);
        if (check_av_enable) {
            get_network_bw = get_fpga_val(state, FPGA_XPI_BWCNT);
            if (get_network_bw == state->network_bw)
                state->hang_retry_cnt ++ ;
            state->network_bw = get_network_bw ;
        }
        else {
            state->hang_retry_cnt = 0 ;
            state->hang_reset_cnt = 0 ;
        }
    
        if (state->hang_retry_cnt > 3) {
            if (state->hang_reset_cnt == 0) {
                state->hang_retry_cnt = 0 ;
                state->hang_reset_cnt ++ ;
                xpi_info(state, 
                        "Workaround for TSM0090-182 related, resetting FPGA\n");
                //reset FPGA
                set_gpio_pin(GPIO_V6FPGA_RSTn, "GPIO_V6FPGA_RSTn", 0);
                mdelay(1);
                set_gpio_pin(GPIO_V6FPGA_RSTn, "GPIO_V6FPGA_RSTn", 1);
            }
            else if (flags & NETVIZXPI_FLAG_RETRYRESET) {
                if (state->hang_reset_cnt < 3) {
                    state->hang_reset_cnt ++ ;
                }
                else {
                    state->hang_reset_cnt = 0;
                }
            }
        }
    }

    memset(&mastertimings, 0, sizeof(struct v4l2_dv_timings));

    if (!state->hdmi_adc_receiver || !driver_enabled(state))
        return;

    /* get current timings from ADV7604 */
    inputmode = adv7604_query_dv_timings(state->hdmi_adc_receiver, 
            &mastertimings);

    /* check stereo if stereo processing is enabled */
    if (get_s6fpga_val(state, S6FPGA_AMDVITREG) & AMTREG_STEREO) {
        stereo = 1;
    }

    /* configure ADV7604 for analog if input is analog */
    if (inputmode == NETVIZXPI_MODE_STDI) {
        adv7604_g_stdi_readback(state->hdmi_adc_receiver, &stdi);
        lutp = find_addside_lut_entry(state, stdi.fcl, stdi.lcf, stdi.lcvs, 
                stereo, debug & NETVIZXPI_DEBUG_LUT);
        lut2dv_timings(lutp, &mastertimings);
        adv7604_set_analog_timings(state->hdmi_adc_receiver, &mastertimings);
        set_input_analog(state);
    }

    /* check if we have an LUT and no change in timings from adv7604 */
    if (!(state->flags & FLAG_XPI_REFRESH_LUT) &&
            (found_lut && !memcmp(&oldmastertimings, &mastertimings, 
                sizeof(struct v4l2_dv_timings)) && (stereo == oldstereo) && 
                (inputmode == oldinputmode))) {
        /* reset DCM if it unlocked */
        if (!addside_dcm_locked(state, inputmode)) {
            if (debug & NETVIZXPI_DEBUG_VIDIN)
                xpi_info(state, 
                        "%s: addside DCM unlocked. resetting DCM\n", __func__);
            (void) reset_addside_dcm(state, inputmode);
        }
        /* reset video enable on TX if video link is not stable */
        if (is_tx(state) && !check_videolink_stable(state)) {
            if (debug & NETVIZXPI_DEBUG_VIDIN)
                xpi_info(state, 
                        "%s: video link unstable. resetting add enable\n", 
                        __func__);
            reset_addside_link_enable(state);
        }
        /* if duallink and slave timing changed keep searching */
        /* for another LUT otherwise do nothing                */
        if (!(interface_duallink(state) && 
                    !check_slave_hres_stable(state, &mastertimings)))
            return;
        if (debug & NETVIZXPI_DEBUG_VIDIN)
            xpi_info(state, "%s: interface %s. trying another LUT match.\n", 
                    __func__, interface_duallink(state) ? "DL" : "SL");
    }

    if (state->flags & FLAG_XPI_REFRESH_LUT) {
        state->flags &= ~FLAG_XPI_REFRESH_LUT;
        state->current_addside_lut = NULL;
    }
    memcpy(&oldmastertimings, &mastertimings, sizeof(struct v4l2_dv_timings));
    oldstereo = stereo;
    oldinputmode = inputmode;

    if (debug & NETVIZXPI_DEBUG_VIDIN_TRACE) {
        xpi_info(state, "%s: master timings %dx%d pixclk %llu "
                "stereo %s mode %d\n", __func__, mastertimings.bt.width, 
                mastertimings.bt.height, mastertimings.bt.pixelclock, 
                (stereo) ? "yes" : "no", inputmode);
    }

    /* find LUT corresponding to current timings */
    if (inputmode < 0) {
        lutp = NULL;
        set_input_none(state);
    }
    /* configure ADV7604 for digital if input is digital */
    else if (inputmode == NETVIZXPI_MODE_HDMI) {
        /* reset S6 DCM so we can check for duallink */
        if (reset_addside_dcm(state, inputmode) < 0) {
            state->flags &= ~FLAG_XPI_DUALLINK_CHECK;
            lutp = NULL;
        }
        else if (!(state->flags & FLAG_XPI_DUALLINK_CHECK)) {
            rc = get_slave_hres(state);
            if (debug & NETVIZXPI_DEBUG_VIDIN)
                xpi_info(state, "%s: slave HRES = %d", __func__, rc);
            if (rc == mastertimings.bt.width) {
                /* remember to wait till next cycle to verify duallink */
                state->flags |= FLAG_XPI_DUALLINK_CHECK;
                if (debug & NETVIZXPI_DEBUG_VIDIN) {
                    xpi_info(state, 
                        "%s: slave HRES matches master HRES %d.\n", 
                        __func__, mastertimings.bt.width);
                }
            }
            else {
                /* slave HRES doesn't match. not duallink. */
                lutp = netvizxpi_get_lut_from_dv_timings(state, 
                        &mastertimings, stereo, 
                        debug & NETVIZXPI_DEBUG_LUT, autolut);
                if (debug & NETVIZXPI_DEBUG_VIDIN_TRACE) { 
                    xpi_info(state, 
                        "%s: slave HRES doesn't match. LUT = %s\n", 
                        __func__, (lutp == NULL) ? "NULL" : lutp->comment);
                }
            }
        }
        else {
            /* we're in the process of trying to verify duallink */
            rc = get_slave_hres(state);
            if (debug & NETVIZXPI_DEBUG_VIDIN_TRACE) { 
                xpi_info(state, 
                    "%s: trying to verify duallink. slave HRES = %d\n",
                    __func__, rc);
            }
            if (rc == mastertimings.bt.width) {
                /* slave HRES matched again. it's duallink. */
                mastertimings.bt.width *= 2;
                mastertimings.bt.hfrontporch *= 2;
                mastertimings.bt.hsync *= 2;
                mastertimings.bt.hbackporch *= 2;
                mastertimings.bt.pixelclock *= 2;
                lutp = netvizxpi_get_lut_from_dv_timings(state, 
                        &mastertimings, stereo, 
                        debug & NETVIZXPI_DEBUG_LUT, autolut);
                duallink = 1;
            }
            else {
                /* slave HRES doesn't match. not duallink. */
                lutp = netvizxpi_get_lut_from_dv_timings(state, 
                        &mastertimings, stereo, 
                        debug & NETVIZXPI_DEBUG_LUT, autolut);
            }
            state->flags &= ~FLAG_XPI_DUALLINK_CHECK;
            if (debug & NETVIZXPI_DEBUG_VIDIN_TRACE) { 
                xpi_info(state, "%s: duallink = %d. LUT = %s\n", __func__, 
                    duallink, (lutp == NULL) ? "NULL" : lutp->comment);
            }
        }
        set_input_digital(state);
    }

    /* got a LUT, no need to continue checking dual link */
    if (lutp)
        state->flags &= ~FLAG_XPI_DUALLINK_CHECK;

    /* keep searching for LUT till we find a match */
    found_lut = (lutp == NULL) ? 0 : 1;

    mutex_lock(&state->lock);

    if (!check_addside_lut_change(state, lutp)) {
        mutex_unlock(&state->lock);
        return;
    }

    if (debug & (NETVIZXPI_DEBUG_VIDIN | NETVIZXPI_DEBUG_INFO)) {
        xpi_info(state, 
            "%s: LUT changed. New LUT %d(%s), pxclk %ld stereo %d, dual %d\n", 
            __func__, (lutp) ? lutp->a_alutreg : 0, 
            (lutp) ? lutp->comment : "NULL", (lutp) ? lutp->a_lpcr : 0, 
            (lutp) ? lutp->a_stereo : 0, duallink); 
    
        if (inputmode == NETVIZXPI_MODE_STDI) {
            xpi_info(state, 
                    "%s: STDI lcf (frame height - 1) = %d, bl = %d, fcl = %d, "
                    "lcvs (vsync) = %d, %s, %chsync, %cvsync\n", __func__,
                    stdi.lcf, stdi.bl, stdi.fcl, stdi.lcvs, 
                    stdi.interlaced ? "interlaced" : "progressive", 
                    stdi.hs_pol, stdi.vs_pol);
        }
    }

    state->current_addside_lut = lutp;

    /* new LUT, check license violation */
    if (is_tx(state) && (!basic_enabled(state) || 
                (stereo && !stereo_enabled(state)) ||
                (duallink && !duallink_enabled(state)))) {
        if (!not_licensed(state))
            xpi_info(state, "%s Input not supported. No license.\n", 
                    (stereo) ? "Stereo" : ((duallink) ? "Dual Link" : ""));
        set_license_violation(state);
    }
    else {
        /* new LUT, update license violation flag */
        if (not_licensed(state)) {
            xpi_info(state, "Input supported. Removing license violation.\n");
            clear_license_violation(state);
        }

        /* new LUT, enforce pixels per second (pps) control */
        if (ppscontrol_enabled(state)) {
            enforce_ppscontrol(state, lutp);
        }

        /* new LUT, set stereo and dual link flags for output */
        if (is_tx(state) || local_mode(state) || overlay_mode(state)) { 
            if (stereo)
                state->current_interface |= INTERFACE_STEREO;
            else
                state->current_interface &= ~INTERFACE_STEREO;
            if (duallink)  
                state->current_interface |= INTERFACE_DUALLINK;
            else
                state->current_interface &= ~INTERFACE_DUALLINK;
        }
    }

    /* configure the FPGA and the chips for the new LUT */
    if (lutp != NULL) {

        /* configure ADV7604 for analog or digital */
        set_gpio_pin(GPIO_7604_RSTN, "GPIO_ADV7604_RSTn", 0);
        mdelay(5);
        set_gpio_pin(GPIO_7604_RSTN, "GPIO_ADV7604_RSTn", 1);
        adv7604_setup_videoinput(state->hdmi_adc_receiver, 
                (inputmode == NETVIZXPI_MODE_STDI) ? &mastertimings : NULL);

        /* reinitialize ADV7612 */
        if (inputmode == NETVIZXPI_MODE_HDMI) {
            v4l2_subdev_call(state->hdmi_receiver, video, s_routing, 
                    ADV7612_MODE_HDMI_GR, 0, 0);
        }

        mdelay(1);

        /* reset addside DCM since we re-configured the ADCs */
        if (reset_addside_dcm(state, inputmode) < 0) {
            state->current_addside_lut = NULL;
        }
        else {
            /* configure the appropriate sections of the FPGA */
            configure_addside(state);
        }
    } 
    mutex_unlock(&state->lock);
}

static void av_timer_timeout(unsigned long data)
{
    struct netvizxpi_state *state = (struct netvizxpi_state *) data;

    state->av_timer_count++;
    schedule_work(&state->work);
    mod_timer(&state->av_timer, jiffies + msecs_to_jiffies(1000));
}

static int netvizxpi_timer_init(struct netvizxpi_state *state)
{
    INIT_WORK(&state->work, netvizxpi_work);
    state->av_timer_count = 0;
    setup_timer(&state->av_timer, av_timer_timeout, (unsigned long) state);
    return mod_timer(&state->av_timer, jiffies + msecs_to_jiffies(1000));
}

/* ----------------------------------------------------------------------- */

static int netvizxpi_querycap(struct file *file,
                              void *priv,
                              struct v4l2_capability *cap)
{
    strcpy(cap->driver, DRIVER_NAME);
    strcpy(cap->card, DRIVER_DESCRIPTION);
    strcpy(cap->bus_info, BUS_NAME);
    cap->version = DRIVER_VERSION;
    cap->capabilities = V4L2_CAP_STREAMING;
    return 0;
}

static int netvizxpi_g_chip_ident(struct file *file,
                                  void *priv,
                                  struct v4l2_dbg_chip_ident *ident)
{
    struct netvizxpi_state *state = priv;
    ident->ident = V4L2_IDENT_BARCOXPI;
    ident->revision = get_fpga_val(state, FPGA_IDREG);
    return 0;
}

static int netvizxpi_g_ctrl(struct file *file,
                            void *priv,
                            struct v4l2_control *ctrl)
{
    struct netvizxpi_state *state = priv;
    int rc, id = ctrl->id;

    if (state->hdmi_adc_receiver == NULL)
        return -1;

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    if (id == NETVIZXPI_CID_HORIZ_POS) 
        rc = netvizxpi_adv7604_platform_data.analog_hstart;
    else if (id == NETVIZXPI_CID_VERT_POS)
        rc = netvizxpi_adv7604_platform_data.analog_vstart;
    else
        rc = v4l2_subdev_call(state->hdmi_adc_receiver, core, g_ctrl, ctrl);
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_s_ctrl(struct file *file,
                            void *priv,
                            struct v4l2_control *ctrl)
{
    struct netvizxpi_state *state = priv;
    int rc, id = ctrl->id;
    struct adv7604_analog_parameters param;

    if (state->hdmi_adc_receiver == NULL)
        return -1;
    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    if (id == NETVIZXPI_CID_HORIZ_POS) {
        netvizxpi_adv7604_platform_data.analog_hstart = ctrl->value;
        memset(&param, 0, sizeof(struct adv7604_analog_parameters));
        param.analog_hstart_set = 1;
        param.analog_hstart = ctrl->value;
        rc = adv7604_set_analog_parameters(state->hdmi_adc_receiver, &param);
    }
    else if (id == NETVIZXPI_CID_VERT_POS) {
        netvizxpi_adv7604_platform_data.analog_vstart = ctrl->value;
        memset(&param, 0, sizeof(struct adv7604_analog_parameters));
        param.analog_vstart_set = 1;
        param.analog_vstart = ctrl->value;
        rc = adv7604_set_analog_parameters(state->hdmi_adc_receiver, &param);
    } 
    else
        rc = v4l2_subdev_call(state->hdmi_adc_receiver, core, s_ctrl, ctrl);
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_s_input(struct file *file, void *priv, unsigned int i)
{
    struct netvizxpi_state *state = priv;
    int rc = -1;

    if (state->hdmi_adc_receiver == NULL)
        return -1;
    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    rc = v4l2_subdev_call(state->hdmi_adc_receiver, video, s_routing, i, 0, 0);
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_log_status(struct file *file, void *priv)
{
    struct netvizxpi_state *state = priv;
    int rc;

    if (state->hdmi_adc_receiver == NULL)
        return -1;

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    rc = v4l2_subdev_call(state->hdmi_adc_receiver, core, log_status);
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_s_dv_timings(struct file *file,
                                  void *priv,
                                  struct v4l2_dv_timings *timings)
{
    struct netvizxpi_state *state = priv;
    int rc;

    if (state->hdmi_adc_receiver == NULL)
        return -1;

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    rc = v4l2_subdev_call(state->hdmi_adc_receiver, video, s_dv_timings, 
		timings);
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_g_dv_timings(struct file *file,
                                  void *priv,
                                  struct v4l2_dv_timings *timings)
{
    struct netvizxpi_state *state = priv;
    int rc;

    if (state->hdmi_adc_receiver == NULL)
        return -1;
    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    rc = v4l2_subdev_call(state->hdmi_adc_receiver, video, g_dv_timings, 
		timings);
    mutex_unlock(&state->lock);
    return rc;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int netvizxpi_dbg_g_register(struct file *file, 
                                    void *fh,
                                    struct v4l2_dbg_register *reg) 
{
    struct video_device *vdev = video_devdata(file);
    struct netvizxpi_state *state = video_get_drvdata(vdev);
    int rc;

    rc = v4l2_device_call_until_err(&state->v4l2_dev, 0, core, g_register, reg);
    return rc;
}

static int netvizxpi_dbg_s_register(struct file *file, 
                                    void *fh,
                                    struct v4l2_dbg_register *reg) 
{
    struct video_device *vdev = video_devdata(file);
    struct netvizxpi_state *state = video_get_drvdata(vdev);
    int rc;

    rc = v4l2_device_call_until_err(&state->v4l2_dev, 0, core, s_register, reg);
    return rc;
}
#endif

static int netvizxpi_g_register(struct netvizxpi_state *state,
                                int regiotype,
                                unsigned short reg,
                                unsigned char *val)  
{
    int rc = 0;
    struct v4l2_subdev *sd = NULL;

    if (regiotype == REGIO_TYPE_ADV7604_GET) {
        if ((sd = state->hdmi_adc_receiver) == NULL)
            return -EFAULT;
    }
    else if (regiotype == REGIO_TYPE_ADV7612_GET) {
        if ((sd = state->hdmi_receiver) == NULL)
            return -EFAULT;
    }
    else if (regiotype == REGIO_TYPE_SII7172_GET ||
            regiotype == REGIO_TYPE_SII7172_SLAVE_GET) {
        if ((sd = state->hdmi_transmitter) == NULL)
            return -EFAULT;
    }

    switch(regiotype) {
    case REGIO_TYPE_ADV7604_GET:
    case REGIO_TYPE_ADV7612_GET:
    case REGIO_TYPE_SII7172_GET: 
    case REGIO_TYPE_SII7172_SLAVE_GET: {
        struct v4l2_dbg_register vreg;
        struct i2c_client *client = v4l2_get_subdevdata(sd);

        if (client == NULL) {
            rc = -EFAULT;
        }
        else {
            vreg.match.type = V4L2_CHIP_MATCH_I2C_ADDR; 
            vreg.match.addr = client->addr; 
            vreg.reg = reg;
            if (regiotype == REGIO_TYPE_SII7172_SLAVE_GET)
                vreg.reg += 0x100;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_G_REGISTER, &vreg);
            *val = (unsigned char) vreg.val;
        }
        break;
    }
    case REGIO_TYPE_MONEDID_GET: {
        *val = monedid_read(state, reg);
        break;
    }
    case REGIO_TYPE_SI5338_GET: {
        *val = si5338_read(state, reg);
        break;
    }
    case REGIO_TYPE_LM63_GET: {
        *val = lm63_read(state, reg);
        break;
    }
    case REGIO_TYPE_ICS9FG104_GET: {
        *val = ics9fg104_read(state, reg);
        break;
    }
    case REGIO_TYPE_MAX6650_GET: {
        *val = max6650_read(state, reg);
        break;
    }
    case REGIO_TYPE_EXPANSIONSWITCH_GET: {
        *val = expansionswitch_read(state);
        break;
    }
    default: {
        rc = -EINVAL;
        break;
    }
    }
    return rc;
}

static int netvizxpi_s_register(struct netvizxpi_state *state,
                                int regiotype,                          
                                unsigned short reg,
                                unsigned char val)
{
    int rc = 0;
    struct v4l2_subdev *sd = NULL;

    if (regiotype == REGIO_TYPE_ADV7604_SET) {
        if ((sd = state->hdmi_adc_receiver) == NULL)
            return -EFAULT;
    }
    else if (regiotype == REGIO_TYPE_ADV7612_SET) {
        if ((sd = state->hdmi_receiver) == NULL)
            return -EFAULT;
    }
    else if (regiotype == REGIO_TYPE_SII7172_SET ||
            regiotype == REGIO_TYPE_SII7172_SLAVE_SET) {
        if ((sd = state->hdmi_transmitter) == NULL)
            return -EFAULT;
    }

    switch(regiotype) {
    case REGIO_TYPE_ADV7604_SET:
    case REGIO_TYPE_ADV7612_SET:
    case REGIO_TYPE_SII7172_SET: 
    case REGIO_TYPE_SII7172_SLAVE_SET: {
        struct v4l2_dbg_register vreg;
        struct i2c_client *client = v4l2_get_subdevdata(sd);

        if (client == NULL) {
            rc = -EFAULT;
        }
        else {
            vreg.match.type = V4L2_CHIP_MATCH_I2C_ADDR;
            vreg.match.addr = client->addr;
            vreg.reg = reg;
            if (regiotype == REGIO_TYPE_SII7172_SLAVE_SET)
                vreg.reg += 0x100;
            vreg.val = val;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_S_REGISTER, &vreg);
        }
        break;
    }
    case REGIO_TYPE_MONEDID_SET: {
        rc = monedid_write(state, reg, val);
        break;
    }
    case REGIO_TYPE_SI5338_SET: {
        rc = si5338_write(state, reg, val);
        break;
    }
    case REGIO_TYPE_LM63_SET: {
        rc = lm63_write(state, reg, val);
        break;
    }
    case REGIO_TYPE_ICS9FG104_SET: {
        rc = ics9fg104_write(state, reg, val);
        break;
    }
    case REGIO_TYPE_MAX6650_SET: {
        rc = max6650_write(state, reg, val);
        break;
    }
    case REGIO_TYPE_EXPANSIONSWITCH_SET: {
        rc = expansionswitch_write(state, val);
        break;
    }
    default: {
        rc = -EINVAL;
        break;
    }
    }
    return rc;
}

/* ----------------------------------------------------------------------- */

static int netvizxpi_open(struct file *filp)
{
    struct netvizxpi_state *state = video_drvdata(filp);
    int rc = 0;

    filp->private_data = (void *) state;
    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    state->users++;
    mutex_unlock(&state->lock);
    return rc;
}

static int netvizxpi_close(struct file *filp) 
{
    struct netvizxpi_state *state = video_drvdata(filp);

    if (mutex_lock_interruptible(&state->lock))
        return -EINTR;
    state->users--;
    mutex_unlock(&state->lock);

    return 0;
}

static long netvizxpi_ioctl2(struct file *filp, 
                             unsigned int cmd, 
                             unsigned long data)
{
    struct netvizxpi_state *state = video_drvdata(filp); 
    unsigned long lval = 0;
    int ival = 0, rc = 0;

    switch(cmd) {
    case FPGA_IOC_MOD_REG: {
        struct reg_io regio;
        struct reg_io *regiop = (struct reg_io *) data;
        unsigned char cval = 0;
        if (regiop == NULL ||
                copy_from_user(&regio, regiop, sizeof(struct reg_io))) {
            return -EFAULT;  
        }
        switch(regio.type) {
        case REGIO_TYPE_FPGA_GET:
            regio.value = get_fpga_val(state, regio.reg);         
            break;
        case REGIO_TYPE_FPGA_SET:
            set_fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_FPGA_ANDIN:
            and_in_fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_FPGA_ORIN:
            or_in_fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_ADV7604_GET:
        case REGIO_TYPE_ADV7612_GET:
        case REGIO_TYPE_SII7172_GET:
        case REGIO_TYPE_MONEDID_GET:
        case REGIO_TYPE_SI5338_GET:
        case REGIO_TYPE_LM63_GET:
        case REGIO_TYPE_ICS9FG104_GET:
        case REGIO_TYPE_MAX6650_GET:
        case REGIO_TYPE_EXPANSIONSWITCH_GET:
        case REGIO_TYPE_SII7172_SLAVE_GET:
            rc = netvizxpi_g_register(state, regio.type, regio.reg, &cval);
            if (rc == 0)
                regio.value = cval;
            break;
        case REGIO_TYPE_S6FPGA_GET:
            regio.value = get_s6fpga_val(state, regio.reg);         
            break;
        case REGIO_TYPE_S6FPGA_SET:
            set_s6fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_S6FPGA_ANDIN:
            and_in_s6fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_S6FPGA_ORIN:
            or_in_s6fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_S3FPGA_GET:
            regio.value = get_s3fpga_val(state, regio.reg);         
            break;
        case REGIO_TYPE_S3FPGA_SET:
            set_s3fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_S3FPGA_ANDIN:
            and_in_s3fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_S3FPGA_ORIN:
            or_in_s3fpga_val(state, regio.reg, regio.value);         
            break;
        case REGIO_TYPE_MAC_GET:
            regio.value = get_mac_val(state, regio.reg);
            break;
        case REGIO_TYPE_MAC_SET:
            set_mac_val(state, regio.reg, regio.value);
            break;
        case REGIO_TYPE_ADV7604_SET:
        case REGIO_TYPE_ADV7612_SET:
        case REGIO_TYPE_SII7172_SET:
        case REGIO_TYPE_MONEDID_SET:
        case REGIO_TYPE_SI5338_SET:
        case REGIO_TYPE_LM63_SET:
        case REGIO_TYPE_ICS9FG104_SET:
        case REGIO_TYPE_MAX6650_SET:
        case REGIO_TYPE_EXPANSIONSWITCH_SET:
        case REGIO_TYPE_SII7172_SLAVE_SET:
            cval = (unsigned char) regio.value;
            rc = netvizxpi_s_register(state, regio.type, regio.reg, cval);
            break;
        default:
            xpi_warn(state, "%s: unsupported IOCTL\n", __func__);
            return -EFAULT;
        }
        if (copy_to_user((struct reg_io *)data, 
                    &regio, sizeof(struct reg_io)) != 0)
            return -EFAULT;
        break;
    }     
    case FPGA_IOC_MOD_OMNITEK: {
        struct omnitek_io omniio;
        struct omnitek_io *omniiop = (struct omnitek_io *) data;
        if (omniiop == NULL ||
                copy_from_user(&omniio, omniiop, sizeof(struct omnitek_io))) {
            return -EFAULT;  
        }
        switch(omniio.type) {
        case OMNITEKIO_TYPE_GET:
            omniio.value = get_omnitek_val(state, omniio.barno, omniio.reg); 
            break;
         case OMNITEKIO_TYPE_SET:
            set_omnitek_val(state, omniio.barno, omniio.reg, omniio.value);
            break;
         case OMNITEKIO_TYPE_ANDIN:
            and_in_omnitek_val(state, omniio.barno, omniio.reg, omniio.value);
            break;
         case OMNITEKIO_TYPE_ORIN:
            or_in_omnitek_val(state, omniio.barno, omniio.reg, omniio.value);
            break;
        }
        if (copy_to_user((struct omnitek_io *)data,
                     &omniio, sizeof(struct omnitek_io)) != 0) {
            return -EFAULT;
        }
        break;
    }
    case FPGA_IOC_MOD_EXPANSION: {
        struct expansion_io expio;
        struct expansion_io *expiop = (struct expansion_io *) data;
        if (expiop == NULL ||
                copy_from_user(&expio, expiop, sizeof(struct expansion_io))) {
            return -EFAULT;  
        }
        switch(expio.type) {
        case EXPANSIONIO_TYPE_GET:
            expio.value = expansion_smbus_read_byte_data(state, expio.port, 
                    expio.addr, expio.reg);
            break;
         case EXPANSIONIO_TYPE_SET:
            expansion_smbus_write_byte_data(state, expio.port, expio.addr, 
                    expio.reg, expio.value);
            break;
        }
        if (copy_to_user((struct expansion_io *)data, &expio, 
                    sizeof(struct expansion_io)) != 0) {
            return -EFAULT;
        }
        break;
    }

    case FPGA_IOC_MOD_PAGEDMDIO: {
        struct pagedmdio_io pmdio;
        struct pagedmdio_io *pmdiop = (struct pagedmdio_io *) data;
        if (pmdiop == NULL ||
                copy_from_user(&pmdio, pmdiop, sizeof(struct pagedmdio_io))) {
            return -EFAULT;  
        }
        switch(pmdio.type) {
        case PAGED_MAINMDIO_TYPE_GET:
            pmdio.value = get_paged_mdio_val(state, MAIN_OFFSET, XPI_MDIO_PHYAD,
                    pmdio.page, pmdio.reg);
            break;
        case PAGED_EXPANSIONMDIO_TYPE_GET:
            pmdio.value = get_paged_mdio_val(state, EXPANSION_OFFSET, 
                    XPI_MDIO_PHYAD, pmdio.page, pmdio.reg);
            break;
        case PAGED_MAINMDIO_TYPE_SET:
            set_paged_mdio_val(state, MAIN_OFFSET, XPI_MDIO_PHYAD,
                    pmdio.page, pmdio.reg, pmdio.value);
            break;
        case PAGED_EXPANSIONMDIO_TYPE_SET:
            set_paged_mdio_val(state, EXPANSION_OFFSET, XPI_MDIO_PHYAD,
                    pmdio.page, pmdio.reg, pmdio.value);
            break;
        }
        if (copy_to_user((struct pagedmdio_io *)data, &pmdio, 
                    sizeof(struct pagedmdio_io)) != 0) {
            return -EFAULT;
        }
        break;
    }

    case FPGA_IOC_RESET_LUT: {
        reset_lut_table(state);
        break;
    }

    case FPGA_IOC_SET_LUT: {
        struct LUT_struct lut, *lutp = (struct LUT_struct *) data;
        if (lutp == NULL) {
            rc = -EINVAL;
        }
        else {
            if (copy_from_user(&lut, lutp, sizeof(struct LUT_struct))) {
                rc = -EFAULT;
            }
            else
                rc = add_lut_entry(state, &lut);
        }
        break;
    }

    case FPGA_IOC_GET_LUT: {
        struct LUT_struct lut, *lutp = (struct LUT_struct *) data;
        if (lutp == NULL)
            rc = -EINVAL;
        else {
            if (copy_from_user(&lut, lutp, sizeof(struct LUT_struct)))
                rc = -EFAULT;
            else {
                if (get_lut_entry(state, &lut) < 0)
                    return -EFAULT;
                if (copy_to_user((struct LUT_struct *)data,
                             &lut, sizeof(struct LUT_struct)) != 0)
                    rc = -EFAULT;
            }
        }
        break;
    }

    case FPGA_IOC_SET_ADD_DRIVERIDLE:
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) {
            return -EFAULT;
        }

        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        if (ival)
            state->flags &= ~FLAG_XPI_DRIVER_ENABLE;
        else
            state->flags |= FLAG_XPI_DRIVER_ENABLE;
        mutex_unlock(&state->lock);

        break;
        
    case FPGA_IOC_LOCK_ADDSIDELUT:
    case FPGA_IOC_LOCK_DROPSIDELUT:
    case FPGA_IOC_LOCK_DISPLAYLUT:
        break;

    case FPGA_IOC_RESET_ADDSIDE:
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        configure_addside(state);
        mutex_unlock(&state->lock);
        break;

    case FPGA_IOC_GET_ADDSIDELUT: {
        struct LUT_struct *lutp = (struct LUT_struct *) data;
        if (lutp == NULL) {
            rc = -EINVAL;
        }
        else {
            if (state->current_addside_lut == NULL) 
                rc = -EFAULT;
            else if (copy_to_user((struct LUT_struct *)data, 
                        state->current_addside_lut, sizeof(struct LUT_struct)))
                rc = -EFAULT;
        }
        break;
    }

    case FPGA_IOC_SET_ADDSIDELUT: {
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) 
            return -EFAULT;
        if (ival) {
            if (mutex_lock_interruptible(&state->lock))
                return -EINTR;
            state->current_addside_lut = get_lut_from_lutid(state, ival);
            configure_addside(state);
            mutex_unlock(&state->lock);
        }
        break;
    }

    case FPGA_IOC_RESET_DROPSIDE:
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        configure_dropside(state);
        mutex_unlock(&state->lock);
        break;

    case FPGA_IOC_GET_DROPSIDELUT: { 
        struct LUT_struct *lutp = (struct LUT_struct *) data;

        if (lutp == NULL) {
            rc = -EINVAL;
        }
        else {
            if (state->current_dropside_lut == NULL) 
                rc = -EFAULT;
            else if (copy_to_user((struct LUT_struct *)data, 
                        state->current_dropside_lut, sizeof(struct LUT_struct)))
                rc = -EFAULT;
        }
        break;
    }
    case FPGA_IOC_SET_DROPSIDELUT: {
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;

        if ((int *)data == NULL) {
            if (state->current_dropside_lut) {
                kfree(state->current_dropside_lut);
                state->current_dropside_lut = NULL;
            }
        }
        else {
            if (state->current_dropside_lut == NULL)
                state->current_dropside_lut = 
                    kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
            if (state->current_dropside_lut == NULL) 
                rc = -ENOMEM;
            else {
                if (copy_from_user(state->current_dropside_lut, 
                    (struct LUT_struct *) data, sizeof(struct LUT_struct))) 
                    rc = -EFAULT;
                else {
                    compute_actual_overlay_params(state, &state->oinfo, 
                            &state->ocfg);
                    configure_dropside(state);
                }
            }
        }
        mutex_unlock(&state->lock);

        break;
    }

    case FPGA_IOC_GET_DISPLAYLUT: {
        struct LUT_struct *lutp = (struct LUT_struct *) data;

        if (lutp == NULL) {
            rc = -EINVAL;
        }
        else {
            if (state->current_display_lut == NULL) 
                rc = -EFAULT;
            else if (copy_to_user((struct LUT_struct *)data, 
                        state->current_display_lut, sizeof(struct LUT_struct)))
                rc = -EFAULT;
        }
        break;
    }

    case FPGA_IOC_SET_DISPLAYLUT: 
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;

        if ((int *)data == NULL) {
            if (state->current_display_lut) {
                kfree(state->current_display_lut);
                state->current_display_lut = NULL;
            }
        }
        else {
            if (state->current_display_lut == NULL)
                state->current_display_lut = 
                    kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
            if (state->current_display_lut == NULL) 
                rc = ENOMEM;
            else {
                if (copy_from_user(state->current_display_lut, 
                        (struct LUT_struct *) data, sizeof(struct LUT_struct))) 
                    rc = -EFAULT;
                else {
                    compute_actual_overlay_params(state, &state->oinfo, 
                            &state->ocfg);
                    configure_dropside(state);
                }
            }
        }
        mutex_unlock(&state->lock);
        break;

    case FPGA_IOC_RESET_OUTPUT:
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        configure_output_registers(state, interface_duallink(state), 0);
        mutex_unlock(&state->lock);
        break;

    case FPGA_IOC_SET_SPLASH: {
        struct LUT_struct lut;
        struct LUT_struct *lutp = (struct LUT_struct *) data;
        if (lutp == NULL ||
                copy_from_user(&lut, lutp, sizeof(struct LUT_struct))) {
            return -EFAULT;
        }
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        if (is_tx(state)) {
            configure_addside_splash(state, &lut);
        }
        else {
            if (state->current_dropside_lut == NULL) {
                state->current_dropside_lut = 
                        kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
            }
            if (state->current_dropside_lut == NULL) {
                mutex_unlock(&state->lock);
                return -ENOMEM;
            }
            memcpy(state->current_dropside_lut, &lut, sizeof(struct LUT_struct));

            if (state->current_display_lut == NULL) {
                state->current_display_lut = 
                    kzalloc(sizeof(struct LUT_struct), GFP_KERNEL);
            }
            if (state->current_display_lut == NULL) {
                rc = -ENOMEM;
            }
            else {
                if (state->oinfo.flags & OV_FLAG_SRC_REMOTE) 
                    memcpy(state->current_display_lut, &lut, 
                            sizeof(struct LUT_struct));
                else if (state->current_addside_lut == NULL)
                    rc = -EFAULT;
                else 
                    memcpy(state->current_display_lut, state->current_addside_lut, 
                        sizeof(struct LUT_struct));
                compute_actual_overlay_params(state, &state->oinfo, &state->ocfg);
                configure_dropside_splash(state, &lut);
            }
        }
        mutex_unlock(&state->lock);
        break;
    }

    case FPGA_IOC_GET_FPGA_FLAGS:
        if ((int *)data == NULL)
            return -EFAULT;
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        ival = state->flags;
        mutex_unlock(&state->lock);
        if (copy_to_user((int *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_FPGA_FLAGS:
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) { 
            return -EFAULT;
        }
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        state->flags &= ~FLAG_XPI_SETMASK;
        state->flags |= (ival & FLAG_XPI_SETMASK);
        mutex_unlock(&state->lock);
        if (!ppscontrol_enabled(state)) 
            remove_ppscontrol(state);
        break;

    case FPGA_IOC_SET_LOGLVL:
        break;

    case FPGA_IOC_SET_PPSCAPACITY:
        if ((unsigned long *) data == NULL || 
                copy_from_user(&lval, (unsigned long *) data, 
                    sizeof(unsigned long))) {
            return -EFAULT;
        }
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        state->pps_capacity = lval;
        mutex_unlock(&state->lock);
        break;

    case FPGA_IOC_SET_FREE_RUN: {
        if ((int *) data == NULL || 
                copy_from_user(&ival, (int *) data, sizeof(int))) {
            return -EFAULT;
        }
        if (mutex_lock_interruptible(&state->lock))
            return -EINTR;
        force_free_run(state, ival);
        mutex_unlock(&state->lock);
        break;
    }
    case FPGA_IOC_LOG_STATUS:
    case FPGA_IOC_INTR_FIRE: {
        struct v4l2_subdev  *sd;
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) {
            return -EFAULT;
        }
        if (ival != V4L2_SUBDEV_SII7172 && ival != V4L2_SUBDEV_ADV7604 &&
                    ival != V4L2_SUBDEV_ADV7612)
            return -EINVAL;

        sd = (ival == V4L2_SUBDEV_SII7172) ? state->hdmi_transmitter :
                ((ival == V4L2_SUBDEV_ADV7604) ? state->hdmi_adc_receiver:
                state->hdmi_receiver);
        if (cmd == FPGA_IOC_LOG_STATUS)
            rc = v4l2_subdev_call(sd, core, log_status);
        else
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_FIRE_INTERRUPT, NULL);
        break; 
    }
    case FPGA_IOC_QUERY_STDI: {
        struct stdi_io stdiio;
        struct stdi_io *stdiiop = (struct stdi_io *) data;

        if (stdiiop == NULL || 
                copy_from_user(&stdiio, stdiiop, sizeof(struct stdi_io)))
            return -EFAULT;
        else if (stdiio.subdev == V4L2_SUBDEV_ADV7604) {
            adv7604_g_stdi_readback(state->hdmi_adc_receiver, &(stdiio.stdi));
        }
        else
            return -EFAULT;
        if (copy_to_user((struct stdi_io *)data, &stdiio, 
                    sizeof(struct stdi_io)) != 0) 
            return -EFAULT;

        break;
    }
    case FPGA_IOC_SUBDEV_GET_EDID: {
        struct edid_io edidio;
        struct edid_io *edidiop = (struct edid_io *) data;
        struct v4l2_subdev *sd;
        struct v4l2_edid v4l2edid;

        if (edidiop == NULL ||
                copy_from_user(&edidio, edidiop, sizeof(struct edid_io))) {
            return -EFAULT;  
        }
        else if (edidio.subdev == V4L2_SUBDEV_ADV7604) {
            if ((sd = state->hdmi_adc_receiver) == NULL) {
                return -ENODEV;
            }
            v4l2edid.edid = edidio.edid;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_G_EDID, &v4l2edid);
        }
        else if (edidio.subdev == V4L2_SUBDEV_ADV7612) {
            if ((sd = state->hdmi_receiver) == NULL) {
                return -ENODEV;
            }
            v4l2edid.edid = edidio.edid;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_G_EDID, &v4l2edid);
        }
        else {
            return -EFAULT;
        }
        if (rc || copy_to_user((struct edid_io *)data,
                     &edidio, sizeof(struct edid_io)) != 0) {
            return -EFAULT;
        }
        break; 
    }
    case FPGA_IOC_SUBDEV_SET_EDID: {
        struct edid_io edidio;
        struct edid_io *edidiop = (struct edid_io *) data;
        struct v4l2_subdev *sd;
        struct v4l2_edid v4l2edid;

        if (edidiop == NULL ||
                copy_from_user(&edidio, edidiop, sizeof(struct edid_io))) {
            return -EFAULT;  
        }
        else if (edidio.subdev == V4L2_SUBDEV_ADV7604) {
            if ((sd = state->hdmi_adc_receiver) == NULL) {
                return -ENODEV;
            }
            v4l2edid.edid = edidio.edid;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_S_EDID, &v4l2edid);
        }
        else if (edidio.subdev == V4L2_SUBDEV_ADV7612) {
            if ((sd = state->hdmi_receiver) == NULL) {
                return -ENODEV;
            }
            v4l2edid.edid = edidio.edid;
            rc = v4l2_subdev_call(sd, core, ioctl, V4L2_S_EDID, &v4l2edid);
        }
        else {
            return -EFAULT;
        }
        break;
    }
    case FPGA_IOC_GET_MONEDID: {
        struct edid_io edidio;
        struct edid_io *edidiop = (struct edid_io *) data;

        if (edidiop == NULL) {
            return -EFAULT;  
        }
        else if (state->i2c_adapters[MONEDID_I2C_BUS]) {
            rc = monedid_read_block(state, sizeof(edidio.edid), edidio.edid);
        }
        else {
            return -EFAULT;
        }
        if (rc || copy_to_user((struct edid_io *)data,
                     &edidio, sizeof(struct edid_io)) != 0) {
            return -EFAULT;
        }
        break; 
    }
    case FPGA_IOC_SUBDEV_S_ROUTING: {
        struct routing_io rio;
        struct routing_io *riop = (struct routing_io *) data;
        struct v4l2_subdev *sd;

        if (riop == NULL ||
                copy_from_user(&rio, riop, sizeof(struct routing_io))) {
            return -EFAULT;  
        }
        if (rio.subdev == V4L2_SUBDEV_ADV7604) {
            if ((sd = state->hdmi_adc_receiver) == NULL) {
                return -ENODEV;
            }
            return v4l2_subdev_call(state->hdmi_adc_receiver, video, s_routing, 
                rio.input, rio.output, rio.config);
        }
        else if (rio.subdev == V4L2_SUBDEV_ADV7612) {
            if ((sd = state->hdmi_receiver) == NULL) {
                return -ENODEV;
            }
            return v4l2_subdev_call(state->hdmi_receiver, video, s_routing, 
                    (rio.input == NETVIZXPI_MODE_HDMI) ? 
                    ADV7612_MODE_HDMI_GR : ADV7612_MODE_HDMI_COMP, 
                    rio.output, rio.config);
        }
        return -EFAULT;
    }

    case FPGA_IOC_GET_VID_AVL:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = video_available(state);
        if (copy_to_user((int *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_VID_AVL:
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) 
            return -EFAULT;
        if (ival) 
            state->current_interface |= INTERFACE_ADDSIDE_VIDEO_UNAVAIL;
        else
            state->current_interface &= ~INTERFACE_ADDSIDE_VIDEO_UNAVAIL;
        return 0;

    case FPGA_IOC_GET_INTERFACE:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = state->current_interface;
        if (copy_to_user((int *)data, &ival, sizeof(int)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_SET_INTERFACE:
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) 
            return -EFAULT;
        set_interface(state, ival);
        break;

    case FPGA_IOC_GET_DISPLAY_HPOL:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = (state->current_interface & INTERFACE_HSYNC_INVERT) ? 1 : 0;
        if (copy_to_user((int *)data, &ival, sizeof(int)))
            return -EFAULT;
        break;

    case FPGA_IOC_SET_DISPLAY_HPOL:
        if ((int *)data == NULL || 
                copy_from_user(&ival, (int *)data, sizeof(int)))
            return -EFAULT;
        if (ival) 
            state->current_interface |= INTERFACE_HSYNC_INVERT;
        else
            state->current_interface &= ~INTERFACE_HSYNC_INVERT;
        break;

    case FPGA_IOC_GET_DISPLAY_VPOL:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = (state->current_interface & INTERFACE_VSYNC_INVERT) ? 1 : 0;
        if (copy_to_user((int *)data, &ival, sizeof(int)))
            return -EFAULT;
        break;

    case FPGA_IOC_SET_DISPLAY_VPOL:
        if ((int *)data == NULL || 
                copy_from_user(&ival, (int *)data, sizeof(int)))
            return -EFAULT;
        if (ival) 
            state->current_interface |= INTERFACE_VSYNC_INVERT;
        else
            state->current_interface &= ~INTERFACE_VSYNC_INVERT;
        break;

    case FPGA_IOC_SET_ASLICEDROP: {
        int retries;
        if ((int *) data == NULL ||
                copy_from_user(&ival, (int *) data, sizeof(int))) 

        if ((int *)data == NULL || 
                copy_from_user(&ival, (int *)data, sizeof(int))) {
            return -EFAULT;
        }
        /* PSCW -- check w/Piyush & Pavan */
        for (retries = 0; retries < 3; retries++) {
            if (get_fpga_val(state, FPGA_IFRM_CTLSTAT) & IFRM_CTLSTAT_VALID_BIT)
                udelay(10);
            else 
                break;
        }
        if (retries >= 3)  
            return -EFAULT;
        or_in_fpga_val(state, FPGA_IFRM_RESET, IFRM_RESET_FRAME);
        udelay(10);
        and_in_fpga_val(state, FPGA_IFRM_RESET, ~IFRM_RESET_FRAME);
        set_fpga_val(state, FPGA_IFRM_FRMCNT, ival + 1);
        or_in_fpga_val(state, FPGA_IFRM_SELECT, IFRM_SELECT_FRAME);
        set_fpga_val(state, FPGA_IFRM_CTLSTAT, IFRM_CTLSTAT_VALID_BIT);

        return 0;
    }        

    case FPGA_IOC_GET_ACNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = (get_fpga_val(state, FPGA_ACNOISET) & ~ACNOISE_MASK) >> 
            ACNOISE_SHIFT;
        if (copy_to_user((int *)data, &ival, sizeof(int)))
            return -EFAULT;
        break;
        
    case FPGA_IOC_SET_ACNOISET:
        if ((int *)data == NULL || 
                copy_from_user(&ival, (int *)data, sizeof(int)))
            return -EFAULT;
        set_fpga_val(state, FPGA_ACNOISET, 
                (ival & ~ACNOISE_MASK) << ACNOISE_SHIFT);
        break;

    case FPGA_IOC_GET_DCNOISET:
        if ((int *)data == NULL)
            return -EFAULT;
        ival = (get_fpga_val(state, FPGA_ACNOISET) & DCNOISE_MASK) >> 
            DCNOISE_SHIFT;
        if (copy_to_user((int *)data, &ival, sizeof(int)))
            return -EFAULT;
        break;
        
    case FPGA_IOC_SET_DCNOISET:
        if ((int *)data == NULL || 
                copy_from_user(&ival, (int *)data, sizeof(int)))
            return -EFAULT;
        set_fpga_val(state, FPGA_DCNOISET, 
                (ival & ~DCNOISE_MASK) << DCNOISE_SHIFT);
        break;

    case FPGA_IOC_GET_HARDWARE_INFO:
        {
            struct hardware_info hw_info;
            if ((struct hardware_info *)data == NULL)
                return -EFAULT;
            hw_info.hardware_type = XPI_DEVICE;
            hw_info.board_type = (state->flags & FLAG_XPI_TX) ? 
                HARDWARE_V2D_TX : HARDWARE_V2D_RX;
            hw_info.hardware_flags = state->flags;
            hw_info.hardware_detected = 
                (state->flags & FLAG_XPI_DRIVER_ENABLE) ? 1 : 0;
            hw_info.number_of_boards = 1;
            if (copy_to_user((struct hardware_info *)data, 
                        &hw_info, sizeof(struct hardware_info)) != 0) 
                return -EFAULT;
        }
        break;

    case FPGA_IOC_GET_OV_PARAM:
        {
            struct ov_param_io *ov;
            if ((int *)data == NULL)
                return -EFAULT;
            ov = &state->ocfg;
            if (copy_to_user((struct ov_param_io *)data,
                            ov, sizeof(struct ov_param_io)) != 0)
            return -EFAULT;
        }
        break;

    case FPGA_IOC_SET_OV_PARAM:
        {
            struct ov_param_io *ov, a_ov;
            if ((int *)data == NULL)
                return -EFAULT;
            ov = &state->ocfg;
            if (copy_from_user(ov, (struct ov_param_io *)data,
                         sizeof(struct ov_param_io)) != 0) {
                return -EFAULT;
            }

            if (mutex_lock_interruptible(&state->lock))
                return -EINTR;
            if (ov->setmask & OV_SET_PARAM_CROP) {
                if (ov->flags & OV_FLAG_CROP)
                    state->oinfo.flags |= OV_FLAG_CROP;
                else
                    state->oinfo.flags &= ~OV_FLAG_CROP;
                state->oinfo.hcropleft = ov->hcropleft;
                state->oinfo.hcropright = ov->hcropright;
                state->oinfo.vcroptop = ov->vcroptop;
                state->oinfo.vcropbottom = ov->vcropbottom;
            }
            if (ov->setmask & OV_SET_PARAM_OVERLAY) {
                compute_actual_overlay_params(state, &state->oinfo, 
                        &state->ocfg);
                configure_dropside(state);
            }
            else {
                if (ov->setmask & OV_SET_PARAM_POSITION) {
                    compute_actual_overlay_params(state, &a_ov, ov);
                    set_fpga_val(state, FPGA_DHSHIFT, a_ov.hoffset);
                    set_fpga_val(state, FPGA_DVSHIFT, a_ov.voffset);
                    state->oinfo.hoffset = a_ov.hoffset;
                    state->oinfo.voffset = a_ov.voffset;
                }
                if (ov->setmask & OV_SET_PARAM_ALPHA) {
                    int oalpha = (1024 * ov->oalpha) / 100;
                    set_fpga_val(state, FPGA_DOVLPALPHA1, 1024 - oalpha);
                    set_fpga_val(state, FPGA_DOVLPALPHA2, oalpha);
                    state->oinfo.oalpha = oalpha;
                }
            }
            mutex_unlock(&state->lock);
        }
        break;

    case FPGA_IOC_GET_OV_INFO:
        {
            struct ov_param_io *ov;
            if ((int *)data == NULL)
                return -EFAULT;
            ov = &state->oinfo;
            if (copy_to_user((struct ov_param_io *)data,
                            ov, sizeof(struct ov_param_io)) != 0)
            return -EFAULT;
        }
        break;

    case FPGA_IOC_PC_PARAM_GET:
        {
            struct pc_param_io pcpio;
            struct pc_param_io *pcpiop = (struct pc_param_io *) data;

            if ((pcpiop == NULL) || (copy_from_user(&pcpio, pcpiop, 
                            sizeof(struct pc_param_io))))
                return -EFAULT;
            // PSCW - Fix this when FPGA implements progressive compression
            pcpio.pc_xblocks = 0;
            pcpio.pc_ylow = 0;
            pcpio.pc_yhigh = 0;
            pcpio.pc_rlow = get_fpga_val(state, FPGA_ARSLICEREG);
            pcpio.pc_rhigh = get_fpga_val(state, FPGA_MAXREFRESH);
            if (copy_to_user((struct pc_param_io *)data, 
                        &pcpio, sizeof(struct pc_param_io)) != 0)
                return -EFAULT;
        }
        break;

    case FPGA_IOC_PC_PARAM_SET:
        {
            struct pc_param_io pcpio;
            struct pc_param_io *pcpiop = (struct pc_param_io *) data;

            if ((pcpiop == NULL) || (copy_from_user(&pcpio, pcpiop, 
                        sizeof(struct pc_param_io))))
                return -EFAULT;
            /* Only min and max refresh are supported on the XPi */
            if (pcpio.setmask & PC_SET_PARAM_RLOW)
                set_fpga_val(state, FPGA_ARSLICEREG, pcpio.pc_rlow);
            if (pcpio.setmask & PC_SET_PARAM_RHIGH)
                set_fpga_val(state, FPGA_MAXREFRESH, pcpio.pc_rhigh);
        }
        break;
    case FPGA_IOC_RESET:
        {
            /* cause panic */
            xpi_info(state, "FPGA_IOC_RESET: forcing kernel panic...\n");
            panic("Oh no!");
        }
    default:
        rc = video_ioctl2(filp, cmd, data);
        break;
    }    

    return rc;
}

static void netvizxpi_notify(struct v4l2_subdev *sd, 
                             unsigned int notification, 
                             void * arg)  
{
    int dbg = (debug & NETVIZXPI_DEBUG_INFO) ? 2 : 0;
    switch(notification) {
    case ADV7604_HOTPLUG:
        v4l2_dbg(2, dbg, sd, "ADV7604_HOTPLUG\n");
        break;
    case ADV7604_FMT_CHANGE:
        v4l2_dbg(1, dbg, sd, "ADV7604_FMT_CHANGE\n");
        break;
    case ADV7604_FMT_CHANGE_DIG:
        v4l2_dbg(1, dbg, sd, "ADV7604_FMT_CHANGE_DIG\n");
        break;
    case ADV7604_AUDIO_CHANGE:
        v4l2_dbg(1, dbg, sd, "ADV7604_AUDIO_CHANGE\n");
        break;
    case ADV7604_CABLE_DETECT:
        v4l2_dbg(1, dbg, sd, "ADV7604_CABLE_DETECT\n");
        break;
    case ADV7612_HOTPLUG:
        v4l2_dbg(1, dbg, sd, "ADV7612_HOTPLUG\n");
        break;
    case ADV7612_FMT_CHANGE:
        v4l2_dbg(1, dbg, sd, "ADV7612_FMT_CHANGE\n");
        break;
    case ADV7612_CABLE_DETECT:
        v4l2_dbg(1, dbg, sd, "ADV7612_CABLE_DETECT\n");
        break;
    case ADV7612_AUDIO_CHANGE:
        v4l2_dbg(1, dbg, sd, "ADV7612_AUDIO_CHANGE\n");
        break;
    case SII7172_RESET:
        v4l2_dbg(1, dbg, sd, "SII7172_RESET\n");
        break;
    case SII7172_MONITOR_DETECT:
        v4l2_dbg(1, dbg, sd, "SII7172_MONITOR_DETECT\n");
        break;
    default:
        v4l2_err(sd, "%s: invalid notification 0x%x\n", __func__, 
                notification); 
        break;
    }
}

/* ------------------------------------------------------------------------- */

static int proc_print_version(struct netvizxpi_state *state, 
        char *filebuf, int len)
{
    int rc = len;

    rc += sprintf(filebuf + rc, "netvizxpi_module (compiled %s %s)",
            __DATE__, __TIME__);

    return rc;
}

static int netvizxpi_procfile_version_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
    struct netvizxpi_state *state = (struct netvizxpi_state *) data;
    int writelen;
    static int len = 0;
    static char *filebuf;

    if (off > 0)
        goto showdata;

    filebuf = kzalloc(PROC_FILE_SIZE, GFP_KERNEL);
    if (filebuf == NULL)
        goto showdata;

    len += proc_print_version(state, filebuf, len);
    len += sprintf(filebuf + len, "\n");

showdata:
    if (filebuf == NULL) {
        *eof = 1;
        return 0;
    }
    writelen = count > len ? len : count;
    memcpy(page + off, filebuf + off, writelen);
    len -= writelen;
    if (len == 0) {
        kfree(filebuf);
        filebuf = NULL;
        *eof = 1;
    }
    *start = page + off;
    return writelen;
}

static int proc_print_flags_and_lut(struct netvizxpi_state *state,
        char *filebuf, int len)
{
    int rc = len;
    rc += sprintf(filebuf + rc,
        "\n************     Flag, Interface & Look Up Tables     ************");
    rc += sprintf(filebuf + rc, "\nFlags               = 0x%08x\n", 
            state->flags);
    rc += sprintf(filebuf + rc, "Interface           = 0x%08x\n", 
            state->current_interface);
    rc += sprintf(filebuf + rc, "PPS capacity        = %ld\n", 
            state->pps_capacity);

    rc += sprintf(filebuf + rc, "Addside LUT         = %d (%s)\n", 
            (state->current_addside_lut == NULL) ? 0 : 
            state->current_addside_lut->a_alutreg,
            (state->current_addside_lut == NULL) ? "NULL" : 
            state->current_addside_lut->comment);
    rc += sprintf(filebuf + rc, "Dropside LUT        = %d (%s)\n", 
            (state->current_dropside_lut == NULL) ? 0 : 
            state->current_dropside_lut->a_alutreg,
            (state->current_dropside_lut == NULL) ? "NULL" : 
            state->current_dropside_lut->comment);
    rc += sprintf(filebuf + rc, "Display LUT         = %d (%s)\n", 
            (state->current_display_lut == NULL) ? 0 : 
            state->current_display_lut->a_alutreg,
            (state->current_display_lut == NULL) ? "NULL" : 
            state->current_display_lut->comment);
    return rc;
}

static int proc_print_overlay_information(struct netvizxpi_state *state,
        char *filebuf, int len)
{
    int rc = len;
    rc += sprintf(filebuf + rc,
        "\n*******************     Overlay Information    *******************");
    rc += sprintf(filebuf + rc, "\nOVC flags           = 0x%08x  ",
            state->ocfg.flags);
    rc += sprintf(filebuf + rc, "OVI flags           = 0x%08x\n",
            state->oinfo.flags);
    rc += sprintf(filebuf + rc, "OVC scale           = %03d/%03d     ",
            state->ocfg.hscale, state->ocfg.vscale);
    rc += sprintf(filebuf + rc, "OVI scale           = %03d/%03d\n",
            state->oinfo.hscale, state->oinfo.vscale);
    rc += sprintf(filebuf + rc, "OVC location        = %04d/%04d   ",
            state->ocfg.hlocation, state->ocfg.vlocation);
    rc += sprintf(filebuf + rc, "OVI location        = %04d/%04d\n",
            state->oinfo.hlocation, state->oinfo.vlocation);
    rc += sprintf(filebuf + rc, "OVC offset          = %04d/%04d   ",
            state->ocfg.hoffset, state->ocfg.voffset);
    rc += sprintf(filebuf + rc, "OVI offset          = %04d/%04d\n",
            state->oinfo.hoffset, state->oinfo.voffset);
    rc += sprintf(filebuf + rc, "OVI hcrop           = %04d/%04d   ",
            state->oinfo.hcropleft, state->oinfo.hcropright);
    rc += sprintf(filebuf + rc, "OVI vcrop           = %04d/%04d\n",
            state->oinfo.vcroptop, state->oinfo.vcropbottom);
    return rc;
}

static int netvizxpi_procfile_lut_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
    struct netvizxpi_state *state = (struct netvizxpi_state *) data;
    int writelen;
    static int len = 0;
    static char *filebuf;

    if (off > 0)
        goto showdata;

    filebuf = kzalloc(PROC_FILE_SIZE, GFP_KERNEL);
    if (filebuf == NULL)
        goto showdata;

    len += proc_print_flags_and_lut(state, filebuf, len);
    if (!is_tx(state)) {
        len += proc_print_overlay_information(state, filebuf, len);
    }
    len += sprintf(filebuf + len, "\n");

showdata:
    if (filebuf == NULL) {
        *eof = 1;
        return 0;
    }
    writelen = count > len ? len : count;
    memcpy(page + off, filebuf + off, writelen);
    len -= writelen;
    if (len == 0) {
        kfree(filebuf);
        filebuf = NULL;
        *eof = 1;
    }
    *start = page + off;
    return writelen;
}

/* ------------------------------------------------------------------------- */

static const struct v4l2_file_operations netvizxpi_fops = {
    .owner          = THIS_MODULE,
    .open           = netvizxpi_open,
    .release        = netvizxpi_close,
    .unlocked_ioctl = netvizxpi_ioctl2,
};

static const struct v4l2_ioctl_ops netvizxpi_ioctl_ops = {
    .vidioc_querycap            = netvizxpi_querycap,
    .vidioc_g_chip_ident        = netvizxpi_g_chip_ident,
    .vidioc_g_ctrl              = netvizxpi_g_ctrl,
    .vidioc_s_ctrl              = netvizxpi_s_ctrl,
    .vidioc_s_input             = netvizxpi_s_input,
    .vidioc_log_status          = netvizxpi_log_status,
    .vidioc_s_dv_timings        = netvizxpi_s_dv_timings,
    .vidioc_g_dv_timings        = netvizxpi_g_dv_timings,
#ifdef CONFIG_VIDEO_ADV_DEBUG
    .vidioc_g_register          = netvizxpi_dbg_g_register,
    .vidioc_s_register          = netvizxpi_dbg_s_register,
#endif
};

static struct video_device netvizxpi_video_template = {
    .name           = "netviz-xpi",
    .fops           = &netvizxpi_fops,
    .ioctl_ops      = &netvizxpi_ioctl_ops,
    .release        = video_device_release,
    .tvnorms        = V4L2_STD_UNKNOWN,
    .current_norm   = V4L2_STD_UNKNOWN,
};

static inline struct netvizxpi_state *to_state(struct v4l2_device *vd)
{
    return container_of(vd, struct netvizxpi_state, v4l2_dev);
}

static int __devinit netvizxpi_probe(struct platform_device *platdev)
{
    struct netvizxpi_platform_data *platdata = platdev->dev.platform_data;
    struct i2c_adapter *i2c_adapter;
    struct video_device *vdev;
    struct netvizxpi_state *state;
    struct i2c_board_info info;
    int rc, i;
    struct proc_dir_entry *pdr;

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        dev_info(&platdev->dev, "%s: entered\n", __func__);

    state = kzalloc(sizeof(struct netvizxpi_state), GFP_KERNEL);
    if (state == NULL) {
        dev_err(&platdev->dev, "%s: unable to alloc netvizxpi device\n", 
		    __func__);
        rc = -ENOMEM;
        goto out;
    }

    state->pdata = platdata;
    state->pdev = platdev;
    mutex_init(&state->lock);

    /* register V4L2 device */
    strlcpy(state->v4l2_dev.name, DEVICE_NAME, sizeof(state->v4l2_dev.name));
    rc = v4l2_device_register(&platdev->dev, &state->v4l2_dev);
    if (rc < 0) {
        dev_err(&platdev->dev, "%s: unable to register v4l2 device\n", 
		    __func__);
        goto fail_register_v4l2;
    }
    state->v4l2_dev.notify = netvizxpi_notify;

    /* register video device */
    vdev = video_device_alloc();
    if (vdev == NULL) {
        dev_err(&platdev->dev, "%s: unable to alloc video device\n", __func__);
        goto fail_alloc_video;
    }
    
    memcpy(vdev, &netvizxpi_video_template, sizeof(netvizxpi_video_template));
    vdev->v4l2_dev = &state->v4l2_dev;
    state->vdev = vdev;
    video_set_drvdata(state->vdev, state);

    rc = video_register_device(state->vdev, VFL_TYPE_GRABBER, -1);
    if (rc < 0) {
        dev_err(&platdev->dev, "%s: unable to register video device\n", 
		    __func__);
        goto fail_register_video;
    }

    /* initialize I2C communication to subdevices */
    for (i = 0; i < NUM_I2C_BUSSES; i++) {
        if ((i2c_adapter = i2c_get_adapter(i)) == NULL) {
            rc = -ENODEV;
            dev_err(&platdev->dev, "%s: unable to get I2C bus %d adapter\n", 
                __func__, XPISWITCH_I2C_BUS);
            goto fail_i2c;
        }
        state->i2c_adapters[i] = i2c_adapter;
    }
    memcpy(&(netvizxpi_sii7172_platform_data.slave_adapter),
            state->i2c_adapters[SII7172_SLAVE_I2C_BUS], 
            sizeof(struct i2c_adapter));

    /* enable video */
    set_gpio_pin(GPIO_DVI_TXS_PDN, "GPIO_DVI_TXS_PDn", 0);
    set_gpio_pin(GPIO_DVI_TXM_PDN, "GPIO_DVI_TXM_PDn", 0);
    set_gpio_pin(GPIO_DVI1_RESET, "GPIO_ADV7612_RSTn", 1);
    set_gpio_pin(GPIO_7604_PDN, "GPIO_ADV7604_PDn", 0);
    set_gpio_pin(GPIO_7604_RSTN, "GPIO_ADV7604_RSTn", 0);
    set_gpio_pin(GPIO_7612_RSTN, "GPIO_ADV7612_RSTn", 0);
    mdelay(1);
    set_gpio_pin(GPIO_DVI_TXS_PDN, "GPIO_DVI_TXS_PDn", 1);
    set_gpio_pin(GPIO_DVI_TXM_PDN, "GPIO_DVI_TXM_PDn", 1);
    set_gpio_pin(GPIO_7604_PDN, "GPIO_7604_PDn", 1);
    set_gpio_pin(GPIO_DVI1_RESET, "GPIO_DVI1_RESET", 0);
    set_gpio_pin(GPIO_7604_RSTN, "GPIO_7604_RSTn", 1);
    set_gpio_pin(GPIO_7612_RSTN, "GPIO_7612_RSTn", 1);
    mdelay(10);

    /* create subdevices */
    if (debug & NETVIZXPI_DEBUG_INFO)
        dev_info(&platdev->dev, "Probing SII7172 device\n");
    memset(&info, 0, sizeof(info));
    strlcpy(info.type, SII7172_DRIVER_NAME, sizeof(info.type));
    info.addr = SII7172_I2C_ADDR;
    info.platform_data = &netvizxpi_sii7172_platform_data;
    state->hdmi_transmitter = v4l2_i2c_new_subdev_board(&state->v4l2_dev,
            state->i2c_adapters[SII7172_I2C_BUS], &info, NULL);
    if (state->hdmi_transmitter == NULL) {
        rc = -ENODEV;
        dev_err(&platdev->dev, "%s: error loading SII7172 module\n", __func__);
    }
    else {
        if (debug & NETVIZXPI_DEBUG_INFO)
            dev_info(&platdev->dev, "Successfully loaded SII7172 module\n");
    }

    if (debug & NETVIZXPI_DEBUG_INFO)
        dev_info(&platdev->dev, "Probing ADV7604 device\n");
    memset(&info, 0, sizeof(info));
    strlcpy(info.type, ADV7604_DRIVER_NAME, sizeof(info.type));
    info.addr = ADV7604_I2C_ADDR;
    info.platform_data = &netvizxpi_adv7604_platform_data;
    state->hdmi_adc_receiver = v4l2_i2c_new_subdev_board(&state->v4l2_dev,
            state->i2c_adapters[ADV7604_I2C_BUS], &info, NULL);
    if (state->hdmi_adc_receiver == NULL) {
        rc = -ENODEV;
        dev_err(&platdev->dev, "%s: error loading ADV7604 module\n", __func__);
    }
    else {
        if (debug & NETVIZXPI_DEBUG_INFO)
            dev_info(&platdev->dev, "Successfully loaded ADV7604 module\n");
    }

    if (debug & NETVIZXPI_DEBUG_INFO)
        dev_info(&platdev->dev, "Probing ADV7612 device\n");
    memset(&info, 0, sizeof(info));
    strlcpy(info.type, ADV7612_DRIVER_NAME, sizeof(info.type));
    info.addr = ADV7612_I2C_ADDR;
    info.platform_data = &netvizxpi_adv7612_platform_data;
    state->hdmi_receiver = v4l2_i2c_new_subdev_board(&state->v4l2_dev,
            state->i2c_adapters[ADV7612_I2C_BUS], &info, NULL);
    if (state->hdmi_receiver == NULL) {
        rc = -ENODEV;
        dev_err(&platdev->dev, "%s: error loading ADV7612 module\n", __func__);
    }
    else if (debug & NETVIZXPI_DEBUG_INFO)
        dev_info(&platdev->dev, "Successfully loaded ADV7612 module\n");

#ifdef ENABLE_EXPANSION_I2CMUX
    if (netvizxpi_expansion_init(state) < 0) {
        dev_err(&platdev->dev, 
                "%s: failed to initialize expansion I2C\n", __func__);
    }
    else {
        for (i = 0; i < NUM_EXPANSION_I2C_BUSSES; i++) {
            if ((i2c_adapter = i2c_get_adapter(i+NUM_I2C_BUSSES)) == NULL) {
                rc = -ENODEV;
                dev_err(&platdev->dev, 
                        "%s: unable to get I2C bus %d adapter\n", 
                        __func__, XPISWITCH_I2C_BUS);
                goto fail_expansion_i2c;
            }
            state->i2c_expansion_adapters[i] = i2c_adapter;
        }
        if (debug & NETVIZXPI_DEBUG_INFO) 
            dev_info(&platdev->dev, "Successfully initialized expansion I2C\n");

        if (state->i2c_expansion_adapters[TLV320AIC3254_I2C_PORT]) {
            state->i2c_audiocodec_client = 
                i2c_new_device(
                        state->i2c_expansion_adapters[TLV320AIC3254_I2C_PORT], 
                        &i2c_audiocodec_info);
            if (state->i2c_audiocodec_client == NULL) {
                xpi_warn(state, 
                        "%s: error instantiating tlv20aic32x4 audio codec\n",
                        __func__);
                rc = -EFAULT;
                goto fail_expansion_i2c;
            }
#ifdef INIT_AUDIOCODEC 
            initialize_audiocodec(state);
#endif
        }
        else {
            xpi_warn(state, 
                    "%s: error instantiating audio codec tlv320aic32x4\n",
                    __func__);
            rc = -EFAULT;
            goto fail_expansion_i2c;
        }
        if (debug & NETVIZXPI_DEBUG_INFO) 
            dev_info(&platdev->dev, 
                    "Successfully instantiated audio codec tlv320aic32x4\n");
    }
#endif

    /* initialize LUT */
    (void) lut_table_init(state);
    state->current_addside_lut = state->current_dropside_lut =
        state->current_display_lut = state->current_splash_lut = NULL;

    /* initialize overlay parameters */
    memset(&state->ocfg, 0, sizeof(struct ov_param_io));
    memset(&state->oinfo, 0, sizeof(struct ov_param_io));
    state->ocfg.flags = state->oinfo.flags = 
        OV_FLAG_SRC_REMOTE | OV_FLAG_VSCALESRC | OV_FLAG_HSCALESRC;
    state->ocfg.hscale = state->ocfg.vscale = 
        state->oinfo.hscale = state->oinfo.vscale = RX_MAX_SCALE_VAL;
    state->ocfg.hlocation = state->oinfo.hlocation = OV_RIGHT_ADJUSTED;
    state->ocfg.vlocation = state->oinfo.vlocation = OV_TOP_ADJUSTED;

    /* initialize timer */
    rc = netvizxpi_timer_init(state);
    if (rc != 0) {
        dev_err(&platdev->dev, "%s: error initializing timer\n", __func__);
        goto fail_expansion_i2c;
    }

    /* setup to use omnitek driver */
    state->pExt = OmniTekGetExtension(0);
    if (state->pExt == NULL) {
        dev_err(&platdev->dev, "%s: unable to get OmniTek extension\n", 
		    DRIVER_NAME);
        rc = -EINVAL;
        goto fail_omnitek;
    } 

    /* initialize flags */
    state->flags = FLAG_XPI_DRIVER_ENABLE;

    if ((get_fpga_val(state, FPGA_IDREG) & 0xf000) == 0xa000) 
        state->flags |= FLAG_XPI_TX;
    else
        state->flags |= FLAG_XPI_RX;

    /* create proc directory */
    state->pdr = proc_mkdir(DRIVER_NAME, NULL);
    if (state->pdr == NULL) {
        dev_warn(&platdev->dev, "%s: error create directory for procfile\n", 
                DRIVER_NAME);
        goto fail_omnitek;
    }
    else {
        /* create proc files */
        pdr = create_proc_read_entry("luts", 0444, state->pdr, 
                netvizxpi_procfile_lut_read, state);
        if (pdr == NULL) {
            dev_warn(&platdev->dev, "%s: unable to create 'luts' proc entry\n", 
		        DRIVER_NAME);
            goto fail_proc_luts;
        }

        pdr = create_proc_read_entry("version", 0444, state->pdr, 
                netvizxpi_procfile_version_read, state);
        if (pdr == NULL) {
            dev_warn(&platdev->dev, 
                    "%s: unable to create 'version' proc entry\n", 
                    DRIVER_NAME);
            goto fail_proc_version;
        }
    }

    /* create sysfs device attributes */
    rc = sysfs_create_group(&platdev->dev.kobj, &netvizxpi_attribute_group);
    if (rc) {
        dev_warn(&platdev->dev, 
                "%s: unable to create sysfs device attributes\n", __func__);
        goto fail_sysfs;
    }

    /* enable addside video on RX */
    if (state->flags & FLAG_XPI_RX)
        state->current_interface = INTERFACE_ADDSIDE_VIDEO_ENABLE;
    
    if (netvizxpi_smbus_read_byte_data_check(state, LM63_I2C_BUS, 
                LM63_I2C_ADDR, 0xfe, false) == 0x01) 
        state->flags |= FLAG_XPI_MAIN_LM63;
    else if (expansion_smbus_read_byte_data_check(state, 
                EXPANSIONLM63_I2C_PORT, LM63_I2C_ADDR, 0xfe, false) == 0x01) 
        state->flags |= FLAG_XPI_EXPANSION_LM63;

    if (expansion_smbus_read_byte_data_check(state, EXPANSIONMAX6650_I2C_PORT,
                MAX6650_I2C_ADDR, 0x00, false) >= 0) 
        state->flags |= FLAG_XPI_HAS_MAX6650;

    /* create work queues */
    state->work_queues = create_singlethread_workqueue(DRIVER_NAME);
    if (!state->work_queues) {
        dev_err(&platdev->dev, "%s: error creating work_queue\n", __func__);
        rc = -ENOMEM;
        goto fail_work_queues;
    }
    else {
        INIT_DELAYED_WORK_DEFERRABLE(&state->delayed_work_sii7172, 
                netvizxpi_delayed_work_sii7172);
    }

    /* Initialize value for V6 register 0x6e0 */
    state->reg6e0_value = is_tx(state) ? get_fpga_val(state, FPGA_REG6E0) : 0;

    /* Set V6 audio priority register priority bit to default */
    and_in_fpga_val(state, FPGA_XPI_AUD_PATCTL, ~AUDPATCTL_PRIORITY_BIT);
    or_in_fpga_val(state, FPGA_XPI_AUD_PATCTL, AUDPATCTL_PRIORITY_BIT);

    /* Set lower 16 bits of V6 audio FIFO threshold register to default */
    and_in_fpga_val(state, FPGA_XPI_AUD_THRES, 0xffff0000);
    or_in_fpga_val(state, FPGA_XPI_AUD_THRES, 0x00000300);

    /* Enable audio mixer */
    set_fpga_val(state, FPGA_DAUDCREG, DAUDCREG_AMIXER);

    /* reset S6 FPGA */
    set_gpio_pin(GPIO_S6_RESET, "GPIO_S6_RESET", 0);
    mdelay(1);
    set_gpio_pin(GPIO_S6_RESET, "GPIO_S6_RESET", 1);

    /* set splash pattern register to default */
    set_s6fpga_val(state, S6FPGA_SPLASHPATT, S6FPGA_SPLASHPATT_VALUE_BLUE);

    /* S3 FPGA is reset by the Ethernet driver */

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        dev_info(&platdev->dev, "%s: returning 0\n", __func__);
    return 0;

fail_work_queues:
    sysfs_remove_group(&platdev->dev.kobj, &netvizxpi_attribute_group);

fail_sysfs:
    remove_proc_entry("version", state->pdr);

fail_proc_version:
    remove_proc_entry("luts", state->pdr);

fail_proc_luts:
    remove_proc_entry(DEVICE_NAME, NULL);

fail_omnitek:
    del_timer_sync(&state->av_timer);

fail_expansion_i2c:
#ifdef ENABLE_EXPANSION_I2CMUX
    for (i = 0; i < NUM_EXPANSION_I2C_BUSSES; i++) {
        if (state->i2c_expansion_adapters[i])
            i2c_put_adapter(state->i2c_expansion_adapters[i]);
    }
    netvizxpi_expansion_exit(state);
#endif

fail_i2c:
    for (i = 0; i < NUM_I2C_BUSSES; i++) {
        if (state->i2c_adapters[i])
            i2c_put_adapter(state->i2c_adapters[i]);
    }
    if (state->vdev)
        video_unregister_device(state->vdev);

fail_register_video:
    if (state->vdev)
        video_device_release(state->vdev);

fail_alloc_video:
    v4l2_device_unregister(&state->v4l2_dev);

fail_register_v4l2:
    kfree(state);

out:
    if (debug & NETVIZXPI_DEBUG_TRACE) 
        dev_info(&platdev->dev, "%s: returning %d\n", __func__, rc);
    return rc; 
}
    
static int netvizxpi_remove(struct platform_device *platdev)
{
    struct v4l2_device *v4l2dev = platform_get_drvdata(platdev);
    struct netvizxpi_state *state = to_state(v4l2dev);
    int i;

    if (debug & NETVIZXPI_DEBUG_INFO) 
        dev_info(&platdev->dev, "%s: unregistering %s\n", __func__,
		        video_device_node_name(state->vdev));

    if (state->current_dropside_lut)
        kfree(state->current_dropside_lut);
    state->current_dropside_lut = NULL;

    if (state->current_display_lut)
        kfree(state->current_display_lut);
    state->current_display_lut = NULL;

    if (state->current_splash_lut)
        kfree(state->current_splash_lut);
    state->current_splash_lut = NULL;

    cancel_delayed_work(&state->delayed_work_sii7172);
    destroy_workqueue(state->work_queues);

    remove_proc_entry("version", state->pdr);
    remove_proc_entry("luts", state->pdr);
    remove_proc_entry(DEVICE_NAME, NULL);

    sysfs_remove_group(&platdev->dev.kobj, &netvizxpi_attribute_group);

    cancel_work_sync(&state->work);
    del_timer_sync(&state->av_timer);

#ifdef ENABLE_EXPANSION_I2CMUX
    for (i = 0; i < NUM_EXPANSION_I2C_BUSSES; i++) {
        if (state->i2c_expansion_adapters[i]) {
            i2c_put_adapter(state->i2c_expansion_adapters[i]);
        }
    }
    netvizxpi_expansion_exit(state);
#endif

    for (i = 0; i < NUM_I2C_BUSSES; i++) {
        if (state->i2c_adapters[i]) {
            i2c_put_adapter(state->i2c_adapters[i]);
        }
    }
    if (state->vdev) {
        video_unregister_device(state->vdev);
        video_device_release(state->vdev);
    }
    v4l2_device_unregister(&state->v4l2_dev);
    kfree(state);

    if (debug & NETVIZXPI_DEBUG_TRACE) 
        dev_info(&platdev->dev, "%s: returning 0\n", __func__);

    return 0;
}

static struct platform_driver netvizxpi_driver = {
    .remove = __devexit_p(netvizxpi_remove),
    .driver = {
         .name = DRIVER_NAME,
         .owner = THIS_MODULE,
    }
};

static int __init netvizxpi_module_init(void)
{
    return platform_driver_probe(&netvizxpi_driver, netvizxpi_probe);
}

static void __exit netvizxpi_module_exit(void)
{
    platform_driver_unregister(&netvizxpi_driver);
}

module_init(netvizxpi_module_init);
module_exit(netvizxpi_module_exit);

