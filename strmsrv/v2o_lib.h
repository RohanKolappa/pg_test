/*
 * v2o_lib.h
 *
 *  Created on: Nov 22, 2010
 *      Author: rkale
 */

#ifndef V2O_LIB_H_
#define V2O_LIB_H_

#if (defined(__WIN32__) || defined(_WIN32))
#define WINDOWS
#else
#ifdef WINDOWS
#undef WINDOWS
#endif
#endif

#ifndef WINDOWS

#include "v2o.h"

#else

#define HAL void

struct LUT_struct {

    char comment[64]           ; /* Name */
    unsigned short a_stereo    ; /* Stereo? (Yes, No or Both) */
    unsigned short a_hres      ; /* Horizontal Resolution */
    unsigned short a_vres      ; /* Vertical Resolution */
    double  a_refresh          ; /* Refresh Rate (Hz) */
    unsigned long  a_lrefresh  ; /* Refresh Rate (Hz) */
    unsigned short a_hfp       ; /* Horiz Front Porch */
    unsigned short a_hsync     ; /* Horiz Sync */
    unsigned short a_hbp       ; /* Horiz Back Porch */
    unsigned short a_vfp       ; /* Vert Front Porch */
    unsigned short a_vsync     ; /* Vert Sync */
    unsigned short a_vbp       ; /* Vert Back Porch */
    double  a_pcr              ; /* Pixel Clock Rate (MHz) */
    unsigned long  a_lpcr      ; /* Pixel Clock Rate (MHz) */

    unsigned short a_orig_hres ; /* Original Horizontal Resolution (not */
                                 /* yet adjusted for V2D) */
    unsigned short a_orig_vres ; /* Original Vertical Resolution (not */
                                 /* yet adjusted for V2D) */
#if 0
    unsigned short a_cspace    ; /* 1 if YUV else RGB */
    unsigned char  a_rgain     ; /* Red gain */
    unsigned char  a_ggain     ; /* Green gain */
    unsigned char  a_bgain     ; /* Blue gain */
    unsigned char  a_roffset   ; /* Red offset */
    unsigned char  a_goffset   ; /* Green offset */
    unsigned char  a_boffset   ; /* Blue offset */
    int            a_hshift    ; /* Horizontal Adjust */
    int            a_vshift    ; /* Vertical Adjust */
    int            a_phase     ; /* Phase Adjust */
    int            a_noise     ; /* Noise Levels */
    unsigned short a_vmargin   ; /* Delta allowed for total vert lines */
    unsigned short a_adcvco73  ; /* ADC 9887 VCO Reg (0x03) */
    unsigned short a_cpmp      ; /* Set cpmp bits in adcvco73 to this value */
                               ; /* if between 0 and 7 otherwise auto compute */

    /* the following six fields are used by RX to override video from TX */
    unsigned short d_tx_hres   ; /* TX Horizontal Resolution */
    unsigned short d_tx_vres   ; /* TX Vertical Resolution */
    double  d_tx_refresh       ; /* TX Refresh Rate (Hz) */
    unsigned long  d_ltx_refresh; /* TX Refresh Rate (Hz) */
    double  d_tx_pcr           ; /* TX Pixel Clock Rate (MHz) */
    unsigned long  d_ltx_pcr   ; /* TX Pixel Clock Rate (MHz) */
    unsigned short d_tx_stereo ; /* TX Stereo */
    unsigned short d_tx_lutidx ; /* TX LUT ID*/

    unsigned short d_slicereg  ; /* # slices per frame for TX LUT */
    unsigned short d_ov_control; /* Drop override control */

    unsigned short a_amhreg    ; /* AMHREG */
    unsigned short a_amvreg    ; /* AMVREG */
    unsigned short a_hsyncc    ; /* Addside Total Lines */
    unsigned short a_hsyncc2   ; /* AMTREG */
#endif
    unsigned short a_alutreg   ; /* Addside LUTREG */
#if 0
    unsigned       a_fflwm     ; /* Addside FIFO Low Watermark */
    unsigned       a_ffhwm     ; /* Addside FIFO High Watermark */
    unsigned short a_acreg     ; /* Add Control Register */
    unsigned short a_aafdreg   ; /* Add Audio FIFO Delay */
    unsigned short a_avhareg   ; /* Add Video Horizontal Active */
    unsigned short a_avhdreg   ; /* Add Video Horiz Delay */
    unsigned short a_avvareg   ; /* Add Video Vertical Active */
    unsigned short a_avvdreg   ; /* Add Video Vertical Delay*/
    unsigned short a_adcpll01  ; /* Add ADC PLL Divisor MSB (0x01) */
    unsigned short a_adcpll02  ; /* Add ADC PLL Divisor LSB (0x02) */
    unsigned short a_adcvco03  ; /* Add ADC VCO/CPMP (0x03) */
    unsigned short a_adcmc1    ; /* Add ADC Mode Control 1 (0x15) */
    unsigned short a_adcvco16  ; /* Add ADC VCO (0x16) */
    unsigned short d_dlutreg   ; /* Dropside LUTREG */
    unsigned short d_dafdreg   ; /* Drop Audio FIFO Delay */
    unsigned short d_dcreg     ; /* Drop Control Register */
    unsigned short d_dvhareg   ; /* Drop Video Horizontal Active */
    unsigned short d_dvvareg   ; /* Drop Video Vertical Active */
    unsigned short d_dvhfpreg  ; /* Drop Video Horizontal Front Porch */
    unsigned short d_dvhsreg   ; /* Drop Video Horizontal Sync */
    unsigned short d_dvhbpreg  ; /* Drop Video Horizontal Porch */
    unsigned short d_dvvfpreg  ; /* Drop Video Vertical Front Porch */
    unsigned short d_dvvsreg   ; /* Drop Video Vertical Sync */
    unsigned short d_dvvbpreg  ; /* Drop Video Vertical Back Porch */
    unsigned short d_dvbwreg   ; /* Drop Video Buffer Watermark */
    unsigned short d_dds5      ; /* DDS Frequency Word 5 */
    unsigned short d_dds4      ; /* DDS Frequency Word 4 */
    unsigned short d_dds3      ; /* DDS Frequency Word 3 */
    unsigned short d_dds2      ; /* DDS Frequency Word 2 */
    unsigned short d_dds1      ; /* DDS Frequency Word 1 */
    unsigned short d_dds0      ; /* DDS Frequency Word 0 */
    double  d_ddsclk           ; /* Drop Output Clock (MHz) */
    unsigned long  d_lddsclk   ; /* DDS output clock (MHz) */
    unsigned short d_pixclck   ; /* Drop Pixel clock (MHz) */
    unsigned int   d_chgperbit ; /* Change to DDS Freq Reg */

    struct list_head    lut_list;
#endif
};

#endif

#endif /* V2O_LIB_H_ */
