#ifndef __FPGA_H_INCLUDED__

#define __FPGA_H_INCLUDED__

#ifdef __KERNEL__
#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#else
#include <linux/kernel.h>  
#include <linux/fs.h>
#endif

#define TIMESPENT(x,y) ((((long)((x) - (y))*1000000)/863869)) 

#ifndef ERROR
#define ERROR   -1
#endif

#ifndef OK
#define OK      0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#define FPGA_DEV_MAJOR          60
#define VIDEO_DEV_MINOR         0
#define KEYBOARD_DEV_MINOR      1
#define MOUSE_DEV_MINOR         2
#define AUDIO_DEV_MINOR         3
#define I2C_DEV_MINOR           4
#define V2D_CHAR_DEVICES        5
#define MAX_FPGA_BOARDS         2
#define V2D_DRIVER_NAME         "fpga"

#define HARDWARE_DETECTED       1
#define HARDWARE_NOTDETECTED    0

// TODO Remove all these deprecated HARDWARE_XXX defines
#define HARDWARE_UNKNOWN        -1
#define HARDWARE_DUMMY          10
#define HARDWARE_V2D_TX         11
#define HARDWARE_V2D_RX         12
#define HARDWARE_DMS            13
#define HARDWARE_MS             14
#define HARDWARE_PLX            15

// Refactoring avove HARDWARE_XXX definitions to the following
// BOARDTYPE_XXX definitions used in the application space
#define BOARDTYPE_UNKNOWN       -1
#define BOARDTYPE_TX            11
#define BOARDTYPE_RX            12

#define CODEC_VERSION_UNKNOWN   0
#define CODEC_VERSION_STANDARD  1
#define CODEC_VERSION_SCALABLE  2

#define HARDWARE_VERSION_A      0
#define HARDWARE_VERSION_DA     1

#define HARDWARE_VERSION_PLX_MASK 0x10
#define HARDWARE_VERSION_PLX_V1   0x11
#define HARDWARE_VERSION_PLX_V2   0x12

/***************************************************
 * V2D interface types                             *
 ***************************************************/
#define AUTO_INTERFACE      0

#define INTERFACE_DIGITAL   0x0001
#define INTERFACE_HV        0x0002
#define INTERFACE_SOG       0x0004
#define INTERFACE_COMPOSITE 0x0008
#define INTERFACE_SRCMASK   (INTERFACE_DIGITAL|INTERFACE_HV|\
                            INTERFACE_SOG|INTERFACE_COMPOSITE)
#define INTERFACE_48BIT     0x0010
#define INTERFACE_DUALLINK  0x0020

// Frequency threshold to switch to 48 bit interface for DVI
#define PCR_48BIT_DVI       100 

// Frequency threshold to switch to duallink interface for DVI 
#define PCR_DUALLINK_DVI    165

// Frequency threshold to switch to 48 bit interface for Analog 
#define PCR_48BIT_ANALOG    210
#ifdef SHANKER
Translate the old V2D interface types
#endif

/**********************************************************************
 *  All the supported vendor id and device id                         *
 **********************************************************************/
#define V2DI50_VENDOR   0x10EE
#define V2DI50_DEVICE   0x300
#define PLX_VENDOR      0x10B5
#define PLX_DEVICE      0x5601
#define XPI_VENDOR	    0x11a4
#define XPI_DEVICE	    0x0058

/*********************************************************************
 * XP200 DDS and I2C base addresses 
 *********************************************************************/
#define DDS_BASE                0x300000
#define I2C_BASE                0x200000

/*********************************************************************
 * register offsets (all registers are 16 bits)
 *********************************************************************/
#define FPGA_IDREG          0x0000  /* idenification register */
#define FPGA_ISREG          0x0004  /* interrupt status */
#define FPGA_IMREG          0x0008  /* interrupt mask */
#define FPGA_LEDREG         0x000C  /* LEDs */
#define FPGA_PFIFOREG       0x0010  /* Serial FIFO programming interface */
#define FPGA_PADCREG        0x0014  /* Serial ADC programming interface */
#define FPGA_PDDSREG        0x0018  /* DDS freq synth programming interface */
#define FPGA_PRAMREG        0x001c  /* SDRAM programming i/f  */
#define FPGA_CSREG          0x0020  /* Common status          */
#define FPGA_CDCMRREG       0x0024  /* DCM reset  */
#define FPGA_MKCREG         0x0030  /* Mouse Keyboard config */
#define FPGA_ACNOISET       0x0034  /* AC Noise Threshold */
#define FPGA_XPI_BW_REG     0x0470  /* XPi bandwidth register */

// Audio Mixer related registers Shanker 10/27/2009
#define FPGA_AUDIOALPHA1    0x0144  /* Audio Alpha 1 */
#define FPGA_AUDIOALPHA1C   0x0148  /* One minus Audio Alpha 1 */
#define FPGA_AUDIOALPHA2    0x014c  /* Audio Alpha 2 */
#define FPGA_AUDIOALPHA2C   0x0150  /* One minus Audio Alpha 2 */

#define FPGA_PDDCREG        0x0038  /* Serial DDC/EDID programming interface */
#define FPGA_TSTREG         0x00c0  /* Test register */
#define FPGA_TIMESTAMP_L    0x0240  /* FPGA build date lower 16 bits */
#define FPGA_TIMESTAMP_U    0x0244  /* FPGA build date upper 16 bits */
#define FPGA_DCNOISET       0x0248  /* DC Noise Threshold */
#define FPGA_AUDSTSREG      0x0274  /* DAC LRCLK status */ 
#define FPGA_FRMCNT_REG     0x0374  /* FPGA frame counter fifo register */
#define FPGA_FRMCNT_LVL     0x037c  /* FPGA frame counter fifo level */
#define FPGA_I2CDDSREG      0x03c0  /* FPGA I2C & DDS control register */
#define FPGA_KBDOUT_DATA    0x03c4  /* FPGA keyboard data output */
#define FPGA_MSEOUT_DATA    0x03c8  /* FPGA mouse data output */
#define FPGA_KBDIN_DATA     0x03cc  /* FPGA keyboard data input */
#define FPGA_MSEIN_DATA     0x03d0  /* FPGA mouse data input */
#define FPGA_KMSEL_REG      0x03dc  /* 0 - TX KBM, 1 - RX KBM, 10 - test mode */
#define FPGA_MKCREG_Y       0x03e0  /* KBM config for Y mouse */
#define FPGA_KBDOUT_DATA_Y  0x03e4  /* Keyboard Y-mouse data output */
#define FPGA_MSEOUT_DATA_Y  0x03e8  /* Mouse Y-mouse data output */
#define FPGA_REG400         0x0400  /* Mode register for single/dual link */
#define FPGA_VIDEOHITHRES   0x0580  /* Interrupt if video FIFO > this */
#define FPGA_AUDIOHITHRES   0x0588  /* Interrupt if audio FIFO > this */
#define FPGA_VIDEOLOTHRES   0x0594  /* Interrupt if video FIFO < this */
#define FPGA_AUDIOLOTHRES   0x059c  /* Interrupt if audio FIFO < this */
#define FPGA_CPLDIDREG      0x0600  /* FPGA CPLD ID register */
#define FPGA_XP200LEDREG    0x0700  /* XP200 LED register */

/***************************************************
 * Frame counter register masks                    *
 ***************************************************/
#define FRMCNT_DUMP_BIT                 0x01
#define FRMCNT_READ_BIT                 0x04
#define FRMCNT_RESET_BIT                0x08

/***************************************************  
 * Drop override control register masks            *
 ***************************************************/
#define DOVCONTROL_ENABLE       0x100

/***************************************************
 * FPGA Board Status                               *
 ***************************************************/
#define FPGA_BOARD_INITIALIZED_BIT      0
#define FPGA_BOARD_IDLE_BIT             1
#define FPGA_BOARD_I2C_BUSY_BIT         2
#define FPGA_BOARD_PLX_INITIALIZED_BIT  3
#define FPGA_BOARD_ADDSIDELUT_BUSY_BIT  4
#define FPGA_BOARD_DROPSIDELUT_BUSY_BIT 5
#define FPGA_BOARD_DISPLAYLUT_BUSY_BIT  6
#define FPGA_BOARD_RDPIXEL_BUSY_BIT     7

#define FPGA_BOARD_INITIALIZED          (1 << FPGA_BOARD_INITIALIZED_BIT)
#define FPGA_BOARD_IDLE                 (1 << FPGA_BOARD_IDLE_BIT)
#define FPGA_BOARD_I2C_BUSY             (1 << FPGA_BOARD_I2C_BUSY_BIT)
#define FPGA_BOARD_PLX_INITIALIZED      (1 << FPGA_BOARD_PLX_INITIALIZED_BIT)
#define FPGA_BOARD_ADDSIDELUT_BUSY      (1 << FPGA_BOARD_ADDSIDELUT_BUSY_BIT)
#define FPGA_BOARD_DROPSIDELUT_BUSY     (1 << FPGA_BOARD_DROPSIDELUT_BUSY_BIT)

