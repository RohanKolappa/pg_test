
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/io.h>

#include <asm/uaccess.h>

//#include <math.h>

#include "fpga.h"
#include "lut_table.h"
#include "fpga_ioctl.h"
#include "log_control.h"


#define FALSE   0
#define TRUE    1



static struct LUT_struct *default_lut=NULL;
struct LUT_struct lookupTable_copy[] = {


    { "INVALID/NO VIDEO",
      FALSE, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 
      0, 0, 0, -2, 0, 0, 0x0, 0x80, 0, 0, 0, 0, 0,
      0x69, 0xd0, 0x98, 0x46, 0xfe, 
      0, 0x400, 0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ,

};


#ifdef __KERNEL__
//LIST_HEAD(board->v_lut_table_head);
#endif


int lut_table_init(struct v2d_board *board) {
    int i;
    struct LUT_struct *lutp;

    default_lut = &lookupTable_copy[0];

    board->v_lut_size = sizeof(lookupTable_copy)/sizeof(lookupTable_copy[0]);
    board->v_lut_table_head.next= &board->v_lut_table_head;
    board->v_lut_table_head.prev= &board->v_lut_table_head;



    /***************************
     * Build default lut table *
     ***************************/
    if (list_empty(&board->v_lut_table_head)) {
        board->v_lut_size = sizeof(lookupTable_copy)/sizeof(struct LUT_struct);
        for (i = 1; i < board->v_lut_size; i++) {
            lutp = kmalloc(sizeof(struct LUT_struct), GFP_KERNEL);
            if (lutp != NULL) {
                memcpy(lutp, &lookupTable_copy[i], sizeof(struct LUT_struct));
                list_add_tail(&lutp->lut_list, &board->v_lut_table_head);
            }
        }
    }
    
    return 0;

}

#define SEARCH_SPAN     128
#define LOG_LEVEL       LOG_LEV_DEBUG

struct LUT_struct *find_lut_entry( struct v2d_board *board,
        unsigned short hsyncc, unsigned short amvreg, 
        unsigned short amhreg, unsigned short amtreg, 
        int duallink, int is_hardware_interrupt) {
    
    int csdiff, bsdiff, bsidx;
    int cvdiff, bvdiff=0;
    int chdiff, bhdiff=0;
    static struct LUT_struct *set_1_h[SEARCH_SPAN];
    static struct LUT_struct *set_1_s[SEARCH_SPAN];
    struct LUT_struct **set_1;
    int n_set1 = 0;
    static struct LUT_struct *set_2_h[SEARCH_SPAN];
    static struct LUT_struct *set_2_s[SEARCH_SPAN];
    struct LUT_struct **set_2;
    int n_set2 = 0;
    int gotStereo=0;
    struct list_head *tmp;
    struct LUT_struct *lutp;
    int i;

    if (is_hardware_interrupt) {
        set_1 = set_1_h;
        set_2 = set_2_h;
    }
    else {
        set_1 = set_1_s;
        set_2 = set_2_s;
    }

    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Current HSYNC    = %x\n", hsyncc);
    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Current H Res    = %x\n", amhreg);
    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Current V Res    = %x\n", amvreg);
    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Current AMTREG   = %x\n", amtreg);

    
    /*-----------------2/15/2002 2:51PM-----------------
     * A step
     * --------------------------------------------------*/
    if ( (amtreg & (AMTREG_HSYNC|AMTREG_VSYNC)) != 
         (AMTREG_HSYNC|AMTREG_VSYNC) ) {
        LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                 "No Video... aborted ...\n");
        return NULL;
    }
    /*-----------------2/15/2002 2:51PM-----------------
     * Step 1.
     * --------------------------------------------------*/
    gotStereo = amtreg & AMTREG_STEREO;

