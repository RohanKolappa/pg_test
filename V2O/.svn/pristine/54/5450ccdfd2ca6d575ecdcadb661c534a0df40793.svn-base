#ifndef __RX_SPLASH_CONFIG_H_
#define __RX_SPLASH_CONFIG_H_

struct rx_splash_config {
    int                 rx_splash           ; /* 0 - blue, 1 - gridlines */
    int                 enable_splash       ; /* Enable generating splash screen */
    int                 splash_codecversion ; /* Codec version for Splash screen */
    int                 splash_slicewidth   ; /* slice width */
    int                 splash_sliceheight  ; /* slice height */
    unsigned short      splash_stereo       ; /* Stereo? */
    unsigned short      splash_hres         ; /* Horizontal Resolution */
    unsigned short      splash_vres         ; /* Vertical Resolution */
    unsigned short      splash_orig_hres    ; /* Horiz Resolution for display */
    unsigned short      splash_orig_vres    ; /* Vert Resolution for display */
    double              splash_refresh      ; /* Refresh Rate (Hz) */
    double              splash_pcr          ; /* Pixel Clock Rate (MHz) */
    unsigned short      splash_hfp          ; /* Horiz Front Porch */
    unsigned short      splash_vfp          ; /* Vert Front Porch */
    unsigned short      splash_hbp          ; /* Horiz Back Porch */
    unsigned short      splash_vbp          ; /* Vert Back Porch */
    unsigned short      splash_hsync        ; /* Horiz Sync */
    unsigned short      splash_vsync        ; /* Vert Sync */
    unsigned short      splash_slicesize    ; /* Slice Size */
};

#ifdef __cplusplus
extern "C" {
#endif

extern int updateRxSplashLutParameters(HAL *hp, int lutid);

extern int get_rx_splash_config(HAL *hp, struct rx_splash_config *splash_cfg);

extern int get_rx_splash_lut(HAL *hp, struct LUT_struct *lutp);

#ifdef __cplusplus
}
#endif
#endif                          /* __RX_SPLASH_CONFIG_H_ */