/*********************************************************************
 * I2C reset register bit positions                                  *
 *********************************************************************/
#define PADCREG_PCA9545_RST     0x40    /* toggle 0 - 1 to reset the */
                                        /* Main PCA9545 I2C switch   */
#define PADCREG_PM_I2C_SW_RST   0x80    /* toggle 0 - 1 to reset the */
                                        /* PM PCA9545 I2C switch     */

/*********************************************************************
 * Add side registers
 *********************************************************************/
#define FPGA_ACREG          0x0040  /* Add config */
#define FPGA_ASREG          0x0044  /* Add status */
#define FPGA_AVHAREG        0x0048  /* Add Horizontal active */
#define FPGA_AVVAREG        0x004c  /* Add Vertical active   */
#define FPGA_AVHDREG        0x0050  /* Add Horizontal delay  */
#define FPGA_AVVDREG        0x0054  /* Add Vertical delay    */
#define FPGA_AAFDREG        0x0058  /* Add audio FIFO delay  */
#define FPGA_ALFLREG        0x00ac  /* Add LAN FIFO delay    V2 */
#define FPGA_AMHREG         0x0060  /* Measure Horizontal  freq  */
#define FPGA_AMVREG         0x0064  /* Measure Vertical  freq    */
#define FPGA_AMTREG         0x0068  /* Measure input transitions  */
#define FPGA_ALUTREG        0x006c  /* Video LUT value          */
#define FPGA_AMLREG         0x0070
#define FPGA_ANOBREG        0x0074  /* Add No. of Blocks per slice */
#define FPGA_ANSLICEREG     0x0078  /* Add No. of Slices per frame */
#define FPGA_AQUANT         0x007c  /* Add Quantization/Slice drop */
#define FPGA_ATESTREG       0x00c0  /* Add side test register */
#define FPGA_AFRMCNTREG     0x0100  /* Frame count statistic register */
#define FPGA_ASLICECNTREG   0x0104  /* Slice count statistic register */
#define FPGA_AISLICEREG     0x0108  /* Islice Register stores progressive */
                                    /* compression parameters */
#define FPGA_AXBLOCKREG     0x010c  /* Threshold of number of blocks per    */
                                    /* slice beyond which I-slice is forced */
#define FPGA_AAUDCREG       0x024c  /* Addside audio control register */
#define FPGA_REG250         0x0250  /* FPGA debug TX frame count register */ 
#define FPGA_REG254         0x0254  /* FPGA debug TX slice count register */
#define FPGA_REG258         0x0258  /* FPGA debug RX frame count register */
#define FPGA_REG25C         0x025c  /* FPGA debug RX slice count register */
#define FPGA_REG260         0x0260  /* FPGA debug RX error count 1 */
#define FPGA_REG264         0x0264  /* FPGA debug RX error count 2 */
#define FPGA_REG268         0x0268  /* FPGA debug RX error count 3 */
#define FPGA_AAUDSTSREG     0x0274  /* Add Side audio status register*/
#define FPGA_VIDEOINPUT     0x0278  /* FPGA video input selection register */

#define FPGA_ARSLICEREG     0x02a0  /* Addside refresh slices per frame */
#define FPGA_PIXEXTRA       0x02a8  /* Extra pixels to read at EOF */
#define FPGA_FRAMECTLWD     0x02ac  /* Frame control word.               */
                                    /* Bits 0-2: left SOF (default 100)  */ 
                                    /* Bits 3-5: right SOF (default 001) */ 
#define FPGA_AALHDELAY      0x0380  /* Addside Analog Loopback horiz delay */   
#define FPGA_AALVDELAY      0x0384  /* Addside Analog Loopback vert delay */    
#define FPGA_SOF_DELAY      0x03a0  /* Start-of-Frame delay register */
#define FPGA_ASKIPCNT       0x03a4  /* Addside frame skip count register */
#define FPGA_AVVD_DELAY     0x03a8  /* Addside initial delay before starting */
                                    /* the vertical delay counter (AVVDREG) */
#define FPGA_ASKIPCNTBITS   0x03ac  /* Number of bits in frame skip pattern */
#define FPGA_EDID_START     0x0400  /* EDID address 0 */
#define FPGA_ARESETREG      0x05a8  /* TX FPGA soft reset register */
#define FPGA_IFRM_CLKGCNT   0x0690  /* I-FRAME clkg count */
#define FPGA_IFRM_FRMCNT    0x0694  /* I-FRAME frame count */
#define FPGA_IFRM_CTLSTAT   0x0698  /* Bit 0 = 1 -> set I-FRAME count valid */
#define FPGA_IFRM_SELECT    0x069c  /* Selects clkg or frame based count */
#define FPGA_IFRM_DEBUG     0x06a4  /* I-FRAME debug counter */
#define FPGA_IFRM_RESET     0x06a8  /* I-FRAME reset register */
#define FPGA_XPI_BWCNT      0x0704  /* FPGA bandwidth control counter */

#define FPGA_ALATCONTROL    0x0714  /* Add Latency feature control register */
#define FPGA_ALATTHRES0     0x0718  /* Add Latency feature bank 0 threshold */
#define FPGA_ALATTHRES1     0x071c  /* Add Latency feature bank 1 threshold */
#define FPGA_ALATTHRES2     0x0720  /* Add Latency feature bank 2 threshold */
#define FPGA_ALATSLICE0     0x0724  /* Add Latency feature (0 fill) slice 0 */
#define FPGA_ALATSLICE1     0x0728  /* Add Latency feature (0 fill) slice 1 */
#define FPGA_ALATSLICE2     0x072c  /* Add Latency feature (0 fill) slice 2 */
#define FPGA_ALATSLICE3     0x0730  /* Add Latency feature (0 fill) slice 3 */
#define FPGA_ALATSLICE4     0x0734  /* Add Latency feature (0 fill) slice 4 */
#define FPGA_ALATSLICE5     0x0738  /* Add Latency feature (0 fill) slice 5 */
#define FPGA_AMDVIHREG      0x0160  /* Measure horiz freq for DVI input */
#define FPGA_AMDVIVREG      0x0164  /* Measure vertl freq for DVI input */
#define FPGA_AMDVITREG      0x0168  /* Measure input transition for DVI input */
#define FPGA_AMDVILREG      0x016c  /* Total lines in frame for DVI input */
#define FPGA_AAVHAREG       0x079c  /* Add Actual Horizontal active */
#define FPGA_AAVVAREG       0x07a0  /* Add Actual Vertical active   */
#define FPGA_AAVHDREG       0x0798  /* Add Actual Horizontal delay  */
#define FPGA_AAVVDREG       0x07a4  /* Add Actual Vertical delay    */
#define FPGA_AMLTHRES       0x0170  /* Threshold for line change to trigger */
#define FPGA_ADCHPOLREG     0x07d0  /* 1 if ADC hsync polarity is active low */
#define FPGA_ADCVPOLREG     0x07d4  /* 1 if ADC vsync polarity is active low */
#define FPGA_DVIHPOLREG     0x07e8  /* 1 if DVI hsync polarity is active low */
#define FPGA_DVIVPOLREG     0x07ec  /* 1 if DVI vsync polarity is active low */


/*********************************************************************
 * Drop side registers
 **********************************************************************/
