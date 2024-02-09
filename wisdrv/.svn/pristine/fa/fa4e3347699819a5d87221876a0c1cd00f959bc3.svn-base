// DeviceCtl.c

#include "platform.h"
#include "DeviceCtl.h"
#include "function.h"
#include "I2C.h"
#include "DeviceIO.h"
#include "usb_base.h"
#include "motion_detection_api.h"
#include "WisGoDrv.h"

#include <asm/uaccess.h>

#if defined(_WINDOWS)   
int SetPowerState(PDEVICE_EXTENSION_COMMON pdxc, ULONG PowerState);
#endif

static int md_region_detections[MAX_DEVICES];
static int md_running[MAX_DEVICES];

#define GETUINT16(Buf,Index)  ((((UINT16)Buf[Index+1])<<8)+Buf[Index])

/* 1350 macroblocks divided by the number of bits in each array element */
//#define MACROBLOCK_MAP_SIZE_WORDS   ((1350/16)+2)
#define MACROBLOCK_MAP_SIZE_WORDS   ((1620/16)+3)
#define MACROBLOCK_MAP_SIZE_BYTES   (MACROBLOCK_MAP_SIZE_WORDS*sizeof(SINT16))

/* define this if you want to leave the macroblock map on the end of the */
/* video stream */
#define APPEND_MD_MAP_TO_STREAM
#undef APPEND_MD_MAP_TO_STREAM

/* the last location actually contains the region information */
//#define MAP_REGION_INFO_OFFSET      85
#define MAP_REGION_INFO_OFFSET      102

#define TRUE            1
#define FALSE           0
typedef struct _TFrameInfo
{
    SINT32 VideoLength;
    SINT32 VBILength;
    SINT32 AudioFingerprint;
    SINT32 VIPNumber;
    SINT32 AudioTimestamp;
    SINT32 FrameType;
    SINT32 MDRegionMap;
    SINT32 MDMapFlag;
//    UINT16 MDMacroblockMap[86]; //MACROBLOCK_MAP_SIZE_WORDS];
    UINT16 MDMacroblockMap[MACROBLOCK_MAP_SIZE_WORDS];
    UINT16 frame_seq_pos; /*0: first frame, 1:in themiddle, 2: last frame*/

}TFrameInfo;

// Must be duplicated in Sw/Include/VideoCapture.h
typedef struct _AVStats
{
  UINT32 V_acquired;
  UINT32 V_dropped;
  UINT32 V_duplicated;
  UINT32 A_acquired;
  UINT32 A_dropped;
  UINT32 A_duplicated;
}AVStats;

/*
 * Frame Option on-the-fly change - I/O definitions.
 */

/* Post 01/17/04 firmware */
#define FORCE_I_FRAME               0x0040
#define FORCE_NEW_SEQUENCE          0x0041
#define CHANGE_PFRAME_RATE          0x0081
#define CHANGE_I_Q_SCALE            0x0082
#define CHANGE_P_Q_SCALE        0x0084
#define SET_FPS                     0x0088
#define CBR_TO_VBR_1                0x0101
#define CBR_TO_VBR_2                0x0102
#define CBR_TO_CBR_1                0x0104
#define CBR_TO_CBR_2                0x0108
#define VBR_TO_CBR                  0x0200


/*
 * Standard Video rates.
 */
enum TVStandard
{
    NORM_NTSC=30,
    NORM_PAL=25,
    NORM_SECAM=25
};
enum TVType
{
    NTSC=0,
    PAL=1,
    SECAM=2
};

typedef
struct tv_norm_s
{
    char* name;
    int rate;
} tv_norm_t;

/*
 * Support norms.
 * TODO: add flag to set/get.
 */
static 
tv_norm_t tv_standard[] = {
    {"NTSC", NORM_NTSC},
    {"PAL",  NORM_PAL},
    {"SECAM", NORM_SECAM},
};

/*
 *
 * The relationship between TV Standard scan rate and outgoing digital
 * fps rate is:
 *
 * fps_out = fps_in / (1+ fps_level) where fps_level range is [0,...fps_in-1]
 * fps_out range is fps_in, 1/2 fps_in, 1/3 fps_in, ... 1fps
 * e.g. fps_in = 30fps, we can get fps_out like:
 * 30fps, 15fps, 10fps, 7fps, 6fps, 5fps, 4fps, 3fps, 2fps and 1fps.
 * 
 * NOTE: fps_in is the TV standard or (Norm) - scanrate of the digital TV 
 * decoder (e.g. SAA711x)
 *
 */
static void
set_fps(int unit, int fps_in, int requested, int* actual, unsigned short* l)
{
    int fps, level, error, norm;

    norm = tv_standard[fps_in].rate;

    for ( fps = 1; fps <= tv_standard[fps_in].rate; fps++ )
    {

        level = (tv_standard[fps_in].rate - fps) / fps;
        error = fps - ( norm / ( 1+ level) );

        if ( error == 0 && fps >= requested )
        {

            PrintMsg(DEBUG_PROPERTY, ("wis%d: requested %dFPS in %s(%d FPS) :fps_level=%d, "
                   "using highest next available rate of %dFPS error=%d\n",
                   unit, requested,
                   tv_standard[fps_in].name,
                   tv_standard[fps_in].rate, 
                   level,
                   fps,
                   error));

            *actual= fps;
            *l = (short)level;      
            break;  
        }
    }
}


/*
 * Bitrate Control: On-The-Fly Bitrate driver API
 * 
 * Argument definitions
 * 
 *  change_direction   : 0 = change from VBR to VBR
 *                       1 = change from CBR to VBR
 *                       2 = change from VBR to CBR
 *                       3 = change from CBR to CBR
 *                       invalid if > 3 or < 0
 * 
 *  new_i_qscale       : the IQ value for VBR
 *  new_p_qscale       : the PQ value for VBR
 *  new_target_rate    : the target_rate for CBR
 *  new_peak_rate      : the peak_rate for CBR
 *  new_vbv_buffer     : the vbv_buffer for CBR
 *  new_converge_speed : the converge_speed for CBR
 *  new_lambda         : the lambda for CBR
 *
 * Return values definitions
 *  -1 - change_direction error
 *  -2 - i_qscale out of range
 *  -3 - p_qscale out of range
 *  -4 - target_rate out of range
 *  -5 - peak_rate out of range
 *  -6 - converge_speed out of range
 *  -7 - lambda out of range
 *  -8 - vbv_buffer out of range
 *   0 - change_BRC_OTF job done
 *
 * CBR and VBR definitions
 *  CBR means constant bit rate control, i.e.
 *  SET i_qscale = p_qscale = 0
 *  SET target_rate = N bps (N>0)
 *  SET peak_rate, vbv_buffer, converge_speed and lambda either = 0 or > 0
 * 
 * VBR means constant quantizer control, i.e.
 *  SET QMIN < i_qscale < 31
 *  SET QMIN < p_qscale < 31
 *  SET target_rate = peak_rate = vbv_buffer = converge_speed = lambda = 0
 * 
 * 
 *  
 */
