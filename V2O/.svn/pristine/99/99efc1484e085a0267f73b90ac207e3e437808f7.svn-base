#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
                                                                                
#include "plx9056pciif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"
#include "vlib.h"
#include "xp_i2c_control.h"
#include "system_utils.h"

/* PCA 9564PW registers */
#define I2CSTA            (0x00<<2)    /* STATUS read only */
#define I2CTO             (0x00<<2)    /* TIMEOUT write only */
#define I2CDAT            (0x01<<2)    /* STATUS read/write */
#define I2CADR            (0x02<<2)    /* STATUS read/write */
#define I2CCON            (0x03<<2)    /* STATUS read/write */

/* PCA 9564PW control register bits */
#define PCA9564_I2C_CON_AA            0x80    /* Assert Acknowledge */
#define PCA9564_I2C_CON_ENSIO         0x40    /* Enable */
#define PCA9564_I2C_CON_STA           0x20    /* Start */
#define PCA9564_I2C_CON_STO           0x10    /* Stop */
#define PCA9564_I2C_CON_SI            0x08    /* Serial Interrupt */
#define PCA9564_I2C_CON_CR            0x07    /* Clock Rate (mask) */

#define PCA9564_I2C_CON_330KHZ        0x00
#define PCA9564_I2C_CON_288KHZ        0x01
#define PCA9564_I2C_CON_217KHZ        0x02
#define PCA9564_I2C_CON_146KHZ        0x03
#define PCA9564_I2C_CON_88KHZ         0x04
#define PCA9564_I2C_CON_59KHZ         0x05
#define PCA9564_I2C_CON_44KHZ         0x06
#define PCA9564_I2C_CON_36KHZ         0x07

#define PCA9564_I2C_CON_DEFAULT       (PCA9564_I2C_CON_ENSIO | \
                                      PCA9564_I2C_CON_36KHZ)

/* PCA 9564PW address types */
#define PCA_WRITE                   0
#define PCA_READ                    1

/* PCA 9564PW states */
#define I2C_IDLE                    0xf8
#define I2C_START_XMITTED           0x08
#define I2C_REPSTART_XMITTED        0x10
#define I2C_SLA_W_XMITTED_ACK_RCVD  0x18
#define I2C_SLA_W_XMITTED_NACK_RCVD 0x20
#define I2C_DATA_XMITTED_ACK_RCVD   0x28
#define I2C_DATA_XMITTED_NACK_RCVD  0x30
#define I2C_SLA_R_XMITTED_ACK_RCVD  0x40
#define I2C_SLA_R_XMITTED_NACK_RCVD 0x48
#define I2C_DATA_RCVD_ACK_XMITTED   0x50
#define I2C_DATA_RCVD_NACK_XMITTED  0x58

#define I2C_WAIT_LOOP_COUNT         200
#define I2C_STO_LOOP_COUNT          100
#define I2C_LOCK_RETRY_COUNT        10
#define I2C_RESET_FAILURE_COUNT     10

/* I2C device access functions */
#define CREATE_I2C_GET_FUNC_K(devname, devaddr) \
unsigned char hal_get_##devname##_val_k (HAL *hp, unsigned short reg) \
{ \
    int i, rc; \
    unsigned char b[1]; \
    for (i = 0; i < 3; i++) { \
        rc = hal_read_i2c_val_k(hp, I2CIO_TYPE_READ, devaddr, reg, 1, b); \
        if (rc == 0) break; \
    } \
    if (rc < 0) { \
        VLOG_INFO_CAT(VLOG_I2C, \
                "Channel %d Error read I2C addr 0x%x reg 0x%x", \
                hal_get_board_number(hp)+1, devaddr, reg); \
        errno = -1; \
    } \
    else \
        errno = 0; \
    return b[0]; \
}
#define CREATE_I2C_SET_FUNC_K(devname, devaddr) \
void hal_set_##devname##_val_k (HAL *hp, unsigned short reg, unsigned char value) \
{ \
    int i, rc; \
    unsigned char b[1]; \
    b[0] = value; \
    for (i = 0; i < 3; i++) { \
        rc = hal_write_i2c_val_k(hp, I2CIO_TYPE_WRITE, devaddr, reg, 1, b); \
        if (rc == 0) break; \
    } \
    if (rc < 0) { \
        VLOG_INFO_CAT(VLOG_I2C, \
                "Channel %d Error write I2C addr 0x%x reg 0x%x", \
                hal_get_board_number(hp)+1 , devaddr, reg); \
        errno = -1; \
    } \
    else \
        errno = 0; \
}
                                                                                