#define FPGA_DCREG              0x0080  /* Drop config */
#define FPGA_DSREG              0x0084  /* Drop status */
#define FPGA_DVHAREG            0x0088  /* Drop horizontal active size */
#define FPGA_DVHFPREG           0x008c  /* Drop horizontal front porch */
#define FPGA_DVHSREG            0x0090  /* Drop horizontal sync width  */
#define FPGA_DVHBPREG           0x0094  /* Drop horizontal back porch  */
#define FPGA_DVVAREG            0x0098  /* Drop vertical active size   */
#define FPGA_DVVFPREG           0x009c  /* Drop vertical front porch   */
#define FPGA_DVVSREG            0x00a0  /* Drop vertical sync width    */
#define FPGA_DVVBPREG           0x00a4  /* Drop vertical back porch    */
#define FPGA_DAFDREG            0x00a8  /* Drop audio FIFO delay       */
#define FPGA_DLFLREG            0x005c  /* Drop LAN FIFO delay V2      */
#define FPGA_DLUTREG            0x00b0  /* Drop recovered LUT          */
#define FPGA_DTSREG             0x00b4  /* Timecode short term err     */
#define FPGA_DTLREG             0x00b8  /* Timecode long term err      */
#define FPGA_DVBWREG            0x00bc  /* Drop video buffer           */
#define FPGA_DNOBREG            0x00c4  /* Drop No. of Blocks per slice */
#define FPGA_DNSLICEREG         0x00c8  /* Drop No. of Slices per frame */
#define FPGA_DQUANT             0x00cc  /* Drop Quantization/Slice drop */
#define FPGA_SLICE_WIDTH        0x00d0  /* Slice Width in Blocks*/
#define FPGA_SLICE_HEIGHT       0x00d4  /* Slice Height in Blocks */
#define FPGA_DOSDMMENABLE       0x0100  /* Bit 5 = 1 -> MM OSD enable */
#define FPGA_DOSDMMCTL0         0x0104  /* Multi-mouse cursor control 0 */
#define FPGA_DOSDMMCTL1         0x0108  /* Multi-mouse cursor control 1 */
#define FPGA_DOSDMMCTL2         0x010c  /* Multi-mouse cursor control 2 */
#define FPGA_DOSDMMCTL3         0x0110  /* Multi-mouse cursor control 3 */
#define FPGA_DOSDMMCTL4         0x0114  /* Multi-mouse cursor control 4 */
#define FPGA_DOSDMMCTL5         0x0118  /* Multi-mouse cursor control 5 */
#define FPGA_DOSDMMCTL6         0x011c  /* Multi-mouse cursor control 6 */
#define FPGA_DOSDMMCTL7         0x0120  /* Multi-mouse cursor control 7 */
#define FPGA_DOSDMMCTL8         0x0124  /* Multi-mouse cursor control 8 */
#define FPGA_DOSDMMCTL9         0x0128  /* Multi-mouse cursor control 9 */
#define FPGA_DOSDMMCTL10        0x012c  /* Multi-mouse cursor control 10 */
#define FPGA_DOSDMMCTL11        0x0130  /* Multi-mouse cursor control 11 */
#define FPGA_DOSDMMCTL12        0x0134  /* Multi-mouse cursor control 12 */
#define FPGA_DOSDMMCTL13        0x0138  /* Multi-mouse cursor control 13 */
#define FPGA_DOSDMMCTL14        0x013c  /* Multi-mouse cursor control 14 */
#define FPGA_DOSDMMCTL15        0x0140  /* Multi-mouse cursor control 15 */
#define FPGA_DOSD1STARTX        0x0140  /* Drop OSD1 x start */
#define FPGA_DOSD1STOPX         0x0144  /* Drop OSD1 x sop */
#define FPGA_DOSD1STARTY        0x0148  /* Drop OSD1 y start */
#define FPGA_DOSD1STOPY         0x014c  /* Drop OSD1 y sop */
#define FPGA_DOSD1COLOR0        0x0150  /* Drop OSD1 color register 0 */
#define FPGA_DOSD1COLOR1        0x0154  /* Drop OSD1 color register 1 */
#define FPGA_DOSD1COLOR2        0x0158  /* Drop OSD1 color register 2 */
#define FPGA_DOSD1CTL           0x015c  /* Drop OSD1 control register */
#define FPGA_DOSD2STARTY        0x0174  /* Drop OSD2 y start */
#define FPGA_DOSD2STOPY         0x0178  /* Drop OSD2 y stop */
#define FPGA_DOSD2COLOR0        0x017c  /* Drop OSD2 color register 0 */
#define FPGA_DOSD2COLOR1        0x0180  /* Drop OSD2 color register 1 */
#define FPGA_DOSD2COLOR2        0x0184  /* Drop OSD2 color register 2 */
#define FPGA_DOSD2CTL           0x0188  /* Drop OSD1 control register */
#define FPGA_DOSD2NUMLOCS       0x018c  /* # 32 bit locs programmed by s/w */
#define FPGA_DHSHIFT            0x0198  /* Dropside horiz shift */
#define FPGA_DVSHIFT            0x019c  /* Dropside vert shift */
#define FPGA_DSRCHRES           0x01a0  /* Dropside override source HRES */
#define FPGA_DSRCVRES           0x01a4  /* Dropside override source VRES */
#define FPGA_DOVCONTROL         0x01a8  /* Dropside override control register */
#define FPGA_DHSCALE            0x01ac  /* Dropside horiz scale register */
#define FPGA_DVSCALE            0x01b0  /* Dropside vert scale register */
#define FPGA_DNHRES             0x01b4  /* Dropside new horiz res register */
#define FPGA_DNVRES             0x01b8  /* Dropside new vert res register */
#define FPGA_DSCALECTL          0x01bc  /* Dropside scaling control register */
#define FPGA_DOSD1NUMLOCS       0x01c0  /* # 32 bit locs programmed by s/w */
#define FPGA_DOSD2STARTX        0x01cc  /* Drop OSD2 y start */
#define FPGA_DOSD2STOPX         0x01d0  /* Drop OSD2 y stop */
#define FPGA_DOVLPALPHA1        0x01d4  /* Dropside overlap area 1 alpha */
#define FPGA_DOVLPALPHA2        0x01d8  /* Dropside overlap area 2 alpha */
#define FPGA_DNOVLPALPHA1       0x01dc  /* Dropside non-overlap area 1 alpha */
#define FPGA_DNOVLPALPHA2       0x01e0  /* Dropside non-overlap area 2 alpha */
#define FPGA_DALPHAM1_R         0x01e4  /* ALPHAM1 Remote */
#define FPGA_DALPHAM1_L         0x01e8  /* ALPHAM1 Local */
#define FPGA_DALPHAM2_R         0x01ec  /* ALPHAM2 Remote */
#define FPGA_DALPHAM2_L         0x01f0  /* ALPHAM2 Local */
#define FPGA_DFRMCNTREG         0x0200  /* Frame count statistic register */
#define FPGA_DSLICECNTREG       0x0204  /* Slice count statistic register */
#define FPGA_DTHRESM1_L         0x022c  /* Remote matting low threshold */
#define FPGA_DTHRESM1_H         0x0230  /* Remote matting high threshold */
#define FPGA_DTHRESM2_L         0x0234  /* Local matting low threshold */
#define FPGA_DTHRESM2_H         0x0238  /* Local matting high threshold */
#define FPGA_DFRMCNTREG2        0x0258  /* Frame count (counts all SOFs) */
#define FPGA_DSLICECNTREG2      0x025c  /* Slice count (counts all SOSs) */
#define FPGA_DERRORCNT1         0x0260  /* Frame/slice error count 1 */
#define FPGA_DERRORCNT2         0x0264  /* Frame/slice error count 2 */
#define FPGA_DERRORCNT3         0x0268  /* Frame/slice error count 3 */
#define FPGA_DGLKSTATUS         0x0278  /* Drop Side Genlock clock status */
#define FPGA_DAUDCREG           0x027c  /* Drop Side audio control register */
#define FPGA_DGLKSTATUS2        0x0280  /* Drop Side Genlock clock status 2 */
#define FPGA_DNAUDBFRS          0x02cc  /* Drop No. used Audio Buffers (words)*/
#define FPGA_DRESETREG          0x0458  /* Setting to 1 resets dropside FPGA */
#define FPGA_RDPIXELLOC         0x0480  /* Pixel location to be read */
#define FPGA_RPIXINEVEN         0x0484  /* Remote Pixel In Even */
#define FPGA_RPIXINODD          0x0488  /* Remote Pixel In Odd */
#define FPGA_LPIXINEVEN         0x048c  /* Local Pixel In Even */
#define FPGA_LPIXINODD          0x0490  /* Local Pixel In Odd */
#define FPGA_PIXOUTEVEN         0x0494  /* Pixel Out Even */
#define FPGA_PIXOUTODD          0x0498  /* Pixel Out Odd */
#define FPGA_DDR2THRES0         0x0718  /* DDR2 bank toggling threshold 0 */
#define FPGA_DDR2THRES1         0x071c  /* DDR2 bank toggling threshold 1 */
#define FPGA_DDR2THRES2         0x0720  /* DDR2 bank toggling threshold 2 */
#define FPGA_DDR2RDSTART        0x0750  /* Start read addr frame buffer DDR2 */
#define FPGA_DDR2RDEND          0x0754  /* End read addr frame buffer DDR2 */
#define FPGA_DCROPVRES          0x0758  /* Dropside VRES after scale & crop */
#define FPGA_DCROPHSTART        0x075c  /* Horiz crop line start address */
#define FPGA_DCROPHEND          0x0760  /* Horiz crop line end address */
#define FPGA_DCROPHRES          0x0764  /* Dropside HRES after scale & crop */
#define FPGA_DCROPHLEFT         0x0768  /* # Pixels to crop at left of line */
#define FPGA_DCROPHRIGHT        0x076c  /* # Pixels to crop at right of line */

/*********************************
 * V2DI50 DMA specific registers *
 *********************************/
#define FPGA_DMA_COMMAND        0x00d0
#define FPGA_DMA_ADDRHIGH       0x00d4
#define FPGA_DMA_ADDRLOW        0x00d8
#define FPGA_DMA_WLEN           0x00dc

#define DMA_REQLEN_MASK         0x0FFF
#define DMA_DEVTYPE_BIT         0x8000
#define DMA_COMMAND_GO          0x8000

/*************************************
 * DMA specific flags used by V4 PLX *
 *************************************/
#define DMA_STATE_CLOSED        0
#define DMA_STATE_BLOCK         1
#define DMA_STATE_SGL           2

#define DMA_FLAG_VIDEO_READ     0x0001
#define DMA_FLAG_AUDIO_READ     0x0002
#define DMA_FLAG_READ           (DMA_FLAG_VIDEO_READ |  \
                                 DMA_FLAG_AUDIO_READ)
