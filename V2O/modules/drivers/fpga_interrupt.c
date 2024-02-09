#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/signal.h>

#include "fpga.h"
#include "log_control.h"
#include "legacyv2d.h"
#include "v4_fpga.h"
#include "fpga_ioctl.h"
#include "ad9888vif.h"
#include "id18125fifo.h"
#include "plx9056pciif.h"

#define PLX_INTR_INVALID    0x00 
#define PLX_INTR_DMA0       0x01
#define PLX_INTR_DMA1       0x02
#define PLX_INTR_FPGA       0x04 

static irqreturn_t fpga_interrupt_handler(int irq, void *dev_id);

static int fpga_interrupt_procfile_read(char* page, char** start, off_t off, 
                                        int count, int* eof, void* data) ;

static int check_plx_interrupt (struct v2d_board * board);

/******************************************
 * FPGA interrupt initialization function *
 ******************************************/
int fpga_interrupt_init(struct v2d_board *board)
{
    struct proc_dir_entry *pdr;

    if (!board->v_hardware_detected) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
            "Could not initialize interrupt handler. Board %d not detected\n", 
            board->v_id);
        return 0;
    }

    /**************************
     * Disable PLX interrupts *
     **************************/
    if (board->v_pci_dev_type == PLX_DEVICE) {
        plx_interrupt_disable(board, PLX_INT_TYPE_PCI_MAIN |
                                     PLX_INT_TYPE_LOCAL |
                                     PLX_INT_TYPE_DMA_0 |
                                     PLX_INT_TYPE_DMA_1);
    }

    /******************************************
     * Register our interrupt with the kernel *
     ******************************************/
    if (request_irq(board->v_irq, fpga_interrupt_handler, IRQF_SHARED, 
                        "fpga", board) != 0) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Could not register FPGA IRQ %d\n", board->v_irq);
        return ERROR;
    }

    /*****************************************
     * Create interrupt statistics proc file *
     *****************************************/
    pdr = create_proc_read_entry("interrupts", 0, board->v_pdr,
                                 fpga_interrupt_procfile_read, board);
    if (pdr == NULL) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_CRITICAL, 
                 "Could  not create /proc/fpga_interrupts entry\n");
        free_irq(board->v_irq, board);
        return ERROR;
    }
    
    
    /****************************************
     * Initialize interrupt status and mask *
     ****************************************/
    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        set_fpga_val(board, FPGA_IMREG, 0);
        and_in_fpga_val(board, FPGA_ISREG, V2_INTR_BITS);
        set_fpga_val(board, FPGA_IMREG, 0);
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        and_in_fpga_val(board, FPGA_ISREG, V4_INTR_BITS);
        set_fpga_val(board, FPGA_IMREG, 0);
        /*************************
         * Enable PLX interrupts *
         *************************/
        plx_interrupt_enable(board, PLX_INT_TYPE_PCI_MAIN |
                                    PLX_INT_TYPE_LOCAL |
                                    PLX_INT_TYPE_DMA_0 |
                                    PLX_INT_TYPE_DMA_1);
    }

    return 0;
}


/****************************************
 * FPGA interrupt finalization function *
 ****************************************/
void fpga_interrupt_exit(struct v2d_board *board)
{
    /**********************
     * Unregister the IRQ *
     **********************/
    if (board->v_hardware_detected) {
        free_irq(board->v_irq, board);
    }

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
             "Unregistered FPGA IRQ %d\n", board->v_irq);

    /**********************************
     * Unregister the proc file entry *
     **********************************/
    remove_proc_entry("interrupts", board->v_pdr);

    LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG, 
             "Removed /proc/interrupts file entry\n");

}

/***************************************************************
 * FPGA Main Interrupt handler. All FPGA interrupts go through *
 * this. Finds out which sub-interrupt fired and schedules the *
 * corresponding task.                                         *
 ***************************************************************/