//#define __QMIN 2
#include "common.h"
#include "PacGen_7007SB.h"
static
SINT32 update_brc_mem(PDEVICE_EXTENSION_COMMON pdxc,
                      APRIL* brc)
{
    int r;
    // refresh BRC register gpmem
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF20,(UINT16) brc->DQ_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF21,(UINT16) brc->SGOP_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF22,(UINT16) brc->QD0_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF23,(UINT16) brc->QD1_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF24,(UINT16) brc->QD2_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF25,(UINT16) brc->QD3_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF26,(UINT16) (brc->SGOP_EXPT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF27,(UINT16) ((brc->SGOP_EXPT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF28,(UINT16) (brc->SGOP_PEAK_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF29,(UINT16) ((brc->SGOP_PEAK_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2A,(UINT16) (brc->VBV_ALERT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2B,(UINT16) ((brc->VBV_ALERT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2C,(UINT16) (brc->MIN_LMT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2D,(UINT16) ((brc->MIN_LMT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2E,(UINT16) (brc->MAX_LMT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF2F,(UINT16) ((brc->MAX_LMT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF30,(UINT16) (brc->COMPLEX0_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF31,(UINT16) ((brc->COMPLEX0_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF32,(UINT16) (brc->COMPLEX1_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF33,(UINT16) ((brc->COMPLEX1_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF34,(UINT16) (brc->COMPLEX2_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF35,(UINT16) ((brc->COMPLEX2_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF36,(UINT16) (brc->COMPLEX3_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF37,(UINT16) ((brc->COMPLEX3_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF38,(UINT16) (brc->SGOP_USED_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF39,(UINT16) ((brc->SGOP_USED_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3A,(UINT16) (brc->TOTAL_BITS_USED_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3B,(UINT16) ((brc->TOTAL_BITS_USED_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3C,(UINT16) (brc->TOTAL_EXPT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3D,(UINT16) ((brc->TOTAL_EXPT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3E,(UINT16) (brc->VBV_LEVEL_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF3F,(UINT16) ((brc->VBV_LEVEL_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF40,(UINT16) (brc->SGOP_NEXT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF41,(UINT16) ((brc->SGOP_NEXT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF42,(UINT16) (brc->SUM_Q_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF43,(UINT16) ((brc->SUM_Q_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF44,0);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF45,0);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF46,(UINT16) brc->MB_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF47,0);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF48,(UINT16) brc->FRAME_INDEX_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF49,(UINT16) brc->SGOP_Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4A,(UINT16) brc->Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4B,(UINT16) brc->SGOP_COEF_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4C,(UINT16) brc->PQ_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4D,0);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4E,(UINT16) brc->MODE0_Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF4F,(UINT16) brc->MODE1_Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF50,(UINT16) brc->MODE2_Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF51,(UINT16) brc->MODE3_Q_ADDR);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF56,(UINT16) (brc->RC_SGOP0_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF57,(UINT16) ((brc->RC_SGOP0_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF58,(UINT16) (brc->RC_SGOP1_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF59,(UINT16) ((brc->RC_SGOP1_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5A,(UINT16) (brc->RC_SGOP2_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5B,(UINT16) ((brc->RC_SGOP2_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5C,(UINT16) (brc->RC_SGOP3_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5D,(UINT16) ((brc->RC_SGOP3_ADDR & 0xffff0000)>>16));
#ifdef  _EXTRA_BRC_DEBUG
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5E,(UINT16) (brc->SUM_USED_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF5F,(UINT16) ((brc->SUM_USED_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF60,(UINT16) (brc->SUM_EXPT_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF61,(UINT16) ((brc->SUM_EXPT_ADDR & 0xffff0000)>>16));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF62,(UINT16) (brc->SUM_SGOP_ADDR & 0x0000ffff));
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF63,(UINT16) ((brc->SUM_SGOP_ADDR & 0xffff0000)>>16));
#endif
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF64,0);
    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,0xBF65,0);
    return r;
}


SINT32 change_brc_otf(PDEVICE_EXTENSION_COMMON pdxc,
                      SINT32 change_direction,
                      SINT32 new_i_qscale,
                      SINT32 new_p_qscale,
                      SINT32 new_target_bitrate,
                      SINT32 new_peak_rate,
                      SINT32 new_vbv_buffer,
                      SINT32 new_converge_speed,
                      SINT32 new_lambda)
{
    int i,r = 0;
    APRIL *april = NULL;
    CONFIG_MANAGER* cfgman = &pdxc->config_manager;
    TCFGVIDEOEX* vc = &cfgman->_video_config;
    TCFGSTREAM* stc = &vc->strcfg;
    TCFGRESOLUTION* rc = &vc->rescfg;
    TCFGFRAMERATE*  fc = &vc->fpscfg;
    TCFGBRCTRL* bc = &vc->ctlcfg;
    SINT32 qmin = bc->reserved&0x7F;
    SINT32 qmax = (bc->reserved&0x7F00)>>8;
    SINT32 window_size = (bc->reserved&0xFFFF0000)>>16;
    /* the following code is from BitrateControlPatch() of function.c */
    qmin = (qmin<=1||qmin>31)? 2:qmin;
    qmax = (qmax<qmin||qmin>31)? 31:qmax;
    window_size = (window_size==0)? (1*fc->frame_rate/1000):window_size;
    /* end of BitrateControlPatch() */

    if ( (change_direction > 3) || (change_direction < 0) )
        return -1; /* change_direction error */

    if ( change_direction < 2 )
    {

        if ( (new_i_qscale > qmax) || (new_i_qscale < qmin) )
            return -2; /* new_i_qscale out of range */

        if ( (new_p_qscale > qmax) || (new_p_qscale < qmin) )
            return -3; /* new_p_qscale out of range */
    }
    else
    {
        if ( (new_target_bitrate > 0x1000000) || (new_target_bitrate<0) )
            return -4; /* new_target_rate out of range */

        if ( (new_peak_rate > 0x4000000) || (new_peak_rate < 0) )
            return -5; /* new_peak_rate out of range */

        if ( (new_converge_speed > 100) || (new_converge_speed < 0) )
            return -6; /* new_converge_speed out of range */

        if ( (new_lambda > 100) || (new_lambda < 0) )
            return -7; /* new_lambda out of range */
#if 0
        if ( ( new_vbv_buffer >(112*16*1024)) || (new_vbv_buffer < 0) )
            return -8; /* vbv_buffer out of range */
#endif
    }
    april = (APRIL *) PIAllocMem(sizeof(APRIL));

    PrintMsg(DEBUG_PROPERTY, ("wis%d: BRC: %dx%d, bitrate=%dbps, peak=%dbps "
           "fps=%d, seq=%d\n",
           pdxc->unit,
           (int)rc->width,
           (int)rc->height,
           (int)bc->target_bitrate,
           (int)bc->peak_bitrate,
           (int)fc->frame_rate,
           (int)stc->sequence));

    PrintMsg(DEBUG_PROPERTY, ("wis%d: NEW BRC config:"
           "%s: iq=%d,pq=%d,rate=%d,peak=%d,vbv=%d c=%d, l=%d\n",
           pdxc->unit,
           change_direction == 0 ? "VBR->VBR" :
           change_direction == 1 ? "CBR->VBR" :
           change_direction == 2 ? "VBR->CVR" :
           change_direction == 3 ? "CBR->CBR" : "Invalid",
           new_i_qscale, new_p_qscale, new_target_bitrate,
           new_peak_rate, new_vbv_buffer, new_converge_speed,
           new_lambda));




    switch ( change_direction )
    {
        
        case 0:
            PrintMsg(DEBUG_PROPERTY, ("wis%d: BRC: VBR->VBR on the fly change\n",
                   pdxc->unit));
            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CHANGE_I_Q_SCALE,new_i_qscale);
            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CHANGE_P_Q_SCALE,new_p_qscale);

            /*  call change_i_q_scale(new_i_qscale);
             *  call change_p_q_scale(new_p_qscale);
             */
            break;

        case 1:
            PrintMsg(DEBUG_PROPERTY, ("wis%d: BRC: CBR->VBR on the fly change\n",
                   pdxc->unit));
            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CBR_TO_VBR_1,
                                                new_i_qscale);
            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CBR_TO_VBR_2,
                                                new_p_qscale);

            /* write_HPI_int(device,CBR_to_VBR_1,new_i_qscale);
             * write_HPI_int(device,CBR_to_VBR_2,new_p_qscale);
             */
            break;

        case 2:
            PrintMsg(DEBUG_PROPERTY, ("wis%d: BRC: VBR->CBR on the fly change\n",
                   pdxc->unit));

            /* brctrl_q=0 */
            /* fps: frame rate */
            /* width x height:resolution params */      
            /*
             * sequence: 1: I_ONLY
             *           2:IP_ONLY
             *           3:IPB
             */

            Setup(april,
                  new_target_bitrate,
                  new_peak_rate,
                  new_vbv_buffer,
                  new_converge_speed,
                  new_lambda,
                  0,                
                  fc->frame_rate,              
                  (rc->width*rc->height),   
                  (stc->sequence < 3 ? 0 : 2), 
                  qmin,
                  qmax,
                  window_size);      


            /* fps, width, height, sequence, QMIN are global
             * variables.
             */


            update_brc_mem(pdxc, april);

            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                VBR_TO_CBR,
                                                0);

            //bc->target_bitrate = new_target_bitrate;

            break;

        case 3:
            PrintMsg(DEBUG_PROPERTY, ("wis%d: BRC: CBR->CBR on the fly change\n",
                   pdxc->unit));     
            Setup(april,
                  new_target_bitrate,
                  new_peak_rate,
                  new_vbv_buffer,
                  new_converge_speed,
                  new_lambda,
                  0,                
                  fc->frame_rate,              
                  (rc->width*rc->height),   
                  (stc->sequence < 3 ? 0 : 2), 
                  qmin,
                  qmax,
                  window_size);      

            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CBR_TO_CBR_1,
                                                (new_target_bitrate > bc->target_bitrate?0:1));
            PrintMsg(DEBUG_PROPERTY, ("Wait 36ms\n"));
            for ( i = 0; i < 18; i++ )
                mdelay(4);//~33-35ms delay

            update_brc_mem(pdxc, april);

            r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                CBR_TO_CBR_2,
                                                (new_target_bitrate > bc->target_bitrate?0:1));

            //bc->target_bitrate = new_target_bitrate;
            /* update global variable target_rate */

            break;

        default:
            break;
    }

    if ( april!=NULL )
        PIFreeMem(april);

    return r;
}

extern int GenInitialPacket(PDEVICE_EXTENSION_COMMON pdxc, 
                TCFGVIDEOEX *video_config,
                TVIDEOCFGFIX *fix_config,
                UINT8 *pPack, SINT32 *PackLen,
                UINT8 *pFontBitmaps, SINT32 lFontBitmapSize,
                int reset_type);
void
QuickResetChip(PDEVICE_EXTENSION_COMMON context,
           TCFGVIDEOEX *video_config)
{

      char FirmwareFileName[256] = "go7007fw.bin";
      UINT8 *pFirmware;
      UINT16 *pFirmware1;
      int Firmlen;

      UINT8 *pInitPack;
      int InitPackLen;

      PrintMsg(DEBUG_HARDWARE_MISC, ("QuickResetChip : Enter"));

      // reset chip
      (context->hw_funcs.WriteInterrupt)(context,0x0001,0x0001);

      // read firmware file
      Firmlen = 64*1024;
      pFirmware = PIAllocMem(Firmlen);
      FormatSysFileName(FirmwareFileName);

      if(ReadFileToBuffer(FirmwareFileName, pFirmware, &Firmlen)>0)
      {
      PrintMsg(DEBUG_HARDWARE_MISC,
           ("QuickResetChip : Read firmware file fail"));
      PIFreeMem(pFirmware);
      return;
      } 

      pFirmware1 = (UINT16 *)(pFirmware+16);
      pFirmware1[3] = 0;

      //    Download first packet of Firmware
      if( (context->hw_funcs.DownloadBuffer)(context,
                         (UINT8 *)pFirmware1, 64) != 0)
      {

      PrintMsg(DEBUG_HARDWARE_MISC,
           ("QuickResetChip - Download firmware Failed !"))
      PIFreeMem(pFirmware);
      return;
      }
 
      PIFreeMem(pFirmware);

      if (CheckInterruptPipe(context, 0x5a5a, -1) < 0)
      {
      PrintMsg(DEBUG_HARDWARE_MISC,
           ("QuickResetChip - Download firmware Failed !"))
      }
 
      // generate quick reset initialize packet
      InitPackLen = 16*1024;
      pInitPack = PIAllocMem(InitPackLen);

      context->BoardInterface->pIBoardVideo->OnStartCapture(
               context->BoardContext,
               (AVSTREAM_COOKIE)context,
               video_config,
               context->config_manager._sensor_config);

      GenInitialPacket(context, video_config, &(context->fix_config),
               pInitPack, &InitPackLen, 
               NULL, 0, 1);

      // download initialize packet
      (context->hw_funcs.DownloadBuffer)(context, pInitPack, InitPackLen);
      PIFreeMem(pInitPack);

      if (CheckInterruptPipe(context, 0x0000, -1) < 0)
      {
      PrintMsg(DEBUG_HARDWARE_MISC,
           ("QuickResetChip - Download init packet Failed !"))
      }

      PrintMsg(DEBUG_HARDWARE_MISC, ("QuickResetChip : Leave"));
}

/*
 * Change resolution parameters in effect, perform quick
 * reset of Encoder and restart encoding.
 */
int
change_resolution_otf(PDEVICE_EXTENSION_COMMON pdxc,SINT32 width, SINT32 height)
//              TCFGVIDEO* pcfg)
{
    CONFIG_MANAGER* cfgman = &pdxc->config_manager;
    TCFGVIDEOEX* vc = &cfgman->_video_config;

    if(width>vc->syscfg.sensor_h||height>vc->syscfg.sensor_v) return 1;

    /* Adjust rescfg */
    vc->rescfg.width = width;
    vc->rescfg.height = height;
    vc->rescfg.h_sub_window = 1;
    vc->rescfg.v_sub_window = 1;
    if(width<=vc->syscfg.sensor_h/4 && height<=vc->syscfg.sensor_v/4)
    {
        vc->rescfg.sub_sample = 1;
        vc->rescfg.h_scale_enb = 1;
        vc->rescfg.v_scale_enb = 1;
        vc->rescfg.h_sub_offset = (vc->syscfg.sensor_h/4 - width)/2;
        vc->rescfg.v_sub_offset = (vc->syscfg.sensor_v/4 - height)/2;
    }
    else
    {
        vc->rescfg.sub_sample = 0;
        vc->rescfg.h_scale_enb = (width<=vc->syscfg.sensor_h/2)? 1:0;
        vc->rescfg.v_scale_enb = (height<=vc->syscfg.sensor_v/2)? 1:0;
        vc->rescfg.h_sub_offset = (vc->syscfg.sensor_h/(vc->rescfg.h_scale_enb?
2:1) - width)/2;
        vc->rescfg.v_sub_offset = (vc->syscfg.sensor_v/(vc->rescfg.v_scale_enb?
2:1) - height)/2;
    }

    if(vc->syscfg.format==1) vc->strcfg.deinterlace_mode = 1;
 
    /* Reset configuration */
    cfgman->_video_config_valid = 1;

    PrintMsg(DEBUG_PROPERTY, ("change_resolution_otf: Got locks\n"));

    QuickResetChip(pdxc, vc);

    PrintMsg(DEBUG_PROPERTY, ("Reset config done\n"));
    
    return 0;
}

/*
 * Change fps parameter in effect, perform quick
 * reset of Encoder and restart encoding.
 */
int
change_fps_otf(PDEVICE_EXTENSION_COMMON pdxc,SINT32 fps)
{
	CONFIG_MANAGER* cfgman = &pdxc->config_manager;
	TCFGVIDEOEX* vc = &cfgman->_video_config;

	int target_frame_rate=0;	
	/* Reset configuration */
	target_frame_rate=fps*1000;
	//vc->fpscfg.frame_rate = fps*1000; 
	cfgman->_video_config_valid = 1;
	if (vc->fpscfg.frame_rate<=1000)
		vc->fpscfg.frame_rate+=1000;	
	while (vc->fpscfg.frame_rate<target_frame_rate)
		{					
		QuickResetChip(pdxc, vc);		
		if (vc->fpscfg.frame_rate<5000)
			vc->fpscfg.frame_rate+=2000;	
		else
			vc->fpscfg.frame_rate+=4000;
		}		
	PrintMsg(DEBUG_PROPERTY, ("change_fps_otf: Got locks\n"));
	vc->fpscfg.frame_rate=target_frame_rate;
/*	StreamBuffer_Reset();*/
	pdxc->StreamBuffer._frame_serial=0;
	QuickResetChip(pdxc, vc);
	PrintMsg(DEBUG_PROPERTY, ("Reset config done\n"));
	return 0;
}


int ProcessIOCTL(PDEVICE_EXTENSION_COMMON pdxc,
                 VOID *ioBuffer, SINT32 *ret_len)
{
    IO_PACKET *pIOPacket = (IO_PACKET *)ioBuffer;
    int r;
    CONFIG_MANAGER* cfgman = &pdxc->config_manager;
    TCFGVIDEOEX* vc = &cfgman->_video_config;
    //    TCFGSTREAM* stc = &vc->strcfg;
    TCFGFRAMERATE*  fc = &vc->fpscfg;
    TCFGBRCTRL* bc = &vc->ctlcfg;       

  //PrintMsg(DEBUG_ALWAYS_DISPLAY,
  //              ("<DRV> ProcessIOCTL : Packet Type = %d",pIOPacket->Type));


    switch ( pIOPacket->Type )
    {

        case IO_SIGDETECT:
        {
	    int Value;
            r= pdxc->BoardInterface->pIBoardVideo->
                GetVideoProperty(
                    pdxc->BoardContext,
                    pdxc,
                    PROP_VIDEO_HLOCK, &Value, NULL);
	   if (r==0)
               pIOPacket->Param1 = Value;
	   else 
               pIOPacket->Param1 = 1;
           //PrintMsg(DEBUG_ALWAYS_DISPLAY,("r =%d, value=%d",r,Value));
           pIOPacket->BufLen = 0;
	   break;
	}	
        case IO_SET_RESOLUTION:
	{
	    //TCFGVIDEO *pcfg = (TCFGVIDEO *)(pIOPacket->Buf);        
	    int r;
	    r = change_resolution_otf(pdxc, pIOPacket->Param1, pIOPacket->Param2);
	    set_seq_end(&(pdxc->StreamBuffer));
            pIOPacket->Param1 = r;
	    break;
	}	

        case IO_SETVIDEO_SOURCE:
        {

	    unsigned int mode;

	    /* Device only provided 4  bytes of data in buf pointer */
	    if ( copy_from_user(&mode, (void*)pIOPacket->Buf, sizeof(int)) )
		return -EFAULT;

	    PrintMsg(DEBUG_PROPERTY, ("IO_SETVIDEO_SOURCE: (original) %d\n", mode));      

	    if ( mode >=0 && mode <=1 )
            {

		pdxc->BoardInterface->
                    pIBoardVideo->SetVideoProperty(&pdxc->BoardContext,
                                                   (AVSTREAM_COOKIE)pdxc,
                                                   PROP_VIDEO_SOURCE,
                                                   mode, NULL);

		PrintMsg(DEBUG_PROPERTY, (" Set mode=0x%x\n", mode));
	    }

	    if ( copy_to_user((void*)pIOPacket->Buf, &mode, sizeof(int)) )
		return -EFAULT;
	    break;
	}

        case IO_GETVIDEO_SOURCE:
        {
	    unsigned int mode;
	    
	    PrintMsg(DEBUG_PROPERTY, ("IO_GETVIDEO_SOURCE\n"));     
	    
	    mode = pdxc->BoardInterface->
		pIBoardXBar->GetCurrentVideoInputID(&pdxc->BoardContext,
						    (AVSTREAM_COOKIE)pdxc);
	    PrintMsg(DEBUG_PROPERTY, (" Get mode=0x%x\n", mode));
	    
	    if ( copy_to_user((void*)pIOPacket->Buf, &mode, sizeof(int)) )
		return -EFAULT;
	    break;
	}

        case IO_GETCHIPREVISION:
        {
	    REVISION_INFO *p_ri = (REVISION_INFO *)(pIOPacket->Buf);
	    
	    p_ri->BoardRevision = pdxc->BoardInfo.RevisionNum;
	    strcpy(p_ri->BoardName, pdxc->BoardInfo.Name);
	    p_ri->BoardCapability = pdxc->BoardInfo.Capability;
	    p_ri->SourceWidth  =
		pdxc->config_manager._video_config.syscfg.sensor_h;
	    p_ri->SourceHeight =
		pdxc->config_manager._video_config.syscfg.sensor_v;

	    pIOPacket->BufLen = sizeof(REVISION_INFO);
	    break;
	}

        case IO_SETINSTANCENUM:

#if defined(_WINDOWS)
            pdxc->NotifyEvent = PICreateNotificationEvent(pIOPacket->Param1, &(pdxc->NotifyHandle)); 
#endif
            pIOPacket->Param1 = 0;
            pIOPacket->BufLen = 0;
            break;

        case IO_SETVIDEOCONFIG:
        {
	    TCFGVIDEO *pcfg = (TCFGVIDEO *)(pIOPacket->Buf);
	    
	    pdxc->config_manager._video_config.misccfg = pcfg->misccfg; 
	    pdxc->config_manager._video_config.strcfg = pcfg->strcfg; 
	    pdxc->config_manager._video_config.rescfg = pcfg->rescfg; 
	    pdxc->config_manager._video_config.fpscfg = pcfg->fpscfg; 
	    pdxc->config_manager._video_config.ctlcfg = pcfg->ctlcfg; 

	    pdxc->config_manager._video_config_valid = 1;
	    pIOPacket->Param1 = 0;
	    pIOPacket->BufLen = 0;
	    break;
	}

        case IO_SETAUDIOCONFIG:
        {
	    AUDIO_CONFIG *pcfg = (AUDIO_CONFIG *)(pIOPacket->Buf);
	    
	    pdxc->config_manager._audio_config = *pcfg; 
	    
	    pdxc->config_manager._audio_config_valid = 1;
	    pIOPacket->Param1 = 0;
	    pIOPacket->BufLen = 0;
	    break;
	}
        case IO_SETFIXCONFIG:
        {
	    TVIDEOCFGFIX *pfixcfg = (TVIDEOCFGFIX *)(pIOPacket->Buf);
        memcpy ((void *)&(pdxc->fix_config),
                            (void *) (pfixcfg),
                    sizeof(TVIDEOCFGFIX));
	    pIOPacket->Param1 = 0;
	    pIOPacket->BufLen = 0;
	    break;
	}

        case IO_STARTDEVICE:
	{
            if ( pdxc->BoardInfo.Capability & BOARD_CAP_AUDIO )
            {
                pdxc->DoAudio = 1;
                if (pdxc->BoardInfo.RevisionNum == 0x0200 ||
                    pdxc->BoardInfo.RevisionNum == 0x0205 ||
                    pdxc->BoardInfo.RevisionNum == 0x0207 ||
                    pdxc->BoardInfo.RevisionNum == 0x0209 ||
                    pdxc->BoardInfo.RevisionNum == 0x0211)
                    pdxc->DoAudio = 2;
            }
            else
                pdxc->DoAudio = 0;

            pdxc->TotalVideoChannel = 1;
            pdxc->DoVideo = 1;

            AllocateBuffers(pdxc);
            r = InitializeDevice(pdxc);
            pIOPacket->Param1 = r;
            pIOPacket->BufLen = 0;
            break;
	}

        case IO_STOPDEVICE:
	{
            PrintMsg(DEBUG_PROPERTY, ("IO_STOPDEVICE"));
            r = UninitializeDevice(pdxc);
            ReleaseBuffers(pdxc);
            pIOPacket->Param1 = r;
            pIOPacket->BufLen = 0;
            break;
	}

        case IO_SET_TV_STANDARD:
        {
	    int i ;
	    unsigned int tvstandard = pIOPacket->Param1;
	    r = -1;
	    PrintMsg(DEBUG_PROPERTY, ("IO_SET_TV_STANDARD: %d options for this board module\n",
		   pdxc->config_manager._num_of_system_configs));
	    PrintMsg(DEBUG_PROPERTY, ("IO_SET_TV_STANDARD: standard=0x%x\n", tvstandard));

	    for ( i = 0;
		  i < pdxc->config_manager._num_of_system_configs;
		  i++ )
            {
		PrintMsg(DEBUG_PROPERTY, ("IO_SET_TV_STANDARD: config[%d]: 0x%x\n",i,
		       pdxc->config_manager._system_configs[i].tv_standard));

		if ( tvstandard ==
		     pdxc->config_manager._system_configs[i].tv_standard )
                {
		    PrintMsg(DEBUG_PROPERTY, ("IO_SET_TV_STANDARD: match: config[%d]: 0x%x\n",i,
			  pdxc->config_manager._system_configs[i].tv_standard));

		    pdxc->config_manager._active_system_config = i;

                        memcpy(&pdxc->config_manager._video_config.syscfg,
                               &pdxc->config_manager._system_configs[i],
                               sizeof(TCFGSYSTEM));
                        DumpSystemConfig(&pdxc->config_manager
                                         ._video_config.syscfg);
                        r= pdxc->BoardInterface->pIBoardVideo->
                          InitializeSensor(
                                    pdxc->BoardContext,
                                    pdxc,
                                    TVStandard_Default,
				                    VS_DEFAULT,
				                    pdxc->config_manager._sensor_config);
                        PrintMsg(DEBUG_PROPERTY, ("Using system config index[%d]\n", i));
                        break;
                    }
                }
                pIOPacket->Param2 = r;
                break;
            }

            case IO_SETAUDIO:
	    {
                int audio_mode;
                int current_mode;

#if defined(__AVSYNC__)
	PrintMsg(DEBUG_PROPERTY, ("the audio function not support\n"));

#else

				//  PrintMsg(DEBUG_PROPERTY, ("set audio%d\n",audio_mod)); 
                audio_mode= (int)pIOPacket->Param1;

                current_mode = pdxc->ReadAudioStreamState;

                PrintMsg(DEBUG_PROPERTY, ("wis%d: set audiostate=%d\n",pdxc->unit, audio_mode));

                if ( audio_mode==0 )
                {
                    pdxc->ReadAudioStreamState=-1;
                }
                else if ( audio_mode==1 )
                    pdxc->ReadAudioStreamState=1;
                else if ( audio_mode==2 )
                    pdxc->ReadAudioStreamState=0;

                pIOPacket->Param1 = current_mode;
#endif
                break;      
            }
            case IO_READCBUSREGFW:
            {
                unsigned short addr = (unsigned short) pIOPacket->Param1;
                unsigned short data;
                r  = -1;
                while ( r != 0 )
                {
                    r = ReadCBusRegFW(pdxc,
                                      addr,
                                      &data);
                }
                pIOPacket->Param1 = r;
                pIOPacket->Param2 = data;
                break;
            }
            case IO_WRITECBUSREGFW:
            {
                unsigned short addr = (unsigned short)pIOPacket->Param1;
                unsigned short data = (unsigned short)pIOPacket->Param2;
                r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
                                                    addr|0x8000, data);
                pIOPacket->Param1 = r;
                break;
            }
            case IO_I2CWRITEREGISTER:
	    {
	      UINT16 regAdd = (unsigned short)pIOPacket->Param1;
	      UINT8 regData = (unsigned short)pIOPacket->Param2;
	      
	      PrintMsg(DEBUG_PROPERTY, ("IO_I2CWRITEREGISTER regAdd=%x,regData=%x\n",
		     regAdd,regData)); 
	      r = I2C_WriteRegister(pdxc,0xa0,8,regAdd, regData,0);
	      pIOPacket->Param1 = r;
	      pIOPacket->BufLen = 0;
	      break;
	    }
/*
    case IO_READHPIREGISTER:
    {
        UINT16 Data;
        r = ReadHPIRegister(pdxc, pIOPacket->Param1, &Data);
        pIOPacket->Param1 = r;
        pIOPacket->Param2 = Data;
        pIOPacket->BufLen = 0;
        break;
    }

    case IO_WRITEHPIREGISTER:
        r = 0;//WriteHPIRegister(pdxc, pIOPacket->Param1, pIOPacket->Param2);
        pIOPacket->Param1 = r;
        pIOPacket->BufLen = 0;
        break;

    case IO_I2CREADREGISTER:
    {
        UINT8 dev_addr;
        SINT32 read_num, i2c_mode, addr_len;
        UINT16 reg_addr;
        UINT8 *data;

        i2c_mode = pIOPacket->Buf[0];
        dev_addr = pIOPacket->Buf[1];
        addr_len = pIOPacket->Buf[2];
        reg_addr = GETUINT16(pIOPacket->Buf, 3);
        read_num = GETUINT16(pIOPacket->Buf, 5);

        data = pIOPacket->Buf;

        if(read_num == 1)
            r = I2C_ReadRegister(pdxc, dev_addr, addr_len, reg_addr, data, i2c_mode);
        else
            r = 1; // Burst read

        pIOPacket->Param1 = r;
        pIOPacket->BufLen = read_num;
        break;
    }

    case IO_SPIREADREGISTER:
    {
        SINT32 op_len, addr_len, data_len, spi_mode, read_num, r;
        UINT8  op_code;
        UINT16 reg_addr, *reg_data;

        spi_mode = GETUINT16(pIOPacket->Buf, 0);
        op_len   = pIOPacket->Buf[2];
        op_code  = pIOPacket->Buf[3];
        addr_len = pIOPacket->Buf[4];
        reg_addr = GETUINT16(pIOPacket->Buf, 5);
        data_len = pIOPacket->Buf[7];
        read_num = GETUINT16(pIOPacket->Buf, 8);
        reg_data = (UINT16 *)(pIOPacket->Buf);

        if(read_num == 1)
            r = SPI_ReadRegister(pdxc, op_len, op_code, addr_len, reg_addr, data_len, reg_data, spi_mode);
        else
            r = 1; // Burst read

        pIOPacket->Param1 = r;
        pIOPacket->BufLen = read_num*2;

        break;
    }

    case IO_SPIWRITEREGISTER:
    {
        SINT32 op_len, addr_len, data_len, spi_mode, write_num, r;
        UINT8  op_code;
        UINT16 reg_addr, *reg_data;

        spi_mode = GETUINT16(pIOPacket->Buf, 0);
        op_len   = pIOPacket->Buf[2];
        op_code  = pIOPacket->Buf[3];
        addr_len = pIOPacket->Buf[4];
        reg_addr = GETUINT16(pIOPacket->Buf, 5);
        data_len = pIOPacket->Buf[7];
        write_num = GETUINT16(pIOPacket->Buf, 8);
        reg_data = (UINT16 *)(pIOPacket->Buf+12);

        if(write_num == 1)
            r = SPI_WriteRegister(pdxc, op_len, op_code, addr_len, reg_addr, data_len, reg_data[0], spi_mode);
        else
            r = 1; // Burst read

        pIOPacket->Param1 = r;
        pIOPacket->BufLen = 0;
        break;
    }

    case IO_READINTERRUPT:
    {
        UINT16 IntVal, IntData;
        r = ReadInterrupt(pdxc, &IntVal, &IntData);
        if( r == 0)
        {
            pIOPacket->Param2 = (((UINT32)IntVal)<<16) | (IntData & 0x0000ffff);
            pIOPacket->Param1 = 0;
        }
        else
        {
            pIOPacket->Param1 = 1;
        }
        pIOPacket->BufLen = 0;
        break;
    }

    case IO_WRITEINTERRUPT:
        r = WriteInterrupt(pdxc, (UINT16)pIOPacket->Param1, (UINT16)pIOPacket->Param2);
        if( r == 0)
            pIOPacket->Param1 = 0;
        else
            pIOPacket->Param1 = 1;
        pIOPacket->BufLen = 0;
        break;

    case IO_SENDPACKET:
        r = DownloadBuffer(pdxc, pIOPacket->Buf, pIOPacket->BufLen);
        if( r == 0)
            pIOPacket->Param1 = 0;
        else
            pIOPacket->Param1 = 1;
        pIOPacket->BufLen = 0;
        break;

    case IO_READSTREAMPIPE:
        r = ReadStream(pdxc, pIOPacket->Buf, pIOPacket->BufLen);
        if( r == 0)
            pIOPacket->Param1 = 0;
        else
        {
            pIOPacket->Param1 = 1;
            pIOPacket->BufLen = 0;
        }
        break;

    case IO_ADVANCED:
    {
        int Type = pIOPacket->Param1;

        if(Type==0) // set power state
        {
#if defined(_WINDOWS)
            pIOPacket->Param1 = SetPowerState(pdxc, pIOPacket->Param2);
#else 
            pIOPacket->Param1 = 1;
#endif
            pIOPacket->BufLen = 0;
        }
        else
        {
            pIOPacket->Param1 = 1;
            pIOPacket->BufLen = 0;
        }
        break;
    }
    */
        case IO_GET_BOARD_TYPE:
	{
	  if ( copy_to_user((void*)pIOPacket->Buf,
			    &pdxc->BoardInfo,
			    sizeof(BOARD_INFO)) )
	    return -EFAULT;
	  pIOPacket->BufLen = sizeof(BOARD_INFO);
	  pIOPacket->Param1 = 0;
	  break;
        }

        case IO_FORCE_IFRAME:
        {
	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
						FORCE_I_FRAME,1);
	    pIOPacket->Param2 = r;
	    break;
	}

        case IO_CHANGE_PFRAME_RATE:
        {
	    unsigned short rate = (unsigned short)pIOPacket->Param1;
	    
	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
						CHANGE_PFRAME_RATE,rate);

	    pIOPacket->Param2 = r;      
	    break;
	}

        case IO_CHANGE_IFRAME_QUANTIZER:
        {

	    unsigned short iqscale = (unsigned short)pIOPacket->Param1;
	    

	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
						CHANGE_I_Q_SCALE,iqscale);

	    pIOPacket->Param2 = r;

	    break;
	}
        case IO_CHANGE_PFRAME_QUANTIZER:
        {
	    unsigned short pqscale = (unsigned short)pIOPacket->Param1;
	    

	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
						CHANGE_P_Q_SCALE,pqscale);

	    pIOPacket->Param2 = r;

	    break;
	}

        case IO_INSERT_SEQUENCE_HEADER_AT_NEXT_GOP:
        {
	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc,
						FORCE_NEW_SEQUENCE,1);
	    pIOPacket->Param2 = r;   

	    break;
	}

        case IO_SET_FPS:
        {
	    change_fps_otf(pdxc, pIOPacket->Param1);
	    set_seq_end(&(pdxc->StreamBuffer));
	    pIOPacket->Param2 = pIOPacket->Param1;
	    /*int fps = pIOPacket->Param1;        
	    int actual_fps;
	    unsigned short fps_level;
	    
	    set_fps(pdxc->unit, NTSC, fps, &actual_fps, &fps_level);

	    //if ( stc->deinterlace_mode==0 && vc->syscfg.format==1 )
	    if(((UINT32)(vc->rescfg.height*2)<=
		(UINT32)vc->syscfg.sensor_v) && vc->syscfg.format==1)
		fps_level=2*fps_level+1;

	    r = (pdxc->hw_funcs.WriteInterrupt)(pdxc, SET_FPS, fps_level);

	    pIOPacket->Param2 = actual_fps;     
	    // update frame rate and BRC parameters if necessary
	    fc->frame_rate = actual_fps * 1000;
	    if ( bc->target_bitrate>0 )
		change_brc_otf(pdxc,
			       3,
			       0,
			       0,
			       bc->target_bitrate,
			       bc->peak_bitrate,
			       bc->vbv_buffer,
			       bc->converge_speed,
			       bc->lambda);*/
	    break;
	}

	case IO_SET_BRIGHTNESS: /*added by Wiseway 2004-03-16*/
		{
			unsigned int brightness_value;			
	    /* Device only provided 4  bytes of data in buf pointer */
		    if ( copy_from_user(&brightness_value, (void*)pIOPacket->Buf, sizeof(int)) )
			return -EFAULT;

	    		PrintMsg(DEBUG_PROPERTY, ("IO_SET_BRIGHTNESS: (original) %d\n", brightness_value));      

		    if ( brightness_value>=0 && brightness_value <=100)
       	     {
			pdxc->BoardInterface->
                    	pIBoardVideo->SetVideoProperty(&pdxc->BoardContext,
                                                   (AVSTREAM_COOKIE)pdxc,
                                                   PROP_VIDEO_BRIGHTNESS,
                                                   brightness_value, NULL);

			PrintMsg(DEBUG_PROPERTY, (" Set brightness_value=0x%x\n",brightness_value));			
	    		}

	    	if ( copy_to_user((void*)pIOPacket->Buf, &brightness_value, sizeof(int)) )
		return -EFAULT;
	    break;

		}		
	case IO_SET_CONTRAST: /*added by Wiseway 2004-03-16*/
		{
			unsigned int contrast_value;			
	    /* Device only provided 4  bytes of data in buf pointer */
		    if ( copy_from_user(&contrast_value, (void*)pIOPacket->Buf, sizeof(int)) )
			return -EFAULT;

	    		PrintMsg(DEBUG_PROPERTY, ("IO_SET_CONTRAST: (original) %d\n", contrast_value));      

		    if ( contrast_value>=0 && contrast_value <=100)
       	     {			
			pdxc->BoardInterface->
                    	pIBoardVideo->SetVideoProperty(&pdxc->BoardContext,
                                                   (AVSTREAM_COOKIE)pdxc,
                                                   PROP_VIDEO_CONTRAST,
                                                   contrast_value, NULL);
			
			PrintMsg(DEBUG_PROPERTY, (" Set contrast_value=0x%x\n", contrast_value));
			}

	    	if ( copy_to_user((void*)pIOPacket->Buf, &contrast_value, sizeof(int)) )
		return -EFAULT;
	    break;

		}		


	case IO_SET_HUE: /*added by Wiseway 2004-03-16*/
		{
			int hue_value;
			
	    /* Device only provided 4  bytes of data in buf pointer */
		    if ( copy_from_user(&hue_value, (void*)pIOPacket->Buf, sizeof(int)) )
			return -EFAULT;

	    		PrintMsg(DEBUG_PROPERTY, ("IO_SET_HUE: (original) %d\n", hue_value));      

		    if ( hue_value>=-50 && hue_value <=50)
       	     {
			pdxc->BoardInterface->
                    	pIBoardVideo->SetVideoProperty(&pdxc->BoardContext,
                                                   (AVSTREAM_COOKIE)pdxc,
                                                   PROP_VIDEO_HUE,
                                                   hue_value, NULL);

			PrintMsg(DEBUG_PROPERTY, (" Set hue_value=0x%x\n", hue_value));
		}

	    	if ( copy_to_user((void*)pIOPacket->Buf, &hue_value, sizeof(int)) )
		return -EFAULT;
	    break;

		}		

	case IO_SET_SATURATION: /*added by Wiseway 2004-03-16*/
		{
			unsigned int saturation_value;
			
	    /* Device only provided 4  bytes of data in buf pointer */
		    if ( copy_from_user(&saturation_value, (void*)pIOPacket->Buf, sizeof(int)) )
			return -EFAULT;

	    		PrintMsg(DEBUG_PROPERTY, ("IO_SET_SATURATION: (original) %d\n", saturation_value));      

		    if ( saturation_value>=0 && saturation_value <=100)
       	     {			
			pdxc->BoardInterface->
                    	pIBoardVideo->SetVideoProperty(&pdxc->BoardContext,
                                                   (AVSTREAM_COOKIE)pdxc,
                                                   PROP_VIDEO_SATURATION,
                                                   saturation_value, NULL);

			PrintMsg(DEBUG_PROPERTY, (" Set saturation_value=0x%x\n", saturation_value));				
	    		}

	    	if ( copy_to_user((void*)pIOPacket->Buf, &saturation_value, sizeof(int)) )
		return -EFAULT;
	    break;

		}	

        case IO_SET_BITRATE:
        {
	    SINT32 change_direction;
	    SINT32 new_i_qscale;
	    SINT32 new_p_qscale;
	    SINT32 new_target_bitrate;
	    SINT32 new_peak_rate;
	    SINT32 new_vbv_buffer;
	    SINT32 new_converge_speed;
	    SINT32 new_lambda;
	    

	    if ( copy_from_user(&change_direction,
				(void*)pIOPacket->Buf + 0*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_i_qscale,
				(void*)pIOPacket->Buf + 1*sizeof(int),
				sizeof(int)) )
		return -EFAULT;


	    if ( copy_from_user(&new_p_qscale,
				(void*)pIOPacket->Buf + 2*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_target_bitrate,
				(void*)pIOPacket->Buf + 3*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_peak_rate,
				(void*)pIOPacket->Buf + 4*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_vbv_buffer,
				(void*)pIOPacket->Buf + 5*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_converge_speed,
				(void*)pIOPacket->Buf + 6*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    if ( copy_from_user(&new_lambda,
				(void*)pIOPacket->Buf + 7*sizeof(int),
				sizeof(int)) )
		return -EFAULT;

	    r = change_brc_otf(pdxc,
			       change_direction,
			       new_i_qscale,
			       new_p_qscale,
			       new_target_bitrate,
			       new_peak_rate,
			       new_vbv_buffer,
			       new_converge_speed,
			       new_lambda);

	    if ( r==0 )
            { // if BRC OTF successful, update BRC parameters
		bc->IQ=new_i_qscale;
		bc->PQ=new_p_qscale;
		bc->target_bitrate=new_target_bitrate;
		bc->peak_bitrate=new_peak_rate;
		bc->vbv_buffer=new_vbv_buffer;
		bc->converge_speed=new_converge_speed;
		bc->lambda=new_lambda;
	    }

	    pIOPacket->Param1 = r;
	    break;
	    
	}

        case IO_INIT_MD:
        {
	    r = ED_MDInit();
	    
	    md_running[pdxc->unit] = 1;
	    
	    pIOPacket->Param1 = r;
	    break;
	}

        case IO_SET_MD_THRESHOLDS_AND_SENSITIVITIES:
        {
	    /* sensitivity array starts at the beginning */
	    UINT16 arru8MotionSensitivity[MAX_REGIONS_OF_INTEREST];
	    
	    /* thresholds array starts at the offset of the # of regions */
	    /* of interest that we have.  For now assume that it is aligned */
	    UINT16 arrs16MotionThresholds[MAX_REGIONS_OF_INTEREST][NUM_MOTION_TYPES];

	    /* copy the user supplied motion thresholds and
	     * sensitivities based on how this ioctl should be called */
	    if ( copy_from_user(arru8MotionSensitivity,
				pIOPacket->Buf,
				sizeof(UINT16)*MAX_REGIONS_OF_INTEREST) )
		return -EFAULT;

	    if ( copy_from_user(arrs16MotionThresholds,
				pIOPacket->Buf+sizeof(UINT16)*MAX_REGIONS_OF_INTEREST,
				sizeof(UINT16)*MAX_REGIONS_OF_INTEREST*NUM_MOTION_TYPES) )
            {
		pIOPacket->Param1 = -EFAULT;
	    }
	    else
            {
		r = ED_SetMDThresholdsAndSensitivity(arrs16MotionThresholds,
						     arru8MotionSensitivity,
						     pdxc);
		pIOPacket->Param1 = r;
	    }
	    break;
	}

        case IO_SET_MD_REGIONS:
        {
	    UINT32 u32MaxXCoordinate, u32MaxYCoordinate;
	    UINT8 arru8MotionCoordinates[MAX_NUM_MACROBLOCKS];
	    
	    /* MAX X coordinate is at bytes 0 to 3 */
	    /* MAX Y coordinate is at bytes 4 to 7 */
	    /* finally get the motion coordinates list */
	    if ( copy_from_user(&u32MaxXCoordinate, &pIOPacket->Buf[0],
				sizeof(UINT32)) ||
		 copy_from_user(&u32MaxYCoordinate, &pIOPacket->Buf[4],
				sizeof(UINT32)) ||
		 copy_from_user(arru8MotionCoordinates, &pIOPacket->Buf[8],
				MAX_NUM_MACROBLOCKS) )
                {
                    pIOPacket->Param1 = -EFAULT;
                }
                else
                {
                    PrintMsg(DEBUG_PROPERTY, ("maxx=%d, maxy=%d\n",
                           u32MaxXCoordinate,
                           u32MaxYCoordinate));
                    /* now actually write the regions to the chip */
                    r = ED_SetMDRegions(arru8MotionCoordinates,
                                        u32MaxXCoordinate,
                                        u32MaxYCoordinate, pdxc);

                    pIOPacket->Param1 = r;
                    pdxc->MDInterruptFlag = 1;
                    pIOPacket->BufLen = 0;
                }

                break;
	}

        case IO_WAIT_MD_EVENT:
        {
	    UINT16 IntVal, IntData;
	    
	    PrintMsg(DEBUG_PROPERTY, ("Wait for MD interrupt****"));
//	    printk(KERN_INFO "MD: wait for interrupt\n");
	    pdxc->CheckInterruptType =MOTION_DETECTION_INTERRUPT;
	    pdxc->CheckInterruptData = -1;
	    r = (pdxc->hw_funcs.ReadInterrupt)(pdxc, &IntVal, &IntData);
	    
	    if ( !r )
            {
		PrintMsg(DEBUG_PROPERTY,
			 ("Enable MD interrupt***** got event %x",
			  IntVal));
		if ( (IntVal & ENCODER_RETURN_VALUE_MASK)
		     != MOTION_DETECTION_INTERRUPT )
                {
		    r = 2;
		}
	    }

	    if (r==0)
            {
		md_region_detections[pdxc->unit] = IntData; /* Save last */
	    }
	    else
            {
		md_region_detections[pdxc->unit] = 0;
	    }
	    pIOPacket->Param1 = r;
	    pIOPacket->Param2 = (r==0)? IntData:0;
	    pIOPacket->BufLen = 0;
	    break;
	}
        case IO_MD_STOP:
        {
	    int i;
	    /* Reset state array */
	    for ( i = 0; i < MAX_DEVICES; i++ )
            {
		md_region_detections[i] = 0;
		md_running[pdxc->unit] = 0;     
	    }
	    break;
	}

		case IO_OSD_DISPLAY:
	{
		unsigned char i=0,j,strlen;
		osd_string_t osd_str[2];//={
		copy_from_user(&osd_str[0], &pIOPacket->Buf[0], pIOPacket->BufLen);
		
		strlen = sizeof(osd_string_t);
		if (pIOPacket->Param1==0)
			clean_osd();
		if (pIOPacket->Param1==1)
		{
			j=0;
			while (j<pIOPacket->Param2)
			{
				osd_str[j].x=pIOPacket->Buf[j*strlen];
				osd_str[j].y=pIOPacket->Buf[j*strlen+1];
				i=0;
				while (pIOPacket->Buf[j*strlen+2+i]!=0)
				{
					osd_str[j].osd_string[i]=pIOPacket->Buf[j*strlen+2+i];
					i++;
				}
				osd_str[j].osd_string[i++]=0;
				
				PrintMsg(DEBUG_PROPERTY,("Stored x=%d, y=%d, strings %s,%d \n",
				osd_str[j].x,osd_str[j].y,&osd_str[j].osd_string[0],pIOPacket->BufLen));//,&OsdStr[0].String));
				
				j++;
			}
			
			get_osd_frame(pIOPacket->Param2,&osd_str[0]);
		}
		
		if (pIOPacket->Param1==2)
			show_osd_frame(0);
		
		break;
		}
	case IO_AV_SYNC_SETTING:
	{
 #if defined(__AVSYNC__)
		SetAVSyncPara(pdxc,pIOPacket->Param1, pIOPacket->Param2);
#endif
		break;
	}

    case IO_GET_AVSTATS:
      {
	AVStats avs;
	unsigned long long tmp;
			
	StreamBuffer_GetStatistics(&(pdxc->StreamBuffer), 
			     &(avs.V_acquired), 
			     &(avs.V_dropped), 
			     &(avs.V_duplicated),
			     &tmp);
	CAudioStreamBuffer_GetStatistics(pdxc->AudioStreamBuffer, 
				   &(avs.A_dropped), 
				   &(avs.A_duplicated), 
				   &tmp);
	avs.A_acquired = (unsigned long)tmp;

	pIOPacket->BufLen = sizeof(AVStats);
	if ( copy_to_user((void*)pIOPacket->Buf, &avs, pIOPacket->BufLen) )
	  return -EFAULT;

	pIOPacket->Param1 = 0;

	break;
      }
    case IO_GET_SEQHEADER:
      {
	// return the seq header saved during setup - 2 byte length (in bits) followed by data
	pIOPacket->BufLen = ((UINT8)(*pdxc->pSeqHeader)|((UINT8)(*pdxc->pSeqHeader+1))<<8)+7 >> 3;
	if ( copy_to_user((void*)pIOPacket->Buf, (pdxc->pSeqHeader + 2), pIOPacket->BufLen) )
	  return -EFAULT;
	
	pIOPacket->Param1 = 0;

	break;
      }

		           /* Fall through */
    default:

	PrintMsg(DEBUG_ALWAYS_DISPLAY,
		 ("<DRV> ProcessIOCTL : Unknown Packet Type = %d",
		  pIOPacket->Type));
	pIOPacket->BufLen = 0;
	*ret_len = 4*sizeof(SINT32) + pIOPacket->BufLen;
	return 1;
    }

    *ret_len = 4*sizeof(SINT32) + pIOPacket->BufLen;
    return 0;
}

#define MD_SYNC_WORD    0x000001F8
#define MD_SYNC_WORD_BYTE0  0x00
#define MD_SYNC_WORD_BYTE1  0x00
#define MD_SYNC_WORD_BYTE2  0x01
#define MD_SYNC_WORD_BYTE3  0xF8

#define MD_SYNC_WORD_SIZE   4


// return : 0  -- got a frame
//          >0 -- no new frame
int ReadOneVideoFrame(PDEVICE_EXTENSION_COMMON pdxc,
                      UINT8 *pBuffer, SINT32 *buf_len)
{
    FRAME_INFO frmInfo;
    TFrameInfo FI;
    NTSTATUS ntStatus;
    int ii, rr, bMotionMacroblockMapFound;

    bMotionMacroblockMapFound = FALSE;

    /* initialize it to false to indicate there is no map in case of
     * an error return
     */
    FI.MDMapFlag = FALSE;

    //  PrintMsg(DEBUG_ALWAYS_DISPLAY,
    //  ("******* ProcessReadRequest %d", *buf_len));

    if ( pdxc->DeviceConnected == 0 )
        return 1;

    frmInfo._frame_buffer = pBuffer;
    frmInfo._map_buffer = (UINT8*)FI.MDMacroblockMap;
    frmInfo._max_frame_length = *buf_len;

    frmInfo._vbi_buffer = pdxc->pVBIBuffer;
    frmInfo._max_vbi_length = 4096;

    ntStatus = StreamBuffer_GetChannelFrame(&(pdxc->StreamBuffer),
                                            0,&frmInfo);

//  PrintMsg(DEBUG_ALWAYS_DISPLAY,
//             ("<DRV> ******* Read a frame return %08x", ntStatus));

    if ( ntStatus == STATUS_BUFFER_TOO_SMALL )
    {
        PrintMsg(DEBUG_ALWAYS_DISPLAY,
                 ("<DRV> ******* Read a frame buffer too small 1"));
        return 1;
    }
    // no frame in buffer, need waiting
    else if ( ntStatus == STATUS_UNSUCCESSFUL )
    {
        return 4;

        PIResetEvent(&(pdxc->StreamReady));
        rr = PIWaitEvent(&(pdxc->StreamReady), 5000);

        if ( rr != 0 )  // time out
        {
            PrintMsg(DEBUG_ALWAYS_DISPLAY,
                     ("<DRV> ******* Read a frame timeout"));
            return 2;
        }
        else if ( pdxc->ReadStreamState==0 )       // stop capturing
        {
            return 3;
        }
        else
        {
            frmInfo._frame_buffer = pBuffer;
            frmInfo._max_frame_length = *buf_len;

            frmInfo._vbi_buffer = pdxc->pVBIBuffer;
            frmInfo._max_vbi_length = 4096;

            ntStatus = StreamBuffer_GetChannelFrame(&(pdxc->StreamBuffer),0,&frmInfo);
            if ( ntStatus == STATUS_BUFFER_TOO_SMALL )
            {
                PrintMsg(DEBUG_ALWAYS_DISPLAY,
                         ("<DRV> ******* Read a frame buffer too small 2"));
                return 1;
            }
            else if ( ntStatus != STATUS_SUCCESS )
            {
                PrintMsg(DEBUG_ALWAYS_DISPLAY,
                         ("******* Read a frame other error %08x",
                          (int)ntStatus));
                return 4;
            }
        }
    }

    else if ( ntStatus != STATUS_SUCCESS )
    {
        PrintMsg(DEBUG_ALWAYS_DISPLAY,
                 ("******* Read a frame other error %08x",
                  (int)ntStatus));
        return 4;
    }

    if ( *buf_len < (SINT32)(frmInfo._actual_frame_length + frmInfo._actual_vbi_length + sizeof(TFrameInfo)) )
    {
        PrintMsg(DEBUG_ALWAYS_DISPLAY, ("******* Read a frame buffer too small 3"));
        return 4;
    }
    //  PrintMsg(DEBUG_ALWAYS_DISPLAY, ("!!!!!!! %d %d %d %d",frmInfo._actual_frame_length,frmInfo._actual_vbi_length,frmInfo._vip_number,frmInfo._frame_type));

    FI.VideoLength = frmInfo._actual_frame_length;
    FI.VBILength = frmInfo._actual_vbi_length;
    FI.AudioFingerprint = frmInfo._finger_print;
    FI.VIPNumber = frmInfo._vip_number;
    FI.AudioTimestamp = frmInfo._timestamp;
    FI.FrameType = frmInfo._frame_type;

    /* if the motion detection unit is running, make the region information */
    /* available to the user, and check for a macroblock map at the end of  */
    /* the stream. */
    if ( md_running[pdxc->unit] )
    {
        FI.MDRegionMap = md_region_detections[pdxc->unit];
        md_region_detections[pdxc->unit] = 0;

        /* since we assigned the _map_buffer pointer to the TFrameInfo */
        /* element above, at this point, if there was a map, the data  */
        /* is already in the MDMacroblockMap element.                  */
        if (frmInfo._flag & MAP_PRESENT)
        {
            /* when the map is present, the region info is appended to the */
            /* end of the macroblock map in the extra slot */
            FI.MDRegionMap = FI.MDMacroblockMap[MAP_REGION_INFO_OFFSET];
            /* always swap the region map.  The host may re-swap if */
            /* the endian-ness is incorrect */
        FI.MDRegionMap = (((FI.MDRegionMap&0xff00)>>8)
                  | ((FI.MDRegionMap&0xff) << 8));
            FI.MDMapFlag = TRUE;
        }
    }
    else 
    {
    /*
     * Motion detection is not running, so if a map appears, get
     * rid of it
     */ 
        for ( ii=0; ii < MACROBLOCK_MAP_SIZE_WORDS; ii++ )
        {
            FI.MDMacroblockMap[ii] = 0;
        }

        FI.MDMapFlag = FALSE;
        FI.MDRegionMap = 0; /* No MD */
    }

	FI.frame_seq_pos=frmInfo._frame_sequence_position;
        /*2005-04-05 delete*/
/*if((frmInfo._frame_sequence_position==2)||(frmInfo._frame_sequence_position==0 ))
	printk("device ctl: seq_position=%d vbi_length=%d\n",frmInfo._frame_sequence_position,frmInfo._max_vbi_length);*/
        /*2005-04-05 end*/
 
    /*2005-03-30 modified*/
    //PICopyMem(pBuffer+frmInfo._actual_frame_length,
    //      pdxc->pVBIBuffer, frmInfo._actual_vbi_length);
    copy_to_user(pBuffer+frmInfo._actual_frame_length,
          pdxc->pVBIBuffer, frmInfo._actual_vbi_length);
   /*2005-03-30 end*/
/*    
    PICopyMem(pBuffer+frmInfo._actual_frame_length+frmInfo._actual_vbi_length,
          &FI, sizeof(TFrameInfo));
*/

    copy_to_user(pBuffer+frmInfo._actual_frame_length+frmInfo._actual_vbi_length,
          &(frmInfo._video_timestamp), sizeof(struct timeval));
    copy_to_user(pBuffer+frmInfo._actual_frame_length+frmInfo._actual_vbi_length+sizeof(struct timeval),
          &FI, sizeof(TFrameInfo));

/*
    *buf_len = frmInfo._actual_frame_length +
    frmInfo._actual_vbi_length + sizeof(TFrameInfo);
*/
    *buf_len = frmInfo._actual_frame_length +
    frmInfo._actual_vbi_length + sizeof(struct timeval) + sizeof(TFrameInfo) ;

    return 0;
}

int ReadAudioSamples(PDEVICE_EXTENSION_COMMON pdxc,
                     UINT8 *pBuffer, SINT32 *buf_len)
{
    int lReturn;
    lReturn = CAudioStreamBuffer_GetData(pdxc->AudioStreamBuffer,
                                         pBuffer, *buf_len, 0);

    if ( lReturn > 0 )
    {
        *buf_len = lReturn;
        return 0;
    }

    return 1;
}

int ProcessReadRequest(PDEVICE_EXTENSION_COMMON pdxc,
                       UINT8 *pBuffer, SINT32 *buf_len)
{
#if 1  /* Jimmy */
    if ( *buf_len < 7000 )
#else
    if ( *buf_len%10==0 )
#endif
        return ReadAudioSamples(pdxc, pBuffer, buf_len);
    else
        return ReadOneVideoFrame(pdxc, pBuffer, buf_len);
}