    /*-----------------2/15/2002 2:52PM-----------------
     * Step 2, create Set 1 ...
     *     Include all entries from LUT with an AMLREG
     *     that is in the same screen resolution as
     *     reported by the V2O box.
     * --------------------------------------------------*/
    /* Add to set 1 all stereo or non-stereo entries as applicable */
    list_for_each(tmp, &board->v_lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        // Code to filter out LUT's based on dualink and
        // pixelclk : Shanker 11/09/09
        if ((duallink == DUALLINK_NO) && (lutp->a_lpcr > 165000)) continue;
        if ((duallink == DUALLINK_YES) && (lutp->a_lpcr <= 165000)) continue;
        /* PSCW 07/23/07 - Changed code to allow "both" for stereo. */
        if (lutp->a_stereo == LUT_STEREO_BOTH ||
            ((lutp->a_stereo == LUT_STEREO_YES) && gotStereo) || 
            ((lutp->a_stereo == LUT_STEREO_NO) && !gotStereo)) { 
            set_1[n_set1] = lutp;
            n_set1++;
            if (n_set1 >= SEARCH_SPAN)
                break;

            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Add to Set 1 LUT lutreg 0x%x\n", 
                     lutp->a_alutreg);
        }
    }
    
    /*-----------------2/15/2002 3:04PM-----------------
     * Step 3. create Set 2 ...
     *     Include all entries from Set 1 with an AMVREG
     *     that is +/- 0x100 the value reported by the
     *     V2O box.
     * --------------------------------------------------*/
    for(i=0;i<n_set1;i++) {
        if ( (set_1[i]->a_amvreg - 0x100) <= amvreg && 
             (set_1[i]->a_amvreg + 0x100) > amvreg) {
            set_2[n_set2] = set_1[i];
            n_set2++;
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Add to Set 2 LUT 0x%x (amvreg 0x%x) : %s\n",
                     board->v_id, set_1[i]->a_alutreg, set_1[i]->a_amvreg,
                     set_1[i]->comment);
        }
        else {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Discarded Set 2 LUT 0x%x (amvreg 0x%x) INPUT AMV 0x%x : %s\n",
                     board->v_id, set_1[i]->a_alutreg, 
                     set_1[i]->a_amvreg, amvreg, set_1[i]->comment);
        }
    }
 
    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Board %d. %d elements added to Set 2\n", board->v_id, n_set2);
    if ( !n_set2) {
        return NULL;
    }
    /*-----------------2/15/2002 3:17PM-----------------
     * Step 4. create Set 3
     *     Include all entries from Set 2 with an AMLREG
     *     that is identical to the value reported by
     *     the V2O box
     * --------------------------------------------------*/
    for(i=0,n_set1=0;i<n_set2;i++) {
        if ( set_2[i]->a_hsyncc >= hsyncc - set_2[i]->a_vmargin && 
             set_2[i]->a_hsyncc <= hsyncc + set_2[i]->a_vmargin) {
            set_1[n_set1] = set_2[i];
            n_set1++;
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Add to Set 3 LUT 0x%x\n", 
                     board->v_id, set_2[i]->a_alutreg);
        }
    }

    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Board %d. %d elements added to Set 3\n", board->v_id, n_set1);

    if ( !n_set1) {
        return NULL;
    }

    /*  if ( n_set1 == 1) { */
    /*          return set_1[0]; */
    /*  } */

    /*-----------------2/15/2002 3:17PM-----------------
     * Step 5. create Set 4
     *     Include all entries from Set 3 with an AMHREG
     *     that is +/- 0x10 the value reported by the
     *     V2O box.
     * --------------------------------------------------*/
    for(i=0,n_set2=0;i<n_set1;i++) {
        if ( (set_1[i]->a_amhreg - 0x10) <= amhreg && 
             (set_1[i]->a_amhreg + 0x10) > amhreg) {
            set_2[n_set2] = set_1[i];
            n_set2++;
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Add to Set 4 LUT 0x%x\n", 
                     board->v_id, set_1[i]->a_alutreg);
        }
    }

    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
             "Board %d. %d elements added to Set 4\n", board->v_id, n_set2);
    if ( !n_set2) {
        return NULL;
    }
    if ( n_set2 == 1) {
        return set_2[0];
    }

    /*-----------------2/15/2002 3:51PM-----------------
     *  Step 6 looks close ...
     * --------------------------------------------------*/
    bsdiff = 2147483647;
    bvdiff = 2147483647;
    bhdiff = 2147483647;
    bsidx = -1;
        
    for(i=0;i<n_set2;i++) {
                
        LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                 "Board %d. Evaluating LUT idx 0x%x\n", 
                 board->v_id, set_2[i]->a_alutreg);
                         
        csdiff = set_2[i]->a_hsyncc - hsyncc;
        if(csdiff<0) csdiff = -csdiff;
                
        cvdiff = set_2[i]->a_amvreg - amvreg;
        if(cvdiff<0) cvdiff = -cvdiff;
                
        chdiff = set_2[i]->a_amhreg - amhreg;
        if(chdiff<0) chdiff = -chdiff;
                
        if(cvdiff < bvdiff) {
                        
            bsidx = i; /* remember this index */
            bsdiff = csdiff; /* remember this difference */
            bvdiff = cvdiff;
            bhdiff = chdiff;
                        
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Found new best hsync idx=%d, hsync=%x\n",
                     board->v_id, bsidx, set_2[i]->a_hsyncc);
        }
        else if(cvdiff == bvdiff) {
            LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                     "Board %d. Tie on Hsyncs %d vs %d\n", 
                     board->v_id, bsidx, i);
            /* Check vertical res */
            if(csdiff < bsdiff) {
                bsidx = i; /* remember this index */
                bsdiff = csdiff;
                bvdiff = cvdiff;
                bhdiff = chdiff;
                                
                LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                         "Board %d. Found new best vres idx=%d, vres=%x\n",
                         board->v_id, bsidx, set_2[i]->a_amvreg);
            }
            else if(csdiff == bsdiff) {
                                
                LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                         "Board %d. Tie on V Res %d vs %d\n", 
                         board->v_id, bsidx, i);
                if(chdiff < bhdiff) {
                                        
                    bsidx = i; /* remember this index */
                    bsdiff = csdiff;
                    bvdiff = cvdiff;
                    bhdiff = chdiff;
                    LOGPRINT(LOG_MOD_FPGA | LOG_LEVEL,
                             "Board %d. Found new best hres idx=%d, vres=%x\n",
                             board->v_id, bsidx, set_2[i]->a_amhreg);
                }
            }
        }
    }

    if (bsidx != -1) {
        return set_2[bsidx];
    }

    return NULL;
}

