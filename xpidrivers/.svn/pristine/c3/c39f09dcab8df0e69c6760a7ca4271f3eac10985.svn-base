/*
 * lut_table.c
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/videodev2.h>
#include <linux/types.h>
#include <linux/math64.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>

#include "v4l2-hdmi.h"
#include "netvizxpi.h"
#include "lut_table.h"
#include "slice_size.h"

MODULE_DESCRIPTION("Netviz XPI Lookup Table Module");
MODULE_AUTHOR("Barco IP Video Systems");
MODULE_LICENSE("GPL");
                                                                                
static int debug = 0;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "debug level, default is 0 (no debug messages)");

/* Timings standards */
#define V4L2_DV_BT_STD_CEA861   (1 << 0)  /* CEA-861 Digital TV Profile */
#define V4L2_DV_BT_STD_DMT      (1 << 1)  /* VESA Discrete Monitor Timings */
#define V4L2_DV_BT_STD_CVT      (1 << 2)  /* VESA Coordinated Video Timings */
#define V4L2_DV_BT_STD_GTF      (1 << 3)  /* VESA Generalized Timings Formula */

struct LUT_struct cp_frun_luts[] = {

    { .comment = "HDMI-GR 1280x1024@60 Hz",
      .a_alutreg = RESERVED_FRUN_1280_LUTID,
      .a_hres = 1280,
      .a_vres = 1024,
      .a_lrefresh = (unsigned long) 60020,
      .a_hfp = 48,
      .a_hsync = 112,
      .a_hbp = 248,
      .a_vfp = 1,
      .a_vsync = 3,
      .a_vbp = 38,
      .a_lpcr = 108000,
      .a_orig_hres = 1280,
      .a_orig_vres = 1024,
    },
    { .comment = "HDMI-GR 800x600@60 Hz",
      .a_alutreg = RESERVED_FRUN_800_LUTID,
      .a_hres = 800,
      .a_vres = 600,
      .a_lrefresh = (unsigned long) 60320,
      .a_hfp = 40,
      .a_hsync = 128,
      .a_hbp = 88,
      .a_vfp = 1,
      .a_vsync = 4,
      .a_vbp = 23,
      .a_lpcr = 40000,
      .a_orig_hres = 800,
      .a_orig_vres = 600,
    },
    { .comment = "HDMI-GR 1024x768@60 Hz",
      .a_alutreg = RESERVED_FRUN_1024_LUTID,
      .a_hres = 1024,
      .a_vres = 768,
      .a_lrefresh = (unsigned long) 60000,
      .a_hfp = 24,
      .a_hsync = 136,
      .a_hbp = 160,
      .a_vfp = 3,
      .a_vsync = 6,
      .a_vbp = 29,
      .a_lpcr = 65000,
      .a_orig_hres = 1024,
      .a_orig_vres = 768,
    },
};

static inline unsigned htotal(const struct v4l2_bt_timings *t)
{
    return t->width + t->hfrontporch + t->hsync + t->hbackporch;
}

static inline unsigned vtotal(const struct v4l2_bt_timings *t)
{
    return t->height + t->vfrontporch + t->vsync + t->vbackporch;
}

/*
 * This function will return the standard timings corresponding to the LUT
 * entry 'lutp'.
 */
bool lut2dv_timings(struct LUT_struct *lutp, struct v4l2_dv_timings *dv)
{
    struct v4l2_bt_timings *bt;

    if (dv == NULL || lutp == NULL)
        return false;

    bt = &dv->bt;

    bt->width = lutp->a_orig_hres;
    bt->height = lutp->a_orig_vres;
    bt->interlaced = 0;
    bt->pixelclock = (__u64) (lutp->a_lpcr * 1000);
    bt->hfrontporch = lutp->a_hfp;
    bt->hsync = lutp->a_hsync;
    bt->hbackporch = lutp->a_hbp;
    bt->vfrontporch = lutp->a_vfp;
    bt->vsync = lutp->a_vsync;
    bt->vbackporch = lutp->a_vbp;
    bt->il_vfrontporch = 0;
    bt->il_vsync = 0;
    bt->il_vbackporch = 0;
    return true;
}

/*
 * This function initialized the LUT table. It must be called for the
 * LUT routines to work. 
 */
int lut_table_init(struct netvizxpi_state *state)
{
    mutex_init(&state->lut_lock);
    INIT_LIST_HEAD(&state->lut_table_head);
    state->lut_size = 0;
    return 0;
}

/*
 * This function will return the total number of LUT entries in 
 * the LUT table
 */
