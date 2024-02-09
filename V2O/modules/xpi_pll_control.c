#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <syslog.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "vlog.h"

#ifdef __XPI__
#include "platform-netvizxpi.h"
#include "netvizxpi/netvizxpi.h"
#endif
#include "xpi_i2c_control.h"
#include "xpi_pll_control.h"

#define PLLS_PER_CHIP			4
#define PLL_REGS_BASE			53
#define NUM_PLL_REGS			11
#define OEB_REG				    230	    /* output enable register */
#define PS_REG				    255	    /* page select register */ 
#define TEN_P6          		1000000 /* 10 pow 6 */
#define MSx_SEL        			(MS0)
#define REF_XTAL           		25000000UL
#define DEFAULT_BASE_FREQ 		2500000000UL
#define MAX_NUMERATOR   		0x3FFFFFFF
#define MAX_DENOMINATOR 		0x3FFFFFFF

enum msx {
    MS0 = 0,
    MS1,
    MS2,
    MS3
};

typedef struct {
    unsigned char fidp1;
    unsigned char fidp2;
    unsigned char fidp3;

} fidpx_addr;

const fidpx_addr fidpx_address_map[] = {
    {123, 136, 137 }, // MS0
    {152, 165, 166 }, // MS1
    {174, 187, 188 }, // MS2
    {196, 209, 210 }  // MS3
};

const unsigned char fidincdec_address_map[] = {
    52,
    63,
    74,
    85
};

const unsigned char si5338_write_mask[] = 
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00,	/*   0 -   7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*   8 -  15 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	/*  16 -  23 */
    0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, /*  24 -  31 */
    0xff, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0x1f, 0x1f, /*  32 -  39 */
    0xff, 0x7f, 0x3f, 0x00, 0x00, 0xff, 0xff, 0xff, /*  40 -  47 */
    0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, /*  48 -  55 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, /*  56 -  63 */
    0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, /*  64 -  71 */
    0xff, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, /*  72 -  79 */
    0xff, 0xff, 0xff, 0xff, 0x3f, 0x7f, 0xff, 0xff, /*  80 -  87 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, /*  88 -  95 */
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /*  96 - 103 */
    0xff, 0xff, 0xbf, 0xff, 0x7f, 0xff, 0xff, 0xff, /* 104 - 111 */
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 112 - 119 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 120 - 127 */
    0xff, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, /* 128 - 135 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 136 - 143 */
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 144 - 151 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, /* 152 - 159 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 160 - 167 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 168 - 175 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, /* 176 - 183 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 184 - 191 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 192 - 199 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 200 - 207 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 208 - 215 */
    0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 216 - 223 */
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xff, 0x00, /* 224 - 231 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, /* 232 - 239 */
    0x00, 0xff, 0x02, 0x00, 0x00, 0x00, 0xff, 0x00, /* 240 - 247 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, /* 248 - 255 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 256 - 263 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 264 - 271 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 272 - 279 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, /* 280 - 287 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 288 - 295 */
    0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xff, /* 296 - 303 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 304 - 311 */
    0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xff, /* 312 - 319 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 320 - 327 */
    0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0xff, /* 328 - 335 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 336 - 343 */
    0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00     	/* 344 - 350 */
};

#ifdef __XPI__
static int read_si5338(HAL *hp, int reg, unsigned char *val) 
{
    int rc = 0;

    if (reg > 255) 
        xpi_set_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
                PS_REG, 0x01);
    else
        xpi_set_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
                PS_REG, 0x00);

    rc = xpi_get_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
            (unsigned char) (reg & 0xff), val);

    return rc;
}

static int write_si5338(HAL *hp, int reg, unsigned char val) 
{
    unsigned char newval;

    if (si5338_write_mask[reg] == 0x00) {
        /* ignore registers with mask of 0x00 */
        return -1;
    } 
    if (reg > 255) 
        xpi_set_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
                PS_REG, 0x01);
    else
        xpi_set_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
                PS_REG, 0x00);

    if (si5338_write_mask[reg] == 0xff) {
        newval = val;
    }
    else {
        unsigned char curr_val = 0, clear_curr_val, clear_new_val;
        /* do a read-modify-write using I2C bit-wise operation */
        if (xpi_get_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
                    (unsigned char) (reg & 0xff), &curr_val) < 0)
            return -1; 

        /* get the non-writable bits in the curr value */
        clear_curr_val = curr_val & ~si5338_write_mask[reg]; 

        /* clear the non-writable bits in the value to be written */
        clear_new_val = val & si5338_write_mask[reg]; 

        /* combine the values to get the new value for the register */
        newval = clear_curr_val | clear_new_val; 
    }

    return xpi_set_main_i2c_val(hp, SI5338_I2C_PORT, SI5338_I2C_ADDR, 
            (unsigned char) (reg & 0xff), newval);
}

