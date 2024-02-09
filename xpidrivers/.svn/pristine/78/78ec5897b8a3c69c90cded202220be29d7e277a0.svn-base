#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../driver/netvizxpi/netvizxpi.h"
#include "../../driver/netvizxpi/lut_table.h"

static int netvizxpi_open(char *dev_name) {
    struct stat st;
    int fd = -1;

    if (stat(dev_name, &st) == -1) {
        printf("Failed to identify %s: %m errno %d\n", dev_name, errno);
    }
    else if (!S_ISCHR(st.st_mode)) {
        printf("%s is not a device: %m errno %d\n", dev_name, errno);
    }
    else {
        fd = open(dev_name, O_RDWR|O_NONBLOCK, 0);
        if (fd == -1) {
            printf("Failed to open %s: %m errno %d\n", dev_name, errno);
        } 
    }
    return fd;
}

static void netvizxpi_close(int fd) {
    if (fd > 0)
        close(fd);
}

/*********************************************************************
 * Compute charge pump current based on the Analog Devices AD9888 or * 
 * AD9887A PLL setting calculator spreadsheet.                       *
 *********************************************************************/
static int charge_pump_current(struct LUT_struct *lutp, int ad9887_flag)
{

    double pcr, hfq, sratio, kvco, lp, tmp, ct, n, p;
    int current;

    pcr = lutp->a_pcr;
    hfq = pcr * 1000 / (double) (lutp->a_orig_hres + lutp->a_hfp +
                                 lutp->a_hbp + lutp->a_hsync);
    ct = 0.039 * 0.000001;
    if (ad9887_flag) {
        sratio = (pcr < 37) ? (double) 11 : (double) 14;
        kvco = (pcr < 140) ? 150 : 180;
        p = (pcr < 37) ? 4 : ((pcr < 74) ? 2 : ((pcr < 140) ? 1 : 1));
    }
    else {
        sratio = 14.3;
        kvco = (pcr < 150) ? 150 : 180;
        p = (pcr < 41) ? 4 : ((pcr < 82) ? 2 : ((pcr < 150) ? 1 : 1));
    }
    n = (int) (pcr / hfq * (double) 1000);
    tmp = hfq * 6280 / sratio;
    lp = tmp * tmp * ((ct * n * p) / (kvco * 1000000)) * 1000000;
    if (lp < 75)
        current = 0;
    else if (lp < 125)
        current = 1;
    else if (lp < 200)
        current = 2;
    else if (lp < 300)
        current = 3;
    else if (lp < 425)
        current = 4;
    else if (lp < 625)
        current = 5;
    else if (lp < 1125)
        current = 6;
    else
        current = 7;

#ifdef VCOCPMP_FORMULA_INFO
    if (ad9887_flag) {
        printf("Pxclk = %f, Hfreq = %f, PLL stability ratio = %d\n",
               pcr, hfq, (int) sratio);
        printf("N = %d, Kvco = %d, P = %d, lp = %f, current = %x\n",
               (int) n, (int) kvco, (int) p, lp, current);
    }
#endif

    return current;
}

/****************************************************************
 * Set the hres and vres values in the LUT to use what we've    *
 * computed for the FPGA to use for encoding.                   *
 ****************************************************************/
static int fill_lut_hvres(struct LUT_struct *lutp)
{
    int hres, vres, h, v, ssize;

    hres = lutp->a_orig_hres;
    vres = lutp->a_orig_vres;

    if (compute_slice_size(hres, vres, &ssize, &h, &v) < 0) {
        return -1;
    }
    else if (((h - hres) < lutp->a_hbp) && ((v - vres) < lutp->a_vbp)) {
        lutp->a_hres = h;
        lutp->a_vres = v;
        return 0;
    }
    return -1;
}

/*****************************************************
 * Populate all the computed parameter values in LUT *
 *****************************************************/