CREATE_I2C_SET_FUNC_K(pca9555_clk, PCA9555_CLK_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(pca9555_clk, PCA9555_CLK_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(ak4683ad, AK4683AD_I2C_ADDR);
CREATE_I2C_SET_FUNC_K(ak4683aa, AK4683AA_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(pm_pca9555, PM_PCA9555_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(pm_pca9555, PM_PCA9555_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(pca9545a, PCA9545A_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(pca9545a, PCA9545A_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(pca9555_dvi, PCA9555_DVI_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(pca9555_dvi, PCA9555_DVI_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(v4_ad9888, AD9888_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(v4_ad9888, AD9888_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(sii1178_1, SII1178_1_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(sii1178_1, SII1178_1_I2C_ADDR);

CREATE_I2C_SET_FUNC_K(sii1178_2, SII1178_2_I2C_ADDR);
CREATE_I2C_GET_FUNC_K(sii1178_2, SII1178_2_I2C_ADDR);

/*********************************************************
 * PCA 9564PW I2C Routines - See i2c-algo-pca.c in Linux *
 * 2.6.10 kernel and Philips Semiconducator AN10148.     *
 *********************************************************/
static int hal_get_i2c_debug(HAL *hp) { 
    char name[64];
    struct stat stat;

    sprintf(name, "/tmp/i2cdebug.%d", hal_get_board_number(hp));
    if (lstat(name, &stat) < 0)
        return 0;
    return 1;
}

void hal_set_i2c_debug(HAL *hp, int val) { 
    char command[64];
    int i = hal_get_board_number(hp);

    if (val) {
        if (!hal_get_i2c_debug(hp)) {
            sprintf(command, "/bin/touch /tmp/i2cdebug.%d", i);
            system_command(command);
        }
    }
    else {
        if (hal_get_i2c_debug(hp)) {
            sprintf(command, "/bin/rm /tmp/i2cdebug.%d", i);
            system_command(command);
        }
    }
}

/********************************************************* 
 * Since none of the PCA 9564PW registers should be      *
 * written via the parallel interface once the Serial    *
 * Interrupt line has been de-asserted, this routine     *
 * waits till Serial Interrupt is asserted.              *
 *********************************************************/
static int pca_i2c_wait_k(HAL *hp) 
{ 
    int loopcnt = 0, con;
    int cno = hal_get_board_number(hp) + 1;

    /* loop waiting for si bit in 9564 register 3 to be set */ 
    while (loopcnt++ < I2C_WAIT_LOOP_COUNT) {
        con = hal_get_bar2_val_k(hp, I2C_BASE | I2CCON);
        if (con & PCA9564_I2C_CON_SI)
            break;
        usleep(1);
    }
    if (loopcnt >= I2C_WAIT_LOOP_COUNT) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C,
                    "Channel %d: %s returning -1. SI not asserted.", 
                    cno, __FUNCTION__); 
        }
        return -1;
    }
    else 
        return 0;
} 

/********************************************************* 
 * Generate a start condition on the i2c bus.            * 
 * Returns after the start condition has occured.        *
 *********************************************************/
static int pca_i2c_start_k(HAL *hp) 
{ 
    int sta, rc;
    int cno = hal_get_board_number(hp) + 1;

    if (hal_get_i2c_debug(hp)) 
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. === START", cno);

    sta = PCA9564_I2C_CON_DEFAULT | PCA9564_I2C_CON_STA;

    /* set the enable, sta and clock bits in 9564 register 3 */
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, sta); 

    rc = pca_i2c_wait_k(hp); 

    if (hal_get_i2c_debug(hp) && rc)
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. %s returning %d", 
                cno, __func__, rc);
    return rc;
} 

/********************************************************* 
 * Generate a stop condition on the i2c bus. Returns     * 
 * after the stop condition has been generated.          *
 *                                                       *
 * STOPs do not generate an interrupt or set the SI flag,*
 * since the part returns the the idle state (0xf8).     *
 * Hence we don't need to i2c_wait here> Instead we check* 
 * whether the STO bit is cleared or not.                *
 *********************************************************/
static void pca_i2c_stop_k(HAL *hp) 
{ 
    int loopcnt = 0, sta;
    int cno = hal_get_board_number(hp) + 1;

    if (hal_get_i2c_debug(hp))
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. === STOP", cno);

    sta = PCA9564_I2C_CON_DEFAULT | PCA9564_I2C_CON_STO;

    /* set the enable, sto and clock bits in 9564 register 3 */
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, sta); 

    /* loop waiting for sto bit in 9564 register 3 clears */ 
    while (++loopcnt <= I2C_STO_LOOP_COUNT) {
        sta = hal_get_bar2_val_k(hp, I2C_BASE | I2CCON);
        if ((sta & PCA9564_I2C_CON_STO) == 0)
            break; 
        usleep(1);
    }
} 
 
/********************************************************* 
 * Send the slave address and R/W bit. Returns after the *
 * address has been sent.                                *
 *********************************************************/
static int pca_i2c_address_k(HAL *hp, int type, int slave_addr) 
{ 
    int sta, addr, rc;
    int cno = hal_get_board_number(hp) + 1;

    addr = ((0x7f & slave_addr) << 1); 
    if (type == PCA_READ)
        addr |= 1; 

    if (hal_get_i2c_debug(hp))
        VLOG_INFO_CAT(VLOG_I2C,
                "Channel %d. === SLAVE ADDRESS %#04x+%c=%#04x", cno,
                slave_addr, (type == PCA_READ) ? 'R' : 'W', addr);
    sta = PCA9564_I2C_CON_DEFAULT;

    hal_set_bar2_val_k(hp, I2C_BASE | I2CDAT, addr); 
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, sta); 

    rc = pca_i2c_wait_k(hp); 

    if (hal_get_i2c_debug(hp) && rc)
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. %s returning %d", 
                cno, __func__, rc);

    return rc;
} 
 
/********************************************************* 
 * Transmit a byte.  Returns after the byte has been     * 
 * transmitted.                                          *
 *********************************************************/
static int pca_i2c_tx_byte_k(HAL *hp, unsigned char b) 
{ 
    int sta, rc;
    int cno = hal_get_board_number(hp) + 1;

    if (hal_get_i2c_debug(hp))
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. === WRITE %#04x", cno, b);

    sta = PCA9564_I2C_CON_DEFAULT;
    hal_set_bar2_val_k(hp, I2C_BASE | I2CDAT, b); 
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, sta); 

    rc = pca_i2c_wait_k(hp); 

    if (hal_get_i2c_debug(hp) && rc)
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. %s returning %d", 
                cno, __func__, rc);

    return rc;

} 
 
/********************************************************* 
 * Receive a byte. Returns immediately.                  * 
 *********************************************************/
static void pca_i2c_rx_byte_k(HAL *hp, unsigned char *b, int ack) 
{ 
    int cno = hal_get_board_number(hp) + 1;

    *b = (unsigned char) hal_get_bar2_val_k(hp, I2C_BASE | I2CDAT);

    if (hal_get_i2c_debug(hp))
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. === READ %s (%#02x)", 
                cno, ack ? "ACK" : "NACK", *b);
} 
 
/********************************************************* 
 * Setup ACK or NACK for next received byte and wait for *
 * it to arrive.  Returns after next byte has arrived.   * 
 *********************************************************/
static int pca_i2c_rx_ack_k(HAL *hp, int ack) 
{ 
    int sta, rc;

    if (ack)
        sta = PCA9564_I2C_CON_DEFAULT |PCA9564_I2C_CON_AA;
    else
        sta = PCA9564_I2C_CON_DEFAULT;
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, sta); 
 
    rc = pca_i2c_wait_k(hp); 

    if (hal_get_i2c_debug(hp) && rc)
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. %s returning %d", 
                hal_get_channel_number(hp), __func__, rc);

    return rc;
} 
 
/********************************************************* 
 * Reset the i2c bus / SIO                               * 
 *********************************************************/
static void pca_i2c_do_reset_i2c(int cno) 
{
    char buff[80];

    sprintf(buff, "/bin/touch /tmp/reseti2c.%d", cno - 1);
    system_command(buff);
}