static int disable_clock(HAL *hp, const unsigned char clock_idx)
{
    unsigned char oeb;

    if (read_si5338(hp, OEB_REG, &oeb) < 0) {
         VLOG_INFO("Error: failed to read reg 0x%02x", OEB_REG);
         return -1;           
    } 
    oeb |= 1 << clock_idx;
    if (write_si5338(hp, OEB_REG, oeb) < 0) {
         VLOG_INFO("Error: failed to write reg 0x%02x", OEB_REG);
         return -1;           
    } 
    return 0;
}

static unsigned long get_base_frequency(HAL *hp) 
{
    unsigned char val;
    unsigned long msn_p1 = 0, msn_p2 = 0, msn_p3 = 0;

    read_si5338(hp, 97, &val); 
    msn_p1 = val;
    read_si5338(hp, 98, &val); 
    msn_p1 |= ((unsigned long) val << 8);
    read_si5338(hp, 99, &val); 
    msn_p1 |= ((unsigned long) (val & 0x03) << 16);

    msn_p2 = (val >> 2);
    read_si5338(hp, 100, &val); 
    msn_p2 |= ((unsigned long) val << 6);
    read_si5338(hp, 101, &val); 
    msn_p2 |= ((unsigned long) val << 14);
    read_si5338(hp, 102, &val); 
    msn_p2 |= ((unsigned long) val << 22);

    read_si5338(hp, 103, &val); 
    msn_p3 = val;
    read_si5338(hp, 104, &val); 
    msn_p3 |= ((unsigned long) val << 8);
    read_si5338(hp, 105, &val); 
    msn_p3 |= ((unsigned long) val << 16);
    read_si5338(hp, 106, &val); 
    msn_p3 |= ((unsigned long) (val & 0x3f) << 16);

    if ((msn_p2 != 0) || (msn_p3 != 1)) {
        return DEFAULT_BASE_FREQ;
    }

    return ((msn_p1 + 512) / 128) * REF_XTAL;
} 

static int set_parameters(HAL *hp,
                          enum Si5338PllOutputs plloutput,
                          unsigned int i, 
                          unsigned int n, 
                          unsigned int d)
{
    const unsigned char clock_idx = plloutput & (PLLS_PER_CHIP - 1);
    unsigned int ii, nn;
    unsigned char oeb, addr;

    /* Disable output */
    if (read_si5338(hp, OEB_REG, &oeb) < 0) {
        return -1; 
    }
    usleep(100000);
    oeb |= 1 << clock_idx;
    if (write_si5338(hp, OEB_REG, oeb) < 0) {
        return -1;
    }

    /* Convert from i, n, d to actual register values */
    ii = ((i * d + n) * 128) / d - 512;
    nn = (n * 128) % d;

    /* Write new parameter values */
    addr = PLL_REGS_BASE + clock_idx * NUM_PLL_REGS;
    if (write_si5338(hp, addr++, ii) < 0)	        /* INTDIV[7:0] */
        return -1;
    if (write_si5338(hp, addr++, (ii >> 8)) < 0)	/* INTDIV[15:8] */
        return -1;
    if (write_si5338(hp, addr++, ((ii >> 16)|(nn << 2))) < 0)	
        return -1;				            /* NUM[5:0] INTDIV[17:16] */
    if (write_si5338(hp, addr++, (nn >> 6)) < 0)	/* NUM[13:6] */
        return -1;
    if (write_si5338(hp, addr++, (nn >> 14)) < 0)	/* NUM[21:14] */
        return -1;
    if (write_si5338(hp, addr++, (nn >> 22)) < 0)	/* NUM[29:22] */
        return -1;
    if (write_si5338(hp, addr++, d) < 0)		    /* DEN[7:0] */
        return -1;
    if (write_si5338(hp, addr++, (d >> 8)) < 0)	    /* DEN[15:8] */
        return -1;
    if (write_si5338(hp, addr++, (d >> 16)) < 0)	/* DEN[23:16] */
        return -1;
    if (write_si5338(hp, addr++, (d >> 24)) < 0)	/* DEN[29:24] */
        return -1;

    /* Enable output */
    oeb &= ~(1 << clock_idx); 
    if (write_si5338(hp, OEB_REG, oeb) < 0) {
        return -1;
    }
       
    /* Success */
    return 0;     
}

