#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __XPI__
#include <linux/videodev2.h>
#include "netvizxpi/netvizxpi.h"
#else
#include "fpga.h"
#include "lut_table.h"
#endif
#include "lut_utils.h"
#include "slice_size.h"
#include "vlog.h"
#include "commonutils.h"
#include "system_utils.h"
#include "ad9852dds.h"

/********************************************************************
 * Returns LUT list params for entry with lut_list_id 'index'. Must *
 * initialize LUT list params table before calling this routine.    * 
 ********************************************************************/
int get_lut_list_params_by_index(struct lut_list_params *params, int index) {
    struct lut_list_params *paramsp;
    int rc = 0, count = 0;

    foreach_lut_list_entry(paramsp) {
        if (count++ == index) {
            memcpy(params, paramsp, sizeof(struct lut_list_params));
            rc = 1;
            break;
        }
    }
    return rc;
}

/********************************************************************
 * Returns LUT list params for entry with LUT id 'lutid'. Must      *
 * initialize LUT list params table before calling this routine     * 
 ********************************************************************/
int get_lut_list_params_by_lutid(struct lut_list_params *params, int lutid) {
    struct lut_list_params *paramsp;
    int rc = 0;

    foreach_lut_list_entry(paramsp) {
        if (paramsp->alutreg_value == lutid) {
            memcpy(params, paramsp, sizeof(struct lut_list_params));
            rc = 1;
            break;
        }
    }
    return rc;
}

/****************************************************************
 * Routines to read and work with LUT entries from config file  *
 ****************************************************************/
static int check_lutid(struct list_head *lut_head, int alutreg)
{
    struct list_head *tmp;
    struct LUT_struct *tmplutp;

    /*****************************
     * Check if lut id is unique *
     *****************************/
    if (lut_head != NULL) {
        list_for_each(tmp, lut_head) {
            tmplutp = list_entry(tmp, struct LUT_struct, lut_list);
            if (tmplutp->a_alutreg == alutreg) 
                return 1;
        }
    }
    return 0;
}

int parse_lut(HAL *hp, struct list_head *lut_head) 
{
    struct lut_list_params *params;
    struct LUT_struct lut, *lutp;
    int count = 0;

    /* Zero out the LUT linked list */
    INIT_LIST_HEAD(lut_head);
    memset(&lut, 0, sizeof(struct LUT_struct));

    /* Retrieve lut list from config file */
    retrieveLUTListValues(hp);

    foreach_lut_list_entry(params) {
        if (check_lutid(lut_head, params->alutreg_value)) 
            continue;
        strncpy(lut.comment, params->name_value, 64);
        lut.comment[63] = '\0';
        lut.a_orig_hres = params->orig_hres_value;
        lut.a_orig_vres = params->orig_vres_value;
        lut.a_refresh = params->refresh_value;
        lut.a_pcr = params->pcr_value;
        lut.a_alutreg = params->alutreg_value;
        if (params->stereo_value == LUT_LIST_STEREO_YES)
            lut.a_stereo = LUT_STEREO_YES;
        else if (params->stereo_value == LUT_LIST_STEREO_NO)
            lut.a_stereo = LUT_STEREO_NO;
        else
            lut.a_stereo = LUT_STEREO_BOTH;
        lut.a_hfp = params->hfp_value;
        lut.a_hsync = params->hsync_value;
        lut.a_hbp = params->hbp_value;
        lut.a_vfp = params->vfp_value;
        lut.a_vsync = params->vsync_value;
        lut.a_vbp = params->vbp_value;
        if (params->cspace_value == LUT_LIST_CSPACE_YPRPB)
           lut.a_cspace = LUT_CSPACE_YPRPB;
        else
           lut.a_cspace = LUT_CSPACE_RGB;
        if (params->cpmp_value == LUT_LIST_CPMP_50)
            lut.a_cpmp = 0;
        else if (params->cpmp_value == LUT_LIST_CPMP_100)
            lut.a_cpmp = 1;
        else if (params->cpmp_value == LUT_LIST_CPMP_150)
            lut.a_cpmp = 2;
        else if (params->cpmp_value == LUT_LIST_CPMP_250)
            lut.a_cpmp = 3;
        else if (params->cpmp_value == LUT_LIST_CPMP_350)
            lut.a_cpmp = 4;
        else if (params->cpmp_value == LUT_LIST_CPMP_750)
            lut.a_cpmp = 5;
        else if (params->cpmp_value == LUT_LIST_CPMP_500)
            lut.a_cpmp = 6;
        else if (params->cpmp_value == LUT_LIST_CPMP_1500)
            lut.a_cpmp = 7;
        else
            lut.a_cpmp = LUT_CPMP_AUTO;
        lut.a_rgain = params->rgain_value;
        lut.a_roffset = params->roffset_value;
        lut.a_ggain = params->ggain_value;
        lut.a_goffset = params->goffset_value;
        lut.a_bgain = params->bgain_value;
        lut.a_boffset = params->boffset_value;
        lut.a_phase = sampling_phase_level_to_value(params->phase_value);
        lut.a_vmargin = params->vmargin_value;
        lut.a_hshift = params->hshift_value;
        lut.a_vshift = params->vshift_value;
        lut.a_noise = dc_noise_level_to_value(params->cnoise_value) << 
            LUT_DCNOISE_SHIFT;
        lut.a_noise |= ac_noise_level_to_value(params->fnoise_value) << 
                LUT_ACNOISE_SHIFT;
        lutp = malloc(sizeof(struct LUT_struct));
        if (lutp != NULL) {
            memcpy(lutp, &lut, sizeof(struct LUT_struct));
            list_add_tail(&(lutp->lut_list), lut_head);
            count++;
        }
    }
    return count;
}