static int count_lut_entries(struct netvizxpi_state *state) 
{
    struct list_head *tmp;
    int count = 0;

    list_for_each(tmp, &state->lut_table_head) {
        count++;
    }

    return count;
}

/*
 * This function sets the a_hres and a_vres fields in the LUT to what
 * we've computed for the FPGA to use for encoding
 */
static int fill_lut_hvres(struct LUT_struct *lutp)
{
    int hres, vres, h, v, ssize;

    hres = lutp->a_orig_hres;
    vres = lutp->a_orig_vres;

    if (compute_slice_size(hres, vres, &ssize, &h, &v) < 0) {
        return -1;
    }
    else if (((h - hres) < lutp->a_hbp) && ((v - vres) < lutp->a_vbp)) {
        lutp->a_hres = h;
        lutp->a_vres = v;
        lutp->a_hsyncc = lutp->a_orig_vres + lutp->a_vfp + lutp->a_vsync + 
            lutp->a_vbp;
        return 0;
    }
    return -1;
}

static inline int abs_diff(int val1, int val2)
{
    return (val1 >= val2) ? (val1 - val2) : (val2 - val1);
}

static inline bool in_range(int val1, int val2, int diff)
{
    return (abs_diff(val1, val2) <= diff) ? true : false;
}

/*
 * This function will return the LUT table entry matching the STDI 
 * readings bl, lcf, lcvs and the input stereo and duallink flags
 *
 * Note since STDI readback values for GR have a range of ~3 for LCF
 * and LCVS (Sec. 10.8.3.5, Table 55 ADV7604 HW RvF Aug'10) we allow
 * (3 + vertical margin) in comparing the LUT entries for matching.
 */
#define ANALOG_MARGIN   3

struct LUT_struct *find_addside_lut_entry(struct netvizxpi_state *state,
                                          u16 fcl,
                                          u16 lcf,
                                          u16 lcvs,
                                          int stereo,
                                          int verbose)
{
    struct LUT_struct *lutp, *bestlutp = NULL;
    struct list_head *tmp;
    int height, refresh, analog_margin;

    if (fcl == 0)
        return NULL;

    height = lcf + 1;
    refresh = ADV7604_fsc / (fcl * 256);

    if (debug || verbose) {
        printk(KERN_INFO \
                "%s: fcl %d height %d lcvs %d stereo %d, refresh %d\n", 
                __func__, fcl, height, lcvs, stereo, refresh);
    }

    list_for_each(tmp, &state->lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (lutp->a_alutreg == 0 || fill_lut_hvres(lutp) < 0)
            continue;
        if (lutp->a_stereo != LUT_STEREO_BOTH && 
                ((lutp->a_stereo == LUT_STEREO_YES && !stereo) || 
                 (lutp->a_stereo == LUT_STEREO_NO && stereo)))  
            continue;
        analog_margin = lutp->a_vmargin + ANALOG_MARGIN;
        if (in_range(height, lutp->a_hsyncc, analog_margin) &&
                in_range(lcvs, lutp->a_vsync, analog_margin) && 
                in_range(refresh, (int) (lutp->a_lrefresh + 500) / 1000, 1)) {
            if (bestlutp == NULL) {
                bestlutp = lutp;
                continue;
            }
            else if (abs_diff(bestlutp->a_hsyncc, height) < 
                    abs_diff(lutp->a_hsyncc, height)) {
                continue;
            }
            else {
                bestlutp = lutp;
            }
        }
        else if (debug || verbose) {
            printk(KERN_INFO "%s: LUT ID %d (%d/%d/%ld) margin %d " 
                    "does not match (%d/%d/%d)\n", __func__, lutp->a_alutreg, 
                    lutp->a_hsyncc, lutp->a_vsync, lutp->a_lrefresh, 
                    lutp->a_vmargin, height, lcvs, refresh);
        }
    }
    if (debug || verbose) {
        if (bestlutp == NULL) 
            printk(KERN_INFO "%s: no matching LUT found for (%d/%d)\n", 
                    __func__, height, refresh);
        else
            printk(KERN_INFO "%s: returning LUT ID %d (%s).\n", 
                __func__, bestlutp->a_alutreg, bestlutp->comment);
    }

    return bestlutp;
}

/*
 * This function adds a new auto-created LUT table entry to the LUT list 
 * if there's no auto-created entry in the list, or replaces the existing 
 * auto-created LUT table entry in the LUT list with the new entry if 
 * there's already an auto-created entry in the list
 */
static struct LUT_struct *add_reserved_auto_lut_entry(
        struct netvizxpi_state *state, struct LUT_struct *lutp)
{
    int found = 0;
    struct list_head *tmp;
    struct LUT_struct *tmplutp = NULL;