static int get_parameters(HAL *hp,
                          enum Si5338PllOutputs plloutput,
                          unsigned int *i, 
                          unsigned int *n, 
                          unsigned int *d)
{
    const unsigned char clock_idx = plloutput & (PLLS_PER_CHIP - 1);
    unsigned int ii, nn, dd, nnn;
    unsigned char addr, cval;

    /* Read parameter values */
    addr = PLL_REGS_BASE + clock_idx * NUM_PLL_REGS;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* INTDIV[7:0] */
        return -1;
    ii = (unsigned int) cval;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* INTDIV[15:8] */
        return -1;
    ii |= (unsigned int) cval << 8;
    if (read_si5338(hp, addr++, &cval) < 0)	
        return -1;	    			            /* NUM[5:0] INTDIV[17:16] */
    ii |= (unsigned int) (cval & 0x03) << 16;
    nn = (unsigned int) (cval & 0xfc) >> 2;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* NUM[13:6] */
        return -1;
    nn |= (unsigned int) cval << 6;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* NUM[21:14] */
        return -1;
    nn |= (unsigned int) cval << 14;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* NUM[29:22] */
        return -1;
    nn |= (unsigned int) cval << 22;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* DEN[7:0] */
        return -1;
    dd = (unsigned int) cval;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* DEN[15:8] */
        return -1;
    dd |= (unsigned int) cval << 8;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* DEN[23:16] */
        return -1;
    dd |= (unsigned int) cval << 16;
    if (read_si5338(hp, addr++, &cval) < 0)	    /* DEN[29:24] */
        return -1;
    dd |= (unsigned int) cval << 24;

    /* Convert actual register values to i, n, d */
    *d = dd;
    *n = 0;
    /* compute n from nn = (n * 128) % d */
    if (dd > 0) {
        for (nnn = 0; nnn < dd; nnn++) {
            if (((nnn * 128) % dd) == nn) {
                *n = nnn;
                break;
            }
        }
    }

    /* compute i as (integer part of (ii / 128) + 4) */
    *i = (ii / 128) + 4;

    /* Success */
    return 0;     
}

static int set_step_resolution(HAL *hp,
                               enum Si5338PllOutputs plloutput,
                               unsigned int freq,
                               unsigned int ssize,
                               unsigned int i,
                               unsigned int n,
                               unsigned int d)
{
    const unsigned char clock_idx = plloutput & (PLLS_PER_CHIP - 1);
    unsigned int ppm;
    unsigned long long fidp1, fidp2, fidp3;
    unsigned char oeb, addr;

    /* Output disabled --> no stepsize required */
    if (freq == 0) 
        return 0;

    /* Calculate Fstep/Fout..must be an integer multiple of 1 ppm */
    ppm = ((unsigned long) TEN_P6 * (unsigned long) ssize) / 
					(unsigned long) freq;
    if (!ppm)
        ppm = 1; 

    /* Calculate step size resolution of 1 ppm (AN411 rev05 p15) */
    fidp1 = (unsigned long long) TEN_P6 * (unsigned long long) d;    
    fidp2 = (unsigned long long) (d * ppm);    
    fidp3 = (unsigned long long) TEN_P6 * (((unsigned long long) i * 
			(unsigned long long) d) + (unsigned long long) n);    

    /* Disable output */
    if (read_si5338(hp, OEB_REG, &oeb) < 0)
        return -1; 

    oeb |= 1 << clock_idx;
    if (write_si5338(hp, OEB_REG, oeb) < 0)
        return -1;

    /* Write new parameter values: write MSx_FIDP1[7:0]->MSx_FIDP1[51:48] */
    addr = fidpx_address_map[MSx_SEL].fidp1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 0)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 8)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 16)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 24)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 32)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 40)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp1 >> 48)) < 0)
        return -1;

    /* Write MSx_FIDP2[7:0]->MSx_FIDP2[51:48] */
    addr = fidpx_address_map[MSx_SEL].fidp2;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 0)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 8)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 16)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 24)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 32)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 40)) < 0)
        return -1;
    if (write_si5338(hp, addr--, (unsigned char) (fidp2 >> 48)) < 0)
        return -1;

    /* Write MSx_FIDP3[7:0]->MSx_FIDP3[51:48] */
    addr = fidpx_address_map[MSx_SEL].fidp3;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 0)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 8)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 16)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 24)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 32)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 40)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 48)) < 0)
        return -1;
    if (write_si5338(hp, addr++, (unsigned char) (fidp3 >> 56) & 0x7f) < 0)
        return -1;
    
    /* Enable output */
    oeb &= ~(1 << clock_idx);
    if (write_si5338(hp, OEB_REG, oeb) < 0)
        return -1;

    /* Disable all MSx freq inc dec functions */
    if (write_si5338(hp, fidincdec_address_map[clock_idx], 0x10) < 0)
        return -1;

    /* Set bit 4 MS0_FIDDIS to 0 prior to writing freq inc dec command */
    if (write_si5338(hp, fidincdec_address_map[clock_idx], 0x00) < 0)
        return -1;

    /* Enable pin control inc/dec on MSx (only possible on MS0) */
    if (write_si5338(hp, fidincdec_address_map[clock_idx], 0x20) < 0)
        return -1;

    /* Success */
    return 0;
}

