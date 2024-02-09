
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "fpga.h"
#include "legacyv2d.h"
#include "ad9852dds.h"
#include "log_control.h"

int ad9852_init(struct v2d_board *board)
{
    reset_ad9852(board);
    return(OK);
}

void ad9852_exit(struct v2d_board *board) {
}

/* resets the dds */
void reset_ad9852(struct v2d_board *board) {

    /* put dds in reset */
    set_fpga_val(board, FPGA_PDDSREG, PDDSREG_RESET);

    /* delay 1 ms */
    mdelay(1);

    /* take dds out of reset */
    set_fpga_val(board, FPGA_PDDSREG, 0x0);

    set_ad9852_val(board,DDS_CNTL_REG_03,0x00);

    /* provision UPD clock as an output */
    set_fpga_val(board, FPGA_PDDSREG, PDDSREG_P_UDCK);

    /* initialize dds */
    set_ad9852_val(board,DDS_CNTL_REG_01, 0x00);
    set_ad9852_val(board,DDS_CNTL_REG_02, 0x49);
    set_ad9852_val(board,DDS_CNTL_REG_04, 0x60);
    set_ad9852_val(board,DDS_OSK_MULT_01, 0x0F);
    set_ad9852_val(board,DDS_OSK_MULT_02, 0xFF);
    set_ad9852_val(board,DDS_CNTL_DAC_01, 0x2F);
    set_ad9852_val(board,DDS_CNTL_DAC_02, 0xFF);

    /* set initial frequency */
    set_ad9852_val(board,DDS_FREQ_WORD1_01,0x15);
    set_ad9852_val(board,DDS_FREQ_WORD1_02,0x8c);
    set_ad9852_val(board,DDS_FREQ_WORD1_03,0x7f);
    set_ad9852_val(board,DDS_FREQ_WORD1_04,0x91);
    set_ad9852_val(board,DDS_FREQ_WORD1_05,0xab);
    set_ad9852_val(board,DDS_FREQ_WORD1_06,0x87);

    /* Enable update clock in FPGA */
    or_in_fpga_val(board, FPGA_PDDSREG, PDDSREG_UDCK);
    mdelay(1);
    and_in_fpga_val(board, FPGA_PDDSREG, ~PDDSREG_UDCK);
}

/* tweak the dds */
void tweak_frequency(struct v2d_board *board, int tweak_hi, int tweak_lo) {
    
    set_ad9852_val(board, DDS_FREQ_WORD1_01,(tweak_hi >> 16) & 0x7f);
    udelay(1);
    set_ad9852_val(board, DDS_FREQ_WORD1_02,(tweak_hi >> 8)  & 0xff);
    udelay(1);
    set_ad9852_val(board, DDS_FREQ_WORD1_03, tweak_hi & 0xff);
    udelay(1);
    set_ad9852_val(board, DDS_FREQ_WORD1_04,(tweak_lo >> 16) & 0xff);
    udelay(1);
    set_ad9852_val(board, DDS_FREQ_WORD1_05,(tweak_lo >> 8)  & 0xff);
    udelay(1);
    set_ad9852_val(board, DDS_FREQ_WORD1_06, tweak_lo & 0xff);
    udelay(1);

    or_in_fpga_val(board, FPGA_PDDSREG, PDDSREG_UDCK);
    mdelay(1);
    and_in_fpga_val(board, FPGA_PDDSREG, ~PDDSREG_UDCK);
}
