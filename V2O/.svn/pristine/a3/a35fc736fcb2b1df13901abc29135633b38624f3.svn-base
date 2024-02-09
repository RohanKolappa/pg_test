#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "fpga.h"
#include "ad9888vif.h"

/* #define delay(nanosec) { volatile int i; int k=nanosec; while( k-- > 0 ) i=0; } */

/* I2C defines */

static volatile unsigned short *i2c_ifreg;   /* I2C interface register */

static int ad9888initialized = 0;

#define DELAY           udelay(1)
#define CLOCK_HI        *i2c_ifreg |= PADCREG_SCLK
#define CLOCK_LO        *i2c_ifreg &= ~PADCREG_SCLK
#define DATA_HI         *i2c_ifreg |= PADCREG_SDAT_O
#define DATA_LO         *i2c_ifreg &= ~PADCREG_SDAT_O
#define OUTPUT_ON       *i2c_ifreg |= PADCREG_SDAT_EN
#define OUTPUT_OFF      *i2c_ifreg &= ~PADCREG_SDAT_EN
#define I2C_START       DATA_HI;CLOCK_HI;DELAY;DATA_LO;DELAY;CLOCK_LO;DELAY
#define I2C_STOP        DATA_LO;DELAY;CLOCK_HI;DELAY;DATA_HI
#define I2C_DATA_IN     ((*i2c_ifreg & PADCREG_SDAT_I) >> 2)

/* ad9888 defines */

#define I2C_SLAVE_ADDR0      0x98
#define I2C_SLAVE_ADDR1      0x9A

#define I2C_SLAVE_ADDR       I2C_SLAVE_ADDR0


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
 * ad9888Init - initializes the Analog Devices video input device interface.
 *
 * (should call one time)
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */
int ad9888_init(struct v2d_board *board)
{
    /* setup i2c interface register */
    if (board->v_ConfigMemBase == 0)
        return ERROR;

    i2c_ifreg = (unsigned short *)(board->v_ConfigMemBase+ FPGA_PADCREG);
    
    /* turn everything off, use slave address '0' */
    *i2c_ifreg = 0;

    ad9888initialized = 1;
    return(OK);
}

/*******************************************************************************
 *
 * ad9888Write - writes register values to the Analog Devices video input
 * interface
 *
 *
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */

int ad9888_write
(
    unsigned char start_addr,     /* starting base register address */
    unsigned char *data,          /* array of register values */
    int number_of_bytes           /* number of values to write */
    )
{
    int i;

    if(!ad9888initialized)
        return ERROR;

    /* enable I2C output */
    OUTPUT_ON;
    DELAY;
    
    /* start the I2C transaction */
    I2C_START;
    
    /* write slave address byte */
    if (i2c_wbyte(I2C_SLAVE_ADDR)!=0) goto abort;
    
    /* write base register address byte */
    if (i2c_wbyte(start_addr)!=0) goto abort;
    
    /* write data bytes */
    for (i=0; i < number_of_bytes; i++)
        if (i2c_wbyte(*data++)!=0) goto abort;
    
    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF;  */
    OUTPUT_ON;
    return(OK);
    
abort:
    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF; */
    OUTPUT_ON;
    return(ERROR);
}

/*******************************************************************************
 *
 * ad9888Read - reads register values from the Analog Devices video input
 * interface
 *
 *
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */

int ad9888_read
(
    unsigned char start_addr,     /* starting base register address */
    unsigned char *data,          /* array of register values */
    int number_of_bytes           /* number of values to read */
    )
{
    int i;

    if(!ad9888initialized)
        return ERROR;
    
    /* enable I2C output */
    OUTPUT_ON;
    DELAY;
    
    /* start the I2C transaction */
    I2C_START;
    
    /* write slave address byte */
    if (i2c_wbyte(I2C_SLAVE_ADDR)!=0) goto abort;
    
    /* write base register address byte */
    if (i2c_wbyte(start_addr)!=0) goto abort;
    
    /* re-start the I2C transaction */
    I2C_START;
    
    /* write slave address byte with read bit */
    if (i2c_wbyte(I2C_SLAVE_ADDR|1)!=0) goto abort;

    /* read data bytes with acks */
    for (i=0; i < number_of_bytes; i++)
        *data++ = i2c_rbyte_ack();

    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF; */
    OUTPUT_ON;
    return(OK);
    
abort:
    /* stop the I2C transaction */
    I2C_STOP;
    /*    OUTPUT_OFF;  */
    OUTPUT_ON;
    return(ERROR);
}