void free_lut(struct list_head *lut_head) 
{
    struct list_head *tmp;
    struct LUT_struct *lutp;

    if (lut_head == NULL)
        return;

    list_for_each(tmp, lut_head) {
        lutp = list_entry(tmp, struct LUT_struct, lut_list);
        list_del(&(lutp->lut_list));
        free(lutp);
    }
}

int get_lut_entry_by_lutid(HAL *hp, struct LUT_struct *lutp, int lutid) {
    struct list_head *tmp, lut_list_head;
    struct LUT_struct *tmplutp;
    int rc = 0, hardware_type = hal_get_hardware_type(hp);

    if (parse_lut(hp, &lut_list_head) > 0) {
        list_for_each(tmp, &lut_list_head) {
            tmplutp = list_entry(tmp, struct LUT_struct, lut_list);
            if (tmplutp->a_alutreg == lutid) {
                memcpy(lutp, tmplutp, sizeof(struct LUT_struct));
                rc = 1;
                break;
            }
        }
        free_lut(&lut_list_head);
    }
    if (rc)
        build_lut_entry(lutp, hardware_type);
    return rc;
}

int compare_luts(struct LUT_struct *lut1p, struct LUT_struct *lut2p) {
    if ((lut1p->a_stereo == lut2p->a_stereo) &&
        (lut1p->a_orig_hres == lut2p->a_orig_hres) &&
        (lut1p->a_orig_vres == lut2p->a_orig_vres) &&
        (lut1p->a_hfp == lut2p->a_hfp) &&
        (lut1p->a_hbp == lut2p->a_hbp) &&
        (lut1p->a_hsync == lut2p->a_hsync) &&
        (lut1p->a_vfp == lut2p->a_vfp) &&
        (lut1p->a_vbp == lut2p->a_vbp) &&
        (lut1p->a_vsync == lut2p->a_vsync) &&
        (lut1p->a_refresh >= lut2p->a_refresh - 0.20) &&
        (lut1p->a_refresh <= lut2p->a_refresh + 0.20) &&
        (lut1p->d_dcreg == lut2p->d_dcreg)) {
        return 0;
    }
    else { 
        return -1;
    }
}