static void pca_i2c_reset_k(HAL *hp) 
{ 
    unsigned short padcreg = hal_get_fpga_val_k(hp, FPGA_PADCREG);
    int repeat = 0;
    int cno = hal_get_board_number(hp) + 1;
    static int count = 0;

    VLOG_WARNING_CAT(VLOG_I2C, "Channel %d. resetting I2C", cno); 

    /* check if read data stuck low */
    while ((padcreg & PADCREG_SDAT_I) == 0 || 
            hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) != 0xf8) {
        hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, 0x47);
        usleep(3000);
        hal_set_fpga_val_k(hp, FPGA_PADCREG, 0xe8);
        usleep(3000);
        hal_set_fpga_val_k(hp, FPGA_PADCREG, 0xc7);
        usleep(3000);
        padcreg = hal_get_fpga_val_k(hp, FPGA_PADCREG);
        if (++repeat > 3) {
            count++;
            break; 
        }
    }

    if (repeat <= 3)
        count = 0;

    if (count > I2C_RESET_FAILURE_COUNT) {
        count = 0;
        pca_i2c_do_reset_i2c(cno); 
    }

    VLOG_INFO_CAT(VLOG_I2C, 
            "Channel %d FPGA Reg 0x%x = 0x%x, after %d iterations",
            cno, FPGA_PADCREG, padcreg, repeat); 

    usleep(1000);
} 

/*********************************************************
 * Force external reset on i2c bus by reloading FPGA     *
 *********************************************************/
static void pca_i2c_external_reset_k(HAL *hp) {
    VLOG_WARNING_CAT(VLOG_I2C, "Board %d. Please reload the FPGA to reset I2C\n", 
            hal_get_board_number(hp) + 1);
    VLOG_INFO_CAT(VLOG_I2C, "%s not implemented yet.", __FUNCTION__);
}

/********************************************************* 
 * Write the register value and data bytes to the slave *
 * address                                              * 
 *********************************************************/
static int pca_i2c_write_k(HAL *hp, 
                           int noackflag,
                           int slave_addr, 
                           int reg, 
                           int numbytes, 
                           unsigned char dat[]) { 
    int num = 0, addr, done = 0, retcode = 0;
    int state, expected_state = I2C_IDLE; 
    int reg_sent = (reg == I2C_REG_NONE) ? 1 : 0;
    int control, cno = hal_get_board_number(hp) + 1;

    state = (int) (hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff);

    if (state != 0xf8) {
        if (hal_get_i2c_debug(hp))
            VLOG_INFO_CAT(VLOG_I2C,
                    "Channel %d. STATE 0x%x. Expecting 0xf8.", cno, state);
        if (state == 0x08 || state == 0x10)
            pca_i2c_address_k(hp, PCA_WRITE, BAD_I2C_ADDR);
        else
            pca_i2c_reset_k(hp);
    }

    state = (int) (hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff);
    if (state != 0xf8) 
        return -1; 

    addr = (0x7f & slave_addr); 

    if (hal_get_i2c_debug(hp)) {
        int i; 
        VLOG_INFO_CAT(VLOG_I2C,
                "%s: Channel %d. Slave 0x%x. WRITE message",
                __FUNCTION__, cno, slave_addr); 
        if (reg == I2C_REG_NONE)
            VLOG_INFO_CAT(VLOG_I2C,
                    " [%d] WR %d bytes to %#02x [%#02x%s", cno, numbytes, 
                    addr, addr<<1, numbytes == 0 ? "" : ", "); 
        else
            VLOG_INFO_CAT(VLOG_I2C,
                    " [%d] WR %d bytes to reg %#02x on %#02x [%#02x%s", cno, 
                    numbytes, reg, addr, addr<<1, numbytes == 0 ? "" : ", "); 
        for(i = 0; i < numbytes; i++) 
            VLOG_INFO_CAT(VLOG_I2C,
                    "%#04x%s", dat[i], i == numbytes - 1 ? "" : ", "); 
        VLOG_INFO_CAT(VLOG_I2C, "]"); 
    } 

    while (!done) {
        switch (state) { 
        case 0xf8: /* On reset or stop the bus is idle */ 
            if (pca_i2c_start_k(hp) < 0) {
                if (state == 0x08 || state == 0x10)
                    VLOG_WARNING_CAT(VLOG_I2C, 
                            "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                            "SI not cleared after i2c_start", 
                            cno, slave_addr, state); 
                pca_i2c_reset_k(hp);
                retcode = -1;
            }
            expected_state = I2C_START_XMITTED;
            break; 
        case 0x08: /* A START condition has been transmitted */ 
        case 0x10: /* A repeated start condition has been transmitted */ 
            if (pca_i2c_address_k(hp, PCA_WRITE, addr) < 0) {
                VLOG_WARNING_CAT(VLOG_I2C, 
                            "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                            "SI not cleared after i2c_address", 
                            cno, slave_addr, state); 
                pca_i2c_reset_k(hp);
                retcode = -1;
            }
            expected_state = I2C_SLA_W_XMITTED_ACK_RCVD;
            break; 
        case 0x18:  /* SLA+W has been transmitted; ACK has been received */ 
        case 0x28:  /* Data byte in I2CDAT has been transmitted; ACK has */
                    /* been received. */ 
            if (!reg_sent) {
                if (pca_i2c_tx_byte_k(hp, (unsigned char) reg) < 0) {
                    VLOG_WARNING_CAT(VLOG_I2C, 
                            "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                            "SI not cleared after tx_byte", 
                            cno, slave_addr, state); 
                    pca_i2c_reset_k(hp);
                    retcode = -1;
                }
                else {
                    expected_state = I2C_DATA_XMITTED_ACK_RCVD;
                    reg_sent = 1;
                }
            } 
            else if (num < numbytes) { 
                if (pca_i2c_tx_byte_k(hp, dat[num]) < 0) {
                    VLOG_WARNING_CAT(VLOG_I2C, 
                            "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                            "SI not cleared after tx_byte", 
                            cno, slave_addr, state); 
                    pca_i2c_reset_k(hp);
                    retcode = -1;
                }
                else {
                    expected_state = I2C_DATA_XMITTED_ACK_RCVD;
                    num++; 
                }
            } 
            else {
                done = 1;
                pca_i2c_stop_k(hp);
                expected_state = I2C_IDLE;
            }
            break; 
        case 0x20: /* SLA+W has been transmitted; NOT ACK has been received */ 
                   /* Send stop. */
            if (!noackflag)
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "NOT ACK received afer SLA+W", cno, slave_addr, state); 
            pca_i2c_stop_k(hp);
            retcode = (noackflag) ? 0 : -1;
            break;
        case 0x30: /* Data byte (register value) has been transmitted; */
                   /* NOT ACK has been received -- Send stop. */ 
            if (!noackflag)
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "NOT ACK received afer data byte", 
                        cno, slave_addr, state); 
            pca_i2c_stop_k(hp);
            retcode = (noackflag) ? 0 : -1;
            break;
        case 0x38:  /* Arbitration lost during SLA+W, SLA+R or data bytes */ 
                    /* Send start again. */
            VLOG_WARNING_CAT(VLOG_I2C,
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "Arbitration lost", cno, slave_addr, state); 
            pca_i2c_reset_k(hp);
            retcode = -1;
            break;
        case 0x70:  /* Bus error - SDA stuck low. Reset the bus.*/ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - SDA stuck low", cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        case 0x90:  /* Bus error - SCL stuck low. Reset the bus. */ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - SCL stuck low", cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        case 0x00:  /* Bus error during master or slave mode due to */
                     /* illegal START or STOP condition. Reset the bus. */ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - Illegal start or STOP", 
                    cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        default:   /* Undefined states. Reset the bus. */
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "Unhandled SIO state. Expecting 0x%02x", 
                    cno, slave_addr, state, expected_state); 
            pca_i2c_reset_k(hp);
            retcode = -1;
            break;
        } 

        if (retcode < 0) 
            return retcode; 

        state = hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff;
    }

    if (hal_get_i2c_debug(hp)) {
        state = hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff;
        control = hal_get_bar2_val_k(hp, I2C_BASE | I2CCON) & 0xff;
        VLOG_INFO_CAT(VLOG_I2C,
                "}}} transfered message status %#04x. control %#04x", 
                state, control);
    }

    return 0; 
} 