#define DMA_FLAG_VIDEO_WRITE    0x0004
#define DMA_FLAG_AUDIO_WRITE    0x0008
#define DMA_FLAG_WRITE          (DMA_FLAG_VIDEO_WRITE | \
                                 DMA_FLAG_AUDIO_WRITE)

/******************************************
 * DMA specific registers  used by V4 PLX *
 ******************************************/
#define DMA_DIR_TO_LOCAL        0
#define DMA_DIR_FROM_LOCAL      1
#define MAX_DMA_CHANNELS        2
#define DMA_RCHAN       0   /* DMA read channel */
#define DMA_WCHAN       1   /* DMA write channel */


#define DSCALE_COEFF_RAM_ADDR       0x24000

/***************************************************
 * FPGA addside configuration flag bits            *
 ***************************************************/
#define AVFLAG_DVIAUTO          0x0001
#define AVFLAG_LATENCY          0x0002
#define AVFLAG_CONNECTIONLED    0x0010
#define AVFLAG_VIDEOLED         0x0020
#define AVFLAG_LOCALRESET       0x0100
#define AVFLAG_MOTION444        0x0200
#define AVFLAG_GENLOCKMASTER    0x0400
#define AVFLAG_GENLOCKSLAVE     0x0800

/*********************************************************************
 * Add config register bit positions
 **********************************************************************/

#define ACREG_ADD_ENABLE        0x01          /* Add side master enable */
#define ACREG_VID_ENABLE        0x02          /* Video enable           */
#define ACREG_AUD_ENABLE        0x04          /* Audio enable           */
#define ACREG_LAN_ENABLE        0x08          /* LAN enable             */
#define ACREG_STEREO            0x10          /* Stereo video           */
#define ACREG_IFRAME            0x20          /* LAN enable             */
#define ACREG_FIFO_XCLK         0x40          /* FIFO xfer clock select */
#define ACREG_MOTION_444_SEL    0x4000        /* Motion 444 compression */
#define ACREG_444_SEL           0x8000        /* 444 Compression        */

/*********************************************************************
 * Drop config register bit positions
 **********************************************************************/

#define DCREG_DROP_ENABLE       0x01          /* Drop side master enable */
#define DCREG_VID_ENABLE        0x02          /* Video enable            */
#define DCREG_AUD_ENABLE        0x04          /* Audio enable            */
#define DCREG_LAN_ENABLE        0x08          /* LAN enable              */
#define DCREG_STEREO            0x10          /* Stereo video            */
#define DCREG_VID_DAC_ENABLE    0x20          /* 0 - disable, 1 - enable */
#define DCREG_HSYNC_POL         0x40          /* 0 - pos, 1 - neg        */
#define DCREG_VSYNC_POL         0x80          /* 0 - pos, 1 - neg        */
#define DCREG_LRSYNC_POL        0x100         /* LRsync polarity         */
#define DCREG_GENLOCK_MODE      0x400         /* XP: 1 - genlock slave   */
                                              /*     0 - genlock master  */
#define DCREG_WAIT_FOR_SKIP     0x800         /* (I50 only)              */
#define DCREG_LRSYNC_DISABLE    0x800         /* On XP 1 to disable LRSYN*/
#define DCREG_VID_BYPASS        0x2000        /* Video Bypass (I50 only) */
#define DCREG_FRLOCK_EXT        0x4000        /* I50: 1 - genlock slave  */
                                              /*      0 - genlock master */
#define DCREG_444_SEL           0x8000        /* Enable 444 Compression  */

/******************************
 * AMT Register bit positions *
 ******************************/
#define AMTREG_VSYNC            0x01
#define AMTREG_HSYNC            0x02
#define AMTREG_HVSYNC           (AMTREG_HSYNC|AMTREG_VSYNC)
#define AMTREG_STEREO           0x04

/*********************************************************************
 * FIFO SPI programming register bit positions
 *
 * The SEN bits are inverted before driving the FIFO pins, so a '1' 
 * enables a FIFO for writing (active high to FW)
 * All the FIFO 1 colors and all the FIFO 2 colors can be enabled and
 * written at the same time to reduce write time.
 **********************************************************************/

#define PFIFOREG_SCLK     0x01          /* serial clock              */
#define PFIFOREG_SDAT     0x02          /* serial write data         */
#define PFIFOREG_SEN_0    0x04          /* red fifo 1 enable         */
#define PFIFOREG_SEN_1    0x08          /* red fifo 2 enable         */
#define PFIFOREG_SEN_2    0x10          /* green fifo 1 enable       */
#define PFIFOREG_SEN_3    0x20          /* green fifo 2 enable       */
#define PFIFOREG_SEN_4    0x40          /* blue fifo 1 enable        */
#define PFIFOREG_SEN_5    0x80          /* blue fifo 2 enable        */

/*********************************************************************
 * ADC SPI programming register bit positions
 *
 * The PADCREG_SDAT_EN bit enables the FPGA's data output driver.
 * This bit must be '0' during read operations, and should be '0'
 * anytime the output operations are not occuring. The hardware
 * could be harmed if left on.
 **********************************************************************/

#define PADCREG_SCLK      0x01          /* serial clock              */
#define PADCREG_SDAT_O    0x02          /* write data                */
#define PADCREG_SDAT_I    0x04          /* read data                 */
#define PADCREG_SDAT_EN   0x08          /* data output enable        */
#define PADCREG_SADR      0x10          /* address                   */

#define PADCREG_SCLK2     0x01          /* serial clock              */
#define PADCREG_SDAT2_O   0x02          /* write data                */
#define PADCREG_SDAT2_I   0x04          /* read data                 */
#define PADCREG_SDAT2_EN  0x08          /* data output enable        */
#define PADCREG_SADR21    0x10          /* address                   */
#define PADCREG_SADR22    0x10          /* address                   */

/*********************************************************************
 * EDID Data Size and EDID Offsets
 **********************************************************************/
#define EDID_DATA_LEN           128   
#define DDC_EDID_DATA_START     0x0   
#define I50_EDID_DATA_START     0x400   

/*********************************************************************
 * DDC EDID SPI programming register bit positions
 **********************************************************************/

#define PDDCREG_SCLK      0x01          /* serial clock              */
#define PDDCREG_SDAT_O    0x02          /* write data                */
#define PDDCREG_SDAT_I    0x04          /* read data                 */
#define PDDCREG_SDAT_EN   0x08          /* data output enable        */

/*********************************************************************
 * Video DDS programming register bit positions
 **********************************************************************/

#define PDDSREG_UDCK      0x01          /* update clock              */
#define PDDSREG_HOLD      0x02          /* hold                      */
#define PDDSREG_SHPD      0x04          /* shaped keying             */
#define PDDSREG_RESET     0x08          /* reset                     */
#define PDDSREG_P_UDCK    0x10          /* provision update clock    */
#define PDDSREG_PCA9564_RST     0x20    /* toggle 0 - 1 to reset the */
                                        /* PCA9564 I2C controller    */


/*********************************************************************
 * Addside Frame Skip Count register bit positions
 **********************************************************************/
#define ASKIPCNT_ENABLE   0x800         /* 1 to enable frame skip    */ 


/**********************************************************************
 * Addside TEST register (ATESTREG) bit positions                     *
 **********************************************************************/
#define FPGA_DATA_ENABLE  0x00000001        /* 1: select data enable     */
                                            /* from DVI chip             */
#define FPGA_VIDEO_24BIT  0x00000002        /* 1: 24 bit, 0: 48 bit      */
#define LPBK_DATA_ENABLE  0x00000004        /* 0 for loopback to select  */
                                            /* data enable from DVI chip */
#define RXLPBK_CONTROL    0x00000010        /* 1 for local mode, 0 for   */
                                            /* remote or overlay mode    */
#define DVIHPOL_INVERT    0x00000040        /* 1 to enable DVI Vsync     */
                                            /* inversion by the FPGA     */
#define DVIVPOL_INVERT    0x00000080        /* 1 to enable DVI Hsync     */
                                            /* inversion by the FPGA     */
#define EXTSYNC_ENABLE    0x00000100        /* 0 to use external sync    */ 
#define DVIINPUT_ENABLE   0x00000200        /* 1 to select DVI input     */ 
#define RXLPBK_ENABLE     0x00000400        /* 1 to enable RX loopback   */
#define FRAMECNT_ENABLE   0x00000800        /* 1 to enable FPGA frame &  */
                                            /* slice cnts w/o data being */
                                            /* read from the FIFO        */
#define SYNCOFF_ENABLE    0x00001000        /* 1 to turn off sync output */
                                            /* on both DAC and DVI       */
#define DACOFF_ENABLE     0x00002000        /* 1 to turn off sync output */
                                            /* on DAC interface          */
#define VCROP_ENABLE      0x00010000        /* 1 - enable vertical crop  */
#define BUFGMAX_MASK      0x00300000        /* mask for output DVI pixclk*/
                                            /* delay wrt sampling clock  */