int compare_luts_without_polarity(struct LUT_struct *lut1p, 
                                  struct LUT_struct *lut2p) {

    if ((lut1p->a_stereo == lut2p->a_stereo) &&
        (lut1p->a_orig_hres == lut2p->a_orig_hres) &&
        (lut1p->a_orig_vres == lut2p->a_orig_vres) &&
        (lut1p->a_hfp == lut2p->a_hfp) &&
        (lut1p->a_hbp == lut2p->a_hbp) &&
        (lut1p->a_hsync == lut2p->a_hsync) &&
        (lut1p->a_vfp == lut2p->a_vfp) &&
        (lut1p->a_vbp == lut2p->a_vbp) &&
        (lut1p->a_vsync == lut2p->a_vsync) &&
        (lut1p->a_refresh >= lut2p->a_refresh - 0.20) &&
        (lut1p->a_refresh <= lut2p->a_refresh + 0.20)) {
        return 0;
    }
    else {
        return -1;
    }
}

int get_matching_lut_entry(HAL *hp, struct LUT_struct *lutp) {
    struct list_head *tmp, lut_list_head;
    struct LUT_struct *tmplutp;
    int rc = 0;

    if (parse_lut(hp, &lut_list_head) > 0) {
        list_for_each(tmp, &lut_list_head) {
            tmplutp = list_entry(tmp, struct LUT_struct, lut_list);
            if (compare_luts_without_polarity(tmplutp, lutp) == 0) {
                rc = tmplutp->a_alutreg; 
                break;
            }
        }
        free_lut(&lut_list_head);
    }
    return rc;
}

/*******************************************************************
 * Returns 1 if found entry that matches hsyncc etc else returns 0.*
 * The matching LUT is put in the struct pointed to by 'lutp'.     * 
 *******************************************************************/
int find_matching_lut_entry(HAL *hp, 
                            unsigned short hsyncc, 
                            unsigned short amvreg,
                            unsigned short amhreg, 
                            unsigned short amtreg, 
                            int duallink,
                            struct LUT_struct *lutp)
{
    struct list_head *tmp, lut_list_head;
    struct LUT_struct *tmplutp;
    int rc = 0, gotstereo, hardware_type, clk;
    
    VLOG_DEBUG_CAT(VLOG_HAL,
        "%s: hsyncc %d, amvreg %d, amhreg %d, amtreg %d, duallink %d\n", 
    	  __func__, hsyncc, amvreg, amhreg, amtreg, duallink);

    if ((amtreg & AMTREG_HVSYNC) != AMTREG_HVSYNC)
        return 0;

    hardware_type = hal_get_hardware_type(hp);
    if (hardware_type == XPI_DEVICE)
        clk = XPI_CLOCK_FREQ;
    else if (hardware_type == V2DI50_DEVICE)
        clk = V2_CLOCK_FREQ;
    else
        clk = V4_CLOCK_FREQ;

    gotstereo = amtreg & AMTREG_STEREO;