    if (lutp == NULL) 
        return NULL;

    lutp->a_alutreg = RESERVED_AUTO_LUTID;

    mutex_lock(&state->lut_lock);

    // reset addside LUT if it happens to be an auto-created LUT since 
    // we will be replacing it with the new LUT
    if (state->current_addside_lut && 
            state->current_addside_lut->a_alutreg == RESERVED_AUTO_LUTID)
        state->current_addside_lut = NULL;

    list_for_each(tmp, &state->lut_table_head) {
        tmplutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (tmplutp->a_alutreg == RESERVED_AUTO_LUTID) {
            memcpy(&lutp->lut_list, &tmplutp->lut_list, 
                    sizeof(struct list_head));
            memcpy(tmplutp, lutp, sizeof(struct LUT_struct));
            found = 1;
            break;
        }
    }
    mutex_unlock(&state->lut_lock);

    if (found) 
        return tmplutp;

    if (add_lut_entry(state, lutp) < 0) 
        return NULL;
    
    return get_lut_from_lutid(state, RESERVED_AUTO_LUTID);
}

/*
 * This function creates a new LUT table entry matching the DV timings 
 * 'dv'and the input stereo flag and adds the entry to the LUT list
 */
struct LUT_struct *add_dv_timings_lut_entry(struct netvizxpi_state *state, 
        struct v4l2_dv_timings *dv, int stereo, int verbose)
{
    struct v4l2_bt_timings *bt;
    struct LUT_struct lut; 

    if (dv == NULL) {
        if (debug || verbose) 
            printk("%s: NULL dv_timings.\n", __func__);
        return NULL;
    }
    bt = &dv->bt;
    if (htotal(bt) == 0 || vtotal(bt) == 0) {
        if (debug || verbose) 
            printk("%s: Invalid H/V total (%d/%d).\n", __func__, 
                    htotal(bt), vtotal(bt));
        return NULL;
    }
    memset(&lut, 0, sizeof(struct LUT_struct));
    lut.a_rgain = lut.a_ggain = lut.a_bgain = 128;
    lut.a_roffset = lut.a_goffset = lut.a_boffset = 64;
    lut.a_noise = 0x0f09;
    lut.a_stereo = stereo;
    lut.a_orig_hres = bt->width;
    lut.a_orig_vres = bt->height;
    lut.a_lrefresh = div_u64(bt->pixelclock * 1000, htotal(bt) * vtotal(bt)); 
    lut.a_lpcr = div_u64(bt->pixelclock, 1000);
    sprintf(lut.comment, "XPI-DV %dx%d@%d", lut.a_orig_hres, lut.a_orig_vres, 
            (int) (lut.a_lrefresh + 500) / 1000);
    lut.a_hfp = bt->hfrontporch;
    lut.a_hsync = bt->hsync;
    lut.a_hbp = bt->hbackporch;
    lut.a_vfp = bt->vfrontporch;
    lut.a_vsync = bt->vsync;
    lut.a_vbp = bt->vbackporch;
    if (fill_lut_hvres(&lut) < 0) {
        printk(KERN_INFO "%s: failed to find valid slice for LUT %s\n", 
                __func__, lut.comment);
    }
    if (debug || verbose) {
        printk(KERN_INFO \
                "%s: adding LUT %s (stereo %s) to list of LUT entries\n", 
                __func__, lut.comment, (stereo) ? "Yes" : "No");
    }
    return add_reserved_auto_lut_entry(state, &lut);
}

/*
 * This function will return the LUT table entry matching the timing 
 * given in the dv timings struct 'dv'
 */