/********************************************************* 
 * Write the register value to the slave address then    *
 * read from the device at the slave address             *
 *********************************************************/
static int pca_i2c_read_k(HAL *hp, 
                          int noackflag,
                          int slave_addr, 
                          int reg, 
                          int numbytes, 
                          unsigned char *dat) { 
    int num = 0, addr, done = 0, retcode = 0;
    int state, expected_state = I2C_IDLE; 
    int reg_sent = (reg == I2C_REG_NONE) ? 1 : 0;
    int control, cno = hal_get_board_number(hp) + 1;
    
    state = (int) (hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff);

   if (state != 0xf8) { 
        if (hal_get_i2c_debug(hp)) 
            VLOG_INFO_CAT(VLOG_I2C,
                    "Channel %d. Slave 0x%x. STATE 0x%x. Expecting 0xf8.", 
                    cno, slave_addr, state);
        if (state == 0x08 || state == 0x10) 
            pca_i2c_address_k(hp, PCA_READ, BAD_I2C_ADDR);
        else
            pca_i2c_reset_k(hp);
    }

    state = (int) (hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff);
    if (state != 0xf8) 
        return -1; 
    
    addr = (0x7f & slave_addr); 
 
    if (hal_get_i2c_debug(hp)) {
        VLOG_INFO_CAT(VLOG_I2C,
                "%s: Channel %d. Slave 0x%x. READ message\n", 
                __FUNCTION__, cno, slave_addr); 
        if (reg == I2C_REG_NONE)
            VLOG_INFO_CAT(VLOG_I2C, "[%d] RD %d bytes from %#02x [%#02x, ...]",
                 cno, numbytes, addr, (addr<<1) | 1); 
        else
            VLOG_INFO_CAT(VLOG_I2C, "[%d] RD %d bytes from reg %#02x on %#02x "
                       "[%#02x, ...]", cno, numbytes, reg, addr, (addr<<1)|1); 
    } 

    while (!done) {
        switch (state) { 
        case 0xf8: /* On reset or stop the bus is idle */ 
            if (pca_i2c_start_k(hp) < 0) {
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "SI not cleared after i2c_start", 
                        cno, slave_addr, state); 
                pca_i2c_reset_k(hp);
                retcode = -1;
            }
            expected_state = I2C_START_XMITTED;
            break; 
        case 0x08: /* A START condition has been transmitted */ 
        case 0x10: /* A repeated start condition has been transmitted */ 
            if (pca_i2c_address_k(hp, (!reg_sent) ? PCA_WRITE : 
                        PCA_READ, addr) < 0) {
                VLOG_WARNING_CAT(VLOG_I2C,
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "SI not cleared after i2c_address", 
                        cno, slave_addr, state); 
                pca_i2c_reset_k(hp);
                retcode = -1;
            }
            expected_state = (!reg_sent) ? I2C_SLA_W_XMITTED_ACK_RCVD :
                    I2C_SLA_R_XMITTED_ACK_RCVD;
            break; 
        case 0x18: /* SLA+W has been transmitted; ACK has been received */ 
            if (!reg_sent) {
                if (pca_i2c_tx_byte_k(hp, (unsigned char) reg) < 0) {
                    VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "SI not cleared after tx_byte", 
                        cno, slave_addr, state); 
                    pca_i2c_reset_k(hp);
                    retcode = -1;
                }
                else {
                    expected_state = I2C_DATA_XMITTED_ACK_RCVD;
                    reg_sent = 1;
                }
            }
            else {
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "ERROR register already sent, expecting state 0x%02x", 
                        cno, slave_addr, state, expected_state); 
                pca_i2c_stop_k(hp);
                retcode = -1;
            }
            break; 
        case 0x20: /* SLA+W has been transmitted; NOT ACK has been received */ 
                   /* Send stop. */
            if (!noackflag)
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "NOT ACK received after SLA+W", 
                        cno, slave_addr, state); 
            pca_i2c_stop_k(hp);
            retcode = (noackflag) ? 0 : -1;
            break;
        case 0x28:  /* Data byte (register value) has been transsmitted; */
                    /* ACK has been received. Send repeat start. */ 
            if (pca_i2c_start_k(hp) < 0) {
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "SI not cleared after i2c_start", 
                        cno, slave_addr, state); 
                retcode = -1;
            }
            expected_state = I2C_REPSTART_XMITTED;
            break; 
        case 0x30: /* Data byte (register value) has been transmitted;  */
                   /* NOT ACK has been received. Send stop. */ 
            if (!noackflag)
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "NOT ACK received after data byte", 
                        cno, slave_addr, state); 
            pca_i2c_stop_k(hp);
            retcode = (noackflag) ? 0 : -1;
            break;
        case 0x40: /* SLA+R has been transmitted; ACK has been received */ 
            if (pca_i2c_rx_ack_k(hp, num < numbytes - 1) < 0) {
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "SI not cleared after rx_ack", 
                        cno, slave_addr, state); 
                pca_i2c_reset_k(hp);
                retcode = -1;
            }
            expected_state = (num < numbytes - 1) ? I2C_DATA_RCVD_ACK_XMITTED :
                I2C_DATA_RCVD_NACK_XMITTED;
            break; 
        case 0x48: /* SLA+R has been transmitted; NOT ACK has been received */ 
                   /* Send stop. */
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "NOT ACK received after SLA+R", cno, slave_addr, state); 
            pca_i2c_stop_k(hp);
            retcode = -1;
            break;
        case 0x50: /* Data bytes has been received; ACK has been returned */ 
            if (num < numbytes) { 
                pca_i2c_rx_byte_k(hp, &dat[num], 1);
                num++; 
                pca_i2c_rx_ack_k(hp, num < numbytes - 1);
                expected_state = (num < numbytes - 1) ? 
                    I2C_DATA_RCVD_ACK_XMITTED : I2C_DATA_RCVD_NACK_XMITTED;
            } 
            else {
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "ERROR num %d,numbytes %d,expected_state 0x%02x",
                        cno, slave_addr, state, num, numbytes, expected_state); 
                pca_i2c_stop_k(hp);
                retcode = -1;
            }
            break; 
        case 0x58: /* Data byte has been received; NOT ACK has been returned */ 
            if (num == numbytes - 1) { 
                pca_i2c_rx_byte_k(hp, &dat[num], 0);
                pca_i2c_stop_k(hp); 
                expected_state = I2C_IDLE;
                done = 1;
            }
            else { 
                VLOG_WARNING_CAT(VLOG_I2C, 
                        "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                        "NOT ACK sent after data byte received. " 
                         "Not final byte. numbytes %d. len %d", 
                         cno, slave_addr, state, num, numbytes); 
                pca_i2c_stop_k(hp);
                retcode = -1;
                break;
            } 
            break; 
        case 0x38: /* Arbitration lost during SLA+W, SLA+R or data bytes */ 
           /* Send start again. */
            VLOG_WARNING_CAT(VLOG_I2C,
                    "Channel %d. Slave 0x%x. STATE 0x%02x. Arbitration lost", 
                    cno, slave_addr, state); 
            pca_i2c_start_k(hp);
            expected_state = 0x08;
            break;
        case 0x70:  /* Bus error - SDA stuck low. Reset the bus. */ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - SDA stuck low", cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        case 0x90:  /* Bus error - SCL stuck low. Reset the bus. */ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - SCL stuck low", cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        case 0x00:  /* Bus error during master or slave mode due to */
             /* illegal START or STOP condition. Reset the bus. */ 
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "BUS ERROR - Illegal START or STOP", 
                    cno, slave_addr, state); 
            pca_i2c_external_reset_k(hp);
            retcode = -1;
            break;
        default:   /* Undefined states. Wait 1 usec. Retry. */
            VLOG_WARNING_CAT(VLOG_I2C, 
                    "Channel %d. Slave 0x%x. STATE is 0x%02x. "
                    "Unhandled SIO state. Expecting 0x%02x.", 
                    cno, slave_addr, state, expected_state ); 
            pca_i2c_reset_k(hp);
            break;
        } 

        if (retcode < 0) 
            return retcode; 

        state = hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff;
    }

    if (hal_get_i2c_debug(hp)) {
        state = hal_get_bar2_val_k(hp, I2C_BASE | I2CSTA) & 0xff;
        control = hal_get_bar2_val_k(hp, I2C_BASE | I2CCON) & 0xff;
        VLOG_INFO_CAT(VLOG_I2C, 
                "}}} transfered message status %#04x. control %#04x", 
                state, control);
    }

    return 0; 
} 