    if (parse_lut(hp, &lut_list_head) > 0) {
        list_for_each(tmp, &lut_list_head) {
            int oldhsynccdiff, oldamvdiff, oldamhdiff;
            int hsynccdiff, amvdiff, amhdiff;
            unsigned short tmphsyncc, tmpvreg, tmphreg; 

            oldhsynccdiff = oldamvdiff = oldamhdiff = 0;
            tmplutp = list_entry(tmp, struct LUT_struct, lut_list);

            /*****************************************************
             * Eliminate entries not matching stereo or duallink * 
             *****************************************************/
            if ((tmplutp->a_stereo == LUT_STEREO_YES && !gotstereo) || 
                    (tmplutp->a_stereo == LUT_STEREO_NO && gotstereo)) {
                VLOG_DEBUG_CAT(VLOG_HAL,
	        	"LUT %s: stereo mismatch (%d/%d)\n",
	 		tmplutp->comment, gotstereo, tmplutp->a_stereo);
                continue; 
            }
            if ((duallink == DUALLINK_NO && (tmplutp->a_pcr > 165.0)) ||
            	    (duallink == DUALLINK_YES && (tmplutp->a_pcr < 165.0))) {
                VLOG_DEBUG_CAT(VLOG_HAL,
			"LUT %s: duallink mismatch (%d/%f)\n",
			tmplutp->comment, duallink, tmplutp->a_pcr);
                continue;
            }

            tmphreg = clk / (tmplutp->a_refresh * (tmplutp->a_orig_vres + 
		tmplutp->a_vfp + tmplutp->a_vsync + tmplutp->a_vbp));
            tmpvreg = (clk / tmplutp->a_refresh) / (double) 256;
            tmphsyncc = (tmplutp->a_orig_vres + tmplutp->a_vfp + 
		tmplutp->a_vsync + tmplutp->a_vbp);

            /**************************************************
             * Eliminate entries with AMVREG not within range * 
             **************************************************/
            if (tmpvreg - 0x100 > amvreg || tmpvreg + 0x100 < amvreg) {
                VLOG_DEBUG_CAT(VLOG_HAL,
			"LUT %s: AMVREG out of range (%d/%d)\n",
			tmplutp->comment, amvreg, tmpvreg);
               continue;
            }
            /**************************************************
             * Eliminate entries with AMLREG not within range * 
             **************************************************/
            if (tmphsyncc - tmplutp->a_vmargin > hsyncc ||
                    tmphsyncc + tmplutp->a_vmargin < hsyncc) {
                VLOG_DEBUG_CAT(VLOG_HAL,
			"LUT %s: HSYNCC out of range (%d/%d)\n",
			tmplutp->comment, hsyncc, tmphsyncc);
                continue;
            }
            /**************************************************
             * Eliminate entries with AMHREG not within range * 
             **************************************************/
            if (tmphreg - 0x10 > amhreg || tmphreg + 0x10 < amhreg) {
                VLOG_DEBUG_CAT(VLOG_HAL,
			"LUT %s: AMHREG out of range (%d/%d)\n",
			tmplutp->comment, amhreg, tmphreg);
                continue;
            }
            /***************************************************
             * Compare and select the entry with closest match * 
             ***************************************************/
            if (rc == 0) {
                rc = 1;
                memcpy(lutp, tmplutp, sizeof(struct LUT_struct));
            }
            else {
                hsynccdiff = (tmphsyncc > hsyncc) ? 
                    (tmphsyncc - hsyncc) : (hsyncc - tmphsyncc);
                amvdiff = (tmpvreg > amvreg) ? 
                    (tmpvreg - amvreg) : (amvreg - tmpvreg);
                amhdiff = (tmphreg > amhreg) ? 
                    (tmphreg - amhreg) : (amhreg - tmphreg);
                if (amvdiff < oldamvdiff) {
                    memcpy(lutp, tmplutp, sizeof(struct LUT_struct));
                    oldamvdiff = amvdiff;
                    oldhsynccdiff = hsynccdiff;
                    oldamhdiff = amhdiff;
                }
                else if (hsynccdiff < oldhsynccdiff) {
                    memcpy(lutp, tmplutp, sizeof(struct LUT_struct));
                    oldamvdiff = amvdiff;
                    oldhsynccdiff = hsynccdiff;
                    oldamhdiff = amhdiff;
                }
                else if (amhdiff < oldamhdiff) {
                    memcpy(lutp, tmplutp, sizeof(struct LUT_struct));
                    oldamvdiff = amvdiff;
                    oldhsynccdiff = hsynccdiff;
                    oldamhdiff = amhdiff;
                }
            } 
        }
        free_lut(&lut_list_head);
    }
    if (rc > 0)
        build_lut_entry(lutp, hardware_type);
    return rc;
}

int get_unique_lutid(HAL *hp) {
    struct list_head *tmp, lut_list_head;
    struct LUT_struct *tmplutp;
    int unique_lutid = 1;

    if (parse_lut(hp, &lut_list_head) > 0) {
again:
        list_for_each(tmp, &lut_list_head) {
            tmplutp = list_entry(tmp, struct LUT_struct, lut_list);
            if (tmplutp->a_alutreg == unique_lutid) {
                unique_lutid++;
                goto again;
            }
        }
        free_lut(&lut_list_head);
    }

    if (unique_lutid >= RESERVED_TX_LUTID)
        return -1;

    return unique_lutid;
}