static bool found(unsigned long base_freq,
                  unsigned long freq_out,
                  unsigned long n,
                  unsigned long d,
                  unsigned long *best_n,
                  unsigned long *best_d,
                  unsigned long *best_delta) 
{
    const unsigned long actual = ((unsigned long long) d * base_freq) / n;
    unsigned long diff = freq_out - actual;
    if (diff < *best_delta) {
        *best_n = n;
        *best_d = d;
        *best_delta = diff;
    }
    return (*best_delta == 0);
}

static void calc_pll_values(unsigned long base_freq, 
                            unsigned long freq_out,
                            unsigned long *integer_val,
                            unsigned long *numerator,
                            unsigned long *denominator)
{
    unsigned long remainder = base_freq;

    /* Compute integer portion */
    if (freq_out == 0) {
        *integer_val = 0;
        remainder = 0;
    }
    else if (freq_out <= base_freq) {
        *integer_val = base_freq / freq_out;
        remainder -= freq_out * (*integer_val);
    }         
    else {
        *integer_val = 0;
    }

    /* Compute fractional portion */
    if (remainder == 0) {
        *numerator = 0;
        *denominator = 1; 
    }
    else {
        unsigned long n;
        unsigned long best_n = 1;
        unsigned long best_d = 1;
        unsigned long best_delta = ~0;
        for (n = 1; n <= MAX_NUMERATOR; n++) {
            unsigned long d;
            d = ((unsigned long long) n * freq_out) / remainder; 
            if (d > MAX_DENOMINATOR)
                break;
            if (found(remainder, freq_out, n, d, &best_n, &best_d, &best_delta))
                break;
            if (found(remainder, freq_out, n, d+1, &best_n, &best_d, &best_delta))
                break;
        }
        *numerator = best_n;
        *denominator = best_d;
    }
}
#endif

void xpi_set_pll_frequency(HAL *hp, double freq_in_mhz)
{
    enum Si5338Pll pll = PLL_1;
    enum Si5338PllOutputs plloutput = SI_PLL_2;
    unsigned int ssize = DECODER_PIXCLK_PLL_STEPSIZE;
    unsigned long freq_in_hz;

    if (hal_get_hardware_type(hp) != XPI_DEVICE)
        return;

#ifdef __XPI__
    if (freq_in_mhz == 0) {
        const unsigned char clock_idx = pll & (PLLS_PER_CHIP - 1);
        disable_clock(hp, clock_idx);
        VLOG_INFO("XPI: clock disabled on PLL %d", pll); 
    }
    if (freq_in_mhz > (PCR_DUALLINK_XPI / 1000))
        freq_in_hz = (unsigned long)(freq_in_mhz * (double) 1000000) / 2;
    else
        freq_in_hz = (unsigned long)(freq_in_mhz * (double) 1000000);

    if (freq_in_hz > PCR_DUALLINK_XPI * 1000) {
        VLOG_WARNING("XPI: invalid frequency %ld", freq_in_hz); 
    }
    else {
        unsigned long base_freq, i, n, d, ii = 0, nn = 0, dd = 0;
        base_freq = get_base_frequency(hp);
        calc_pll_values(base_freq, freq_in_hz, &i, &n, &d);
        if (set_parameters(hp, plloutput, i, n, d) == 0 &&
            set_step_resolution(hp, plloutput, freq_in_hz, ssize, i, n, d) == 0)
            VLOG_INFO("XPI: PLL %d output %d set to freq %lu, step size %u",
		        pll, plloutput, freq_in_hz, ssize); 
        else
            VLOG_INFO(
                "XPI: Error setting PLL %d output %d to freq %lu, step size %u",
		        pll, plloutput, freq_in_hz, ssize); 
    }
#endif
}

unsigned long xpi_get_pll_frequency(HAL *hp)
{
    enum Si5338Pll pll = PLL_1;
    enum Si5338PllOutputs plloutput = SI_PLL_2;
    int rc;
    unsigned long base = 0;
    unsigned int i = 0, n = 0, d = 0;
    double dfreq = (double) 0;

    if (hal_get_hardware_type(hp) != XPI_DEVICE)
        return 0;

#ifdef __XPI__
    base = get_base_frequency(hp);
    if ((rc = get_parameters(hp, plloutput, &i, &n, &d)) < 0) {
        VLOG_INFO("XPI: failed to get parameters for ouptut %d\n", plloutput);
    }
    else if (i == 0 && n == 0) {
        VLOG_INFO("XPI: get_parameters returned i = %u, n = %u!", i, n);
    }
    else {
        dfreq = (double) base * (double) d / (double) (i * d + n);
        VLOG_INFO("%s: base %lu, i %u, n %u, d %u, ouptut freq = %f\n",
                __func__, base, i, n, d, dfreq);
    }
#endif

    return (unsigned long) dfreq;
}

