#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "fpga.h"
#include "id18125fifo.h"


/* serial interface defines */

static volatile int *ser_ifreg;   /* serial interface register */
static int _empty_;
static int _full_;
static int id18125initialized = 0;

#define FIFO_DELAY      udelay(1)
#define FIFO_CLOCK_HI   *ser_ifreg |= PFIFOREG_SCLK
#define FIFO_CLOCK_LO   *ser_ifreg &= ~PFIFOREG_SCLK
#define FIFO_DATA_HI    *ser_ifreg |= PFIFOREG_SDAT
#define FIFO_DATA_LO    *ser_ifreg &= ~PFIFOREG_SDAT
#define FIFO1_ENABLE    *ser_ifreg &= ~(PFIFOREG_SEN_0 | PFIFOREG_SEN_2 | PFIFOREG_SEN_4)
#define FIFO2_ENABLE    *ser_ifreg &= ~(PFIFOREG_SEN_1 | PFIFOREG_SEN_3 | PFIFOREG_SEN_5)
#define FIFO1_DISABLE   *ser_ifreg |= (PFIFOREG_SEN_0 | PFIFOREG_SEN_2 | PFIFOREG_SEN_4)
#define FIFO2_DISABLE   *ser_ifreg |= (PFIFOREG_SEN_1 | PFIFOREG_SEN_3 | PFIFOREG_SEN_5)

/*******************************************************************************
 *
 * serial_write - writes 20 bits to the fifo's LSB first.
 *
 * RETURNS: nothing
 */

static void serial_write(int data)
{
    int i;
    
    for (i=0; i < 20; i++) {
        FIFO_CLOCK_LO;
        if (data & 1)
            FIFO_DATA_HI;
        else
            FIFO_DATA_LO;
        data >>= 1;
        FIFO_DELAY;
        FIFO_CLOCK_HI;
        FIFO_DELAY;
    }
    FIFO_CLOCK_LO;
    FIFO_DELAY;
}


/*******************************************************************************
 *
 * id18125Init - initializes the FIFO offset device interface.
 *
 * (should call one time)
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */

int id18125_init(struct v2d_board *board)
{
    if (board->v_ConfigMemBase == 0)
        return ERROR;

    /* setup i2c interface register */
    ser_ifreg = (int *)(board->v_ConfigMemBase+ FPGA_PFIFOREG);
    
    /* turn everything off */
    FIFO1_DISABLE;
    FIFO2_DISABLE;

    id18125initialized = 1;

    return id18125_write(0xf, 0x3FFF1);
    
}

/*******************************************************************************
 *
 * id18125Write - writes empty and full offset values to the fifo's.
 *
 *
 *
 * RETURNS: OK or ERROR if a hardware failure is detected.
 */

int id18125_write
(
    int empty,     /* empty offset */
    int full       /* full offset */
    )
{
    if (!id18125initialized)
        return ERROR;
    
    _empty_ = empty;
    _full_ = full;
    
    /* initially disable FIFOs */
    FIFO_CLOCK_LO;
    FIFO_DELAY;
    FIFO1_DISABLE;
    FIFO2_DISABLE;
    FIFO_DELAY;

    /* enable FIFO 1 */
    FIFO1_ENABLE;

    FIFO_CLOCK_LO;
    /*    FIFO_CLOCK_HI; */
    FIFO_DELAY;

    /* write empty and full offsets */
    serial_write(empty);

    /* PSCW 05/15/06 - FPGA fix for Bugzilla#377 depends on the empty */
    /* and full levels for FIFO 1 be both set to the value for empty. */
    serial_write(1048576 - empty);

    /* disable FIFO 1 */
    FIFO1_DISABLE;
    FIFO_DELAY;

    /* enable FIFO 2 */
    FIFO2_ENABLE;
    /*    FIFO_CLOCK_HI; */
    FIFO_DELAY;

    /* write empty and full offsets */
    serial_write(empty);
    serial_write(full);

    /* disable FIFO 2 */
    FIFO2_DISABLE;
    FIFO_CLOCK_LO;
    
    return(OK);
}


int id18125_read(int *empty, int *full) {

    if (!id18125initialized) {
        *empty = 0;
        *full = 0;
        return ERROR;
    }

    *empty = _empty_;
    *full = _full_;
    return 0;
}
