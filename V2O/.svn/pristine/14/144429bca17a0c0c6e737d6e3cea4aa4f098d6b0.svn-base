#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "xpi_color_params_menu_gen.h"
#include "vlog.h"
#include "hal_v4l2.h"

#ifdef __XPI__
#define rgb_range_value_from_code(x) \
    ((x == XPI_COLOR_PARM_RGB_RANGE_AUTO) ? NETVIZXPI_RGB_RANGE_AUTO : \
    ((x == XPI_COLOR_PARM_RGB_RANGE_LIMITED) ? NETVIZXPI_RGB_RANGE_LIMITED : \
    NETVIZXPI_RGB_RANGE_FULL))

#define rgb_range_code_from_value(x) \
    ((x == NETVIZXPI_RGB_RANGE_AUTO) ? XPI_COLOR_PARM_RGB_RANGE_AUTO : \
    ((x == NETVIZXPI_RGB_RANGE_LIMITED) ? XPI_COLOR_PARM_RGB_RANGE_LIMITED : \
    XPI_COLOR_PARM_RGB_RANGE_FULL))
#else
#define rgb_range_value_from_code(x)    (0)
#define rgb_range_code_from_value(x)    (0)
#endif

static int get_brightness(HAL *hp) 
{
    int ival = 0;
#ifdef __XPI__
    ival = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_BRIGHTNESS);
    if (ival > 128) 
        ival = 128 - ival;
#endif
    return ival;
}

static float get_contrast(HAL *hp) 
{
    int ival;
    float fval = 0.0;
#ifdef __XPI__
    ival = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_CONTRAST);
    fval = (ival & 0x80) ? (float) 1.0 : (float) 0.0;
    ival &= 0x7f;
    fval += (float) ival / (float) 128;
#endif
    return fval;
}

static float get_saturation(HAL *hp) 
{
    int ival;
    float fval = 0.0;
#ifdef __XPI__
    ival = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_SATURATION);
    fval = (ival & 0x80) ? (float) 1.0 : (float) 0.0;
    ival &= 0x7f;
    fval += (float) ival / (float) 128;
#endif
    return fval;
}

static int get_hue(HAL *hp) 
{
    int ival = 0;
#ifdef __XPI__
    ival = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_HUE);
    if (ival > 128) 
        ival = 128 - ival;
#endif
    return ival;
}

static int get_rgb_range(HAL *hp) {
    int rc = 0;
#ifdef __XPI__
    rc = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_RX_RGB_RANGE);
#endif
    return rc;
}

static int get_sampling_phase(HAL *hp) {
    int rc = 0;
#ifdef __XPI__
    rc = hal_v4l2_g_ctrl(hp, NETVIZXPI_CID_SAMPLING_PHASE);
#endif
    return rc;
}

static void set_brightness(HAL *hp, int val) 
{
#ifdef __XPI__
    int ival;
    val = (val < -127) ? -127 : ((val > 127) ? 127 : val);
    if (val < 0) 
        ival = 128 - val;
    else
        ival = val;
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_BRIGHTNESS, ival);
#endif
}

static void set_contrast(HAL *hp, float fval) 
{
#ifdef __XPI__
    int ival = 0;
    if (fval >= 1.0) {
        fval -= 1.0;
        ival = 0x80;
    }
    ival = ival + (int) (fval * (float) 128); 
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_CONTRAST, ival);
#endif
}

static void set_saturation(HAL *hp, float fval) 
{
#ifdef __XPI__
    int ival = 0;
    if (fval >= 1.0) {
        fval -= 1.0;
        ival = 0x80;
    }
    ival = ival + (int) (fval * (float) 128); 
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_SATURATION, ival);
#endif
}

static void set_hue(HAL *hp, int val) 
{
#ifdef __XPI__
    int ival;
    val = (val < -127) ? -127 : ((val > 127) ? 127 : val);
    if (val < 0) 
        ival = 128 - val;
    else
        ival = val;
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_HUE, val);
#endif
}

static void set_rgb_range(HAL *hp, int value) {
#ifdef __XPI__
    if (value != NETVIZXPI_RGB_RANGE_AUTO && 
            value != NETVIZXPI_RGB_RANGE_LIMITED && 
            value != NETVIZXPI_RGB_RANGE_FULL)
        return;
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_RX_RGB_RANGE, value);
#endif
}

static void set_sampling_phase(HAL *hp, int value) {
#ifdef __XPI__
    int val;
    val = (value < 0) ? 0 : ((value > 31) ? 31 : value);
    hal_v4l2_s_ctrl(hp, NETVIZXPI_CID_SAMPLING_PHASE, val);
#endif
}

static void paramsInitXpiColorParameters(struct xpi_color_parm_params *params) {
    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    params->brightness_set = 1;
    params->brightness_value = get_brightness(hp);
    params->contrast_set = 1;
    params->contrast_value = get_contrast(hp);
    params->saturation_set = 1;
    params->saturation_value = get_saturation(hp);
    params->hue_set = 1;
    params->hue_value = get_hue(hp);
    params->rgb_range_set = 1;
    params->rgb_range_value = rgb_range_code_from_value(get_rgb_range(hp));
    params->sampling_phase_set = 1;
    params->sampling_phase_value = get_sampling_phase(hp);
}

int applyXpiColorParameters(struct xpi_color_parm_params *params,
                         struct xpi_color_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    if (params->brightness_value != orig_params->brightness_value)
        set_brightness(hp, params->brightness_value);
    if (params->contrast_value != orig_params->contrast_value)
        set_contrast(hp, params->contrast_value);
    if (params->saturation_value != orig_params->saturation_value)
        set_saturation(hp, params->saturation_value);
    if (params->hue_value != orig_params->hue_value)
        set_hue(hp, params->hue_value);
    if (params->rgb_range_value != orig_params->rgb_range_value)
        set_rgb_range(hp, rgb_range_value_from_code(params->rgb_range_value));
    if (params->sampling_phase_value != orig_params->sampling_phase_value)
        set_sampling_phase(hp, params->sampling_phase_value);

    return RC_NORMAL;
}

void XpiColorParametersInit() {
    registerXpiColorParametersParamsInitFunction(paramsInitXpiColorParameters);
}
