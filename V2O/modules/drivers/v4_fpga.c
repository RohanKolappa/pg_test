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
#include "v4_fpga.h"
#include "plx9056pciif.h"
#include "lut_table.h"
#include "log_control.h"

static int v4_fpga_pci_init (struct v2d_board *board);
static int v4_fpga_pci_exit (struct v2d_board *board);
static int barinfo_procfile_read(char *page, char **start, off_t off,
                                    int count, int *eof, void *data) ;

static int barinfo_procfile_read(char *page, char **start, off_t off,
                                    int count, int *eof, void *data) {
    struct v2d_board *board= (struct v2d_board*) data;
    int len=0;
    len += sprintf(page + len, "  %5s %10s %10s %10s   %10s %10s %10s\n",
           "Board", "Phys Addr", "Addr", "Size", "Phys Addr", "Addr", "Size");
    len += sprintf(page + len, 
                "    %d     %8x   %8lx   %8x     %8x   %8lx   %8x\n", 
                board->v_id+1,
                (unsigned int) board->v_pci_dev->resource[0].start, 
                board->v_PlxMemBase, 
                (unsigned int) pci_resource_len(board->v_pci_dev, 0),
                (unsigned int) board->v_pci_dev->resource[2].start, 
                board->v_FpgaProgBase, 
                (unsigned int) pci_resource_len(board->v_pci_dev, 2));
     len += sprintf(page + len, 
                "          %8x   %8lx   %8x\n", 
                (unsigned int) board->v_pci_dev->resource[3].start, 
                board->v_ConfigMemBase, 
                (unsigned int) pci_resource_len(board->v_pci_dev, 3));
    return len;
}

int v4_fpga_hardware_init(struct v2d_board *board) {

    if (v4_fpga_pci_init(board)) {
        return -ENODEV;
    }
    /*******************************
     * Initialize PLX module       *
     *******************************/
    plx_control_init(board);
    board->v_status |= FPGA_BOARD_PLX_INITIALIZED;
    flow_control_init(board);
    lut_table_init(board);

    return 0;
}

int v4_fpga_board_init(struct v2d_board *board) {
    unsigned short boardtype = get_fpga_val(board, FPGA_IDREG) & 0xF000;

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
            "Board %d %d Version %d\n", 
            board->v_id, boardtype, board->v_hardware_version);

    if (boardtype == 0xA000) {
        board->v_type = HARDWARE_V2D_TX;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Initializing Transmitter Board %d IRQ %d Type %x\n",
                 board->v_id, board->v_irq, boardtype);
    }
    else if (boardtype == 0xD000) {
        board->v_type = HARDWARE_V2D_RX;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Initializing Receiver Board %d IRQ %d Type %x\n",
                 board->v_id, board->v_irq, boardtype);
    }
    else {
        board->v_type = 0;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO,
                 "Not Initializing Board %d IRQ %d Type %x\n",
                 board->v_id, board->v_irq, boardtype);
        return 0;
    }

    /****************************************
     * Initialize the FPGA interrupt vector *
     ****************************************/
    if (fpga_interrupt_init(board) != OK) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Failed to register FPGA interrupts\n");
        return -1;
    }

    /**************************************
     * Initialize the FPGA control module *
     **************************************/
    if (fpga_control_init(board) != OK) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Failed to Initialize Control IRQ %d\n", board->v_irq);
        return -1;
    }

    /**********************************
     * Initialize flow control module *
     **********************************/
    flow_control_init2(board);

    kbm_control_init(board);
    kbm_control_init2(board);

    /***********************************************************
     * Initialize kbm control module kbm_control_init2(board); *
     ***********************************************************/
    board->v_status |= FPGA_BOARD_INITIALIZED;

    return 0;
}

int v4_fpga_hardware_exit(struct v2d_board *board) {
    v4_fpga_board_exit(board);

    /**********************************
     * Release Flow control resources *
     **********************************/
    if (board->v_status & FPGA_BOARD_PLX_INITIALIZED) {
        flow_control_exit(board);
        plx_control_exit(board);
        v4_fpga_pci_exit(board);
    }
    board->v_status =0;
    return 0;
}

int v4_fpga_board_exit(struct v2d_board *board) {

    if ((board->v_status & FPGA_BOARD_INITIALIZED) !=
            FPGA_BOARD_INITIALIZED) return 0;

    /**********************************
     * Release KBM  control resources *
     **********************************/
     kbm_control_exit(board);

    /**********************************
    * Release FPGA control resources *
    **********************************/
    fpga_control_exit(board);

    flow_control_exit2(board);
        
    if (board->v_hardware_detected) {
        plx_interrupt_disable(board, PLX_INT_TYPE_PCI_MAIN |
        PLX_INT_TYPE_LOCAL |
        PLX_INT_TYPE_DMA_0 |
        PLX_INT_TYPE_DMA_1);
    }

    /************************************
    * Release FPGA interrupt resources *
    ************************************/
    fpga_interrupt_exit(board);

    board->v_status = FPGA_BOARD_PLX_INITIALIZED;

    return 0;
}