int build_lut_entry(struct LUT_struct *lp, int device_type)
{
    double ddsfreq, ffactor = -1, clk;
    int vf_hi, vf_lo;

    /* set default LUT parameter values */
    lp->a_stereo = LUT_STEREO_NO;
    lp->a_cspace = LUT_CSPACE_YPRPB;
    lp->a_cpmp = 0;
    lp->a_rgain = lp->a_ggain = lp->a_bgain = 0;
    lp->a_roffset = lp->a_goffset = lp->a_boffset = 0;
    lp->a_noise = 0;
    lp->a_hshift = lp->a_vshift = 0;
    lp->a_phase = 0;
    lp->a_vmargin = 0;

    /* set clock frequency and DDS reference clock */
    if (device_type == V2DI50_DEVICE) {
        clk = V2_CLOCK_FREQ;
        ffactor = V2_DDSFFACTOR;
    }
    else if (device_type == PLX_DEVICE) {
        clk = V4_CLOCK_FREQ;
        ffactor = V4_DDSFFACTOR;
    }
    else if (device_type == XPI_DEVICE) {
        clk = XPI_CLOCK_FREQ;
        lp->a_refresh = (double) lp->a_lrefresh / 1000.0;
        lp->a_pcr = (double) lp->a_lpcr / 1000.0;
    }
    else {
        return -1;
    }

    /* compute hres, vres and check total number of blocks */
    if (fill_lut_hvres(lp) < 0)
        return -1;

    /* compute what the measured horizontal frequency etc should be */
    lp->a_amhreg = clk / (lp->a_refresh * 
            (lp->a_orig_vres + lp->a_vfp + lp->a_vsync + lp->a_vbp));
    lp->a_amvreg = (clk / lp->a_refresh) / (double) 256;
    lp->a_hsyncc = (lp->a_orig_vres + lp->a_vfp+lp->a_vsync + lp->a_vbp);

    /* compute high & low watermarks */
    lp->a_fflwm = 15;
    if ((lp->a_hres * lp->a_vres) < (1024 * 1024))
        lp->a_ffhwm = lp->a_fflwm + 1;
    else 
        lp->a_ffhwm = (lp->a_hres*lp->a_vres) - (1024 * 1024);
    if (lp->a_ffhwm <= lp->a_fflwm)
        lp->a_ffhwm = lp->a_fflwm + 1;
    
    if (device_type == V2DI50_DEVICE && lp->a_pcr < (double) 47.5)
        lp->a_acreg = ACREG_FIFO_XCLK;
    else
        lp->a_acreg = 0;

    lp->a_aafdreg = 128;
    lp->a_avhareg = lp->a_hres;
    lp->a_avvareg = lp->a_vres;

    /* horizontal delays on add side */
    /* actually should be sync + back porch minus 2 (delay through AD9888) */
    lp->a_avhdreg = lp->a_hbp + lp->a_hsync;

    /* vertical delays on add side */
    /* actually should be back porch - vsync - front porch */
    lp->a_avvdreg = lp->a_vbp + lp->a_vsync;

    lp->a_adcpll01 = 
        (lp->a_orig_hres + lp->a_hfp + lp->a_hsync + lp->a_hbp - 1) / 16;
    lp->a_adcpll02 =  
        ((lp->a_orig_hres + lp->a_hfp + lp->a_hsync + lp->a_hbp - 1) % 16) << 4;

#ifdef PRE_VCOCPMP_FORMULA
    if (lp->a_hres == 1024 && lp->a_vres == 768) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 75 ? 0x4 :
                (lp->a_refresh < 85 ? 0x5 : 0x3));
        lp->a_adcvco03 <<= 3;
    }
    else if (lp->a_hres == 1280 && lp->a_vres == 1024) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 75 ? 0x3 : 0x4);
        lp->a_adcvco03 <<= 3;
    }
    else if (lp->a_hres == 1600 && lp->a_vres == 1200) {
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh <= 65 ? 0x4 :
                ((lp->a_refresh <= 75) ? 0x5 : 0x6));
        lp->a_adcvco03 <<= 3;
    }
    else {
        /* Update this when we have another resolution */
        lp->a_adcvco03 = (lp->a_pcr < 45 ? 0x0 :
                (lp->a_pcr < 90 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
        lp->a_adcvco03 <<= 3;
        lp->a_adcvco03 |= (lp->a_refresh<=75?0x3:0x4);
        lp->a_adcvco03 <<= 3;
    }
#else
    lp->a_adcvco03 = (lp->a_pcr < 41 ? 0x0 : 
            (lp->a_pcr < 82 ? 0x1 : (lp->a_pcr < 150 ? 0x2 : 0x3)));
    lp->a_adcvco03 <<= 6;
    lp->a_adcvco03 |= (charge_pump_current(lp, 0) << 3);
#endif

    lp->a_adcmc1 = (lp->a_pcr <= 210) ? 0x46 : 0xc6;    

    lp->a_adcvco16 = 0xfe;

    lp->a_adcvco73 = (lp->a_pcr < 37 ? 0x0 : 
            (lp->a_pcr < 74 ? 0x1 : (lp->a_pcr < 140 ? 0x2 : 0x3)));
    lp->a_adcvco73 <<= 5;
    if (lp->a_cpmp <= 7)
        lp->a_adcvco73 |= (lp->a_cpmp << 2);
    else
        lp->a_adcvco73 |= (charge_pump_current(lp, 1) << 2);
    lp->a_adcvco73 |= 0x80;

#ifdef VCOCPMP_FORMULA_DEBUG
    printf("VCOCPMP = %x\n", lp->a_adcvco73); 
#endif

    if (lp->a_pcr < (double) 88) {
        lp->d_dcreg |= DCREG_VID_BYPASS;
    }
    lp->d_dafdreg = 1024;
    lp->d_dvhareg = lp->a_avhareg;
    lp->d_dvvareg = lp->a_avvareg;
    lp->d_dvhfpreg = lp->a_hfp;
    lp->d_dvhsreg = lp->a_hsync;
    if (lp->a_hres - lp->a_orig_hres >= lp->a_hbp)
        return -1;
    else if (lp->a_hres - lp->a_orig_hres > 0)
        lp->d_dvhbpreg= lp->a_hbp - (lp->a_hres - lp->a_orig_hres);
    else
        lp->d_dvhbpreg= lp->a_hbp;
    lp->d_dvvfpreg = lp->a_vfp;
    lp->d_dvvsreg = lp->a_vsync;
    if (lp->a_vres - lp->a_orig_vres >= lp->a_vbp)
        return -1;
    else if (lp->a_vres - lp->a_orig_vres > 0)
        lp->d_dvvbpreg = lp->a_vbp - (lp->a_vres - lp->a_orig_vres);
    else
        lp->d_dvvbpreg = lp->a_vbp;

    if (device_type == XPI_DEVICE) {
        lp->d_dds0 = lp->d_dds1 = lp->d_dds2 = 
        	lp->d_dds3 = lp->d_dds4 = lp->d_dds5 =  0;
        lp->d_ddsclk = 0;
        lp->d_chgperbit = 0;
    }
    else {
        ddsfreq = lp->a_pcr / (double) 2 / ffactor * TWORAISED48;
        vf_hi = ddsfreq / TWORAISED24;
        vf_lo = ddsfreq - ((double) vf_hi * TWORAISED24);
        lp->d_dds5 = (vf_hi >> 16) & 0xff;
        lp->d_dds4 = (vf_hi >> 8) & 0xff;
        lp->d_dds3 = vf_hi & 0xff;
        lp->d_dds2 = (vf_lo >> 16) & 0xff;
        lp->d_dds1 = (vf_lo >> 8) & 0xff;
        lp->d_dds0 = vf_lo & 0xff;
    
        vf_hi = (unsigned int) (lp->d_dds5 << 16);
        vf_hi += (unsigned int) (lp->d_dds4 << 8);
        vf_hi += (unsigned int) lp->d_dds3;
        vf_lo = (unsigned int) (lp->d_dds2 << 16);
        vf_lo += (unsigned int) (lp->d_dds1 << 8);
        vf_lo += (unsigned int) lp->d_dds0;
        ddsfreq = ((double) vf_hi * TWORAISED24) + vf_lo;
        lp->d_ddsclk = ddsfreq * ffactor / TWORAISED48;
        lp->d_chgperbit = (((100./2) * TWORAISED48) / ffactor / 1000000.);
    }
    lp->d_pixclck = lp->a_pcr;

    return 0;
}

#define CREATE_PRINT_STATEMENT(membername, type)                    \
    len = printf("%s.%d", #membername, count);                      \
    printf("%*s " type "\n", 25 - len, " ", lutp->membername);

static void print_lut(struct LUT_struct *lutp, int count, int full_print)
{
    int len;

    if (lutp == NULL)
        return;

    printf("##################################################\n");
    CREATE_PRINT_STATEMENT(a_alutreg, "0x%x");
    CREATE_PRINT_STATEMENT(a_stereo, "0x%x");
    CREATE_PRINT_STATEMENT(a_hres, "0x%x");
    CREATE_PRINT_STATEMENT(a_vres, "0x%x");
    CREATE_PRINT_STATEMENT(a_orig_hres, "0x%x");
    CREATE_PRINT_STATEMENT(a_orig_vres, "0x%x");
    CREATE_PRINT_STATEMENT(a_refresh, "%f");
    CREATE_PRINT_STATEMENT(a_hfp, "0x%x");
    CREATE_PRINT_STATEMENT(a_hsync, "0x%x");
    CREATE_PRINT_STATEMENT(a_hbp, "0x%x");
    CREATE_PRINT_STATEMENT(a_vfp, "0x%x");
    CREATE_PRINT_STATEMENT(a_vsync, "0x%x");
    CREATE_PRINT_STATEMENT(a_vbp, "0x%x");
    CREATE_PRINT_STATEMENT(a_pcr, "%f");
    CREATE_PRINT_STATEMENT(a_cspace, "%d");
    CREATE_PRINT_STATEMENT(a_rgain, "%d");
    CREATE_PRINT_STATEMENT(a_ggain, "%d");
    CREATE_PRINT_STATEMENT(a_bgain, "%d");
    CREATE_PRINT_STATEMENT(a_roffset, "%d");
    CREATE_PRINT_STATEMENT(a_goffset, "%d");
    CREATE_PRINT_STATEMENT(a_boffset, "%d");
    CREATE_PRINT_STATEMENT(a_hshift, "%d");
    CREATE_PRINT_STATEMENT(a_vshift, "%d");
    CREATE_PRINT_STATEMENT(a_phase, "%d");
    CREATE_PRINT_STATEMENT(a_noise, "0x%x");
    CREATE_PRINT_STATEMENT(a_vmargin, "0x%x");
    if (full_print) {
        CREATE_PRINT_STATEMENT(a_adcvco73, "0x%x");
        CREATE_PRINT_STATEMENT(a_cpmp, "0x%x");
        CREATE_PRINT_STATEMENT(a_amhreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_amvreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_hsyncc, "0x%x");
        CREATE_PRINT_STATEMENT(a_alutreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_fflwm, "0x%x");
        CREATE_PRINT_STATEMENT(a_ffhwm, "0x%x");
        CREATE_PRINT_STATEMENT(a_acreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_aafdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avhareg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avvareg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avhdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_avvdreg, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcpll01, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcpll02, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcvco03, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcmc1, "0x%x");
        CREATE_PRINT_STATEMENT(a_adcvco16, "0x%x");
        CREATE_PRINT_STATEMENT(d_dafdreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dcreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhareg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvareg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhfpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhsreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvhbpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvfpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvsreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvvbpreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dvbwreg, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds5, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds4, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds3, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds2, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds1, "0x%x");
        CREATE_PRINT_STATEMENT(d_dds0, "0x%x");
        CREATE_PRINT_STATEMENT(d_ddsclk, "%f");
        CREATE_PRINT_STATEMENT(d_pixclck, "0x%x");
        CREATE_PRINT_STATEMENT(d_chgperbit, "0x%x");
    }
}

static void kernel_luts(int fd, int full_print)
{
    struct LUT_struct lut;
    int count;

    lut.lut_list.next = NULL;
    count = 1;
    do {
        if (ioctl(fd, FPGA_IOC_GET_LUT, &lut) < 0)
            break;
         build_lut_entry(&lut, XPI_DEVICE);
         print_lut(&lut, count, full_print);
         count++;
    } while (lut.lut_list.next != NULL);
}

static int print_addside_lut(int fd)
{
    struct LUT_struct lut;

    if (ioctl(fd, FPGA_IOC_GET_ADDSIDELUT, &lut) < 0)
        return -1;
    build_lut_entry(&lut, XPI_DEVICE);
    print_lut(&lut, 0, 0);
    return 0;
}

int main(int argc, char **argv) {
    int fd;
    int opt, fullprint = 0, allkernelluts = 1;
    char *dev_name;

    while ((opt = getopt(argc, argv, "kch")) > 0) {
        switch(opt) {
        case 'k':
            allkernelluts = 1;
            break;
        case 'c':
            allkernelluts = 0;
            break;
        case 'h':
        default:
            printf("Usage: printlut [-k(ernel)] [-c(urrent)]\n");
            exit(0);
        }
    }

    dev_name = "/dev/video0";
    fd = netvizxpi_open(dev_name);
    if (fd < 0) {
        printf("Failed to open %s as a V4L2 device.\n", dev_name);
        return -1;
    }

    if (allkernelluts)
        kernel_luts(fd, 0);
    else if (print_addside_lut(fd) < 0) 
        printf("Failed to get addside LUT from driver\n");

    netvizxpi_close(fd);

    return 0;
}