#define VIDEO_DUALLINK    0x01000000        /* 1 if output DVI duallink  */
#define RMATTE_ENABLE     0x02000000        /* 1 if remote matting enabled */
#define LMATTE_ENABLE     0x04000000        /* 1 if local matting enabled */
#define HCROP_ENABLE      0x08000000        /* 1 - enable horizontal crop */
#define SOFCHECK_ENABLE   0x20000000        /* 1 to enable checking gap  */
                                            /* between 2 SOFs            */
#define DYNAMIC_FIFO_BIT  0x40000000        /* 1 to turn on using FIFO   */
                                            /* almost full to decide on  */
                                            /* frame drop                */
#define DYNAMIC_DDR2_BIT  0x40000000        /* 1 to turn on using FIFO   */
                                            /* almost full when writing  */
                                            /* to frame buffer DDR2 to   */
                                            /* decide on frame drop      */
/****************************************
 * XP200 LED register values            *
 ****************************************/
#define XP200LEDREG_TX      0x10
#define XP200LEDREG_RX      0x20
#define XP200LEDREG_CONN    0x40

/****************************************
 * DAC input config register masks      *
 ****************************************/
#define AUDCREG_DISABLE         0x0001  /* Bit = 1 -> disable */
#define AUDCREG_SET_LRCLK       0x0002  /* Bit = 1 -> enable LRCLK */
#define AUDCREG_SET_SCLK        0x0004  /* Bit = 1 -> enable SCLK */
#define AUDCREG_SET_LOOPBACK    0x0008  /* Bit = 1 -> enable loopback */
#define AUDCREG_LOOPBACK        0x0040  /* Bit = 1 -> enable loopback */
#define AUDCREG_SAMPLERATE_IN   0x0300  /* 00 - 8KHz,  01 - 16KHz */
                                        /* 10 - 32KHz, 11 - 64KHz */
#define AUDCREG_SAMPLERATE_OUT  0x0c00  /* 00 - 8KHz,  01 - 16KHz */
                                        /* 10 - 32KHz, 11 - 64KHz */
#define DAUDCREG_RESET          0x0200  /* Bit = 1 -> reset */
#define DAUDCREG_LOOPBACK       0x0100  /* Bit = 1 -> turn on loopback */
#define DAUDCREG_AMIXER         0x0080  /* Bit = 1 -> enble addside mixer */
#define DAUDCREG_AFIFOCHK       0x0040  /* Bit = 1 -> enable add fifo check */
#define DAUDCREG_DMIXER         0x0020  /* Bit = 1 -> enble dropside mixer */
#define DAUDCREG_DFIFOCHK       0x0010  /* Bit = 1 -> enable drop fifo check */


/***********************************************
 * DAC LRCLK status register bit positions     *
 ***********************************************/
#define AUDSTSREG_LRCLK_PRESENT         0x01    /* 1 if LRCLK is present */
#define AUDSTSREG_SCLK_PRESENT          0x02    /* 1 if SCLK is present */

/************************************************
 * Video input selection register bit positions *
 ************************************************/
#define VIDEOINPUT_YUVBIT               0x01    /* 1 if YUV */
#define VIDEOINPUT_DVIBIT               0x02    /* 1 enable DVI hotplug detect*/

/***************************************************
 * Addside I-Frame Control Status Register Bits    *
 ***************************************************/
#define IFRM_CTLSTAT_VALID_BIT          0x01

/***************************************************
 * Addside I-Frame Count Select Register Bits      *
 ***************************************************/
#define IFRM_SELECT_CLKG                0x01
#define IFRM_SELECT_FRAME               0x02

/***************************************************
 * Addside I-Frame Count Reset Register Bits       *
 ***************************************************/
#define IFRM_RESET_CLKG                 0x01
#define IFRM_RESET_FRAME                0x02
#define IFRM_RESET_DEBUG                0x04


/************************************************
 * Mouse Keyboard config register bit postiions *
 ************************************************/
#define MKCREG_MOUSE_ENABLE             0x01
#define MKCREG_KEYBOARD_ENABLE          0x02
#define MKCREG_SCANCODE_MASK            0x0C
#define MKCREG_SCANCODE_SHIFT           2

#define MKCREG_XP_MSEOUT_ENABLE         0x01
#define MKCREG_XP_KBDOUT_ENABLE         0x02
#define MKCREG_XP_MSEIN_ENABLE          0x04
#define MKCREG_XP_KBDIN_ENABLE          0x08
#define MKCREG_XP_SCANCODE_MASK         0x10

/************************************************
 * Audio clock select register bit positions    * 
 ************************************************/
#define AUDIOCLKSEL_ENABLE              0x01    /* 1 to enable 8KHz */ 


#define MAX_FPGA_INTERRUPTS     32      /* Number of FPGA interrupts */

/*-----------------------------------------------------------------------------
 * Interrupt status register bit positions
 *-----------------------------------------------------------------------------*/

#define PLX_DMA_1_BIT           (1<<INT_31_PLX_DMA_1)
#define PLX_DMA_0_BIT           (1<<INT_30_PLX_DMA_0)

#define DROP_DLUT_CHANGE    (1<<14)    /* Drop LUT Change */
#define DROP_TIMECODE       (1<<13)    /* Drop timecode received */
#define DROP_VSYNC          (1<<12)    /* Drop vsync             */
#define SDRAM_FATAL         (1<<11)    /* SRAM buffer conflict (fatal error) */
#define DROP_FIFO           (1<<10)    /* Drop LAN FIFO not empty  */
#define DROP_FIFO_FATAL     (1<<9)     /* Drop internal FIFO overflow (fatal error) */
#define DROP_FATAL          (1<<8)     /* Drop DCM unlocked (fatal error) */
#define UART_INTR           (1<<7)     /* UART */

#define ADD_AMXREG_CHANGE   (1<<4)     /* Add AMXREG change  */
#define ADD_AMTREG_CHANGE   (1<<3)     /* Add AMTREG change  */
#define ADD_FIFO            (1<<2)     /* Add LAN FIFO empty  */
#define ADD_FIFO_FATAL      (1<<1)     /* Add internal FIFO overflow (fatal error) */
#define ADD_FATAL           (1)        /* Add DCM unlocked (fatal error) */

#define FATAL_INTR      (SDRAM_FATAL | DROP_FIFO_FATAL | DROP_FATAL | ADD_FIFO_FATAL | ADD_FATAL)
#define V2_INTR_BITS       (FATAL_INTR | ADD_FIFO | UART_INTR | DROP_FIFO | DROP_VSYNC | DROP_TIMECODE | DROP_DLUT_CHANGE  | ADD_AMXREG_CHANGE | ADD_AMTREG_CHANGE)

#define V4_INTR_BITS        (DROP_AUDIO_FIFO_BIT | \
                             DROP_VIDEO_FIFO_BIT | \
                             ADD_AUDIO_FIFO_BIT | \
                             ADD_VIDEO_FIFO_BIT | \
                             MOUSE_INTR_BIT | \
                             KEYBOARD_INTR_BIT)


/*-----------------------------------------------------------------------------
 * Add status register (FPGA_ASREG) bit positions
 *-----------------------------------------------------------------------------*/

#define ASREG_LAN_FIFO_FULL          (1<<13)
#define ASREG_LAN_FIFO_EMPTY         (1<<12)
#define ASREG_AUDIO_FIFO_FULL        (1<<11)
#define ASREG_AUDIO_FIFO_EMPTY       (1<<10)
#define ASREG_VIDEO_FIFO_FULL        (1<<9)
#define ASREG_VIDEO_FIFO_EMPTY       (1<<8)
#define ASREG_XP_MOUSE_DATA_Y        (1<<8) /* 1 if Y-MS FIFO not full on XP */ 
#define ASREG_XP_KEYBOARD_DATA_Y     (1<<7) /* 1 if Y-KB FIFO not full on XP */ 
#define ASREG_XP_MOUSE_DATA          (1<<6) /* 1 if MS FIFO not full on XP */ 
#define ASREG_XP_KEYBOARD_DATA       (1<<5) /* 1 if KB FIFO not full on XP */ 
#define ASREG_KBM_KEYBOARD_DATA      (1<<3)
#define ASREG_KBM_MOUSE_DATA         (1<<2)
#define ASREG_VIDEO_CLOCK_DCM_LOCKED (1<<1)
#define ASREG_SONET_CLOCK_DCM_LOCKED (1<<0)

/*-----------------------------------------------------------------------------
 * Drop status register (FPGA_DSREG) Masks
 *-----------------------------------------------------------------------------*/