static int v4_fpga_pci_init (struct v2d_board *board) {
    struct pci_dev *pdev = board->v_pci_dev;
    int barreg=3;
    unsigned long fpgabase;
    struct proc_dir_entry *pdr;

    if (!board->v_hardware_detected) {
        return 0;
    }

    if (pci_enable_device(pdev))
        goto err_out_free_region;

    board->v_irq = pdev->irq;
    
    pci_set_master(pdev);


    if (request_mem_region(pci_resource_start(pdev, 3),
                            pci_resource_len(pdev, 3), "pci_fpga")) {
        barreg = 3;
        fpgabase = 0x400000;
    }
    if (request_mem_region(pci_resource_start(pdev, 2),
                            pci_resource_len(pdev, 2), "pci_fpga")) {
        if (barreg != 3) {
            barreg = 2;
            fpgabase = 0;
        }
    }
    else {
        goto err_out_free_region;
    }
    if (!request_mem_region(pci_resource_start(pdev, 0),
                            pci_resource_len(pdev, 0), "pci_fpga")) {
        printk( "fpga_device: cannot reserve MMIO region 0\n");
        goto err_out_free_region;
    }
    

    board->v_ConfigMemBase = (unsigned long)ioremap_nocache(
            pci_resource_start(pdev, barreg), 
            pci_resource_len(pdev, barreg));
    if (!board->v_ConfigMemBase) {
        printk( "fpga_device: cannot reserve MMIO region %d\n", barreg);
            goto err_out_free_region;
    }
    if (barreg == 2) {
        board->v_FpgaProgBase = board->v_ConfigMemBase;
    }
    else {
        board->v_FpgaProgBase = (unsigned long)ioremap_nocache(
            pci_resource_start(pdev, 2), 
            pci_resource_len(pdev, 2));
        if (!board->v_FpgaProgBase) {
            printk( "fpga_device: cannot reserve MMIO region %d\n", 2);
                goto err_out_free_region2;
        }
    }


    board->v_PlxMemBase = (unsigned long)ioremap(pci_resource_start(pdev, 0),
                                                 pci_resource_len(pdev, 0));
    if (!board->v_PlxMemBase) {
        printk(
                 "fpga_device: cannot remap MMIO region %lx @ %lx\n",
                 (unsigned long) pci_resource_len(pdev, 0), 
                 (unsigned long)pci_resource_start(pdev, 0));
        goto err_out_free_region0;
    }
    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
            "Board PCI Info %d %lx %lx %lx\n", 
            board->v_id, board->v_ConfigMemBase, 
            board->v_FpgaProgBase, board->v_PlxMemBase);
    board->v_bar_reg = barreg;
    board->v_fpga_base = fpgabase;

    if ((pdr = create_proc_read_entry("barinfo", 0, board->v_pdr, 
                barinfo_procfile_read, board)) == NULL) {
        printk("Could not create fpga video stats proc file\n");
        return -1;
    }
    return 0;



err_out_free_region0:
    if (barreg != 2) {
        release_mem_region(pci_resource_start(pdev, barreg), 
            pci_resource_len(pdev, barreg));
    }
err_out_free_region2:
    release_mem_region(pci_resource_start(pdev, barreg), 
            pci_resource_len(pdev, barreg));

err_out_free_region:
    return -ENODEV;
}

static int v4_fpga_pci_exit(struct v2d_board *board) {
    struct pci_dev *pdev = board->v_pci_dev;

    if (!board->v_hardware_detected) {
        return 0;
    }

    remove_proc_entry("barinfo", board->v_pdr);


    release_mem_region(pci_resource_start(pdev, 0), 
                       pci_resource_len(pdev, 0));
    iounmap((char *)board->v_PlxMemBase);

    release_mem_region(pci_resource_start(pdev, board->v_bar_reg), 
                       pci_resource_len(pdev, board->v_bar_reg));
    iounmap((char *)board->v_ConfigMemBase);
    if (board->v_bar_reg == 3) {
        release_mem_region(pci_resource_start(pdev, 2), 
                       pci_resource_len(pdev, 2));
        iounmap((char *)board->v_FpgaProgBase);
    }

    
    pci_disable_device(pdev);
    return 0;

}

