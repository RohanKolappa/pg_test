#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "ad9888vif.h"
#include "color_params_menu_gen.h"

static int get_red_gain(HAL *hp) {
    return hal_get_ad9888_val_k(hp, VIF_RED_GAIN);
}

static int get_red_offset(HAL *hp) {
    return (hal_get_ad9888_val_k(hp, VIF_RED_OFFSET) >> 1);
}

static int get_green_gain(HAL *hp) {
    return hal_get_ad9888_val_k(hp, VIF_GRN_GAIN);
}

static int get_green_offset(HAL *hp) {
    return (hal_get_ad9888_val_k(hp, VIF_GRN_OFFSET) >> 1);
}

static int get_blue_gain(HAL *hp) {
    return hal_get_ad9888_val_k(hp, VIF_BLU_GAIN);
}

static int get_blue_offset(HAL *hp) {
    return (hal_get_ad9888_val_k(hp, VIF_BLU_OFFSET) >> 1);
}

static void set_red_gain(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 255) ? 255 : value);
    hal_set_ad9888_val_k(hp, VIF_RED_GAIN, value);
}

static void set_red_offset(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 127) ? 127 : value);
    hal_set_ad9888_val_k(hp, VIF_RED_OFFSET, (value << 1));
}

static void set_green_gain(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 255) ? 255 : value);
    hal_set_ad9888_val_k(hp, VIF_GRN_GAIN, value);
}

static void set_green_offset(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 127) ? 127 : value);
    hal_set_ad9888_val_k(hp, VIF_GRN_OFFSET, (value << 1));
}

static void set_blue_gain(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 255) ? 255 : value);
    hal_set_ad9888_val_k(hp, VIF_BLU_GAIN, value);
}

static void set_blue_offset(HAL *hp, int value) {
    value = (value < 0) ? 0 : ((value > 127) ? 127 : value);
    hal_set_ad9888_val_k(hp, VIF_BLU_OFFSET, (value << 1));
}

static void paramsInitColorParameters(struct color_parm_params *params) {
    HAL *hp = params->userptr;

    if (hp == NULL)
        return;

    params->red_gain_set = 1;
    params->red_gain_value = get_red_gain(hp);
    params->red_offset_set = 1;
    params->red_offset_value = get_red_offset(hp);
    params->green_gain_set = 1;
    params->green_gain_value = get_green_gain(hp);
    params->green_offset_set = 1;
    params->green_offset_value = get_green_offset(hp);
    params->blue_gain_set = 1;
    params->blue_gain_value = get_blue_gain(hp);
    params->blue_offset_set = 1;
    params->blue_offset_value = get_blue_offset(hp);
}

int applyColorParameters(struct color_parm_params *params,
                         struct color_parm_params *orig_params)
{
    HAL *hp = params->userptr;

    if (hp == NULL)
        return RC_ERROR;

    if (params->red_gain_value != orig_params->red_gain_value)
        set_red_gain(hp, params->red_gain_value);
    if (params->red_offset_value != orig_params->red_offset_value)
        set_red_offset(hp, params->red_offset_value);
    if (params->green_gain_value != orig_params->green_gain_value)
        set_green_gain(hp, params->green_gain_value);
    if (params->green_offset_value != orig_params->green_offset_value)
        set_green_offset(hp, params->green_offset_value);
    if (params->blue_gain_value != orig_params->blue_gain_value)
        set_blue_gain(hp, params->blue_gain_value);
    if (params->blue_offset_value != orig_params->blue_offset_value)
        set_blue_offset(hp, params->blue_offset_value);

    return RC_NORMAL;
}

void ColorParametersInit() {
    registerColorParametersParamsInitFunction(paramsInitColorParameters);
}
