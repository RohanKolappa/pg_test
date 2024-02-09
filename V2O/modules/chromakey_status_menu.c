#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "hal.h"
#include "vlib.h"
#include "chromakey_control.h"
#include "chromakey_status_menu_gen.h"

int getChromakeyStatus(struct chromakey_status_params *params) 
{

    struct chromakey_config chromakey_cfg;
    unsigned char r, rt, g, gt, b, bt;
    unsigned char p1r, p1b, p1g, p2r, p2g, p2b, p3r, p3g, p3b;

    HAL *hp = params->userptr;
    if (hal_get_chromakey_enable_k(hp, MATTE_LOCAL))
        strcpy(params->enabled_value, "Yes");
    else
        strcpy(params->enabled_value, "No");

    get_chromakey_config(hp, &chromakey_cfg); 
    if (chromakey_cfg.colorref == COLORREF_VALUE) 
        strcpy(params->colorref_value, "Value");
    else
        strcpy(params->colorref_value, "Location");
    params->transparency_value = hal_get_chromakey_alpha_k(hp, MATTE_LOCAL);
    
    hal_get_chromakey_color_value_k(hp, MATTE_LOCAL, &r, &rt, &g, &gt, &b, &bt);
    params->redvalue_value = r;
    params->redtolerance_value = rt;
    params->greenvalue_value = g;
    params->greentolerance_value = gt;
    params->bluevalue_value = b;
    params->bluetolerance_value = bt;

    hal_get_chromakey_pixel_color_k(hp, 
                                    MATTE_LOCAL,
                                    chromakey_cfg.pix1x,
                                    chromakey_cfg.pix1y,
                                    &p1r, 
                                    &p1g, 
                                    &p1b); 
    params->pix1x_value = chromakey_cfg.pix1x;
    params->pix1y_value = chromakey_cfg.pix1y;
    params->pix1redvalue_value = p1r;
    params->pix1greenvalue_value = p1g;
    params->pix1bluevalue_value = p1b;
    hal_get_chromakey_pixel_color_k(hp, 
                                    MATTE_LOCAL,
                                    chromakey_cfg.pix2x, 
                                    chromakey_cfg.pix2y, 
                                    &p2r, 
                                    &p2g, 
                                    &p2b); 
    params->pix2x_value = chromakey_cfg.pix2x;
    params->pix2y_value = chromakey_cfg.pix2y;
    params->pix2redvalue_value = p2r;
    params->pix2greenvalue_value = p2g;
    params->pix2bluevalue_value = p2b;
    hal_get_chromakey_pixel_color_k(hp, 
                                    MATTE_LOCAL,
                                    chromakey_cfg.pix3x, 
                                    chromakey_cfg.pix3y,
                                    &p3r,
                                    &p3g,
                                    &p3b);
    params->pix3x_value = chromakey_cfg.pix3x;
    params->pix3y_value = chromakey_cfg.pix3y;
    params->pix3redvalue_value = p3r;
    params->pix3greenvalue_value = p3g;
    params->pix3bluevalue_value = p3b;

    return RC_NORMAL;

} 