int add_lut_entry(HAL *hp, struct LUT_struct *lutp) {
    char buf[256];
    int alutreg = get_unique_lutid(hp);
 
    if (alutreg <= 0)
        return -1;
    lutp->a_alutreg = alutreg;
    sprintf(buf, "add_lut_list -name \"%s\" -alutreg %d -stereo %s " 
            "-orig_hres %d -orig_vres %d -refresh %6.2f -pcr %6.2f "
            "-hfp %d -hsync %d -hbp %d -vfp %d -vsync %d -vbp %d "
            "-channelNumber %d", 
            lutp->comment, alutreg, 
            (lutp->a_stereo == LUT_STEREO_YES) ? "Yes" : 
            ((lutp->a_stereo == LUT_STEREO_NO) ? "No" : "Both"),
            lutp->a_orig_hres, lutp->a_orig_vres, lutp->a_refresh, lutp->a_pcr,
            lutp->a_hfp, lutp->a_hsync, lutp->a_hbp, 
            lutp->a_vfp, lutp->a_vsync, lutp->a_vbp, 
            hal_get_board_number(hp)+1); 

    return system_command(buf);
}

/**************************************************************** 
 * Set the hres and vres values in the LUT to use what we've    *
 * computed for the FPGA to use for encoding.                   *
 ****************************************************************/
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
        return 0;
    }
    return -1;
}

/*  The following are the fields that the V2D menu will acquire from the user

Col  Description                             Table member
---  -------------------------------------   ---------------------------
A   Horizontal Resolution (i.e. 1024)       unsigned short a_orig_hres
B   Vertical Resolution (i.e. 768)          unsigned short a_orig_vres
C   Refresh Rate in Hz (i.e. 75)            double         a_refresh
J   Pixel Rate in MHz (i.e. 65.2)           double         a_pcr
G   Lines of Front Porch Blanking           unsigned short a_vfp
I   Lines of Back Porch Blanking            unsigned short a_vbp
H   Lines of Synchronization Blanking       unsigned short a_vsync
D   Columns of Front Porch Blanking         unsigned short a_hfp
F   Columns of Back Porch Blanking          unsigned short a_hbp
E   Columns of Synchronization Blanking     unsigned short a_hsync

optionally:
K   ADC9888/9887A charge pump current       unsigned short a_cpmp      

All other entries in the LUT must be calculated

=(1048576*2)+1)-((a_vres*a_refresh)+a_fflwm)

a_hres        Round up a_orig_hres to comply with FPGA rules  
a_vres        Round up a_orig_vres to comply with FPGA rules
a_amhreg      (2*CLOCK_FREQ/(a_refresh*(a_vres+a_vfp+a_vsync+a_vbp)))
a_amhreg      (2*CLOCK_FREQ/(a_refresh*(a_vres+a_vfp+a_vsync+a_vbp)))
a_amvreg      ((2*CLOCK_FREQ/a_refresh)/256)
a_hsyncc      (a_vres+a_vfp+a_vsync+a_vbp)
a_fflwm       5000 
a_ffhwm       ((1048576*2)+1)-((a_vres*a_refresh+256)+a_fflwm)
a_acreg       (a_pcr<47.5,"x1xxxxxx","x0xxxxxx")
a_aafdreg     (FLOOR(37000/(4*a_refresh),1))
a_avhareg     a_hres
a_avvareg     a_vres
a_avhdreg     a_hbp+a_hsync
a_vvdreg      a_vbp+a_vsync
a_adcpll01    FLOOR(((a_orig_hres+a_hfp+a_hsync+a_hbp)-1)/16,1)
a_adcpll02    FLOOR(MOD(((a_orig_hres+a_hfp+a_hsync+a_hbp)-1),16),1)*2^4
a_adcvco03    BIN2HEX(IF(a_pcr<45,"00",IF(a_pcr<90,"01",IF(a_pcr<150,"10","11")))&IF(a_refresh<=75,"100",IF(a_refresh<85,"101","011"))&"000")
a_adcmc1      0x46
a_adcvco16    0xfe
d_dafdreg     FLOOR((37000/a_refresh)*2.25,1)
d_dvhareg     a_avhareg
d_dvvareg     a_avvareg
d_dvhfpreg    a_hfp
d_dvhsreg     a_hsync
d_dvhbpreg    a_hbp
d_dvvfpreg    a_vfp
d_dvvsreg     a_vsync
d_dvvbpreg    a_vbp
d_dvbwreg     ((a_hres*a_vres)*2.25*0.75)/256
d_dds5        FLOOR(((((a_pcr/2) * 2^48) / 300)/2^40),1)
d_dds4        FLOOR((MOD((((a_pcr/2) * 2^48) / 300)/2^32,2^8)),1)
d_dds3        FLOOR((MOD((((a_pcr/2) * 2^48) / 300)/2^24,2^8)),1)
d_dds2        FLOOR((MOD((((a_pcr/2) * 2^48) / 300)/2^16,2^8)),1)
d_dds1        FLOOR((MOD(MOD(((a_pcr/2) * 2^48) / 300,2^24),2^16)/2^8),1)
d_dds0        FLOOR((MOD(MOD(((a_pcr/2) * 2^48) / 300,2^24),2^8)),1)
d_ddsclk      ((((((((((HEX2DEC(d_dds5)*2^8) + HEX2DEC(d_dds4))*2^8)+HEX2DEC(d_dds3))*2^8)+HEX2DEC(d_dds2))*2^8)+HEX2DEC(d_dds1))*2^8+HEX2DEC(d_dds0))*300)/2^48
d_pixclck     2*d_ddsclk
*/