static int pca_i2c_lock(HAL *hp) {
    int board_number = hal_get_board_number(hp);
    char buf[MAXDEVLEN];

    sprintf(buf, "/dev/fpga%d_i2c", board_number);
    hp->i2c_fd = open(buf, O_RDWR);
    if (hp->i2c_fd <= 0) {
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Could not get I2C lock %s",
                board_number + 1, buf);
        return -1;
    }
    return 0;
}
                                                                                
static int pca_i2c_unlock(HAL *hp) {
    if (hp->i2c_fd > 0)
        close(hp->i2c_fd);
    hp->i2c_fd = -1;
    return 0;
}

static int pca_i2c_trylock(HAL *hp) {
    int i, rc;
    for (i = 0; i < I2C_LOCK_RETRY_COUNT; i++) {
        if ((rc = pca_i2c_lock(hp)) != 0)
            usleep(10000);
        else
            break;
    }
    return rc;
}

/*********************************************************
 * Routine to initialize the PCA9564PW on the FPGA board *
 *********************************************************/
void hal_initialize_i2c(HAL *hp) 
{
    static int freqs[] = {330, 288, 217, 146, 88, 59, 44, 36};
    int own, clock, cno = hal_get_board_number(hp) + 1;
 
    if (hp == NULL)
        return;

    own = PCA9564_I2C_ADDR;
    clock = (PCA9564_I2C_CON_DEFAULT & 0x07);

    if (hal_get_i2c_debug(hp)) {
        VLOG_INFO_CAT(VLOG_I2C, 
                "Channel %d. pca_i2c_init: own addres is %#04x\n", cno, own);
        VLOG_INFO_CAT(VLOG_I2C,
                "Channel %d. pca_i2c_init: clock frequency is %dkHz\n", 
                cno, freqs[clock]);
    }

    /* write own address into 9564 register 2, write
     * clock value and enable bit to 9564 register 3 
     */
    hal_set_bar2_val_k(hp, I2C_BASE | I2CADR, own << 1);
    hal_set_bar2_val_k(hp, I2C_BASE | I2CCON, (PCA9564_I2C_CON_ENSIO | clock));
    usleep(500);    /* 500 usec for oscillator to stabilize */ 
}

/************************************************************************
 * Routines to read and write to I2C devices on the Main and PM boards  *
 * We don't cache the Main or PM switch positions since the switches can*
 * be reset externally in which case the cached values wil be wrong.    * 
 ************************************************************************/