struct LUT_struct *get_lut_from_dv_timings(struct netvizxpi_state *state, 
                                           struct v4l2_dv_timings *dv,
                                           int stereo,
                                           int verbose)
{
    struct LUT_struct *lutp;
    struct list_head *tmp;
    struct v4l2_bt_timings *bt = &dv->bt;
    int vtot, htot, refresh;

    refresh = div_u64(bt->pixelclock * 1000, htotal(bt) * vtotal(bt)); 

    if (debug || verbose) {
        printk(KERN_INFO \
                "%s: DV timings %dx%d refresh %d htot %d vtot %d stereo %s\n",
                __func__, bt->width, bt->height, refresh, htotal(bt), 
                vtotal(bt), (stereo) ? "yes" : "no");
    }

    list_for_each(tmp, &state->lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (lutp->a_alutreg == 0 || fill_lut_hvres(lutp) < 0)
            continue;
        htot = lutp->a_orig_hres + lutp->a_hfp + lutp->a_hsync + lutp->a_hbp;
        vtot = lutp->a_orig_vres + lutp->a_vfp + lutp->a_vsync + lutp->a_vbp;
        if (bt->width == lutp->a_orig_hres &&
                bt->height == lutp->a_orig_vres &&
                in_range(refresh, (int) lutp->a_lrefresh, RFRSHMARGIN) &&
                in_range(vtotal(bt), vtot, lutp->a_vmargin) &&
                in_range(htotal(bt), htot, HMARGIN) &&
                ((lutp->a_stereo == LUT_STEREO_BOTH) || 
                 (lutp->a_stereo == LUT_STEREO_YES && stereo) ||
                 (lutp->a_stereo == LUT_STEREO_NO && !stereo))) {
            if (debug || verbose) {
                printk(KERN_INFO "%s: H %d(%d/%d/%d/%d) V %d(%d/%d/%d/%d) "
                        "refresh %d stereo %d\n", __func__, bt->width, 
                        bt->hfrontporch, bt->hsync, bt->hbackporch, 
                        htotal(bt), bt->height, bt->vfrontporch, bt->vsync, 
                        bt->vbackporch, vtotal(bt), refresh, stereo);  
                printk(KERN_INFO "%s: returing LUT ID %d\n", __func__, 
                        lutp->a_alutreg);
            }
            return lutp;
        }
        else if ((debug || verbose) && (bt->width == lutp->a_orig_hres &&
                bt->height == lutp->a_orig_vres)) {
            printk(KERN_INFO "%s: No match LUT ID %d (%dx%d) refresh %lu "
                    "htot %d vtot %d stereo %d\n", __func__, 
                    lutp->a_alutreg, lutp->a_hres, lutp->a_vres, 
                    lutp->a_lrefresh, htot, vtot, lutp->a_stereo);
        }
    }
    if (debug || verbose) { 
        printk(KERN_INFO "%s: returning NULL\n", __func__); 
    }
    return NULL;
}

/*
 * This function will return the LUT table entry with LUT ID 'lutid'
 */
struct LUT_struct *get_lut_from_lutid(struct netvizxpi_state *state, int lutid)
{
    struct LUT_struct *lutp;
    struct list_head *tmp;
    list_for_each(tmp, &state->lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (lutp->a_alutreg != lutid || fill_lut_hvres(lutp) < 0)
            continue;
        else
            return lutp;
    }
    return NULL;
}

void reset_lut_table(struct netvizxpi_state *state) 
{
    struct list_head *tmp, *tmp2;
    struct LUT_struct *lutp;

    if (state == NULL)
        return;

    mutex_lock(&state->lut_lock);

    state->current_addside_lut = NULL;

    list_for_each_safe(tmp, tmp2, &state->lut_table_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        list_del(&lutp->lut_list);
        kfree(lutp);
    }
    state->lut_size = count_lut_entries(state);

    mutex_unlock(&state->lut_lock);
}

int add_lut_entry(struct netvizxpi_state *state, struct LUT_struct *lutp)
{
    struct LUT_struct *newlutp;

    newlutp = kmalloc(sizeof(struct LUT_struct), GFP_KERNEL);
    if (lutp == NULL)
        return -ENOMEM;

    memcpy(newlutp, lutp, sizeof(struct LUT_struct));
    mutex_lock(&state->lut_lock);
    list_add_tail(&newlutp->lut_list, &state->lut_table_head);
    state->lut_size = count_lut_entries(state);
    mutex_unlock(&state->lut_lock);

    return 0; 
}

int get_lut_entry(struct netvizxpi_state *state, struct LUT_struct *lutp)
{
    struct list_head *tmp;
    struct LUT_struct *newlutp = NULL;

    if (lutp == NULL) {
        return -EINVAL;
    }

    list_for_each(tmp, &state->lut_table_head) {
        newlutp = list_entry(tmp, struct LUT_struct, lut_list);
        if (lutp->lut_list.next == NULL)
            break;
        if ((struct LUT_struct *) lutp->lut_list.next == newlutp) 
            lutp->lut_list.next = NULL;
    }
    if (newlutp != NULL)
        memcpy(lutp, newlutp, sizeof(struct LUT_struct));
    else
        return -EFAULT;

    if (tmp != state->lut_table_head.prev)
        lutp->lut_list.next = (struct list_head *) newlutp;
    else
        lutp->lut_list.next = NULL;
    return 0;
}

struct LUT_struct *get_cp_frun_lut(int lutid)
{
    int i;
    for (i = 0; i < sizeof(cp_frun_luts) /sizeof(struct LUT_struct); i++) {
        if (lutid == cp_frun_luts[i].a_alutreg)
            return &cp_frun_luts[i];
    }
    return NULL;
}