#define DSREG_PACKER_FIFO_FULL  (1<<15) /* Drop unpacker FIFO full/overflow */
#define DSREG_PACKER_FIFO_EMPTY (1<<14) /* Drop unpacker FIFO empty */
#define DSREG_LAN_FIFO_FULL     (1<<13) /* Drop LAN FIFO full/overflow */
#define DSREG_LAN_FIFO_EMPTY    (1<<12) /* Drop LAN FIFO empty */
#define DSREG_AUDIO_FIFO_FULL   (1<<11) /* Drop audio FIFO full/overflow */
#define DSREG_AUDIO_FIFO_EMPTY  (1<<10) /* Drop audio FIFO empty/underflow */
#define DSREG_VIDEO_FIFO_FULL   (1<<9)  /* Drop video FIFO full/overflow */
#define DSREG_VIDEO_FIFO_EMPTY  (1<<8)  /* Drop video FIFO empty/underflow */
#define DSREG_SRAM_CONFLICT     (1<<7)  /* SRAM buffer conflict */
#define DSREG_XP_MOUSE_DATA     (1<<6)  /* 1 if MS FIFO has data on XP */ 
#define DSREG_XP_KEYBOARD_DATA  (1<<5)  /* 1 if KB FIFO has data on XP */ 
#define DSREG_FRAME_DETECT      (1<<4)  /* Frames detected. */
#define DSREG_KBM_KEYBOARD_DATA (1<<3)  /* Keyboard data ready to be written */
#define DSREG_KBM_MOUSE_DATA    (1<<2)  /* Mouse data ready to be written */
                                        /* Warning OverLoaded bit !!!! */
#define DSREG_SRAM2_CLOCK_DCM_LOCKED    (1<<3)  /* Drop 2nd SRAM clock DCM */
                                                /* is locked */
#define DSREG_SRAM_CLOCK_DCM_LOCKED     (1<<2)  /* Drop SRAM clock DCM */
                                                /* is locked */
#define DSREG_VIDEO_CLOCK_DCM_LOCKED    (1<<1)  /* Drop Video clock DCM */
                                                /* is locked */
#define DSREG_SONET_CLOCK_DCM_LOCKED    (1<<0)  /* Drop side SONET DCM */
                                                /* is locked */

/*-----------------------------------------------------------------------------
 * Common status register (FPGA_CSREG) Masks
 *-----------------------------------------------------------------------------*/

#define CSREG_MAX80_LOL         (1<<9)  /* External Max 80 Loss of Lock */
#define CSREG_MAX90_SOS         (1<<7)  /* External Max 90 SOS */
#define CSREG_SLB_ENABLE        (1<<6)  /* External SLB Enable */
#define CSREG_CLKMUL_LOL        (1<<5)  /* External Clk Multiplier Loss Lock */
#define CSREG_3P3_GOOD          (1<<4)  /* External 3.3 power good */
#define CSREG_PLL_LOWHAT        (1<<1)  /* External 8KHz PLL loss of ? */
#define CSREG_PLL_LOL           (1)     /* External 8KHz PLL loss of lock */

/*************************
 * Quant registers Masks *
 *************************/

#define DCQUANT_LOW_MASK        0x0f
#define DCQUANT_LOW_SHIFT       (0)
#define MQUANT_LOW_MASK         0xf0
#define MQUANT_LOW_SHIFT        (4)

#define DCQUANT_MASK            0xf00
#define DCQUANT_SHIFT           (8)
#define MQUANT_MASK             0xf000
#define MQUANT_SHIFT            (12)

/* Mask and shift for how coarse and fine noise values are
   packed into the FPGA_DCNOISET and FPGA_ACNOISE registers */
#define DCNOISE_MASK            0x00FF
#define DCNOISE_SHIFT           (0)
#define ACNOISE_MASK            0xFFF0
#define ACNOISE_SHIFT           (4)

/* Mask and shift for how coarse noise value is packed into
   FPGA_ACNOISE register for old FPGA (TX version 0xA201) */
#define OLD_DCNOISE_MASK         0x000F
#define OLD_DCNOISE_SHIFT        (0)

/*-----------------------------------------------------------
 * Identification register Masks
 * ----------------------------------------------------------*/

#define CODECVERSION_MASK   0x00ff0000
#define CODECVERSION_SHIFT  (16)


/*-----------------------------------------------------------------------------
 * DCM Reset register (FPGA_CDCMRREG) bit positions
 *                                                 
 * Bits autoclear after a write to bits cause DCM reset
 *-----------------------------------------------------------------------------*/   


#define CDCMRREG_ADD_RESET_VIDEO_DCM    (1<<5)  /* Reset Add side video clock DCM */
#define CDCMRREG_ADD_RESET_SONET_DCM    (1<<4)  /* Reset Add side telecom bus clock DCM */
#define CDCMRREG_DROP_RESET_SRAM2_DCM   (1<<2)  /* Reset 2nd Drop side SRAM DCM */
#define CDCMRREG_DROP_RESET_VIDEO_DCM   (1<<1)  /* Reset Drop side video clock DCMs */
#define CDCMRREG_DROP_RESET_SONET_DCM   (1)     /* Reset Drop side telecom bus clock DCM */

/*-----------------------------------------------------------------------------
 * Interrupts:   (FPGA_ISREG) bit positions
 *---------------------------------------------------------------------------*/

#define INT_0_ADD_DCM_OOL               0       /* Add Side DCM's Out Of Lock */
#define INT_1_ADD_FIFO_OF               1       /* Internal FIFO Overflow */
#define INT_2_ADD_FIFO_EMPTY            2       /* Add Side LAN fifo empty */
#define INT_3_ADD_VHLSYNC_ACT_CHANGE    3       /* V/H/L Sync activity change*/
                                                /* AM TReg changed*/
#define INT_4_ADD_VHSYNC_FREQ_CHANGE    4       /* V/H Sync Frequency change */
                                                /* AM H, V, or T Reg change  */
#define INT_7_KBM                       7       /* Keyboard/Mouse Interrupt */
#define INT_8_DRP_DCM_OOL               8       /* Drop Side DCM's OOL */
#define INT_9_DRP_FIFO_OF               9       /* Internal FIFO Overflow */
#define INT_10_LAN_FIFO_HAS_SOMETHING   10      /* Drop LAN FIFO holds at */
                                                /* least 1 block of data */
#define INT_11_SRAM_CONFLICT            11      /* Drop SRAM buffer read/write*/
                                                /* conflict */
#define INT_12_DRP_VSYNC                12      /* Drop VSync */
#define INT_13_DRP_TIMECODE             13      /* Drop timecode received */
#define INT_14_DLUTREG_CHANGE           14      /* DLUTReg has changed state */
#define INT_15_DRP_DETECT               15      /* Drop Frame Detect (in DSReg) has changed state */

/*************************************************************
 * Interrupts: (FPGA_ISREG or PLX_LOCAL_ISREG) bit positions *
 *************************************************************/
#define INT_31_PLX_DMA_1        31    /* PLX DMA channel 0 done */
#define INT_30_PLX_DMA_0        30    /* PLX DMA channel 1 done */
#define INT_28_ADD_STEREO       28    /* Add stereo change interrupt */
#define INT_27_DROP_AUDIO_FIFO  27    /* Drop audio FIFO empty interrupt */
#define INT_25_DROP_VIDEO_FIFO  25    /* Drop video FIFO empty interrupt */
#define INT_22_ADD_AUDIO_FIFO   22    /* Add audio FIFO has something  */
#define INT_20_ADD_VIDEO_FIFO   20    /* Add video FIFO has something  */
#define INT_13_MOUSE            13    /* Mouse FIFO has something */
#define INT_12_KEYBOARD         12    /* Keyboard FIFO has something */
#define INT_11_AMHREG_CHANGE    11    /* FPGA_AMHREG changed */
#define INT_10_AMVREG_CHANGE    10    /* FPGA_AMVREG changed */
#define INT_9_AMLREG_CHANGE     9     /* FPGA_AMLREG changed */
#define INT_8_AMTREG_CHANGE     8     /* FPGA_AMTREG changed */
#define INT_7_AMDVIHREG_CHANGE  7     /* FPGA_AMDVIHREG changed */
#define INT_6_AMDVIVREG_CHANGE  6     /* FPGA_AMDVIVREG changed */
#define INT_5_AMDVILREG_CHANGE  5     /* FPGA_AMDVILREG changed */
#define INT_4_AMDVITREG_CHANGE  4     /* FPGA_AMDVITREG changed */
#define INT_0_ADD_DCM_LOCK      0     /* 1 if addside video DCM unlocked */

/***************************************************
 * Interrupt status register bit positions         *
 ***************************************************/