int hal_read_i2c_val_k(HAL *hp, 
                       int type,
                       unsigned short addr, 
                       unsigned short reg, 
                       int numbytes,
                       unsigned char *dat) {

    int rc = 0;
    unsigned char value[1];


    /* Check board number */
    if ((hp == NULL) || (hp->board_number >= MAX_FPGA_BOARDS) ||
            !hal_get_hardware_detected(hp))
        return -1;

    /* Lock I2C bus */
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));
        }
        return -1;
    }

    /* If device is Main I2C Switch, read the control register */
    if (addr == PCA9545A_I2C_ADDR) {
        if ((rc = pca_i2c_read_k(hp, 0, addr, I2C_REG_NONE, 1, dat)) < 0) {
            goto out;
        }
    }    

    /* Otherwise switch to appropriate port on Main I2C Switch */
    if (type == I2CIO_TYPE_PM_READ || 
        (addr == PM_PCA9545A_I2C_ADDR || addr == AK4683AD_I2C_ADDR || 
        addr == AK4683AA_I2C_ADDR || addr == PM_PCA9555_I2C_ADDR)) { 
        value[0] = 0x04;    /* switch to port 2 on Main switch */
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switching to port 2 on Main switch", 
                    hal_get_channel_number(hp));
        }
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, value)) < 0) {
            if (hal_get_i2c_debug(hp)) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                        hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port 2 on Main switch", 
                    hal_get_channel_number(hp));
        }
    }
    else if (type == I2CIO_TYPE_EDIDEEPROM_READ) {
        if (addr != EDID_EEPROM_I2C_ADDR) {
            if (hal_get_i2c_debug(hp)) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. I2C Access %d doesn't match Addr 0x%x", 
                        hal_get_channel_number(hp), type, addr);
            }
            goto out;
        }
        value[0] = 0x02;    /* switch to port 1 on Main switch */
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, I2C_REG_NONE, 
                        1, value)) < 0) { 
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                       hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    }
    else if (type == I2CIO_TYPE_MONEDID_READ) {
        if (addr != MON_EDID_I2C_ADDR) {
            if (hal_get_i2c_debug(hp)) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. I2C Access %d doesn't match Addr 0x%x", 
                        hal_get_channel_number(hp), type, addr);
            }
            goto out;
        }
        value[0] = 0x01;    /* switch to port 1 on Main switch */
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, I2C_REG_NONE, 
                        1, value)) < 0) { 
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                       hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    }
    else if (type == I2CIO_TYPE_MAIN_READ || 
        (addr == PCA9545A_I2C_ADDR || addr == PCA9555_CLK_I2C_ADDR || 
        addr == PCA9555_DVI_I2C_ADDR || addr == AD9888_I2C_ADDR || 
        addr == SII1178_1_I2C_ADDR || addr == SII1178_2_I2C_ADDR || 
        addr == DDRAM_1_I2C_ADDR || addr == DDRAP_1_I2C_ADDR || 
        addr == DDRAM_2_I2C_ADDR || addr == DDRAP_2_I2C_ADDR)) {
        if (addr == EDID_EEPROM_I2C_ADDR)
            value[0] = 0x02;    /* switch to port 1 on Main switch */
        else
            value[0] = 0x08;    /* switch to port 3 on Main switch */
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switching to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3)); 
        } 
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, I2C_REG_NONE, 
                        1, value)) < 0) { 
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                       hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    } 

    /* If device is PM I2C Switch, read the control register */
    if (type == I2CIO_TYPE_PM_READ && addr == PM_PCA9545A_I2C_ADDR) {
        if ((rc = pca_i2c_read_k(hp, 0, addr, I2C_REG_NONE, 1, dat)) < 0) {
            goto out;
        }
    }

    /* Switch to appropriate port on PM I2C Switch */
    if (type == I2CIO_TYPE_PM_READ || (addr == AK4683AD_I2C_ADDR || 
        addr == AK4683AA_I2C_ADDR || addr == PM_PCA9555_I2C_ADDR)) {
        if (addr == LM63_I2C_ADDR) 
            value[0] = 0x02;    /* switch to port 1 */
        else if (addr == USB_EEPROM_I2C_ADDR) 
            value[0] = 0x08;    /* switch to port 3 */
        else 
            value[0] = 0x04;    /* switch to port 2 */
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d. Switching to port %d on PM switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x08) ? 3 : 2));
        }
        if ((rc = pca_i2c_write_k(hp, 0, PM_PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, value)) < 0) {
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD,
                        "Channel %d. Failed to switch on PM switch", 
                        hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD,
                    "Channel %d. Switched to port %d on PM switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x08) ? 3 : 2));
        }
    }

    /* Set byte read if device is CDCE 706 or 906 */  
    if (addr >= CDCE_I2C_ADDR_MIN && addr <= CDCE_I2C_ADDR_MAX)
        reg |= 0x80;

    /* Read value of register on device */
    if (addr == AK4683AD_I2C_ADDR || addr == AK4683AA_I2C_ADDR) {
        rc = pca_i2c_read_k(hp, 1, addr, reg, numbytes, dat);
    }
    else {
        rc = pca_i2c_read_k(hp, 0, addr, reg, numbytes, dat);
    }
    if (hal_get_i2c_debug(hp)) { 
        if (rc < 0) {
            VLOG_INFO_CAT(VLOG_VLIB, 
                "Channel %d. Failed to read I2C addr 0x%x reg 0x%x", 
                hal_get_channel_number(hp), addr, reg);
        }
        else {
            VLOG_INFO_CAT(VLOG_VLIB, 
                "Channel %d. I2C addr 0x%x reg 0x%x = 0x%x", 
                hal_get_channel_number(hp), addr, reg, *dat);
        }
    }

out:
    /* Unlock I2C bus */
    pca_i2c_unlock(hp);
   
    return rc; 
}

