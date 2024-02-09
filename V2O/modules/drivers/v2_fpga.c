#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>


#include "fpga.h"
#include "legacyv2d.h"
#include "id18125fifo.h"
#include "ad9888vif.h"
#include "ad9887vif.h"
#include "ddcedid.h"
#include "ad9852dds.h"
#include "lut_table.h"
#include "led_control.h"
#include "log_control.h"

static int v2_fpga_pci_init (struct v2d_board *board);
static int v2_fpga_pci_exit (struct v2d_board *board);

static int adc_init(struct v2d_board *board)
{
    unsigned char b[1];
    int retries;

    for (retries = 0; retries < 100; retries++) {
        ad9887_init(board);
        if (ad9887_read(VIF_9887_CHIP_REV, b, 1) == OK) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                     "AD9887 initialized after %d retries. chip revision 0x%02x\n", retries, b[0]);
            ad9887_dvisetup();
            board->v_hardware_version = HARDWARE_VERSION_DA;
            break;
        }
        else {
            ad9888_init(board);
            if (ad9888_read(VIF_CHIP_REV, b, 1) == OK) {
                LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                         "AD9888 initialized after %d retries. chip revision 0x%02x\n", retries, b[0]);
                break;
            } 
        } 
        udelay(100);
    } 
    if (retries >= 100) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Error: Failed to initialize the ADC\n");
    }
    board->v_adcinit_retries = retries;
    return 0;
}


int v2_fpga_hardware_init(struct v2d_board *board) {
    struct pci_dev *pdev =board->v_pci_dev;

    v2_fpga_pci_init(board);


    if ((get_fpga_val(board,FPGA_IDREG) & 0xF000) == 0xD000) {
        board->v_type = HARDWARE_V2D_RX;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, "Identified as Receiver Hardware\n");
    }
    else {
        board->v_type = HARDWARE_V2D_TX;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, "Identified as Transmitter Hardware\n");
    }

    /*************************
     * Reset the FPGA device *
     *************************/
    if (board->v_hardware_detected) {
        pci_write_config_byte(pdev, 0x40, 1);
        udelay(10);
        pci_write_config_byte(pdev, 0x40, 0);
    }

    /*******************************
     * Initialize ad9888vif module *
     *******************************/
    adc_init(board);

    /*********************************
     * Initialize id18125fifo module *
     *********************************/
    id18125_init(board);
    
    /*********************************
     * Initialize DDC EDID interface *
     *********************************/
    ddcedid_init(board);

    /****************************
     * Initialize the lut table *
     ****************************/
    lut_table_init(board);

    /****************************************
     * Initialize the FPGA interrupt vector *
     ****************************************/
    if (fpga_interrupt_init(board) != OK) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Failed to register FPGA interrupts\n");
        return -1;
    }

    
    /*******************************
     * Initialize ad9852dds module *
     *******************************/
    ad9852_init(board);
    
    
    /**************************************
     * Initialize the FPGA control module *
     **************************************/
    if (fpga_control_init(board) != OK)
        return -1;


    /*************************************
     * Initialize the LED control module *
     *************************************/
    if (led_control_init(board) != OK)
        return -1;

    /**********************************
     * Initialize flow control module *
     **********************************/
    flow_control_init(board);
    flow_control_init2(board);

    /**********************************
     * Initialize kbm control module *
     **********************************/
    kbm_control_init(board);
    kbm_control_init2(board);

    /*****************************
     * Mark board as initialized *
     *****************************/
    board->v_status |= FPGA_BOARD_INITIALIZED;

    return 0;
}

int v2_fpga_hardware_exit(struct v2d_board *board) {

    /**************************************
     * Release ad9852dds module resources *
     **************************************/
    ad9852_exit(board);

    /**********************************
     * Release FPGA control resources *
     **********************************/
    fpga_control_exit(board);
        
    /************************************
     * Release FPGA interrupt resources *
     ************************************/
    fpga_interrupt_exit(board);
        
    /*********************************
     * Release LED control resources *
     *********************************/
    led_control_exit(board);

    /**********************************
     * Release Flow control resources *
     **********************************/
    flow_control_exit2(board);
    flow_control_exit(board);
    
    kbm_control_exit(board);
    v2_fpga_pci_exit(board);

    board->v_status &= ~FPGA_BOARD_INITIALIZED;

    return 0;
}