/*********************************************************************
 * Compute charge pump current based on the Analog Devices AD9888 or * 
 * AD9887A PLL setting calculator spreadsheet.                       *
 *********************************************************************/
static int charge_pump_current(struct LUT_struct *lutp, int ad9887_flag)
{

    double pcr, hfq, sratio, kvco, lp, tmp, ct, n, p;
    int current;

    pcr = lutp->a_pcr;
    hfq = pcr * 1000 / (double) (lutp->a_orig_hres + lutp->a_hfp +
                                 lutp->a_hbp + lutp->a_hsync);
    ct = 0.039 * 0.000001;
    if (ad9887_flag) {
        sratio = (pcr < 37) ? (double) 11 : (double) 14;
        kvco = (pcr < 140) ? 150 : 180;
        p = (pcr < 37) ? 4 : ((pcr < 74) ? 2 : ((pcr < 140) ? 1 : 1));
    }
    else {
        sratio = 14.3;
        kvco = (pcr < 150) ? 150 : 180;
        p = (pcr < 41) ? 4 : ((pcr < 82) ? 2 : ((pcr < 150) ? 1 : 1));
    }
    n = (int) (pcr / hfq * (double) 1000);
    tmp = hfq * 6280 / sratio;
    lp = tmp * tmp * ((ct * n * p) / (kvco * 1000000)) * 1000000;
    if (lp < 75)
        current = 0;
    else if (lp < 125)
        current = 1;
    else if (lp < 200)
        current = 2;
    else if (lp < 300)
        current = 3;
    else if (lp < 425)
        current = 4;
    else if (lp < 625)
        current = 5;
    else if (lp < 1125)
        current = 6;
    else
        current = 7;

#ifdef VCOCPMP_FORMULA_INFO
    if (ad9887_flag) {
        printf("Pxclk = %f, Hfreq = %f, PLL stability ratio = %d\n",
               pcr, hfq, (int) sratio);
        printf("N = %d, Kvco = %d, P = %d, lp = %f, current = %x\n",
               (int) n, (int) kvco, (int) p, lp, current);
    }
#endif

    return current;
}

/*****************************************************
 * Populate all the computed parameter values in LUT *
 *****************************************************/