#define PLX_DMA_1_BIT           (1<<INT_31_PLX_DMA_1) 
#define PLX_DMA_0_BIT           (1<<INT_30_PLX_DMA_0)
#define ADD_STEREO_CHANGE_BIT   (1<<INT_28_ADD_STEREO)
#define DROP_AUDIO_FIFO_BIT     (1<<INT_27_DROP_AUDIO_FIFO)
#define DROP_VIDEO_FIFO_BIT     (1<<INT_25_DROP_VIDEO_FIFO)
#define ADD_AUDIO_FIFO_BIT      (1<<INT_22_ADD_AUDIO_FIFO)
#define ADD_VIDEO_FIFO_BIT      (1<<INT_20_ADD_VIDEO_FIFO)
#define MOUSE_INTR_BIT          (1<<INT_13_MOUSE)
#define KEYBOARD_INTR_BIT       (1<<INT_12_KEYBOARD)
#define AMHREG_CHANGE_BIT       (1<<INT_11_AMHREG_CHANGE)
#define AMVREG_CHANGE_BIT       (1<<INT_10_AMVREG_CHANGE)
#define AMLREG_CHANGE_BIT       (1<<INT_9_AMLREG_CHANGE)
#define AMTREG_CHANGE_BIT       (1<<INT_8_AMTREG_CHANGE)
#define AMDVIHREG_CHANGE_BIT    (1<<INT_7_AMDVIHREG_CHANGE)
#define AMDVIVREG_CHANGE_BIT    (1<<INT_6_AMDVIVREG_CHANGE)
#define AMDVILREG_CHANGE_BIT    (1<<INT_5_AMDVILREG_CHANGE)
#define AMDVITREG_CHANGE_BIT    (1<<INT_4_AMDVITREG_CHANGE)
#define ADD_DCM_LOCK_BIT        (1<<INT_0_ADD_DCM_LOCK)

#define RXAMVREG_CHANGE_BIT     (1<<18)
#define RXAMHREG_CHANGE_BIT     (1<<11)
#define RXAMLREG_CHANGE_BIT     (1<<10)
#define RXAMTREG_CHANGE_BIT     (1<<9)
#define RXAMDVIVREG_CHANGE_BIT  (1<<8)
#define RXAMDVIHREG_CHANGE_BIT  (1<<7)
#define RXAMDVILREG_CHANGE_BIT  (1<<6)
#define RXAMDVITREG_CHANGE_BIT  (1<<5)

/************************************
 * Addside video duallink values    *
 ************************************/
#define DUALLINK_NO     0
#define DUALLINK_YES    1
#define DUALLINK_IGNORE 2

/************************************
 * XP200 FPGA OSD parameters        *
 ************************************/
#define OSD1MEMADDR             0x20000
#define OSD2MEMADDR             0x30000
#define MAX_OSD_PIXELS          8192

/************************************
 * XP200 FPGA Multimouse OSD params *
 ************************************/
#define FPGA_CURSOR_MEMSIZE     0x200
#define FPGA_DOSDMMCTL_START    0x104
#define FPGA_CURSOR_PATTSIZE    16

/****************************************
 * XP200 FPGA OSD control register bits *
 ****************************************/
#define OSDCTL_HANDSHAKE        0x8000  /* 0->go for SW, 1->go for HW */
#define OSDCTL_SINGLE           0x0008  /* 1->single color mode */
#define OSDCTL_ENABLE           0x0004  /* 0->disable, 1->enable */
#define OSDCTL_COLORBITS        0x0003  /* color register to use */
                                                                                
#define OSDCTL_COLOR_1          1
#define OSDCTL_COLOR_2          2
#define OSDCTL_NOCOLOR          3


/************************************
 * Default ADC register values      *
 ************************************/
#define DEFAULT_PLL_MSB                 0x69
#define DEFAULT_PLL_LSB                 0xd0
#define DEFAULT_VCO_CPMP                0x98
#define DEFAULT_9887_VCO_CPMP           0xe4
#define DEFAULT_9887_MODE_MUX           0x01 
#define DEFAULT_HSYNC_OUT               0x20
                                                                                
/************************************
 * FPGA max and min slice sizes     *
 ************************************/
#define COMPMODE_NONE               0
#define COMPMODE_444                1
#define COMPMODE_MOTION_444         2

/************************************
 * FPGA max and min slice sizes     *
 ************************************/
#define I50_MAX_NOBPSLICE           32
#define I50_MIN_NOBPSLICE           20
#define MAX_NOBPSLICE2              63
#define MAX_NOBPSLICE               36
#define MIN_NOBPSLICE               20
#define DEFAULT_NOBPSLICE           32
#define MAX_NUM_SLICES_PER_FRAME    2048

/***************************************************
 * PLX VPD field sizes                             *
 ***************************************************/
#define VPD_DATE_SIZE           4
#define VPD_SERIAL_SIZE         16
#define VPD_REVISION_SIZE       4

/**************************************************************
 * VPD serial number is a string starting with "IPV" followed *
 * by a 4 digit number. If the number is 0-999, the board has *
 * LX100 FPGA, if the number is 1000-4999, the board has LX80 *
 * FPGA. Numbers >= 5000 is currently reserved.               *
 **************************************************************/
#define LX100_SERIAL_MIN        0
#define LX100_SERIAL_MAX        999
#define LX80_SERIAL_MIN         1000
#define LX80_SERIAL_MAX         4999
#define RESERVED_SERIAL_MIN     9999

/***************************************************
 * FPGA Types for XPs                              *
 ***************************************************/
#define FPGA_TYPE_LX100         100
#define FPGA_TYPE_LX80          80
#define FPGA_TYPE_UNKNOWN       -1

/***************************************************
 * Macros to convert VPD dates & revisions         *
 ***************************************************/
#define VPD_YEAR(d)      (unsigned char) (((d) & 0xff000000) >> 24)
#define VPD_MONTH(d)     (unsigned char) (((d) & 0x00ff0000) >> 16)
#define VPD_DAY(d)       (unsigned char) (((d) & 0x0000ff00) >> 8)
#define VPD_DATE(y,m,d)  (((unsigned long) (y) << 24) |  \
                          ((unsigned long) (m) << 16) |  \
                          ((unsigned long) (d) << 8))
#define VPD_MAJ_REV(r)   (unsigned short) (((r) & 0xffff0000) >> 16)
#define VPD_MIN_REV(r)   (unsigned short) ((r) & 0x0000ffff)
#define VPD_REV(a,b)     (((unsigned long) (a) << 16) | (unsigned long) (b))
#define VPD_FPGA_TYPE(s) (unsigned char) ((s) & 0x000000ff)

#define TIMEVAL_DIFF(tv1, tv2)                          \
    ({                                                  \
        int __delta_sec = (tv1).tv_sec - (tv2).tv_sec;  \
        int __delta = (tv1).tv_usec - (tv2).tv_usec;    \
        if (__delta_sec) {                              \
            switch (__delta_sec) {                      \
            default:                                    \
                __delta = 0;                            \
            case 9:                                     \
                __delta += 1000000;                     \
            case 8:                                     \
                __delta += 1000000;                     \
            case 7:                                     \
                __delta += 1000000;                     \
            case 6:                                     \
                __delta += 1000000;                     \
            case 5:                                     \
                __delta += 1000000;                     \
            case 4:                                     \
                __delta += 1000000;                     \
            case 3:                                     \
                __delta += 1000000;                     \
            case 2:                                     \
                __delta += 1000000;                     \
            case 1:                                     \
                __delta += 1000000;                     \
            }                                           \
        }                                               \
        __delta;                                        \
    })

/********************************************************
 * Overlay releted variables/definitions                *
 ********************************************************/
#define OV_SET_PARAM_OVERLAY        0x00000001
#define OV_SET_PARAM_POSITION       0x00000002
#define OV_SET_PARAM_ALPHA          0x00000004
#define OV_SET_PARAM_CROP           0x00000008

#define OV_FLAG_SRC_LOCAL           0x00000001
#define OV_FLAG_SRC_OVERLAY         0x00000002
#define OV_FLAG_SRC_REMOTE          0x00000004
#define OV_FLAG_SRCMASK             0x0000000f

#define OV_FLAG_HSCALESRC           0x00000010
#define OV_FLAG_HRES                0x00000020
#define OV_FLAG_HSCALEDISPLAY       0x00000040
#define OV_FLAG_HSIZEMASK           0x000000f0

#define OV_FLAG_VSCALESRC           0x00000100
#define OV_FLAG_VRES                0x00000200
#define OV_FLAG_VSCALEDISPLAY       0x00000400
#define OV_FLAG_VSIZEMASK           0x00000f00

#define OV_FLAG_HPOSLOCATION        0x00001000
#define OV_FLAG_HPOSOFFSET          0x00002000
#define OV_FLAG_HPOSMASK            0x00003000

#define OV_FLAG_VPOSLOCATION        0x00004000
#define OV_FLAG_VPOSOFFSET          0x00008000
#define OV_FLAG_VPOSMASK            0x0000C000

#define OV_FLAG_KEEPASPECT          0x00010000

#define OV_FLAG_CROP                0x00020000

#define OV_LEFT_ADJUSTED        1
#define OV_RIGHT_ADJUSTED       2
#define OV_TOP_ADJUSTED         3
#define OV_BOTTOM_ADJUSTED      4
#define OV_CENTER_ADJUSTED      5

#define RX_MAX_SCALE_VAL        8
#define RX_MIN_SCALE_VAL        2


struct ov_param_io {
    unsigned long   setmask;
    int             flags;
    int             hscale;
    int             vscale;
    int             hres;
    int             vres;
    int             hlocation;
    int             hoffset;
    int             vlocation;
    int             voffset;
    int             oalpha;
    int             hcropleft;
    int             hcropright;
    int             vcroptop;
    int             vcropbottom;
};

#ifndef __NETVIZ_XPI_H_INCLUDE__