int hal_write_i2c_val_k(HAL *hp,  
                    int type,
                    unsigned short addr, 
                    unsigned short reg, 
                    int numbytes,
                    unsigned char *dat) {
    int rc = 0;
    unsigned char value[1];

    /* Check board number */
    if ((hp == NULL) || (hp->board_number >= MAX_FPGA_BOARDS) || 
            (!hal_get_hardware_detected(hp)) < 0)
        return -1;

    /* Lock I2C bus */
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));
        }
        return -1;
    }

    /* If device is Main I2C Switch, write the control register */ 
    if (addr == PCA9545A_I2C_ADDR) { 
        if ((rc = pca_i2c_write_k(hp, 0, addr, I2C_REG_NONE, 1, dat)) < 0) {
            goto out;
        }
    }    

    /* Otherwise switch to appropriate port on Main I2C Switch */
    if (type == I2CIO_TYPE_PM_WRITE || 
        (addr == PM_PCA9545A_I2C_ADDR || addr == AK4683AD_I2C_ADDR || 
        addr == AK4683AA_I2C_ADDR || addr == PM_PCA9555_I2C_ADDR)) {
        value[0] = 0x04;    /* switch to port 2 on Main switch */
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB,
                    "Channel %d. Switching to port 2 on Main switch", 
                    hal_get_channel_number(hp));
        }
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, value)) < 0) {
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_VLIB,
                        "Channel %d. Failed to switch on Main switch", 
                        hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB, 
                    "Channel %d. Switched to port 2 on Main switch", 
                    hal_get_channel_number(hp));
        }
    }
    else if (type == I2CIO_TYPE_EDIDEEPROM_WRITE) {
        if (addr != EDID_EEPROM_I2C_ADDR) {
            if (hal_get_i2c_debug(hp)) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. I2C Access %d doesn't match Addr 0x%x", 
                        hal_get_channel_number(hp), type, addr);
            }
            goto out;
        }
        value[0] = 0x02;    /* switch to port 1 on Main switch */
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, I2C_REG_NONE, 
                        1, value)) < 0) { 
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                       hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    }
    else if (type == I2CIO_TYPE_MONEDID_WRITE) {
        if (addr != MON_EDID_I2C_ADDR) {
            if (hal_get_i2c_debug(hp)) {
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. I2C Access %d doesn't match Addr 0x%x", 
                        hal_get_channel_number(hp), type, addr);
            }
            goto out;
        }
        value[0] = 0x01;    /* switch to port 1 on Main switch */
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, I2C_REG_NONE, 
                        1, value)) < 0) { 
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_HALD, 
                        "Channel %d. Failed to switch on Main switch", 
                       hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_HALD, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    }
    else if (type == I2CIO_TYPE_MAIN_WRITE || 
        (addr == PCA9545A_I2C_ADDR || addr == PCA9555_CLK_I2C_ADDR || 
        addr == PCA9555_DVI_I2C_ADDR || addr == AD9888_I2C_ADDR || 
        addr == SII1178_1_I2C_ADDR || addr == SII1178_2_I2C_ADDR || 
        addr == DDRAM_1_I2C_ADDR || addr == DDRAP_1_I2C_ADDR || 
        addr == DDRAM_2_I2C_ADDR || addr == DDRAP_2_I2C_ADDR)) {
        if (addr == EDID_EEPROM_I2C_ADDR) 
            value[0] = 0x02;    /* switch to port 1 on Main switch */
        else 
            value[0] = 0x08;    /* switch to port 3 on Main switch */
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB, 
                    "Channel %d. Switching to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
        if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, value)) < 0) {
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_VLIB,
                        "Channel %d. Failed to switch on Main switch", 
                        hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB, 
                    "Channel %d. Switched to port %d on Main switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x01) ? 0 : 3));
        }
    } 

    /* If device is PM I2C Switch, write the control register */
    if (type == I2CIO_TYPE_PM_WRITE && addr == PM_PCA9545A_I2C_ADDR) {
        if ((rc = pca_i2c_write_k(hp, 0, PM_PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, dat)) < 0) {
            goto out;
        }
    }

    /* Switch to appropriate port on PM I2C Switch */
    if (type == I2CIO_TYPE_PM_WRITE || 
        (addr == AK4683AD_I2C_ADDR || addr == AK4683AA_I2C_ADDR || 
        addr == PM_PCA9555_I2C_ADDR)) {
        if (addr == LM63_I2C_ADDR)
            value[0] = 0x02;    /* switch to port 1 */
        else if (addr == USB_EEPROM_I2C_ADDR) 
            value[0] = 0x08;    /* switch to port 3 */
        else 
            value[0] = 0x04;    /* switch to port 2 */
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB,
                    "Channel %d. Switching to port %d on PM switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x08) ? 3 : 2));
        }
        if ((rc = pca_i2c_write_k(hp, 0, PM_PCA9545A_I2C_ADDR, 
                    I2C_REG_NONE, 1, value)) < 0) {
            if (hal_get_i2c_debug(hp)) { 
                VLOG_INFO_CAT(VLOG_VLIB,
                        "Channel %d. Failed to switch on PM switch", 
                        hal_get_channel_number(hp));
            }
            goto out;
        }
        if (hal_get_i2c_debug(hp)) { 
            VLOG_INFO_CAT(VLOG_VLIB,
                    "Channel %d. Switched to port %d on PM switch", 
                    hal_get_channel_number(hp), 
                    (value[0] == 0x02) ? 1 : ((value[0] == 0x08) ? 3 : 2));
        }
    }

    /* Set byte write if device is CDCE 706 or 906 */  
    if (addr >= CDCE_I2C_ADDR_MIN && addr <= CDCE_I2C_ADDR_MAX)
        reg |= 0x80;

    /* Write data byte to register on device */
    if (addr == AK4683AD_I2C_ADDR || addr == AK4683AA_I2C_ADDR)
        rc = pca_i2c_write_k(hp, 1, addr, reg, numbytes, dat);
    else
        rc = pca_i2c_write_k(hp, 0, addr, reg, numbytes, dat);

    if (hal_get_i2c_debug(hp)) { 
        if (rc < 0) {
            VLOG_INFO_CAT(VLOG_VLIB, 
                "Channel %d. Failed to write I2C addr 0x%x reg 0x%x", 
                hal_get_channel_number(hp), addr, reg);
        }
        else {
            VLOG_INFO_CAT(VLOG_VLIB, 
                "Channel %d. wrote 0x%x to I2C addr 0x%x reg 0x%x", 
                hal_get_channel_number(hp), *dat, addr, reg);
        }
    }

out:
    /* Unlock I2C bus */
    pca_i2c_unlock(hp);

    return rc; 
}

unsigned char hal_get_pm_i2c_val_k(HAL *hp, 
                               unsigned short addr, 
                               unsigned short reg) {
    unsigned char b[1]; 
    hal_read_i2c_val_k(hp, I2CIO_TYPE_PM_READ, addr, reg, 1, b); 
    return b[0]; 

}

void hal_set_pm_i2c_val_k(HAL *hp, 
                      unsigned short addr, 
                      unsigned short reg, 
                      unsigned char value) {
    unsigned char b[1]; 
    b[0] = value; 
    hal_write_i2c_val_k(hp, I2CIO_TYPE_PM_WRITE, addr, reg, 1, b); 
}

unsigned char hal_get_main_i2c_val_k(HAL *hp, 
                                 unsigned short addr, 
                                 unsigned short reg) {
    unsigned char b[1]; 
    hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, addr, reg, 1, b); 
    return b[0]; 
}

void hal_set_main_i2c_val_k(HAL *hp, 
                       unsigned short addr, 
                       unsigned short reg, 
                       unsigned char value) {
    unsigned char b[1]; 
    b[0] = value; 
    hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_WRITE, addr, reg, 1, b); 
}

/********************
 * Read EDID EEPROM *
 ********************/
