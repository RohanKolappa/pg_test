#include <stdlib.h>
#include <string.h>
#include <syslog.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#define NO_LUT_DETECTED   "No LUT Detected"
#include "custmenu.h"

#include "hal.h"
#include "lut_utils.h"
#include "display_params_menu_gen.h"
#include "display_status_menu_gen.h"

typedef struct display_regs_s {
    int maxhres;
    int maxvres;
    int enchres;
    int encvres;
    int hres;
    int vres;
    int hscale;
    int vscale;
    int hshift;
    int vshift;
    int oalpha;
} DREGS;

static void get_display_regs(HAL *hp, DREGS *regs)  
{
    int hardware_type = hal_get_hardware_type(hp);
    struct ov_param_io ov;

    if (hardware_type == PLX_DEVICE) {
        /* maximum resolution */
        regs->maxhres = hal_get_fpga_val_k(hp, FPGA_DVHAREG);
        regs->maxvres = hal_get_fpga_val_k(hp, FPGA_DVVAREG);
        /* encoded (source) resolution */
        regs->enchres = hal_get_fpga_val_k(hp, FPGA_DSRCHRES);
        regs->encvres = hal_get_fpga_val_k(hp, FPGA_DSRCVRES);
        /* actual (scaled) resolution */
        regs->hres = hal_get_fpga_val_k(hp, FPGA_DNHRES);
        if (hal_get_fpga_val32_k(hp, FPGA_ATESTREG) & VCROP_ENABLE)
            regs->vres = hal_get_fpga_val_k(hp, FPGA_DCROPVRES);
        else
            regs->vres = hal_get_fpga_val_k(hp, FPGA_DVHAREG);
        /* scale */
        regs->hscale = hal_get_fpga_val_k(hp, FPGA_DHSCALE);
        regs->vscale = hal_get_fpga_val_k(hp, FPGA_DVSCALE);
        /* shift */
        regs->hshift = hal_get_fpga_val_k(hp, FPGA_DHSHIFT);
        regs->vshift = hal_get_fpga_val_k(hp, FPGA_DVSHIFT);
        /* overlap area 1 alpha */
        regs->oalpha = hal_get_fpga_val_k(hp, FPGA_DOVLPALPHA1);
    }
    else if (hardware_type == XPI_DEVICE) {
#ifdef __XPI__
        struct ov_param_io ov;
        /* maximum resolution */
        regs->maxhres = hal_get_s6fpga_val32_k(hp, S6FPGA_DVHAREG);
        regs->maxvres = hal_get_s6fpga_val32_k(hp, S6FPGA_DVVAREG);
        /* encoded (source) resolution */
        regs->enchres = hal_get_s6fpga_val32_k(hp, S6FPGA_DSRCHRES);
        regs->encvres = hal_get_s6fpga_val32_k(hp, S6FPGA_DSRCVRES);
        /* actual (scaled) resolution */
        if (hal_get_s6fpga_val32_k(hp, S6FPGA_VIDCFGREG) & HCROP_ENABLE)
            regs->hres = hal_get_s6fpga_val32_k(hp, S6FPGA_DCROPHRES);
        else
            regs->hres = hal_get_s6fpga_val32_k(hp, S6FPGA_DNHRES);
        if (hal_get_s6fpga_val32_k(hp, S6FPGA_VIDCFGREG) & VCROP_ENABLE)
            regs->vres = hal_get_s6fpga_val32_k(hp, S6FPGA_DCROPVRES);
        else
            regs->vres = hal_get_s6fpga_val32_k(hp, S6FPGA_DNVRES);
        /* scale */
        hal_get_overlay_info(hp, &ov);
        regs->hscale = ov.hscale;
        regs->vscale = ov.vscale;
        /* shift */
        regs->hshift = hal_get_s6fpga_val32_k(hp, S6FPGA_DHSHIFT);
        regs->vshift = hal_get_s6fpga_val32_k(hp, S6FPGA_DVSHIFT);
        /* overlap area 1 alpha */
        regs->oalpha = hal_get_s6fpga_val32_k(hp, S6FPGA_DOVLPALPHA1);
#endif
    }
}

