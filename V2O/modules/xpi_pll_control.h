#ifndef __XPI_CLOCK_CONTROL_H__
#define __XPI_CLOCK_CONTROL_H__

enum Si5338Pll {
    PLL_1 = 1,
    PLL_2 = 2
};

enum Si5338PllOutputs {
    SI_PLL_0 = 0,
    SI_PLL_1,
    SI_PLL_2,
    SI_PLL_3
};

#define DECODER_PIXCLK_PLL_STEPSIZE 500

#ifdef __cplusplus
extern "C" {
#endif
    void xpi_set_pll_frequency(HAL *hp, double freq_in_mhz);
    unsigned long xpi_get_pll_frequency(HAL *hp);
#ifdef __cplusplus
}
#endif

#endif          /* __XPI_CLOCK_CONTROL_H__ */