struct reg_io {
    int                 type;
    unsigned long       reg;
    unsigned long       value;
};

#define EEPROM_DATA_LEN         32
struct eeprom_io {
    int                 where;
    int                 length;
    char                data[EEPROM_DATA_LEN];
};

struct interrupt_io {
    int int_cnt[MAX_FPGA_INTERRUPTS];
    char int_status[MAX_FPGA_INTERRUPTS];
};

struct interrupt_info {
    unsigned long ii_count;
    int ii_status;
    int ii_disabled;
    void (*ii_tophalf)(unsigned long data);
};

#ifdef __KERNEL__
/*********************************************************
 * Srtucture used by v4 plx                              *
 *********************************************************/
struct dma_info {
    int state;                  /* Block, sgl or closed */
    int flags;                  /* Flag to note if DMA is pending etc */
    spinlock_t lock;            /* Lock needed to update flags */
};

struct devlist{
    unsigned int            d_minor;
    char                    d_name[32];
    struct file_operations  d_fops;
    struct cdev             *d_pcdev;
};

struct led {
    int l_setting;
    int l_keepalive;
    int l_state;
};

#define V2_NUM_LEDS     9

struct v2d_ctl {
    int default_vco_cpmp;
    int active_interface;
    int video_freeze_check;
    int external_sync;
    int force_interface;
    int progressive_compression;
    int add_drop_timer_count;
    int hd_shift;
    int vd_shift;
    int pix_adjust;
    int pix_tweak_hi;
    int pix_tweak_lo;
    unsigned char phase_shift;
    int force_no_video;
    int force_anobreg;
    int noise_thres_mode;
    int av_flag;
    int num_slices_per_frame;
    unsigned long no_addside_match_count;
    unsigned long addside_lock_count;
    struct LUT_struct *current_dropside_lut;
    struct LUT_struct *current_addside_lut;
    struct LUT_struct *current_display_lut;
    struct ov_param_io ocfg;
    struct ov_param_io oinfo;
    struct timer_list add_drop_timer;
};

struct v2d_board {
    int v_id;
    char v_name[30];
    int v_pci_dev_type;
    int v_type;
    unsigned long  v_status;
    int v_log;
    int v_irq;
    int v_hardware_detected;
    int v_hardware_version;
    int v_video_changed;
    int v_num_slices_per_frame;
    struct pci_dev *v_pci_dev;
    struct devlist v_devlist[V2D_CHAR_DEVICES];

    struct tasklet_struct v_pcitask;

    struct tasklet_struct v_tasklet[MAX_FPGA_INTERRUPTS];
    struct interrupt_info v_intinfo[MAX_FPGA_INTERRUPTS];


    void *v_kbdev;
    void *v_msdev;
    void *v_videodev;
    void *v_audiodev;
    struct v2d_ctl *v_ctl;
    struct list_head v_lut_table_head;
    struct timer_list v_flow_control_timer;
    int v_dma_read_size;
    int v_dontwdequeue;
    unsigned long  v_fpga_timer_cnt;
    unsigned long  v_fpga_wtimer_cnt;
    struct proc_dir_entry   *v_pdr;

    /* Per board data needed by flow_control */
    struct sk_buff_head v_poolq; // Pool of buffers

    /***************************************************
     * V2 Board specfic data                           *
     **************************************************/
    unsigned long       v_ConfigMemBase;
    unsigned long       v_DropMemBase;
    unsigned long       v_AddMemBase;
    int                 v_adcinit_retries;
    struct led          v_leds[V2_NUM_LEDS];
    struct timer_list   v_led_timer;
    unsigned long       v_ledregister;
    unsigned long       v_ledcnt;
    unsigned long       v_normal_operation;
    char                v_farvar[1024];


    /***************************************************
     * V4 Board specfic data                           *
     **************************************************/
    unsigned long       v_FpgaProgBase;
    unsigned long       v_PlxMemBase;
    int                 v_has_bar3;
    struct dma_info     dma_channel[MAX_DMA_CHANNELS];
    spinlock_t          v_plx_isr_lock;    
    spinlock_t          v_curr_buff_lock;    
    spinlock_t          v_lut_lock;    
    int                 v_lut_size;
    /* synchs PLX register modification with ISR */

    char *v_plx_test_nearvar;
    dma_addr_t v_plx_test_nearvardma;
    int v_dma_numbytes;
    int v_dma_test_chan;
    unsigned int v_bar_reg;
    unsigned long v_fpga_base;
};


/********************************************
 * Function prototpyes for ioctl handler    *
 ********************************************/
int plx_control_ioctl(struct v2d_board *board, 
                      unsigned int cmd, 
                      unsigned long data);
int fpga_interrupt_ioctl(struct v2d_board *board, 
                         unsigned int cmd, 
                         unsigned long data);
int fpga_control_ioctl(struct v2d_board *board, 
                       unsigned int cmd, 
                       unsigned long data);
int led_control_ioctl(struct v2d_board *board, 
                      unsigned int cmd, 
                      unsigned long data);
int flow_control_ioctl(struct v2d_board *board, 
                       unsigned int cmd, 
                       unsigned long data);
int kbm_control_ioctl(struct v2d_board *board, 
                      unsigned int cmd, 
                      unsigned long data);

/********************************************
 * Function prototpyes for fpga_interrupt.c *
 ********************************************/
extern int fpga_interrupt_init(struct v2d_board *board);
extern int fpga_interrupt_enable(struct v2d_board *board, int irq);
extern int fpga_interrupt_disable(struct v2d_board *board, int irq);
extern void fpga_interrupt_exit(struct v2d_board *board);
extern int fpga_register_irq(struct v2d_board *board, int irq, 
                             void (*handler)(unsigned long data));
extern int fpga_register_disabled_irq(struct v2d_board *board, int irq, 
                                      void (*handler)(unsigned long data));
extern int fpga_register_tophalf_irq(struct v2d_board *board, int irq, 
                                     void (*handler)(unsigned long data));
extern int fpga_unregister_irq(struct v2d_board *board, int irq);
extern int fpga_interrupt_fire(struct v2d_board *board, int irq);


/******************************
 * Convenient query functions *
 ******************************/
static inline int is_plx_device(struct v2d_board *board) {
    if (board->v_pci_dev_type == PLX_DEVICE)
        return 1;
    else 
        return 0;
}

static inline int is_tx_board(struct v2d_board *board) {
    if (board->v_type == BOARDTYPE_TX)
        return 1;
    return 0;
}

static inline int is_rx_board(struct v2d_board *board) {
    if (board->v_type == BOARDTYPE_RX)
        return 1;
    return 0;
}

static inline int fpga_is_interrupt_enabled(struct v2d_board *board, int irq) {
    return board->v_intinfo[irq].ii_status;
}

/************************************
 * Convenient FPGA access functions *
 ************************************/
static inline void set_fpga_val(struct v2d_board *board, 
                                unsigned long reg,  
                                unsigned long val)
{
    if (!board->v_hardware_detected)
        return;
    *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg) = val;
    mb();
}

static inline unsigned long get_fpga_val(struct v2d_board *board, 
                                         unsigned long reg)
{
    if (!board->v_hardware_detected)
        return 0;
    return *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg);
}

static inline void and_in_fpga_val(struct v2d_board *board, 
                                   unsigned long reg,  
                                   unsigned long val)
{
    volatile unsigned long old_val;
    if (!board->v_hardware_detected)
        return;
    old_val= *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg);
    *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg) = old_val & val;
    mb();
}

static inline void or_in_fpga_val(struct v2d_board *board, 
                                  unsigned long reg,  
                                  unsigned long val)
{
    volatile unsigned long old_val;
    if (!board->v_hardware_detected)
        return;
    old_val= *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg);
    *(volatile unsigned long *)
        (board->v_ConfigMemBase + board->v_fpga_base + reg) = old_val | val;
    mb();
}

static inline void set_bar2_val(struct v2d_board *board, 
                                unsigned long reg,  
                                unsigned long val)
{
    if (!board->v_hardware_detected)
        return;
    *(volatile unsigned long *) (board->v_FpgaProgBase + reg) = val;
    mb();
}

static inline unsigned long get_bar2_val(struct v2d_board *board, 
                                         unsigned long reg)
{
    if (!board->v_hardware_detected)
        return 0;
    return *(volatile unsigned long *) (board->v_FpgaProgBase + reg);
}

static inline void set_bar3_val(struct v2d_board *board, 
                                unsigned long reg,  
                                unsigned long val)
{
    if (!board->v_hardware_detected)
        return;
    *(volatile unsigned long *) (board->v_ConfigMemBase + reg) = val;
    mb();
}

static inline unsigned long get_bar3_val(struct v2d_board *board, 
                                         unsigned long reg)
{
    if (!board->v_hardware_detected)
        return 0;
    return *(volatile unsigned long *) (board->v_ConfigMemBase + reg);
}

#endif  /* __KERNEL__ */

#endif	/* __BARCO_NETVIZ2_XPI_H_INCLUDE__ */

#endif 	/* __FPGA_H_INCLUDED__ */