int build_lut_entry(struct LUT_struct *lp, int device_type)
{
    double ddsfreq, ffactor = -1, clk;
    int vf_hi, vf_lo;

    /* set clock frequency and DDS reference clock */
    if (device_type == V2DI50_DEVICE) {
        clk = V2_CLOCK_FREQ;
        ffactor = V2_DDSFFACTOR;
    }
    else if (device_type == PLX_DEVICE) {
        clk = V4_CLOCK_FREQ;
        ffactor = V4_DDSFFACTOR;
    }
    else if (device_type == XPI_DEVICE) {
        clk = XPI_CLOCK_FREQ;
    }
    else {
        return -1;
    }

    /* compute hres, vres and check total number of blocks */
    if (fill_lut_hvres(lp) < 0)
        return -1;

    /* compute what the measured horizontal frequency etc should be */
    lp->a_amhreg = clk / (lp->a_refresh * 
            (lp->a_orig_vres + lp->a_vfp + lp->a_vsync + lp->a_vbp));
    lp->a_amvreg = (clk / lp->a_refresh) / (double) 256;
    lp->a_hsyncc = (lp->a_orig_vres + lp->a_vfp+lp->a_vsync + lp->a_vbp);

    /* compute high & low watermarks */
    lp->a_fflwm = 15;
    if ((lp->a_hres * lp->a_vres) < (1024 * 1024))
        lp->a_ffhwm = lp->a_fflwm + 1;
    else 
        lp->a_ffhwm = (lp->a_hres*lp->a_vres) - (1024 * 1024);
    if (lp->a_ffhwm <= lp->a_fflwm)
        lp->a_ffhwm = lp->a_fflwm + 1;
    
    if (device_type == V2DI50_DEVICE && lp->a_pcr < (double) 47.5)
        lp->a_acreg = ACREG_FIFO_XCLK;
    else
        lp->a_acreg = 0;

    lp->a_aafdreg = 128;
    lp->a_avhareg = lp->a_hres;
    lp->a_avvareg = lp->a_vres;

    /* horizontal delays on add side */
    /* actually should be sync + back porch minus 2 (delay through AD9888) */
    lp->a_avhdreg = lp->a_hbp + lp->a_hsync;

    /* vertical delays on add side */
    /* actually should be back porch - vsync - front porch */
    lp->a_avvdreg = lp->a_vbp + lp->a_vsync;

    lp->a_adcpll01 = 
        (lp->a_orig_hres + lp->a_hfp + lp->a_hsync + lp->a_hbp - 1) / 16;
    lp->a_adcpll02 =  
        ((lp->a_orig_hres + lp->a_hfp + lp->a_hsync + lp->a_hbp - 1) % 16) << 4;

#ifdef PRE_VCOCPMP_FORMULA
    if (lp->a_hres == 1024 && lp->a_vres == 768) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 75 ? 0x4 :
                (lp->a_refresh < 85 ? 0x5 : 0x3));
        lp->a_adcvco03 <<= 3;
    }
    else if (lp->a_hres == 1280 && lp->a_vres == 1024) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 75 ? 0x3 : 0x4);
        lp->a_adcvco03 <<= 3;
    }
    else if (lp->a_hres == 1600 && lp->a_vres == 1200) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 65 ? 0x4 :
                ((lp->a_refresh <= 75) ? 0x5 : 0x6));
        lp->a_adcvco03 <<= 3;
    }
    else {
        /* Update this when we have another resolution */
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh<=75?0x3:0x4);
        lp->a_adcvco03 <<= 3;
    }
