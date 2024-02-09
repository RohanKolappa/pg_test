#ifndef __CHROMAKEY_CONFIG_H_
#define __CHROMAKEY_CONFIG_H_

#define MATTE_LOCAL             0
#define MATTE_REMOTE            1

#define COLORREF_VALUE          0
#define COLORREF_LOCATION       1

/* Macros to compute color max, min & overlap */
#define OVLP(amax, amin, bmax, bmin)    \
                (((amax) >= (bmin)) && ((bmax) >= (amin)))
                                                                                
#define VALMAX(val, tol) \
                ((((val) + (tol)) > 255) ? 255 : ((val) + (tol)))
                                                                                
#define VALMIN(val, tol) \
                ((((val) - (tol)) > 0) ? ((val) - (tol)) : 0)

struct chromakey_config {
    int enable;                 /* 0 - disable, 1 - enable */
    int colorref;               /* by value or by location */ 
    int transparency;           /* range: 0 - 100 */ 
    int redvalue;               /* range: 0 - 255 */
    int redtolerance;           /* range: 0 - 99 */
    int greenvalue;             /* range: 0 - 255 */
    int greentolerance;         /* range: 0 - 99 */
    int bluevalue;              /* range: 0 - 255 */
    int bluetolerance;          /* range: 0 - 99 */
    int pix1x;                  /* range: 0 - 4095 */
    int pix1y;                  /* range: 0 - 2047 */
    int pix2x;                  /* range: 0 - 4095 */
    int pix2y;                  /* range: 0 - 2047 */
    int pix3x;                  /* range: 0 - 4095 */
    int pix3y;                  /* range: 0 - 2047 */
};

#ifdef __cplusplus
extern "C" {
#endif
    int hal_get_chromakey_enable_k(HAL *hp, int matte);
    void hal_set_chromakey_enable_k(HAL *hp, int matte, int enable);
    int hal_get_chromakey_alpha_k(HAL *hp, int matte);
    void hal_set_chromakey_alpha_k(HAL *hp, int matte, int alpha);
    void hal_get_chromakey_color_value_k(HAL *hp,
                                         int matte, 
                                         unsigned char *redvalue, 
                                         unsigned char *redtol,
                                         unsigned char *greenvalue,
                                         unsigned char *greentol,
                                         unsigned char *bluevalue,
                                         unsigned char *bluetol);
    void hal_set_chromakey_color_value_k(HAL *hp,
                                         int matte,
                                         unsigned char redvalue, 
                                         unsigned char redtol,
                                         unsigned char greenvalue,
                                         unsigned char greentol,
                                         unsigned char bluevalue,
                                         unsigned char bluetol);
    int hal_get_chromakey_pixel_color_k(HAL *hp,
                                        int matte,
                                        int x,
                                        int y,
                                        unsigned char *rval,
                                        unsigned char *gval,
                                        unsigned char *bval);
    int get_chromakey_config(HAL *hp, struct chromakey_config *cfg);
    int set_chromakey_config(HAL *hp, struct chromakey_config *cfg);
    int set_color_registers_from_values(HAL *hp,
                                        int matte,
                                        struct chromakey_config *cfg);
    int set_color_registers_from_pixels(HAL *hp,
                                        int matte,
                                        struct chromakey_config *cfg);
                                                  
#ifdef __cplusplus
}
#endif

#endif                          /* __CHROMAKEY_CONFIG_H_ */