static irqreturn_t fpga_interrupt_handler(int irq, void *dev_id) 
{
    int idx;
    unsigned long save_status, status = 0, mask;
    struct v2d_board *board= (struct v2d_board *) dev_id;
    struct tasklet_struct *pTasklet=NULL;

    if (board->v_pci_dev_type == V2DI50_DEVICE) {
        status = get_fpga_val(board,FPGA_ISREG) & V2_INTR_BITS;
        mask = get_fpga_val(board,FPGA_IMREG) & V2_INTR_BITS;
        save_status = status;
        status &= mask;
        set_fpga_val(board, FPGA_ISREG, (save_status & ~status));
    }
    else if (board->v_pci_dev_type == PLX_DEVICE) {
        int plxstat = check_plx_interrupt(board);

        if (plxstat & PLX_INTR_INVALID) {
            return IRQ_NONE;
        }
        if (plxstat & PLX_INTR_FPGA) {
            status = get_fpga_val(board, FPGA_ISREG) & V4_INTR_BITS;
            mask = get_fpga_val(board,FPGA_IMREG) & V4_INTR_BITS;
            save_status = status;
            status &= mask;
            set_fpga_val(board, FPGA_ISREG, (save_status & ~status));
        }
        if (plxstat & PLX_INTR_DMA0) {
            status |= PLX_DMA_0_BIT;
        }
        if (plxstat & PLX_INTR_DMA1) {
            status |= PLX_DMA_1_BIT;
        }
    }
    else
        return IRQ_NONE;
    
    for (idx = MAX_FPGA_INTERRUPTS - 1; idx >= 0; idx--) {

        if ((status & (1<<idx)) ) {
            board->v_intinfo[idx].ii_count++;
            pTasklet = board->v_tasklet + idx;

            /*****************************************************
             * Need to set status again to acknowledge interrupt *
             *****************************************************/
            if (board->v_pci_dev_type == V2DI50_DEVICE) 
                set_fpga_val(board, FPGA_ISREG, (1<<idx));

            /*********************************************************
             * If bottom half has been registerd, mask this          *
             * interrupt. The bottom half will unmask this interrupt *
             * again if needed                                       *
             *********************************************************/
            if (board->v_intinfo[idx].ii_tophalf == NULL) {
                if (idx < INT_30_PLX_DMA_0) {
                    and_in_fpga_val(board, FPGA_IMREG, ~(1<<idx));
                }
                else if (idx == INT_30_PLX_DMA_0)
                    fpga_interrupt_disable(board, PLX_INT_TYPE_DMA_0);
                else
                    fpga_interrupt_disable(board, PLX_INT_TYPE_DMA_1);
            }

            /*********************************************
             * Schedule a tasklet (bottom half) for each *
             * interrupt fired or call its top half if   *
             * registered                                *
             *********************************************/
            if (board->v_intinfo[idx].ii_tophalf != NULL) {
                (*(board->v_intinfo[idx].ii_tophalf))((unsigned long) board);
            }
            else {
                if (pTasklet->func != NULL) {
                    if (board->v_intinfo[idx].ii_disabled == 0) {
                        fpga_interrupt_enable(board, idx);
                    }
                    tasklet_schedule(pTasklet); 
                }
                else  {
                    fpga_interrupt_disable(board, idx);
                }
            }
        }
    }

    return IRQ_HANDLED;
}

/*******************************
 * Enable given FPGA interrupt *
 *******************************/
int fpga_interrupt_enable(struct v2d_board *board, int irq) {

    unsigned long mask, new_mask;
    
    if ( irq < 0 || irq >= MAX_FPGA_INTERRUPTS ) return ERROR;

    if (board->v_pci_dev_type == PLX_DEVICE) {
        if (irq == INT_30_PLX_DMA_0) {
            plx_interrupt_enable(board, PLX_INT_TYPE_DMA_0);
            return OK;
        }
        else if (irq == INT_31_PLX_DMA_1) {
            plx_interrupt_enable(board, PLX_INT_TYPE_DMA_1);
            return OK;
        }
    }

    /* Acknowledge the interrupt */
    and_in_fpga_val(board, FPGA_ISREG, (1 << irq));     
    
    mask = get_fpga_val(board, FPGA_IMREG);
    
    new_mask = (1<<irq);
    if (board->v_pci_dev_type == PLX_DEVICE) {
        new_mask &= V4_INTR_BITS;
    }
    else {
        new_mask &= V2_INTR_BITS;
    }
    if ( !new_mask) {
        return ERROR;
    }
    
    /* Enable interrupt */
    board->v_intinfo[irq].ii_status = 1;
    if ( (mask & new_mask) ) {
        return OK;
    } else {
        mask |= new_mask;
    }

    set_fpga_val(board, FPGA_IMREG, mask);

    return OK;
} 

/********************************
 * Disable given FPGA interrupt *
 ********************************/