#else
    lp->a_adcvco03 = (lp->a_pcr < 41 ? 0x0 : 
            (lp->a_pcr < 82 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
    lp->a_adcvco03 <<= 6;
    lp->a_adcvco03 |= (charge_pump_current(lp, 0) << 3);
#endif

    lp->a_adcmc1 = (lp->a_pcr <= 210) ? 0x46 : 0xc6;    

    lp->a_adcvco16 = 0xfe;

    lp->a_adcvco73 = (lp->a_pcr < 37 ? 0x0 : 
            (lp->a_pcr < 74 ? 0x1 : (lp->a_pcr < 140 ? 0x2 : 0x3)));
    lp->a_adcvco73 <<= 5;
    if (lp->a_cpmp <= 7)
        lp->a_adcvco73 |= (lp->a_cpmp << 2);
    else
        lp->a_adcvco73 |= (charge_pump_current(lp, 1) << 2);
    lp->a_adcvco73 |= 0x80;

#ifdef VCOCPMP_FORMULA_DEBUG
    printf("VCOCPMP = %x\n", lp->a_adcvco73); 
#endif

    if (lp->a_pcr < (double) 88) {
        lp->d_dcreg |= DCREG_VID_BYPASS;
    }
    lp->d_dafdreg = 1024;
    lp->d_dvhareg = lp->a_avhareg;
    lp->d_dvvareg = lp->a_avvareg;
    lp->d_dvhfpreg = lp->a_hfp;
    lp->d_dvhsreg = lp->a_hsync;
    if (lp->a_hres - lp->a_orig_hres >= lp->a_hbp)
        return -1;
    else if (lp->a_hres - lp->a_orig_hres > 0)
        lp->d_dvhbpreg= lp->a_hbp - (lp->a_hres - lp->a_orig_hres);
    else
        lp->d_dvhbpreg= lp->a_hbp;
    lp->d_dvvfpreg = lp->a_vfp;
    lp->d_dvvsreg = lp->a_vsync;
    if (lp->a_vres - lp->a_orig_vres >= lp->a_vbp)
        return -1;
    else if (lp->a_vres - lp->a_orig_vres > 0)
        lp->d_dvvbpreg = lp->a_vbp - (lp->a_vres - lp->a_orig_vres);
    else
        lp->d_dvvbpreg = lp->a_vbp;

    if (device_type == XPI_DEVICE) {
        lp->d_dds0 = lp->d_dds1 = lp->d_dds2 = 
        	lp->d_dds3 = lp->d_dds4 = lp->d_dds5 =  0;
        lp->d_ddsclk = 0;
        lp->d_chgperbit = 0;
    }
    else {
        ddsfreq = lp->a_pcr / (double) 2 / ffactor * TWORAISED48;
        vf_hi = ddsfreq / TWORAISED24;
        vf_lo = ddsfreq - ((double) vf_hi * TWORAISED24);
        lp->d_dds5 = (vf_hi >> 16) & 0xff;
        lp->d_dds4 = (vf_hi >> 8) & 0xff;
        lp->d_dds3 = vf_hi & 0xff;
        lp->d_dds2 = (vf_lo >> 16) & 0xff;
        lp->d_dds1 = (vf_lo >> 8) & 0xff;
        lp->d_dds0 = vf_lo & 0xff;
    
        vf_hi = (unsigned int) (lp->d_dds5 << 16);
        vf_hi += (unsigned int) (lp->d_dds4 << 8);
        vf_hi += (unsigned int) lp->d_dds3;
        vf_lo = (unsigned int) (lp->d_dds2 << 16);
        vf_lo += (unsigned int) (lp->d_dds1 << 8);
        vf_lo += (unsigned int) lp->d_dds0;
        ddsfreq = ((double) vf_hi * TWORAISED24) + vf_lo;
        lp->d_ddsclk = ddsfreq * ffactor / TWORAISED48;
        lp->d_chgperbit = (((100./2) * TWORAISED48) / ffactor / 1000000.);
    }
    lp->d_pixclck = lp->a_pcr;

    /*******************************************************
     * Kernel does not handler double arithmetic           *
     * Convert doubles to shadow variables here            *
     *******************************************************/
    if (lp->a_lpcr != 0)
        lp->a_pcr = lp->a_lpcr / 1000;
    else
        lp->a_lpcr = lp->a_pcr * 1000;
    lp->a_lrefresh = lp->a_refresh * 1000;
    lp->d_ltx_refresh = lp->d_tx_refresh*1000;
    lp->d_ltx_pcr = lp->d_ltx_pcr*1000;

    return 0;
}