static int v2_fpga_pci_init (struct v2d_board *board) {
    struct pci_dev *pdev = board->v_pci_dev;
    if (!board->v_hardware_detected) {
        return 0;
    }


    if (pci_enable_device(pdev))
        goto err_out_free_region;
    
    pci_set_master(pdev);

    board->v_fpga_base = 0;
    board->v_ConfigMemBase = pci_resource_start(pdev, 2);
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, "Using IO port %lx\n", 
             board->v_ConfigMemBase);
    
    if (!request_mem_region(pci_resource_start(pdev, 2),
                            pci_resource_len(pdev, 2), "pci_fpga")) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot reserve MMIO region 2\n");
        goto err_out_free_region;
    }

    if (!request_mem_region(pci_resource_start(pdev, 0),
                            pci_resource_len(pdev, 0), "pci_fpga")) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot reserve MMIO region 0\n");
        goto err_out_free_region2;
    }
    if (!request_mem_region(pci_resource_start(pdev, 1),
                            pci_resource_len(pdev, 1), "pci_fpga")) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot reserve MMIO region 1\n");
        goto err_out_free_region0;
    }
    

    board->v_AddMemBase = (unsigned long)ioremap(pci_resource_start(pdev, 0),
                                                 pci_resource_len(pdev, 0));
    if (!board->v_AddMemBase) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot remap MMIO region %lx @ %lx\n",
                 (unsigned long) pci_resource_len(pdev, 0), 
                 (unsigned long) pci_resource_start(pdev, 0));
        goto err_out_free_region1;
    }
    board->v_DropMemBase = (unsigned long)ioremap(pci_resource_start(pdev, 1),
                                                  pci_resource_len(pdev, 1));
    if (!board->v_DropMemBase) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot remap MMIO region %lx @ %lx\n",
                 (unsigned long) pci_resource_len(pdev, 1), 
                 (unsigned long) pci_resource_start(pdev, 1));
        goto err_out_free_region1;
    }

    board->v_ConfigMemBase = (unsigned long)ioremap(pci_resource_start(pdev, 2),
                                                    pci_resource_len(pdev, 2));
    if (!board->v_ConfigMemBase) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "fpga_device: cannot remap MMIO region %lx @ %lx\n",
                 (unsigned long) pci_resource_len(pdev, 2), 
                 (unsigned long) pci_resource_start(pdev, 2));
        goto err_out_free_region1;
    }


    if (pci_set_dma_mask(pdev, 0xffffffff)) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "Not Capable of 32 bit DMA\n");

        if (pci_set_dma_mask(pdev, 0xffff)) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                     "Not Capable of 16 bit DMA\n");
        }
    }    
    return 0;
    


err_out_free_region1:
    release_mem_region(pci_resource_start(pdev, 1), pci_resource_len(pdev, 1));

err_out_free_region0:
    release_mem_region(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));

err_out_free_region2:
    release_region(pci_resource_start(pdev, 2), pci_resource_len(pdev, 2));

err_out_free_region:
    return -ENODEV;
}

static int v2_fpga_pci_exit(struct v2d_board *board) {
    struct pci_dev *pdev = board->v_pci_dev;
    if (!board->v_hardware_detected) {
        return 0;
    }

    release_mem_region(pci_resource_start(pdev, 0), 
                       pci_resource_len(pdev, 0));
    release_mem_region(pci_resource_start(pdev, 1), 
                       pci_resource_len(pdev, 1));
    release_mem_region(pci_resource_start(pdev, 2), 
                       pci_resource_len(pdev, 2));
    
    iounmap((char *)board->v_AddMemBase);
    iounmap((char *)board->v_DropMemBase);
    iounmap((char *)board->v_ConfigMemBase);
    
    pci_disable_device(pdev);
    return 0;

}