int xp_read_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize) {
    int rc = 0, len, i2caddr = EDID_EEPROM_I2C_ADDR;
    unsigned char b[1];

    if (hp == NULL) 
        return -1;

    len = ((bufsize > 256) || (bufsize < 0)) ? 128 : bufsize;

    /****************
     * Lock I2C bus *
     ****************/
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));        
        }
        return -1;
    }

    /*******************************************
     * Switch to port 1 on the Main I2C switch *
     ********************************************/
    b[0] = 0x02;
    if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR,
                                            I2C_REG_NONE, 1, b)) < 0) {
        VLOG_INFO_CAT(VLOG_I2C, "Failed to write to PCA9545 \n");
    }
    else {
        /********************
         * Read from EEPROM *
         ********************/
        if ((rc = pca_i2c_read_k(hp, 0, i2caddr, 0, len, buf)) < 0) {
            VLOG_INFO_CAT(VLOG_I2C,
                    "Channel %d. Failed to read Monitor EDID\n", 
                    hal_get_channel_number(hp));
        }
    }
    /******************
     * Unlock I2C bus * 
     ******************/
    pca_i2c_unlock(hp);

    return rc;
}

/*********************
 * Write EDID EEPROM *
 *********************/
#define EEPROM_BYTES_PER_PAGE   8

int xp_load_edid_eeprom(HAL *hp, unsigned char *buf, int bufsize) {
    int i, rc = 0;
    unsigned char regsave1, regsave2;
    unsigned char b[1];

    if (hp == NULL) 
        return -1;

    /*****************************************************************
     * Set EDID EEPROM write-enable by setting the PCA9555 at 0x21   *
     * port 1 bit 0 to 0 (we assume PCA9555 port 1 pin 0 is output,  *
     * i.e. Reg 7 bit 0 = 0). A side effect of this I2C operation is *
     * the Main I2C switch will be set to port 3.                    *
     *****************************************************************/
    regsave1 = hal_get_pca9555_clk_val_k(hp, 0x03);
    hal_set_pca9555_clk_val_k(hp, 0x03, regsave1 & 0xfe);
    regsave2 = hal_get_pca9555_clk_val_k(hp, 0x07);
    hal_set_pca9555_clk_val_k(hp, 0x07, regsave2 & 0xfe);

    /**********************************************************
     * Lock I2C bus so we can use the unlocked version of I2C *
     **********************************************************/
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));        
        }
        return -1;
    }
    /*******************************************
     * Switch to port 1 on the Main I2C switch *
     *******************************************/
    b[0] = 0x02;
    if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR,
                                        I2C_REG_NONE, 1, b)) < 0) {
        VLOG_INFO_CAT(VLOG_I2C, "Failed to write to PCA9545");
    }
    else {
        /******************************************
         * Write the EDID EEPROM a page at a time *
         ******************************************/
        for (i = 0; i < bufsize; i += EEPROM_BYTES_PER_PAGE) {
            rc = pca_i2c_write_k(hp, 0, EDID_EEPROM_I2C_ADDR, i, 
                    EEPROM_BYTES_PER_PAGE, &buf[i]);
            if (rc < 0) {
                VLOG_INFO_CAT(VLOG_I2C, "Failed to write to EEPROM");
                break;
            }
            usleep(5000);
        }
    }
    /******************
     * Unlock I2C bus * 
     ******************/
    pca_i2c_unlock(hp);

    /******************************************************
     * Clear EDID EEPROM write-enable. Do this last since * 
     * this will set the Main I2C switch to port 3.       *
     ******************************************************/
    hal_set_pca9555_clk_val_k(hp, 0x03, regsave1 | 0x01);
    hal_set_pca9555_clk_val_k(hp, 0x07, regsave2);

    return rc;
}

/*******************************************
 * Read EDID from I2C at address 'i2caddr' *
 *******************************************/
int xp_read_mon_edid(HAL *hp, unsigned char *buf, int bufsize) {
    int rc = 0, len, i2caddr = MON_EDID_I2C_ADDR;
    unsigned char b[1];
                                                                                
    len = ((bufsize > 256) || (bufsize < 0)) ? 128 : bufsize;

    /**********************************************************
     * Lock I2C bus so we can use the unlocked version of I2C *
     **********************************************************/
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));        
        }
        return -1;
    }

    /*******************************************
     * Switch to port 0 on the Main I2C switch *
    ********************************************/
    b[0] = 0x01;
    if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR,
                                        I2C_REG_NONE, 1, b)) < 0) {
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to write to Main switch", 
                hal_get_board_number(hp)+1);
    }
    else {
        /********************
         * Read from EEPROM *
         ********************/
        if ((rc = pca_i2c_read_k(hp, 0, i2caddr, 0, len, buf)) < 0) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to read Monitor EDID", 
                hal_get_board_number(hp)+1);
        }
    }

    /******************
     * Unlock I2C bus * 
     ******************/
    pca_i2c_unlock(hp);
                                                                                
    return rc;
}

/*************************************************************
 * Read Extended EDID block(s) from I2C at address 'i2caddr' *
 *************************************************************/
int xp_read_mon_edid_extension(HAL *hp, 
                                int block,
                                unsigned char *buf, 
                                int bufsize) {
    int rc = 0, len, i2caddr = MON_EDID_I2C_ADDR;
    unsigned char b[1];
                                                                                
    len = ((bufsize > 256) || (bufsize < 0)) ? 128 : bufsize;

    /**********************************************************
     * Lock I2C bus so we can use the unlocked version of I2C *
     **********************************************************/
    if (pca_i2c_trylock(hp) < 0) {
        if (hal_get_i2c_debug(hp)) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to lock I2C bus.", 
                    hal_get_channel_number(hp));        
        }
        return -1;
    }
                                                                                
    /*******************************************
     * Switch to port 0 on the Main I2C switch *
    ********************************************/
    b[0] = 0x01;
    if ((rc = pca_i2c_write_k(hp, 0, PCA9545A_I2C_ADDR,
                                        I2C_REG_NONE, 1, b)) < 0) {
        VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to write to Main switch ", 
                hal_get_board_number(hp)+1);
    }
    else {
        /********************
         * Read from EEPROM *
         ********************/
        if ((rc = pca_i2c_read_k(hp, 0, i2caddr, block * EDID_DATA_LEN, 
                        len, buf)) < 0) {
            VLOG_INFO_CAT(VLOG_I2C, "Channel %d. Failed to read Monitor EDID", 
                hal_get_board_number(hp)+1);
        }
    }

    /******************
     * Unlock I2C bus * 
     ******************/
    pca_i2c_unlock(hp);
                                                                                
    return rc;
}