int fpga_interrupt_disable(struct v2d_board *board, int irq) {

    unsigned long mask, new_mask;

    if ( irq < 0 || irq >= MAX_FPGA_INTERRUPTS ) return ERROR;

    if (board->v_pci_dev_type == PLX_DEVICE) {
        if (irq == INT_30_PLX_DMA_0) {
            plx_interrupt_disable(board, PLX_INT_TYPE_DMA_0);
            return OK;
        }
        else if (irq == INT_31_PLX_DMA_1) {
            plx_interrupt_disable(board, PLX_INT_TYPE_DMA_1);
            return OK;
        }
    }

    /* Acknowledge the interrupt */
    and_in_fpga_val(board, FPGA_ISREG, (1 << irq));     

    mask = get_fpga_val(board, FPGA_IMREG);
    
    new_mask = (1<<irq);
    if (board->v_pci_dev_type == PLX_DEVICE) {
        new_mask &= V4_INTR_BITS;
    }
    else {
        new_mask &= V2_INTR_BITS;
    }
    if ( !new_mask) 
        return ERROR;
    
    /* Disable interrupt */
    board->v_intinfo[irq].ii_status = 0;
    if ( (mask & new_mask) ) {
        mask &= ~new_mask;
    } else {
        return OK;
    }

    set_fpga_val(board, FPGA_IMREG, mask);

    return OK;
}

/*******************************
 * Fire a given FPGA interrupt *
 *******************************/
int fpga_interrupt_fire( struct v2d_board *board, int irq) {

    if ( irq < 0 || irq >= MAX_FPGA_INTERRUPTS ) return ERROR;

    set_fpga_val(board, FPGA_ISREG, (1<<irq));

    return OK;
}


/******************************************
 * FPGA irq handler registration function *
 ******************************************/
int fpga_register_irq(struct v2d_board *board, int irq, 
                      void (*handler)(unsigned long data)) {
    struct tasklet_struct *pTasklet=NULL;
    pTasklet = board->v_tasklet + irq;


    board->v_intinfo[irq].ii_disabled = 0;
    board->v_intinfo[irq].ii_tophalf = NULL;

    /************************
     * Register the handler *
    ************************/
    tasklet_init (pTasklet, handler, (unsigned long) board);

    /************************
     * Enable the interrupt *
     ************************/
    fpga_interrupt_enable(board, irq);
    
    return 0;
        
}

/******************************************
 * FPGA irq handler registration function *
 ******************************************/
int fpga_register_disabled_irq(struct v2d_board *board, int irq, 
                               void (*handler)(unsigned long data)) {
    struct tasklet_struct *pTasklet=NULL;
    pTasklet = board->v_tasklet + irq;

    board->v_intinfo[irq].ii_tophalf = NULL;

    /************************
     * Register the handler *
    irq_handler[irq] = handler;
    ************************/
    tasklet_init (pTasklet, handler, (unsigned long) board);

    /*********************************
     * Dont enable the interrupt yet *
     *********************************/
    board->v_intinfo[irq].ii_disabled = 1;

    return 0;
        
}

/******************************************
 * FPGA irq handler registration function *
 ******************************************/
int fpga_register_tophalf_irq(struct v2d_board *board, int irq, 
                              void (*handler)(unsigned long data)) {

    board->v_intinfo[irq].ii_disabled = 0;

    /**************************************
     * Remember the function to call back *
     **************************************/
    board->v_intinfo[irq].ii_tophalf = handler;

    /************************
     * Enable the interrupt *
     ************************/
    fpga_interrupt_enable(board, irq);

    return 0;
        
}

/*********************************************
 * FPGA irq handler un-registration function *
 *********************************************/
int fpga_unregister_irq(struct v2d_board *board, int irq) {

    struct tasklet_struct *pTasklet=NULL;
    pTasklet = board->v_tasklet + irq;

    /*************************
     * Disable the interrupt *
     *************************/
    fpga_interrupt_disable(board, irq);

    /**************************
     * Unregister the handler *
     **************************/
    if (board->v_intinfo[irq].ii_tophalf == NULL)
        tasklet_kill(pTasklet);

    return 0;
        
}

