#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "fpga.h"
#include "ddcedid.h"
#include "log_control.h"

/* I2C defines */

static volatile unsigned short *i2c_ifreg;   /* I2C interface register */

static int ddcedidinitialized = 0;

#define DELAY           udelay(100)
#define CLOCK_HI        *i2c_ifreg |= PDDCREG_SCLK 
#define CLOCK_LO        *i2c_ifreg &= ~PDDCREG_SCLK
#define DATA_HI         *i2c_ifreg |= PDDCREG_SDAT_O
#define DATA_LO         *i2c_ifreg &= ~PDDCREG_SDAT_O
#define OUTPUT_ON       *i2c_ifreg |= PDDCREG_SDAT_EN
#define OUTPUT_OFF      *i2c_ifreg &= ~PDDCREG_SDAT_EN
#define I2C_START       DATA_HI;CLOCK_HI;DELAY;DATA_LO;DELAY;CLOCK_LO;DELAY
#define I2C_STOP        DATA_LO;DELAY;CLOCK_HI;DELAY;DATA_HI
#define I2C_DATA_IN     ((*i2c_ifreg & PDDCREG_SDAT_I) >> 2)

/* DDC EDID I2C address */

#define I2C_SLAVE_ADDR  0xA0


/*******************************************************************************
 *
 * i2c_rbyte_ack - reads 8 bits from the I2C interface MSB first.  And, sends
 * an acknowledge bit.
 *
 * RETURNS: data byte.
 */

static unsigned char i2c_rbyte_ack(void)
{
    int i;
    unsigned char data = 0;
    
    DELAY;
    
    /* turn off data output enable */
    OUTPUT_OFF;

    /* read the I2C bits and form data byte */
    for (i=0; i < 8; i++) {
        CLOCK_HI;
        DELAY;
        data <<= 1;
        data |= I2C_DATA_IN;
        CLOCK_LO;
        DELAY;
    }
    
    /* turn on data output enable */
    OUTPUT_ON;
    
    /* send ACK bit, and exit with clock and data low */
    DATA_LO;
    CLOCK_HI;
    DELAY;
    CLOCK_LO;
    
    return (data);
}

/*******************************************************************************
 *
 * i2c_wbyte - writes 8 bits to the I2C interface MSB first.  And, reads
 * acknowledge bit.
 *
 * RETURNS: acknowledge bit. 0=ack, 1=nack
 */

static int i2c_wbyte(unsigned char data)
{
    int i;
    
    DELAY;
    
    for (i=0; i < 8; i++) {
        CLOCK_LO;
        if (data & 0x80) {
            DATA_HI;
        }
        else {
            DATA_LO;
        }
        data <<= 1;
        DELAY;
        CLOCK_HI;
        DELAY;
    }
    
    /* low clock period */
    CLOCK_LO;
    OUTPUT_OFF;
    DELAY;
    
    /* read ack */
    CLOCK_HI;
    DELAY;
    data = I2C_DATA_IN;
    CLOCK_LO;
    OUTPUT_ON;
    
    return(data);
}


/*******************************************************************************
 *
 * ddcedid_init - initializes the DDC EDID I2C interface.
 *
 * (should call one time)
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */
int ddcedid_init(struct v2d_board *board)
{
    /* setup i2c interface register */
    if (board->v_ConfigMemBase == 0)
        return ERROR;

    i2c_ifreg = (unsigned short *)( board->v_ConfigMemBase+ FPGA_PDDCREG);
    
    /* turn everything off, use slave address '0' */
    *i2c_ifreg = 0;

    ddcedidinitialized = 1;
    return(OK);
}

/*******************************************************************************
 *
 * ddcedid_read - reads register values from the DDC EDID input interface
 *
 *
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */

int ddcedid_read
(
    unsigned char start_addr,     /* starting base register address */
    unsigned char *data,          /* array of register values */
    int number_of_bytes           /* number of values to read */
    )
{
    int i;

    if(!ddcedidinitialized) {
        LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_WARNING,
                 "DDC EDID interface not initialized\n");
        return ERROR;
    }
    
    /* enable I2C output */
    OUTPUT_ON;
    DELAY;
    
    /* start the I2C transaction */
    I2C_START;
    
    /* write slave address byte */
    if (i2c_wbyte(I2C_SLAVE_ADDR)!=0) {
        LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_WARNING,
                 "Failed to write I2C slave address 0x%02x.\n", I2C_SLAVE_ADDR);
        goto abort;
    }
    
    /* write base register address byte */
    if (i2c_wbyte(start_addr)!=0) {
        LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_WARNING,
                 "Failed to write I2C base register address 0x%02x.\n", start_addr);
        goto abort;
    }
    
    /* re-start the I2C transaction */
    I2C_START;
    
    /* write slave address byte with read bit */
    if (i2c_wbyte(I2C_SLAVE_ADDR|1)!=0) {
        LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_WARNING,
                 "Failed to write I2C slave address 0x%02x with read bit.\n", I2C_SLAVE_ADDR);
        goto abort;
    }

    /* read data bytes with acks */
    for (i=0; i < number_of_bytes; i++)
        *data++ = i2c_rbyte_ack();

    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF; */
    OUTPUT_ON;

    LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_INFO,
             "Successfully read from DDC EDID I2C bus. data = 0x%02x\n", *data);
    return(OK);
    
abort:
    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF;  */
    OUTPUT_ON;
    LOGPRINT(LOG_MOD_DEFAULT | LOG_LEV_WARNING,
             "Failed to read from DDC EDID I2C bus.\n");
    return(ERROR);
}