int getDisplayStatus(struct display_status_params *params) 
{
    HAL *hp = params->userptr;
    struct display_parm_params displaycfg;
    DREGS dregs;
    
    if (hp == NULL) {
        return RC_ERROR;
    }
    if (hal_get_overlay_mode(hp) & OV_FLAG_SRC_LOCAL)
        strcpy (params->video_background_value , "Local");
    else 
        strcpy (params->video_background_value , "None");
    displaycfg.userptr = hp;
    retrieveDisplayParametersValues(&displaycfg);
    strcpy (params->video_source_value , displaycfg.video_source);

    memset(&dregs, 0, sizeof(struct display_regs_s));
    get_display_regs(hp, &dregs);
    params->max_horiz_resolution_value = dregs.maxhres;
    params->max_vert_resolution_value = dregs.maxvres;
    params->source_horiz_resolution_value = dregs.enchres;
    params->source_vert_resolution_value = dregs.encvres;
    params->horiz_resolution_value = dregs.hres;
    params->vert_resolution_value = dregs.vres;
    params->horiz_scale_value = (100 * dregs.hscale) / 8;
    params->vert_scale_value = (100 * dregs.vscale) / 8;
    params->horiz_offset_value = dregs.hshift;
    params->vert_offset_value = dregs.vshift;
    params->transparency_value = 100 - (100 * dregs.oalpha) / 1024;

    // Remote LUT Description
    sprintf(params->remote_lut_value, "????");
    struct LUT_struct current_alut;
    memset(&current_alut, 0x00, sizeof(current_alut));
    if (hal_get_dropside_lut(hp, &current_alut) != -1) {
        if (current_alut.a_alutreg != 0 &&
                current_alut.a_alutreg <= RESERVED_RX_LUTID)
            snprintf(params->remote_lut_value,
                    FIELD_DISPLAY_STATUS_REMOTE_LUT_LENGTH,
                    "%d [%s]", current_alut.a_alutreg, current_alut.comment);
        else
            snprintf(params->remote_lut_value,
                    FIELD_DISPLAY_STATUS_REMOTE_LUT_LENGTH,
                    "%s", current_alut.comment);
    }

    // Current Display (i.e. override or overlay) LUT Description
    sprintf(params->display_lut_value, "????");
    memset(&current_alut, 0x00, sizeof(current_alut));

    if (hal_get_display_lut(hp, &current_alut) != -1) {
        if (current_alut.a_alutreg != 0 && 
                current_alut.a_alutreg <= RESERVED_RX_LUTID) 
            snprintf(params->display_lut_value,
                     FIELD_DISPLAY_STATUS_DISPLAY_LUT_LENGTH,
                     "%d [%s]", current_alut.a_alutreg, current_alut.comment);
        else
            snprintf(params->display_lut_value,
                     FIELD_DISPLAY_STATUS_DISPLAY_LUT_LENGTH,
                     "%s", current_alut.comment);
    }

    // Local LUT Description
    sprintf(params->local_lut_value, NO_LUT_DETECTED);

    memset(&current_alut, 0x00, sizeof(current_alut));

    if (hal_get_addside_lut(hp, &current_alut) == 0) { 
        if (strlen(current_alut.comment) > 0)
            snprintf(params->local_lut_value,
                FIELD_DISPLAY_STATUS_LOCAL_LUT_LENGTH,
                "%d [%s]", current_alut.a_alutreg, current_alut.comment);
    }

    // Interface info
    int active = hal_getActiveIntf(hp);
    if (active & INTERFACE_DIGITAL) {
        if (current_alut.a_pcr >= PCR_DUALLINK_DVI) {
            strcpy(params->local_interface_value, "DVI-D");
        }
        else {
            strcpy(params->local_interface_value, "DVI-S");
        }
    }
    else if (active & INTERFACE_HV) 
        strcpy(params->local_interface_value, "Analog HV");
    else if (active & INTERFACE_SOG) 
        strcpy(params->local_interface_value, "Analog SOG");
    else if (active & INTERFACE_COMPOSITE) 
        strcpy(params->local_interface_value, "Composite");
    else
        strcpy(params->local_interface_value, "???");

    // Frequency info
    double freq;
    freq = hal_getHorizontalFrequency(hp);
    if (freq > 0.01 && freq < 1000.0)
        params->horizontal_frequency_value = freq;
        
    freq = hal_getVerticalFrequency(hp);
    if (freq > 0.01 && freq < 1000.0)
        params->vertical_frequency_value = freq;
        
    // Horizontal lines
    int lines = hal_getHorizontalLines(hp);
    if (lines > 0 && lines < 10000)
        params->horizontal_lines_value = lines;

    // Stereo?
    params->stereo_value = hal_getStereo(hp);

    return RC_NORMAL;
}