int fpga_interrupt_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    struct interrupt_io *interruptiop = (struct interrupt_io *)data;
    struct interrupt_io interruptio;
    int i;

    switch (cmd) {
    case FPGA_IOC_GET_INTR_STAT:
        if (interruptiop == NULL)
            return -EFAULT;

        if (copy_to_user(interruptiop, &interruptio, 
                         sizeof(struct interrupt_io)) != 0)
            return -EFAULT;
        return 0;

    case FPGA_IOC_GET_INTR_RST:
        for (i=0; i < MAX_FPGA_INTERRUPTS; i++) {
            board->v_intinfo[i].ii_count = 0;
        }
        return 0;

    case FPGA_IOC_INTR_FIRE:
        if ((int *)data == NULL)
            return -EFAULT;
        if (copy_from_user(&i, (unsigned char *)data, sizeof(unsigned int)) != 0)
            return -EFAULT;
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, "Firing interrupt %d\n", i);
        fpga_interrupt_enable(board, i);
        fpga_interrupt_fire(board, i);
        return 0;
    }

    return -EINVAL;
}

/****************************************************
 * Routine to check if this is a valid PLX interrupt*
 ****************************************************/
int check_plx_interrupt(struct v2d_board *board) {
    unsigned long regintr;
    int rc = 0;

    /************************************************
     * Read PLX and FPGA interrupt status registers *
     ************************************************/
    regintr = get_plx_val(board, PCI9056_INT_CTRL_STAT);

    /***************************************************************
     * If master PCI interrupt not enabled or neither PCI local    *
     * nor one of the DMA interrupts then it is not our interrupt. *
     ***************************************************************/
    if ((regintr & (1 << 8)) == 0 ||
        (((regintr & (1 << 15)) == 0 || (regintr & (1 << 11)) == 0) &&
        ((regintr & (1 << 21)) == 0 || (regintr & (1 << 18)) == 0) &&
        ((regintr & (1 << 22)) == 0 || (regintr & (1 << 19)) == 0))) {
        return 0;
    }

    /***************************************************************
     * If PLX chip is in low power state, local register reads are *
     * disabled and will always return 0xffffffff. Since we share  *
     * PCI interrupts, PLX ISR may continue to be called so we     *
     * ignore the interrupts when this is the case.                *
     ***************************************************************/
    if (regintr == 0xffffffff) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_DEBUG,
                 "Board %p chip in low power state \n", board);
        return 0;
    }

    /*********************************************************
     * For DMA interrupts, verify interrupt is routed to PCI *
     *********************************************************/
    if ((regintr & (1 << 21)) && (regintr & (1 << 18))) {
        unsigned long regval = get_plx_val(board, PCI9056_DMA0_MODE);
        if (regval & (1 << 17)) 
            rc |= PLX_INTR_DMA0;
    }

    if ((regintr & (1 << 22)) && (regintr & (1 << 19))) {
        unsigned long regval = get_plx_val(board, PCI9056_DMA1_MODE);
        if (regval & (1 << 17)) 
            rc |= PLX_INTR_DMA1;
    }

    if ((regintr & (1 << 15)) && (regintr & (1 << 11))) {
        rc |= PLX_INTR_FPGA; 
    }

    return rc;
}

/***************************************************************
 * Provide interrupt statistics to user via proc file system.  *
 ***************************************************************/
static int fpga_interrupt_procfile_read(char* page, char** start, off_t off, 
                                        int count, int* eof, void* data) 
{
    int i;
    int len = 0;  
    struct v2d_board *board;
    board = (struct v2d_board *) data;

    /***********************************************************
     * Since we provide all the data at one shot, any non zero *
     * offset means we dont have anything more to say          *
     ***********************************************************/
    if (off > 0) {
        *eof = -1; /* EOF ?? */
        return 0;
    }
    
    len += sprintf(page + len, 
                   "%-5s %-10s %-6s\t\t%-5s %-10s %-6s\n", 
                   "IRQ", "Count", "Status", "IRQ", "Count", "Status");
    for (i = 0; i < MAX_FPGA_INTERRUPTS; i++) {
        if ((i % 2) == 0)
            len += sprintf(page + len, 
                       "%-5d %-10lu %-6d\t\t",
                       i, board->v_intinfo[i].ii_count, 
                       board->v_intinfo[i].ii_status);
        else
            len += sprintf(page + len, 
                       "%-5d %-10lu %-6d\n",
                       i, board->v_intinfo[i].ii_count, 
                       board->v_intinfo[i].ii_status);
    }

    return len;
}