/**************************************
 * Count current number of lut enties *
 **************************************/
static int count_lut_entries(struct v2d_board *board) {

    struct list_head *tmp;
    int count = 0;
    list_for_each(tmp, &board->v_lut_table_head) {
        count++;
    }
    return count;
}

/**************************************************************
 * Clear the current lut table list and free allocated memory *
 **************************************************************/
static void reset_lut_table(struct v2d_board *board) {
    struct list_head *tmp, *tmp2;
    struct LUT_struct *lutp;
    unsigned long lock_flags;

    spin_lock_irqsave(&board->v_lut_lock, lock_flags);
    if (board->v_ctl != NULL)
        board->v_ctl->current_addside_lut = NULL;
    list_for_each_safe(tmp, tmp2, &board->v_lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        list_del(&lutp->lut_list);
        kfree(lutp);
    
    }

    board->v_lut_size = 1;

    spin_unlock_irqrestore(&board->v_lut_lock, lock_flags);

}

/******************************************
 * Ioctl handler for lut_table sub module *
 ******************************************/
int lut_table_ioctl(struct v2d_board *board, unsigned int cmd, unsigned long data) {

    struct list_head *tmp;
    struct LUT_struct *lutp = (struct LUT_struct *) data;
    struct LUT_struct lut;
    unsigned long lock_flags;

    switch (cmd) {

    case FPGA_IOC_RESET_LUT:    
        /**************************
         * Delete all LUT entries *
         **************************/
        LOGPRINT(LOG_MOD_FPGA | LOG_LEV_INFO, 
                 "Channel %d. Resetting LUT Table on user request.\n", 
                 board->v_id + 1);
        reset_lut_table(board);

        return 0;

        
    case FPGA_IOC_SET_LUT:    
        /*********************
         * Add new LUT entry *
         *********************/
        if (lutp == NULL)
            return -EINVAL;

        if (copy_from_user(&lut, lutp, sizeof(struct LUT_struct)))
            return -EFAULT;

        lutp = kmalloc(sizeof(struct LUT_struct), GFP_KERNEL);
        if (lutp == NULL)
            return -ENOMEM;


        memcpy(lutp, &lut, sizeof(struct LUT_struct));

        spin_lock_irqsave(&board->v_lut_lock, lock_flags);
        list_add_tail(&lutp->lut_list, &board->v_lut_table_head);
        board->v_lut_size = count_lut_entries(board);  
                            /* update current count */
        spin_unlock_irqrestore(&board->v_lut_lock, lock_flags);
        
        return 0;

    case FPGA_IOC_GET_LUT:

        if (lutp == NULL)
            return -EINVAL;

        if (copy_from_user(&lut, lutp, sizeof(struct LUT_struct)))
            return -EFAULT;


        list_for_each(tmp, &board->v_lut_table_head) {
            lutp = list_entry(tmp, struct LUT_struct, lut_list);

            if (lut.lut_list.next == NULL)
                break;

            if ((struct LUT_struct *) lut.lut_list.next == lutp)
                lut.lut_list.next = NULL;

        }

        memcpy(&lut, lutp, sizeof(struct LUT_struct));
        if (tmp != board->v_lut_table_head.prev)
            lut.lut_list.next = (struct list_head *) lutp;
        else
            lut.lut_list.next = NULL;

        if (copy_to_user((struct LUT_struct *)data, 
                         &lut, sizeof(struct LUT_struct)) != 0)
            return -EFAULT;
            
        
        return 0;
    }
    
    return 0;

}




