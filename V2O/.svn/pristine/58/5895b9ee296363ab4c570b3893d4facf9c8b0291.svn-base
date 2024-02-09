#ifndef __TX_SPLASH_CONFIG_H_
#define __TX_SPLASH_CONFIG_H_

struct tx_splash_config {
    int                 tx_splash            ; /* 0 - blue, 1 - gridlines */
    int                 splash_rate          ; /* Bandwidth rate in bytes*/
    int                 enable_splash        ; /* Enable generating splash screen */
    int                 splash_codecversion  ; /* codec version for splash screen encoding */
    int                 splash_slicewidth    ;  /* slice width for version 2 codec */
    int                 splash_sliceheight   ;  /* slice height for version 2 codec */
    unsigned short      splash_stereo        ; /* Stereo? */
    unsigned short      splash_hres          ; /* Horizontal Resolution */
    unsigned short      splash_vres          ; /* Vertical Resolution */
    unsigned short      splash_orig_hres     ; /* Horiz Resolution for display */
    unsigned short      splash_orig_vres     ; /* Vert Resolution for display */
    double              splash_refresh       ; /* Refresh Rate (Hz) */
    double              splash_pcr           ; /* Pixel Clock Rate (MHz) */
    unsigned short      splash_hfp           ; /* Horiz Front Porch */
    unsigned short      splash_vfp           ; /* Vert Front Porch */
    unsigned short      splash_hbp           ; /* Horiz Back Porch */
    unsigned short      splash_vbp           ; /* Vert Back Porch */
    unsigned short      splash_hsync         ; /* Horiz Sync */
    unsigned short      splash_vsync         ; /* Vert Sync */
    unsigned short      splash_mquant        ; /* MQUANT */
    unsigned short      splash_dcquant       ; /* DCQUANT */
    unsigned short      splash_slicesize     ; /* Slice Size */
};
#ifdef __cplusplus
extern "C" {
#endif

extern int updateTxSplashLutParameters(HAL *hp, int lutid);

extern int get_tx_splash_config(HAL *hp, struct tx_splash_config *splash_cfg);

#ifdef __cplusplus
}
#endif
#endif                          /* __TX_SPLASH_CONFIG_H_ */
