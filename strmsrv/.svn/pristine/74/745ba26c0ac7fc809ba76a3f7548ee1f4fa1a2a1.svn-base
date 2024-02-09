
#include <sys/stat.h>

#include "wisencrx.hh"

CMutex CWisEncRx::m_nPortLocks[MAX_WIS_PORTS+1];

// Initialize default config params
// change this to read from file instead
// following values taken exactly from default_setting.txt in driver SupportFiles
void CWisEncRx::InitConfig(void)
{
    m_nContrast                     = 0;
    m_nHue                          = 0;
    m_nSaturation                   = 0;
    m_nBrightness                   = 0;

    m_AudioConfig.Format          = 1;
    m_AudioConfig.SampleRate      = 48000;
    m_AudioConfig.Channels        = 2;
    m_AudioConfig.SampleBits      = 16;
    m_AudioConfig.BlockAlign      = 4;
    m_AudioConfig.AvgBytesPerSec  = 192000;
    m_AudioConfig.SamplesPerBlock = 0;
    m_AudioConfig.ExtSize         = 0;

    m_VideoConfig.misccfg.iip_enable          = 0;
    m_VideoConfig.misccfg.vbi_enable          = 0;
    m_VideoConfig.misccfg.four_channel_enable = 0;
    m_VideoConfig.misccfg.h_filter_mode       = GO7007SB_NOFILTER;  //0;
    m_VideoConfig.misccfg.v_filter_mode       = GO7007SB_NOFILTER;  //0;
    m_VideoConfig.misccfg.filter_nAX          = 0;
    m_VideoConfig.misccfg.filter_nBX          = 0;
    m_VideoConfig.misccfg.filter_nCX          = 0;
    m_VideoConfig.misccfg.filter_nAY          = 0;
    m_VideoConfig.misccfg.filter_nBY          = 0;
    m_VideoConfig.misccfg.filter_nCY          = 0;
    m_VideoConfig.misccfg.reserved            = 0;

    m_VideoConfig.strcfg.compress_mode        = MPEG4;  //4;
    m_VideoConfig.strcfg.sequence             = IPONLY;    //2;
    m_VideoConfig.strcfg.gop_mode             = 1;
    m_VideoConfig.strcfg.gop_size             = 30;
    m_VideoConfig.strcfg.mpeg4_mode           = 1;
    m_VideoConfig.strcfg.deinterlace_mode     = 1;
    m_VideoConfig.strcfg.search_range         = 128;
    m_VideoConfig.strcfg.gop_head_enable      = 1;
    m_VideoConfig.strcfg.seq_head_enable      = 1;
    m_VideoConfig.strcfg.aspect_ratio         = 1;
    m_VideoConfig.strcfg.DVD_compliant        = 0;
    m_VideoConfig.strcfg.reserved             = 1;

    m_VideoConfig.rescfg.tv_standard          = TVStandard_NTSC_Mask;  //2055;
    m_VideoConfig.rescfg.width                = 720;
    m_VideoConfig.rescfg.height               = 480;
    m_VideoConfig.fpscfg.frame_rate           = 30000;

    m_VideoConfig.rescfg.h_sub_window         = 1;
    m_VideoConfig.rescfg.v_sub_window         = 1;
    m_VideoConfig.rescfg.h_sub_offset         = 0;
    m_VideoConfig.rescfg.v_sub_offset         = 0;
    m_VideoConfig.rescfg.h_scale_enb          = 0;
    m_VideoConfig.rescfg.v_scale_enb          = 0;
    m_VideoConfig.rescfg.sub_sample           = 0;

    m_VideoConfig.ctlcfg.target_bitrate       = 2000000;
    m_VideoConfig.ctlcfg.peak_bitrate         = 2400000;
    m_VideoConfig.ctlcfg.vbv_buffer           = 2000000;
    m_VideoConfig.ctlcfg.converge_speed       = 50;
    m_VideoConfig.ctlcfg.lambda               = 100;
    m_VideoConfig.ctlcfg.Q                    = 0;
    m_VideoConfig.ctlcfg.IQ                   = 0;
    m_VideoConfig.ctlcfg.PQ                   = 0;
    m_VideoConfig.ctlcfg.BQ                   = 0;
    m_VideoConfig.ctlcfg.reserved             = 0;

    m_VideoConfig.fpscfg.tv_standard          = TVStandard_None;  //0;
    m_VideoConfig.fpscfg.drop_frame           = 0;
    m_VideoConfig.fpscfg.ivtc_enable          = 0;

    m_pVideoCaptureState                      = new char[1];
    *m_pVideoCaptureState                     = 1;

    memset(&m_VideoConfigFix, 0x0, sizeof(TVIDEOCFGFIX));

    m_nBrightness=DEFAULT_BRIGHTNESS;
    m_nContrast=DEFAULT_CONTRAST;
    m_nHue=DEFAULT_HUE;
    m_nSaturation=DEFAULT_SATURATION;

    return;
}

// Set the Fix config buffer
void CWisEncRx::SetFixConfig()
{
    // osd setting;

    m_VideoConfigFix.osdcfg.DoOSD = 1;
    m_VideoConfigFix.osdcfg.OSDyc0 = 0;
    m_VideoConfigFix.osdcfg.OSDyc1 = 255;
    m_VideoConfigFix.osdcfg.OSDuc0 = 0;
    m_VideoConfigFix.osdcfg.OSDuc1 = 128;
    m_VideoConfigFix.osdcfg.OSDvc0 = 0;
    m_VideoConfigFix.osdcfg.OSDvc1 = 128;

    // Threshold;

    m_VideoConfigFix.thcfg.THACCoeffSet0 = 0;
    m_VideoConfigFix.thcfg.THACCoeffStartpoint = 1;
    m_VideoConfigFix.thcfg.THedge = 20;
    m_VideoConfigFix.thcfg.THmotion = 30;
    m_VideoConfigFix.thcfg.THblending = 4;
    m_VideoConfigFix.thcfg.THbigedge = 900;
    m_VideoConfigFix.thcfg.THsmalledge = 300;
    m_VideoConfigFix.thcfg.THedgestatistics = 20;
    m_VideoConfigFix.thcfg.THmotionstatistics = 30;
    m_VideoConfigFix.thcfg.THbigedgestatistics = 900;
    m_VideoConfigFix.thcfg.THsmalledgestatistics = 300;

    // Deinterlace;

    m_VideoConfigFix.deintercfg.DeInter_Always_Motion = 0;
    m_VideoConfigFix.deintercfg.DeInter_Always_Blending = 1;
    m_VideoConfigFix.deintercfg.DeInter_Always_Weave = 0;

    // IIP setting;

    m_VideoConfigFix.iipcfg.Bit_Length = 10;
    m_VideoConfigFix.iipcfg.CCM_R_0 = 0;
    m_VideoConfigFix.iipcfg.CCM_R_1 = 0;
    m_VideoConfigFix.iipcfg.CCM_R_2 = 256;
    m_VideoConfigFix.iipcfg.CCM_R_3 = 0;
    m_VideoConfigFix.iipcfg.CCM_G_0 = 128;
    m_VideoConfigFix.iipcfg.CCM_G_1 = 0;
    m_VideoConfigFix.iipcfg.CCM_G_2 = 0;
    m_VideoConfigFix.iipcfg.CCM_G_3 = 128;
    m_VideoConfigFix.iipcfg.CCM_B_0 = 0;
    m_VideoConfigFix.iipcfg.CCM_B_1 = 256;
    m_VideoConfigFix.iipcfg.CCM_B_2 = 0;
    m_VideoConfigFix.iipcfg.CCM_B_3 = 0;
    m_VideoConfigFix.iipcfg.R_Gain = 288;
    m_VideoConfigFix.iipcfg.G_Gain = 256;
    m_VideoConfigFix.iipcfg.B_Gain = 270;
    m_VideoConfigFix.iipcfg.R_Offset = 0;
    m_VideoConfigFix.iipcfg.G_Offset = 0;
    m_VideoConfigFix.iipcfg.B_Offset = 0;
    m_VideoConfigFix.iipcfg.Gamma_R_P0_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_R_P0_Y = 0;
    m_VideoConfigFix.iipcfg.Gamma_R_P1_C = 32;
    m_VideoConfigFix.iipcfg.Gamma_R_P1_Y = 7;
    m_VideoConfigFix.iipcfg.Gamma_R_P2_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_R_P2_Y = 15;
    m_VideoConfigFix.iipcfg.Gamma_R_P3_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_R_P3_Y = 29;
    m_VideoConfigFix.iipcfg.Gamma_R_P4_C = 26;
    m_VideoConfigFix.iipcfg.Gamma_R_P4_Y = 57;
    m_VideoConfigFix.iipcfg.Gamma_R_P5_C = 27;
    m_VideoConfigFix.iipcfg.Gamma_R_P5_Y = 83;
    m_VideoConfigFix.iipcfg.Gamma_R_P6_C = 17;
    m_VideoConfigFix.iipcfg.Gamma_R_P6_Y = 110;
    m_VideoConfigFix.iipcfg.Gamma_R_P7_C = 12;
    m_VideoConfigFix.iipcfg.Gamma_R_P7_Y = 178;
    m_VideoConfigFix.iipcfg.Gamma_R_P8_C = 8;
    m_VideoConfigFix.iipcfg.Gamma_R_P8_Y = 223;
    m_VideoConfigFix.iipcfg.Gamma_G_P0_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_G_P0_Y = 0;
    m_VideoConfigFix.iipcfg.Gamma_G_P1_C = 32;
    m_VideoConfigFix.iipcfg.Gamma_G_P1_Y = 7;
    m_VideoConfigFix.iipcfg.Gamma_G_P2_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_G_P2_Y = 15;
    m_VideoConfigFix.iipcfg.Gamma_G_P3_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_G_P3_Y = 29;
    m_VideoConfigFix.iipcfg.Gamma_G_P4_C = 26;
    m_VideoConfigFix.iipcfg.Gamma_G_P4_Y = 57;
    m_VideoConfigFix.iipcfg.Gamma_G_P5_C = 27;
    m_VideoConfigFix.iipcfg.Gamma_G_P5_Y = 83;
    m_VideoConfigFix.iipcfg.Gamma_G_P6_C = 17;
    m_VideoConfigFix.iipcfg.Gamma_G_P6_Y = 110;
    m_VideoConfigFix.iipcfg.Gamma_G_P7_C = 12;
    m_VideoConfigFix.iipcfg.Gamma_G_P7_Y = 178;
    m_VideoConfigFix.iipcfg.Gamma_G_P8_C = 8;
    m_VideoConfigFix.iipcfg.Gamma_G_P8_Y = 223;
    m_VideoConfigFix.iipcfg.Gamma_B_P0_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_B_P0_Y = 0;
    m_VideoConfigFix.iipcfg.Gamma_B_P1_C = 32;
    m_VideoConfigFix.iipcfg.Gamma_B_P1_Y = 7;
    m_VideoConfigFix.iipcfg.Gamma_B_P2_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_B_P2_Y = 15;
    m_VideoConfigFix.iipcfg.Gamma_B_P3_C = 28;
    m_VideoConfigFix.iipcfg.Gamma_B_P3_Y = 29;
    m_VideoConfigFix.iipcfg.Gamma_B_P4_C = 26;
    m_VideoConfigFix.iipcfg.Gamma_B_P4_Y = 57;
    m_VideoConfigFix.iipcfg.Gamma_B_P5_C = 27;
    m_VideoConfigFix.iipcfg.Gamma_B_P5_Y = 83;
    m_VideoConfigFix.iipcfg.Gamma_B_P6_C = 17;
    m_VideoConfigFix.iipcfg.Gamma_B_P6_Y = 110;
    m_VideoConfigFix.iipcfg.Gamma_B_P7_C = 12;
    m_VideoConfigFix.iipcfg.Gamma_B_P7_Y = 178;
    m_VideoConfigFix.iipcfg.Gamma_B_P8_C = 8;
    m_VideoConfigFix.iipcfg.Gamma_B_P8_Y = 223;
    m_VideoConfigFix.iipcfg.Black_P = 60;
    m_VideoConfigFix.iipcfg.White_P = 200;
    m_VideoConfigFix.iipcfg.AWB_WIN_L = 40;
    m_VideoConfigFix.iipcfg.AWB_WIN_T = 30;
    m_VideoConfigFix.iipcfg.AWB_WIN_R = 120;
    m_VideoConfigFix.iipcfg.AWB_WIN_B = 90;
    m_VideoConfigFix.iipcfg.SUM_R1 = 0;
    m_VideoConfigFix.iipcfg.SUM_G1 = 0;
    m_VideoConfigFix.iipcfg.SUM_B1 = 0;
    m_VideoConfigFix.iipcfg.AWB_N_VLD_W1 = 0;
    m_VideoConfigFix.iipcfg.SUM_R2 = 0;
    m_VideoConfigFix.iipcfg.SUM_G2 = 0;
    m_VideoConfigFix.iipcfg.SUM_B2 = 0;
    m_VideoConfigFix.iipcfg.AWB_N_VLD_W2 = 0;
    m_VideoConfigFix.iipcfg.SUM_R3 = 0;
    m_VideoConfigFix.iipcfg.SUM_G3 = 0;
    m_VideoConfigFix.iipcfg.SUM_B3 = 0;
    m_VideoConfigFix.iipcfg.AWB_N_VLD_W3 = 0;
    m_VideoConfigFix.iipcfg.SUM_R4 = 0;
    m_VideoConfigFix.iipcfg.SUM_G4 = 0;
    m_VideoConfigFix.iipcfg.SUM_B4 = 0;
    m_VideoConfigFix.iipcfg.AWB_N_VLD_W4 = 0;
    m_VideoConfigFix.iipcfg.SUM_R5 = 0;
    m_VideoConfigFix.iipcfg.SUM_G5 = 0;
    m_VideoConfigFix.iipcfg.SUM_B5 = 0;
    m_VideoConfigFix.iipcfg.AWB_N_VLD_W5 = 0;
    m_VideoConfigFix.iipcfg.AE_WIN_L = 40;
    m_VideoConfigFix.iipcfg.AE_WIN_T = 30;
    m_VideoConfigFix.iipcfg.AE_WIN_R = 120;
    m_VideoConfigFix.iipcfg.AE_WIN_B = 90;
    m_VideoConfigFix.iipcfg.MAX_Y_CTR = 0;
    m_VideoConfigFix.iipcfg.MIN_Y_CTR = 255;
    m_VideoConfigFix.iipcfg.SUM_Y_CTR = 0;
    m_VideoConfigFix.iipcfg.VALID_NUM_CTR = 0;
    m_VideoConfigFix.iipcfg.MAX_Y_BK = 0;
    m_VideoConfigFix.iipcfg.MIN_Y_BK = 255;
    m_VideoConfigFix.iipcfg.SUM_Y_BK = 0;
    m_VideoConfigFix.iipcfg.VALID_NUM_BK = 0;
    m_VideoConfigFix.iipcfg.B_Strgth = 0;
    m_VideoConfigFix.iipcfg.B_Thrshld = 0;
    m_VideoConfigFix.iipcfg.SAT = 64;
    m_VideoConfigFix.iipcfg.HUE = 07;
    m_VideoConfigFix.iipcfg.Y_Gain = 69;
    m_VideoConfigFix.iipcfg.Y_Offset = 8;
    m_VideoConfigFix.iipcfg.LPF_Switch = 0;
    m_VideoConfigFix.iipcfg.LPF_COEF_Y_0 = 4;
    m_VideoConfigFix.iipcfg.LPF_COEF_Y_1 = 15;
    m_VideoConfigFix.iipcfg.LPF_COEF_Y_2 = 26;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cr_0 = 4;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cr_1 = 15;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cr_2 = 26;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cb_0 = 4;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cb_1 = 15;
    m_VideoConfigFix.iipcfg.LPF_COEF_Cb_2 = 26;

    // else setting;

    m_VideoConfigFix.elsecfg.clockrate = 96;
    m_VideoConfigFix.elsecfg.DRAM = 2;
    m_VideoConfigFix.elsecfg.HPIBufferCapacity = 7;

    m_VideoConfigFix.elsecfg.ivtc_holding_time = 1;
    //the name of ivtc_holding_time is meaningless;
    //it is actually a reserved field in elsecfg;
    //elsecfg.ivtc_holding_time bitplane is like;
    //bit[15:1] Reserved;
    //bit[0]    AVSynch information output enable;
    //          1=enabled, 0=disabled;

    m_VideoConfigFix.elsecfg.v_sync_bitplane = 61536;
    //v_sync_bitplane is for Vertical Synch setting;
    //For camera board, please set it to zero;
    //For capture board, its bitplane is like;
    //bit[15]=VSYNC_ENABLE_BIT, 1=enabled, 0=disabled;
    //bit[14]=FID_DET_ENABLE_BIT, 1=enabled, 0=disabled;
    //bit[13]=REPORT_ENABLE_BIT, 1=enabled, 0=disabled;
    //bit[12]=EXEC_ENABLE_BIT, 1=enabled, 0=disabled;
    //bit[11]=GPIO_DET_ENABLE_BIT, 1=enabled, 0=disabled;
    //bit[10:6]=PERIOD_TIME_SELECT;
    //bit[5:0]=GPIO_PIN_SELECT;

    // motion detection settings;
    unsigned int SAD[4] = {32767,32767,32767,32767};
    unsigned int MV [4] = {32767,32767,32767,32767};
    unsigned int Sensitivity[4] = {32767,32767,32767,32767};

    memcpy ((void *)m_VideoConfigFix.mdcfg.u32SADThresholdValues,
            (void *) SAD, sizeof(SAD));
    memcpy ((void *)m_VideoConfigFix.mdcfg.u32MVThresholdValues,
            (void *) MV, sizeof(MV));
    memcpy ((void *)m_VideoConfigFix.mdcfg.u32SensitivityValues,
            (void *) Sensitivity, sizeof(Sensitivity));

    unsigned int macroblockmap[1620]= {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    memcpy ((void *)m_VideoConfigFix.mdcfg.u32MacroblockMap,
            (void *) macroblockmap, sizeof(macroblockmap));
}

// Setup the fields in the AV config structs that will be used to config the HW
void CWisEncRx::SetConfig(CCommand *pCmd)
{
    //-- parse command line
    //videosource - COMPOSITE, SVIDEO
    char pVideoSource[60] = "";
    pCmd->GetVar("VideoSource", pVideoSource,60);
    if(! strcmp(pVideoSource, "SVIDEO")) {
        m_nVideoSource = VS_SVIDEO;
    }
    else {
        m_nVideoSource = VS_COMPOSITE;
    }
    //videotvstandard - NTSC, PAL
    char pVideoTVStandard[60] = "";
    pCmd->GetVar("VideoFormat", pVideoTVStandard,60);
    if(!(strcmp(pVideoTVStandard, "PAL"))) {
        m_nVideoTVStandard = TVStandard_PAL_Mask;
    }
    else {
        m_nVideoTVStandard = TVStandard_NTSC_Mask;
    }
    //PictureSize - 0,1  (CIF,D1)
    m_nVideoSize = D1;
    pCmd->GetIntVar("PictureSize", &m_nVideoSize);

    if (!(m_nVideoSize == D1 || m_nVideoSize == CIF))
        m_nVideoSize = D1;

    //VideoEncoding - MPEG2, MPEG4
    //VideoBitrate - bps
    char pVideoCompression[60] = "";
    pCmd->GetVar("VideoEncoding", pVideoCompression,60);
    if(!(strcmp(pVideoCompression, "MPEG4"))) {
        m_nVideoCompression = MPEG4;
        m_nVideoBitrate = 2000000;
        m_nQuant = 0;
    }
    else {
        m_nVideoCompression = MPEG2;
        m_nVideoBitrate = 6000000;
        m_nQuant = 0;
    }
    int nVBval=0;
    pCmd->GetIntVar("VideoBitrate", &nVBval);
    // if le 64 treat this as quant config for VBR mode
    if( nVBval <= 64 ) {
        m_nQuant = nVBval;
    }
    else {
        m_nVideoBitrate = nVBval;
    }
    // Make sure video birate value is reasonable
    if (m_nVideoCompression == MPEG2 && m_nVideoBitrate > 6000000)
        m_nVideoBitrate = 6000000;
    else if (m_nVideoCompression == MPEG4 && m_nVideoBitrate > 6000000)
        m_nVideoBitrate = 6000000;


    //audiochannels - 1,2
    m_nAudioChannels = 2;
    pCmd->GetIntVar("audiochannels", &m_nAudioChannels);
    if (!(m_nAudioChannels == 1 || m_nAudioChannels == 2))
        m_nAudioChannels = 2;

    //AudioCompression - MPA(MP2), AAC
    //AudioBitrate - bps
    char pAudioCompression[60] = "";
    pCmd->GetVar("AudioCompression", pAudioCompression,60);
    if(!(strcmp(pAudioCompression, "AAC"))) {
        m_nAudioCompression = AAC;
        m_nAudioBitrate = 128000;
    }
    else {
        m_nAudioCompression = MPA;
        m_nAudioBitrate = 192000;
    }
    pCmd->GetIntVar("AudioBitrate", &m_nAudioBitrate);

    // Make sure audio birate value is reasonable
    if (m_nAudioBitrate > 1000000) {
        if (m_nAudioCompression == AAC)
            m_nAudioBitrate = 128000;
        else
            m_nAudioBitrate = 192000;
    }

    //avdisable - 1=noaudio, 2=novideo
    m_nAVDisable = 0x00;
    pCmd->GetIntVar("avdisable", &m_nAVDisable);

    //Encapsulation - MPEGTS, RAW
    char pEncapsulation[60] = "";
    pCmd->GetVar("Encapsulation", pEncapsulation,60);
    if(!(strcmp(pEncapsulation, "MPEGTS"))) {
        m_nEncapsulation = MPEGTS;
    }
    else {
        m_nEncapsulation = RAW;
    }

    // Now populate the fix config from fix settings.txt
    // this is done so that the driver does not have to perform
    // fileio from the kernel space
    SetFixConfig();

    // Override the deinterlacemode parameter

    char pDeinterlace[60];
    pCmd->GetVar("DeinterlaceMode", pDeinterlace,60);
    if(!(strcmp(pDeinterlace, "motion"))) {
        m_VideoConfigFix.deintercfg.DeInter_Always_Motion=1;
        m_VideoConfigFix.deintercfg.DeInter_Always_Blending=0;
    }
    else {
        m_VideoConfigFix.deintercfg.DeInter_Always_Motion=0;
        m_VideoConfigFix.deintercfg.DeInter_Always_Blending=1;
    }


    //AV workstate - 0,1
    m_bAudioWorkState = 0;
    if(! (m_nAVDisable & NOAUDIO) ) {
        m_bAudioWorkState = 1;
    }
    m_bVideoWorkState = 0;
    if(! (m_nAVDisable & NOVIDEO) ) {
        m_bVideoWorkState = 1;
    }

    //-- set AudioConfig
    m_AudioConfig.Channels                      = m_nAudioChannels;
    m_AudioConfig.AvgBytesPerSec                = m_AudioConfig.SampleRate *
        (m_AudioConfig.SampleBits / 8) *
        m_AudioConfig.Channels;
    if( m_nAudioCompression == AAC ) {
        m_AudioConfig.SamplesPerBlock             = 1024 * m_nAudioChannels; // 1K frame per channel
        m_nAPTSperSec = 48000;
    }
    else if( m_nAudioCompression == MPA ) {
        m_AudioConfig.SamplesPerBlock             = 1152 * m_nAudioChannels; // 24ms @ 48kHz / ch
        m_nAPTSperSec = 90000;
    }
    m_nAPTSperFrame = (m_nAPTSperSec / 1000) * m_AudioConfig.SamplesPerBlock /
        (m_AudioConfig.SampleRate * m_AudioConfig.Channels  / 1000);
    m_fAtsperFrame = (double)(m_nAPTSperFrame * 1000) / (double)m_nAPTSperSec;

    //-- set VideoConfig
    m_VideoConfig.strcfg.compress_mode          = (EVideoFormat)m_nVideoCompression;
    m_VideoConfig.strcfg.sequence               = IPONLY;
    m_VideoConfig.rescfg.tv_standard            = (TV_STANDARD)m_nVideoTVStandard;
    if (m_nVideoTVStandard == TVStandard_NTSC_Mask) {
        m_VideoConfig.fpscfg.frame_rate           = 30000;
        m_VideoConfig.strcfg.gop_size             = 15;
        if (m_nVideoSize == CIF) {
            m_VideoConfig.rescfg.width              = 352;
            m_VideoConfig.rescfg.height             = 240;
            m_VideoConfig.rescfg.h_scale_enb        = 1;
            m_VideoConfig.rescfg.v_scale_enb        = 1;
        }
        else if(m_nVideoSize == D1) {
            m_VideoConfig.rescfg.width              = 720;
            m_VideoConfig.rescfg.height             = 480;
            m_VideoConfig.rescfg.h_scale_enb        = 0;
            m_VideoConfig.rescfg.v_scale_enb        = 0;
        }
    }
    else if (m_nVideoTVStandard == TVStandard_PAL_Mask) {
        m_VideoConfig.fpscfg.frame_rate           = 25025;
        m_VideoConfig.strcfg.gop_size             = 25;
        if (m_nVideoSize == CIF) {
            m_VideoConfig.rescfg.width              = 352;
            m_VideoConfig.rescfg.height             = 288;
            m_VideoConfig.rescfg.h_scale_enb        = 1;
            m_VideoConfig.rescfg.v_scale_enb        = 1;
        }
        else if(m_nVideoSize == D1) {
            m_VideoConfig.rescfg.width              = 720;
            m_VideoConfig.rescfg.height             = 576;
            m_VideoConfig.rescfg.h_scale_enb        = 0;
            m_VideoConfig.rescfg.v_scale_enb        = 0;
        }
    }
    if( m_nQuant != 0 ) {
        m_VideoConfig.ctlcfg.target_bitrate       = 0;
        m_VideoConfig.ctlcfg.Q                    = m_nQuant;
    }
    else {
        m_VideoConfig.ctlcfg.target_bitrate       = m_nVideoBitrate;
        m_VideoConfig.ctlcfg.Q                    = 0;
    }
    m_VideoConfig.ctlcfg.peak_bitrate           = m_VideoConfig.ctlcfg.target_bitrate * 10;
    m_VideoConfig.ctlcfg.vbv_buffer             = m_VideoConfig.ctlcfg.target_bitrate * 10;
    m_VideoConfig.fpscfg.tv_standard            = (TV_STANDARD)m_nVideoTVStandard;

    m_nVPTSperSec = 90000;
    m_nVPTSperFrame = (m_nVPTSperSec * 1001 / m_VideoConfig.fpscfg.frame_rate);
    m_fVtsperFrame = (double)(m_nVPTSperFrame * 1000) / (double)m_nVPTSperSec;

    pCmd->GetIntVar("Hue", &m_nHue);
    pCmd->GetIntVar("Brightness", &m_nBrightness);
    pCmd->GetIntVar("Saturation", &m_nSaturation);
    pCmd->GetIntVar("Contrast", &m_nContrast);
    return;
}

void CWisEncRx::InitAudioCodec(void)
{
    AVCodec *codec=NULL;

    m_acodec = NULL;

    avcodec_init();

    m_acodec = avcodec_alloc_context();
    if(m_acodec)
        {
            m_acodec->codec = NULL;
            m_acodec->codec_type = CODEC_TYPE_AUDIO;
            m_acodec->sample_rate = m_AudioConfig.SampleRate;
            m_acodec->channels = m_AudioConfig.Channels;
            m_acodec->bit_rate = m_nAudioBitrate;
        }
    else{
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR("avcodec_alloc_context() fails to return context, port %d",m_nPort);
    }

    if( m_nAudioCompression == AAC ) {
        codec = &faac_encoder;
    }
    else if( m_nAudioCompression == MPA ) {
        codec = &mp2_encoder;
    }
    if (!codec) {
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR("Port %d : codec not found", m_nPort);
    }
    else {
        if (m_acodec) {
            int r = 0;
            if ((r = avcodec_open (m_acodec, codec))) {
                m_nState |= OBJ_ERROR;
                SYSLOG_ERROR ("Port %d : Could not open codec: ErrCode: %d", m_nPort, r);
            }
        }
    }

#ifdef USE_IPP_AAC
    if( m_nAudioCompression == AAC ) {
        if ( (r = aacencInit(&m_IPPaacencState,
                             m_AudioConfig.SampleRate,
                             m_AudioConfig.Channels,
                             m_nAudioBitrate,
                             AOT_AAC_LC)) != AAC_OK ) {
            m_nState |= OBJ_ERROR;
            SYSLOG_ERROR ("Port %d : Could not init IPP AAC encoder: ErrCode: %d", m_nPort, r);
        }
    }
#endif

    return;
}

// Init audio and video read frame buffers
#define SEQ_HEADER_LENGTH 34
int CWisEncRx::InitVideoSeqHeader(void)
{
    int i;
    // just zero out for now
    for( i=0; i<SEQ_HEADER_LENGTH; i++) {
        *(m_pVideoFrameBuf + i) = 0x00;
    }
    return SEQ_HEADER_LENGTH;
}

void CWisEncRx::InitFrameBuffers(void)
{
    int len = 0;

    // Alloc video frame read buffer
    m_nVideoFrameLen =
        m_VideoConfig.rescfg.width * m_VideoConfig.rescfg.height * 3 / 2 + MAX_VBI_LENGTH;
    m_pVideoFrameBuf = new UINT8[SEQ_HEADER_LENGTH + m_nVideoFrameLen + sizeof (struct timeval)];

    // init sequence header info at the front of the video frame buf
    if( m_nVideoCompression == MPEG2 ) {
        len = InitVideoSeqHeader();
    }
    m_nVideoFrameBufOffset = len;

    // Alloc audio frame read buffer
    m_nAudioFrameLen = m_AudioConfig.SamplesPerBlock * (m_AudioConfig.SampleBits / 8);
    m_pAudioFrameBuf = new UINT8[m_nAudioFrameLen + sizeof (struct timeval)];
    // Alloc encoded audio frame buffer - assume max enc size = unencoded size
    m_pAudioEncFrameBuf = new UINT8[m_nAudioFrameLen];

    return;
}

// Map Port number to USB device number
int CWisEncRx::Port2Dev(int port)
{
    int dev;
    // for now just return 1:1 map
    dev = port;

    return dev;
}

// thread to start capturing audio and video
// launches kernel threads (one for A and V resp) and exits
// the kthreads will daemonize and reparent to init - so dont need to be "wait"ed for
// this is a static memeber func - shared by all objects of type wisencrx - ie all ports
void *CWisEncRx::doStartStopCapture(void *pThis)
{
    int r = -1;
    char *pState=NULL;
    int nPort=0;
    int nDev=0;
    int nState=0;
    int nVideoSource=0;
    int nVideoTVStandard=0;;
    int nHue=0;
    int nContrast=0;
    int nSaturation=0;
    int nBrightness=0;
    AUDIO_CONFIG AudioConfig;
    TCFGVIDEO VideoConfig;
    TVIDEOCFGFIX VideoConfigFix;

    IVideoCapture *pVideoCapture = NULL;

    // cast pointer to obj type
    // BEGIN LOCK
    CWisEncRx *p = static_cast<CWisEncRx *> (pThis);
    pState = p->m_pVideoCaptureState;
    if (*pState) {
        nPort = p->m_nPort;
        nDev  = p->m_nDev;
        nVideoTVStandard    = p->m_nVideoTVStandard;
        nVideoSource        = p->m_nVideoSource;
        nBrightness         = p->m_nBrightness;
        nHue                = p->m_nHue;
        nSaturation         = p->m_nSaturation;
        nContrast           = p->m_nContrast;
        memcpy ((void *)(&AudioConfig), (void *) &(p->m_AudioConfig),
                sizeof (AUDIO_CONFIG));
        memcpy ((void *)(&VideoConfig), (void *) &(p->m_VideoConfig),
                sizeof (TCFGVIDEO));
        memcpy ((void *)(&VideoConfigFix), (void *) &(p->m_VideoConfigFix),
                sizeof (TVIDEOCFGFIX));
    }
    // END LOCK
    CScopedLock lock(m_nPortLocks[nPort]);
    // BEGIN LOCK
    if (!*pState) {
        goto EXIT_LABEL;
    }
    // END LOCK

    // two ports simultaneous start fails - for now serialize all ports start-stop
    // by using one common lock (in addition to port number based lock)
    (m_nPortLocks[MAX_WIS_PORTS]).Lock();

    // Instantiate a new VideoCapture object
    r = IVideoCapture::CreateInstance((void **) &(pVideoCapture),nDev);
    if (r != SUCCESS) {
        nState |= OBJ_ERROR;
        SYSLOG_ERROR("Port %d : Init Capture Failed", nPort);
        goto EXIT_LABEL;
    }
    else {
        SYSLOG_DEBUG("Port %d : Init Capture Succeeded", nPort);
    }
    if (pVideoCapture->DeviceExist() == 0) {
        nState |= OBJ_ERROR;
        SYSLOG_ERROR("Port %d : Device does not exist", nPort);
        goto EXIT_LABEL;
    }

    // Set HW AV Config
    pVideoCapture->SetTVStandard(nVideoTVStandard);
    pVideoCapture->SetVideoSource(nVideoSource);
    pVideoCapture->SetVideoConfig(&VideoConfig);
    pVideoCapture->SetAudioConfig(&AudioConfig);
    pVideoCapture->SetFixConfig(&VideoConfigFix);

    // Kick off the kernel capture threads
    r = pVideoCapture->StartCapturing();
    if (r != SUCCESS) {
        nState |= OBJ_ERROR;
        SYSLOG_ERROR ("Port %d : Start Capture Failed", nPort);
        goto EXIT_LABEL;
    }
    else {
        SYSLOG_DEBUG("Port %d : Start Capture Succeeded", nPort);
    }

    pVideoCapture->ChangeSaturation(nSaturation);
    pVideoCapture->ChangeBrightness(nBrightness);
    pVideoCapture->ChangeHue(nHue);
    pVideoCapture->ChangeContrast(nContrast);

    nState |= OBJ_CONNECTED;
    // BEGIN LOCK
    if (*pState) {
        p->m_pVideoCapture = pVideoCapture;
        p->m_nState |= nState;
    }
    // END LOCK

    if(r == SUCCESS) {
        // drop common lock
        (m_nPortLocks[MAX_WIS_PORTS]).Unlock();

        // go to sleep till its time to stop capture
        // just do this for now - switch to passive sleep later
        while( *pState) {
            if (p->m_nState & OBJ_CONNECTED)
                usleep (1000);
            else
                break;
        }

        // grab common lock
        (m_nPortLocks[MAX_WIS_PORTS]).Lock();
    }
    else {
        // start capture failed. drop common lock and return
        (m_nPortLocks[MAX_WIS_PORTS]).Unlock();
        goto EXIT_LABEL;
    }

    // start-err or obj-disconnect = stop capture
    // destructor on obj has already been called
    // no more access to obj - "p->" not valid anymore
    r = pVideoCapture->StopCapturing();
    if (r != SUCCESS) {
        SYSLOG_ERROR ("Could not stop capturing");
    }
    else {
        SYSLOG_DEBUG("Stop Capture Succeeded");
    }
    pVideoCapture->Release();

    // drop common lock
    (m_nPortLocks[MAX_WIS_PORTS]).Unlock();

 EXIT_LABEL:
    // BEGIN LOCK
    if (*pState) {
        p->m_nState = nState;
    }
    // END LOCK
    // now delete the variable allocated by the wisencrx object
    delete[] pState;

    // this will also automatically drop the per-port lock
    return (void *)r;
}

int CWisEncRx::StartCapture(void)
{
    int r = -1;

    // use "port number" to find out which "device" to open
    m_nDev = Port2Dev(m_nPort);

    // Instantiate a new VideoCapture object
    r = IVideoCapture::CreateInstance((void **) &(m_pVideoCapture),m_nDev);
    if (r != SUCCESS) {
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR("Port %d : Init Capture Failed", m_nPort);
        return r;
    }
    else {
        SYSLOG_DEBUG("Port %d : Init Capture Succeeded", m_nPort);
    }
    if (m_pVideoCapture->DeviceExist() == 0) {
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR("Port %d : Device does not exist", m_nPort);
        return r;
    }

    // Set HW AV Config
    m_pVideoCapture->SetTVStandard(m_nVideoTVStandard);
    m_pVideoCapture->SetVideoSource(m_nVideoSource);
    m_pVideoCapture->SetVideoConfig(&(m_VideoConfig));
    m_pVideoCapture->SetAudioConfig(&(m_AudioConfig));

    // Kick off the kernel capture threads
    r = m_pVideoCapture->StartCapturing();
    if (r != SUCCESS) {
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR ("Port %d : Start Capture Failed", m_nPort);
        return r;
    }
    else {
        SYSLOG_DEBUG("Port %d : Start Capture Succeeded", m_nPort);
    }

    m_nState |= OBJ_CONNECTED;

    return r;
}

// try to get a audio frame from the driver kernel buffer
// if not available set the "sleep" flag to avoid needless retry
// if not available or too long set the "dwf" flag to avoid holding up video
void CWisEncRx::GetAudioFrame()
{
    int r;
    int len = 0;
    struct timeval tFrame;

    // Try to get audio
    len = m_nAudioFrameLen;
    r = m_pVideoCapture->GetAudioSamples(m_pAudioFrameBuf, &len);
    if (r == SUCCESS) {

        m_got_audio = 1;
        m_dwf_audio = 0;
        // copy timeval from tail of buffer and adjust len
        len -= sizeof (struct timeval);
        memcpy(&tFrame, m_pAudioFrameBuf + len, sizeof (struct timeval));
        // convert to timestamp and save
        m_tATime = CTimeKeeper::GetTimeStamp(&tFrame);
        // smooth out jitter in timestamp
        if((! m_fATime)
           || (m_nAbursterr > 10)
           || (m_nAlosserr > 10)
           ) {
            // adjust timestamp to fall on a frame boundary
            //    m_tATime = RawTS2FrameTS(m_tATime, m_nAPTSperSec, m_nAPTSperFrame);
            m_fATime = (double)m_tATime;
            m_AVStatCurr.nATimeSetCount++;
        }
        else {
            m_fATime = m_fATime + m_fAtsperFrame;
        }
        // check if count based smooth timestamp is drifting from raw timstamp
        if((int)((unsigned long long)m_fATime - m_tATime) > ((int)(m_fAtsperFrame) + 1))
            m_nAbursterr++;
        else
            m_nAbursterr = 0;
        if ((int)(m_tATime - (unsigned long long)m_fATime) > ((int)(m_fAtsperFrame) + 1))
            m_nAlosserr++;
        else
            m_nAlosserr = 0;
        // assign smooth timestamp
        m_tATime = (unsigned long long)m_fATime;
        //set data len
        m_nALen = len;
        // update stats
        m_AVStatCurr.nAudioSamples++;
        m_AVStatCurr.nRawAudioBytes += m_nALen;
    }
    else {
        // no audio available - setup to sleep for a while
        m_sleep = 1;
        m_tTime = CTimeKeeper::Now();
        // check if its been too long since last audio (1 s)
        // if so - set the "dont wait for audio" flag to allow video to be sent
        if( (int)(CTimeKeeper::Now() - m_tATime) > 1000 ) {
            m_dwf_audio = 1;
            m_AVStatCurr.nDWF_Audio++;
        }
    }

    return;
}

// try to get a video frame from the driver kernel buffer
// if not available set the "sleep" flag to avoid needless retry
// if not available or too long set the "dwf" flag to avoid holding up audio
void CWisEncRx::GetVideoFrame()
{
    int r;
    UINT8 *pVideoFrameBuf;
    struct timeval tFrame;

    // Try to get video
    bzero( (char *) &m_FrameInfo, sizeof(m_FrameInfo));
    pVideoFrameBuf = m_pVideoFrameBuf + m_nVideoFrameBufOffset;
    r = m_pVideoCapture->GetOneFrame(pVideoFrameBuf,
                                     m_nVideoFrameLen,
                                     &m_FrameInfo);
    if (r == SUCCESS) {
        m_got_video = 1;
        m_dwf_video = 0;
        // copy timeval from tail of buffer
        memcpy(&tFrame,
               pVideoFrameBuf+m_FrameInfo.VideoLength+m_FrameInfo.VBILength,
               sizeof (struct timeval));
        // convert to timestamp and save
        m_tVTime = CTimeKeeper::GetTimeStamp(&tFrame);
        // smooth out jitter in timestamp
        if((! m_fVTime)
           || (m_nVbursterr > 10)
           || (m_nVlosserr > 10)
           ) {
            // adjust timestamp to fall on a frame boundary
            //    m_tVTime = RawTS2FrameTS(m_tVTime, m_nVPTSperSec, m_nVPTSperFrame);
            m_fVTime = (double)m_tVTime;
            m_AVStatCurr.nVTimeSetCount++;
        }
        else {
            m_fVTime = m_fVTime + m_fVtsperFrame;
        }
        // check if count based smooth timestamp is drifting from raw timstamp
        if((int)((unsigned long long)m_fVTime - m_tVTime) > ((int)(m_fVtsperFrame) + 1))
            m_nVbursterr++;
        else
            m_nVbursterr = 0;
        if ((int)(m_tVTime - (unsigned long long)m_fVTime) > ((int)(m_fVtsperFrame) + 1))
            m_nVlosserr++;
        else
            m_nVlosserr = 0;
        // assign smooth timestamp
        m_tVTime = (unsigned long long)m_fVTime;
        //set data len
        m_nVLen = m_FrameInfo.VideoLength;
        // update stats
        m_AVStatCurr.nVideoFrames++;
        m_AVStatCurr.nRawVideoBytes += m_nVLen;
    }
    else {
        // no video available - setup to sleep for a while
        m_sleep = 1;
        m_tTime = CTimeKeeper::Now();
        // check if its been too long since last video (1 s)
        // if so - set the "dont wait for video" flag to allow audio to be sent
        if( (int)(CTimeKeeper::Now() - m_tVTime) > 1000 ) {
            m_dwf_video = 1;
            m_AVStatCurr.nDWF_Video++;
        }
    }

    return;
}

// send AV frame as csframe based on encapsulation
int CWisEncRx::SendFrame()
{
    int r = 0;
    char tspktData[ 7 * TS_PACKET_SIZE];
    char *tp;

    // Set StartTimestamp
    if (m_nTimestamp == 0) {
        m_nStartTimestamp = m_tFrameTime;
        m_nStartWallClock = CTimeKeeper::Now();
        m_bSendNotify = true;
        m_nEventId = NOTIFICATION_EVENT_BEGIN;
    }
    // Ensure that the timestamps are always increasing
    if (m_nTimestamp <= m_tFrameTime)
        m_nTimestamp = m_tFrameTime;
    else
        m_AVStatCurr.nTimeStampSetCount++;

    m_nFrameWallClock = CTimeKeeper::Now();

    // send frame to TS Mux Filter and send its output TS packets to the queue
    if( m_nEncapsulation == MPEGTS ) {
        // this only works if the popTS is done right after the pushAVFrame
        // because the frametype and timestamp info is only valid now
        r = m_pMPEGTSMuxFilter->pushAVFrame(m_pFrameData,
                                            m_nFrameLen, m_nFrameType,
                                            m_tFrameTime);
        if( r == -1) return r;
        m_nFrameType |= FRAMETYPE_MPEGTS;
        m_nFrameLen = TS_PACKET_SIZE;
        // one AV frame will result in multiple TS packets
#if 0
        while( (m_pFrameData = m_pMPEGTSMuxFilter->popTSPacket()) != NULL ) {
            CSFrame * sFrame = new CSFrame(m_pFrameData,
                                           m_nFrameLen,
                                           CSFrame::CreateMediaFlag(m_nFrameType,
                                                                    STREAMTYPE_WIS),
                                           m_nTimestamp);
            m_qQueueSink->InsertFrame(sFrame);
            m_bytesTransfered += m_nFrameLen;
            m_frameCount++;
            // this mem was alloc'd by the Mux Filter
            delete[] (m_pFrameData);
        }
#else
        tp = tspktData;
        int i = 0;
        while(1) {
            m_pFrameData = m_pMPEGTSMuxFilter->popTSPacket();
            if(m_pFrameData == NULL || i >= NO_OF_TS_PKTS) {
                CSFrame * sFrame =
                    new CSFrame(tspktData,
                                i * m_nFrameLen,
                                CSFrame::CreateMediaFlag(m_nFrameType,
                                                         STREAMTYPE_WIS),
                                m_nTimestamp);
                m_qQueueSink->InsertFrame(sFrame);
                m_bytesTransfered += i * m_nFrameLen;
                m_frameCount++;
                if(m_pFrameData == NULL) {
                    break;
                }
                else {
                    tp = tspktData;
                    i = 0;
                }
            }
            memcpy(tp, m_pFrameData, m_nFrameLen);
            tp += TS_PACKET_SIZE;
            // this mem was alloc'd by the Mux Filter
            delete[] m_pFrameData;
            i++;
        }
#endif
    }
    // send raw AV frame to the queue
    else {
        m_nFrameType |= FRAMETYPE_RAW;
        CSFrame * sFrame = new CSFrame(m_pFrameData,
                                       m_nFrameLen,
                                       CSFrame::CreateMediaFlag(m_nFrameType,
                                                                STREAMTYPE_WIS),
                                       m_nTimestamp);
        m_qQueueSink->InsertFrame(sFrame);
        m_bytesTransfered += m_nFrameLen;
        m_frameCount++;
    }

    return r;
}

// send audio frame
int CWisEncRx::SendAudioFrame()
{
    // encode the block
#ifdef USE_IPP_AAC
    if( m_nAudioCompression == AAC )
        aacencGetFrame((Ipp16s *)m_pAudioFrameBuf,
                       &m_nFrameLen,
                       (Ipp8u *)m_pAudioEncFrameBuf,
                       m_IPPaacencState);
    else
#endif
        m_nFrameLen = avcodec_encode_audio( m_acodec,
                                            (uint8_t *)m_pAudioEncFrameBuf,
                                            m_nAudioFrameLen,
                                            (short *)m_pAudioFrameBuf );

    m_AVStatCurr.nAudioBytes += m_nFrameLen;

    // set new frame data ptr
    m_pFrameData = (char *)m_pAudioEncFrameBuf;
    // set new frame type flags
    m_nFrameType = m_nAudioFrameType;
    // set new frame PTS
    //    nFramePTS = m_tATime * (m_nAPTSperSec / 1000);
    m_tFrameTime = m_tATime;
    // send it out based on encaps
    int r = SendFrame();

    return r;
}

// send video frame
int CWisEncRx::SendVideoFrame()
{
    // set new frame data ptr and frame type flags
    m_nFrameLen = m_nVLen;
    m_pFrameData = (char *)m_pVideoFrameBuf + m_nVideoFrameBufOffset;

    m_nFrameType = m_nVideoFrameType;
    m_nFrameType |= (m_FrameInfo.FrameType == I_FRAME) ? FRAMETYPE_I : FRAMETYPE_P;

    // add sequence header for mpeg2 I-frame
    if( m_nVideoCompression == MPEG2 && m_FrameInfo.FrameType == I_FRAME ) {
        // if saved seq header exists add it in
        if((*(unsigned int *)m_pVideoFrameBuf) == 0xb3010000) {
            m_pFrameData = (char *)m_pVideoFrameBuf;
            m_nFrameLen += m_nVideoFrameBufOffset;
        }
        // if not see if it is at the top of the current frame
        // if available - copy to seq header area for future
        else if((*(unsigned int *)m_pFrameData) == 0xb3010000) {
            memcpy(m_pVideoFrameBuf, m_pFrameData, m_nVideoFrameBufOffset);
        }
        // if not - issue a request to add it to next GOP
        else {
            // Insert IOCTL doesn't seem to be working - try custom IOCTL instead
            //            m_pVideoCapture->InsertNewSequenceHeader();
            SINT32 nSeqHeaderLen = 0;
            m_pVideoCapture->GetSequenceHeader(m_pVideoFrameBuf, &nSeqHeaderLen);
            if( nSeqHeaderLen != 0 ) m_nVideoFrameBufOffset = (int)nSeqHeaderLen;
            m_AVStatCurr.nGetSeqHeaderCount++;
        }
    }

    m_AVStatCurr.nVideoBytes += m_nFrameLen;

    // set new frame PTS
    //    nFramePTS = m_tVTime * (m_nVPTSperSec / 1000);
    m_tFrameTime = m_tVTime;

    // send it out based on encaps
    int r = SendFrame();

    return r;
}

int CWisEncRx::ProcessStream()
{
    int retval = 0;

    // no output queue
    if(m_qQueueSink == NULL)
        return retval;

    // start capture not yet complete
    if(! (m_nState & OBJ_CONNECTED) )
        return retval;

    // while in sleep state don't bother checking for AV data
    if( m_sleep ) {
        if( (int)(CTimeKeeper::Now() - m_tTime) < 10 ) {
            return retval;
        }
        else {
            m_sleep = 0;
        }
    }

    // Try to get a Frame
    // max of one Audio and one Video frame at a time - only while workstate = 1
    // if no frame - will set "sleep" and/or "dwf" flag as needed
    if( m_bAudioWorkState ) {
        if( ! m_got_audio ) {
            GetAudioFrame();
        }
    }
    if( m_bVideoWorkState ) {
        if( ! m_got_video ) {
            GetVideoFrame();
        }
    }

    // If got one audio and one video here (OR either/both have been dwf'd)
    // send out the one with the earlier timestamp on it.
    // this makes sure that the AV packets always go out in ascending PTS order
    if( (m_got_audio && m_got_video && (m_tATime <= m_tVTime)) ||
        (m_got_audio && (m_dwf_video || (!m_bVideoWorkState))) ) {
        m_got_audio = 0;

        if(SendAudioFrame() != -1) {
            retval = 1;
        }
    }
    else if( (m_got_video && m_got_audio && (m_tVTime <= m_tATime)) ||
             (m_got_video && (m_dwf_audio || (!m_bAudioWorkState))) ) {
        m_got_video = 0;

        if(SendVideoFrame() != -1) {
            retval = 1;
        }
    }
    else {
        m_dwf_audio = 0;
        m_dwf_video = 0;
    }

    return retval;
}


CWisEncRx::CWisEncRx (int nId, int nPort, CCommand *pCmd) :
    CReceiver(nId)
{
    // make sure we start in disconnected state
    m_nState &=~OBJ_CONNECTED;

    m_nPortType = WISENCODERPORT;
    m_nPort = nPort;
    m_nDev  = Port2Dev(nPort);

    // init default config values
    InitConfig();

    // override defaults based on input
    SetConfig(pCmd);

    // Calc and alloc AV frame buffers
    InitFrameBuffers();

    // Setup SW Audio Compression
    InitAudioCodec();

    // Init output frame types
    m_nAudioFrameType = FRAMETYPE_AUDIO;
    m_nAudioFrameType |= (m_nAudioCompression == AAC ? FRAMETYPE_AAC : FRAMETYPE_MPA);
    m_nVideoFrameType = FRAMETYPE_VIDEO;
    m_nVideoFrameType |= (m_nVideoCompression == MPEG4 ? FRAMETYPE_MP4V : FRAMETYPE_MP2V);

    // Setup MPEGTS Mux Filter if needed
    m_pMPEGTSMuxFilter = NULL;
    if( m_nEncapsulation == MPEGTS ) {
        m_pMPEGTSMuxFilter = new MPEGTSMuxFilter(m_nVideoFrameType, m_nVideoBitrate,
                                                 m_nAudioFrameType, m_nAudioBitrate);
    }

    // init other process stream related state vars
    m_sleep = 0;
    m_got_audio = 0;
    m_got_video = 0;
    m_dwf_audio = 0;
    m_dwf_video = 0;
    m_nALen = 0;
    m_nVLen = 0;
    // init time
    m_tTime = CTimeKeeper::Now();
    m_tATime = m_tTime;
    m_tVTime = m_tTime;
    m_fATime = 0;
    m_fVTime = 0;

    memset(&m_AVStatCurr, 0x0, sizeof(AVStat));
    memset(&m_AVStatLast, 0x0, sizeof(AVStat));
    m_AVStatCurr.tTime = m_AVStatLast.tTime = CTimeKeeper::Now();

    // Check if encoder ports are available
    struct stat fileStat;
    if (stat("/tmp/encoder_error.txt", &fileStat) == 0) {
        SYSLOG_ERROR ("Hardware Errors on encoder ports.");
        m_nState |= OBJ_ERROR;
        return;
    }

    // launch thread to manage start/stop capture without blocking the const/dstr
    m_tStartStopCaptureThread = 0;

    if( pthread_create(&m_tStartStopCaptureThread, NULL, &CWisEncRx::doStartStopCapture, this) ) {
        m_nState |= OBJ_ERROR;
        SYSLOG_ERROR ("Port %d : Could not create StartCaptureThread", m_nPort);
    }
    if( pthread_detach(m_tStartStopCaptureThread) ) {
        SYSLOG_ERROR ("Port %d : Could not detach StartCaptureThread", m_nPort);
    }
}

CWisEncRx::~CWisEncRx()
{
    // mark obj disconnected to shut down any activity
    // for now this also signals the start/stop capture thread to stop capture
    // this is subject to race - m_nState could go away
    // before it is tested by the start/stop thread!!
    m_nState &=~OBJ_CONNECTED;

    // free up dyn alloc mem
    delete (m_pMPEGTSMuxFilter);

    // end audio codec
    avcodec_close(m_acodec);
    av_free(m_acodec);

    *m_pVideoCaptureState=0;

    // free frame buffers
    delete[] m_pVideoFrameBuf;
    delete[] m_pAudioFrameBuf;
    delete[] m_pAudioEncFrameBuf;
}
int
CWisEncRx::Update(CCommand *a_pCmd)
{
    int retval=0;
    if(! (m_nState & OBJ_CONNECTED) )
        return retval;
    if (a_pCmd->GetIntVar("Saturation", &m_nSaturation) != -1) {
        m_pVideoCapture->ChangeSaturation(m_nSaturation);
    }
    if (a_pCmd->GetIntVar("Brightness", &m_nBrightness) != -1) {
        m_pVideoCapture->ChangeBrightness(m_nBrightness);
    }
    if (a_pCmd->GetIntVar("Hue", &m_nHue) != -1) {
        m_pVideoCapture->ChangeHue(m_nHue);
    }
    if (a_pCmd->GetIntVar("Contrast", &m_nContrast) != -1) {
        m_pVideoCapture->ChangeContrast(m_nContrast);
    }
    return retval;
}

void CWisEncRx::UpdateCustomStatus(CCommand *a_pCmd)
{
    a_pCmd->SetObjectParam("Port",
                           (int)m_nPort);

    a_pCmd->SetObjectParam("Encapsulation",
                           (int)m_nEncapsulation);

    a_pCmd->SetObjectParam("sleep",
                           (int)m_sleep);

    a_pCmd->SetObjectParam("nSaturation",
                           (int)m_nSaturation);
    a_pCmd->SetObjectParam("nHue",
                           (int)m_nHue);
    a_pCmd->SetObjectParam("nBrightness",
                           (int)m_nBrightness);
    a_pCmd->SetObjectParam("nContrast",
                           (int)m_nContrast);

    a_pCmd->SetObjectParam("nAudioCompression",
                           (int)m_nAudioCompression);
    a_pCmd->SetObjectParam("nAudioBitrate",
                           (int)m_nAudioBitrate);
    a_pCmd->SetObjectParam("bAudioWorkState",
                           (int)m_bAudioWorkState);
    a_pCmd->SetObjectParam("got_audio",
                           (int)m_got_audio);
    a_pCmd->SetObjectParam("dwf_audio",
                           (int)m_dwf_audio);
    a_pCmd->SetObjectParam("nALen",
                           (int)m_nALen);
    a_pCmd->SetObjectParam("tATime",
                           (unsigned long)m_tATime);
    a_pCmd->SetObjectParam("fATime",
                           (double)m_fATime);
    a_pCmd->SetObjectParam("fAtsperFrame",
                           (double)m_fAtsperFrame);

    a_pCmd->SetObjectParam("nVideoCompression",
                           (int)m_nVideoCompression);
    a_pCmd->SetObjectParam("nVideoBitrate",
                           (int)m_nVideoBitrate);
    a_pCmd->SetObjectParam("bVideoWorkState",
                           (int)m_bVideoWorkState);
    a_pCmd->SetObjectParam("got_video",
                           (int)m_got_video);
    a_pCmd->SetObjectParam("dwf_video",
                           (int)m_dwf_video);
    a_pCmd->SetObjectParam("nVLen",
                           (int)m_nVLen);
    a_pCmd->SetObjectParam("tVTime",
                           (unsigned long)m_tVTime);
    a_pCmd->SetObjectParam("fVTime",
                           (double)m_fVTime);
    a_pCmd->SetObjectParam("fVtsperFrame",
                           (double)m_fVtsperFrame);

    // handle AV stat struct
    m_AVStatCurr.tTime = CTimeKeeper::Now();
    unsigned long delta = (m_AVStatCurr.tTime - m_AVStatLast.tTime);
    if (delta == 0)
        delta = 1;
    a_pCmd->SetObjectParam("nASamplesPerSec",
                           (int)((m_AVStatCurr.nAudioSamples - m_AVStatLast.nAudioSamples) * 1000 /delta));
    a_pCmd->SetObjectParam("nVFramesPerSec",
                           (int)((m_AVStatCurr.nVideoFrames - m_AVStatLast.nVideoFrames) * 1000 /delta));
    a_pCmd->SetObjectParam("nABitsPerSec",
                           (int)((m_AVStatCurr.nAudioBytes - m_AVStatLast.nAudioBytes)*1000/delta)*8);
    a_pCmd->SetObjectParam("nVBitsPerSec",
                           (int)((m_AVStatCurr.nVideoBytes - m_AVStatLast.nVideoBytes)*1000/delta)*8);
    a_pCmd->SetObjectParam("nDWF_Audio",
                           (unsigned long)(m_AVStatCurr.nDWF_Audio));
    a_pCmd->SetObjectParam("nDWF_Video",
                           (unsigned long)(m_AVStatCurr.nDWF_Video));
    a_pCmd->SetObjectParam("nATimeSetCount",
                           (unsigned long)(m_AVStatCurr.nATimeSetCount));
    a_pCmd->SetObjectParam("nVTimeSetCount",
                           (unsigned long)(m_AVStatCurr.nVTimeSetCount));
    a_pCmd->SetObjectParam("nTimeStampSetCount",
                           (unsigned long)(m_AVStatCurr.nTimeStampSetCount));
    a_pCmd->SetObjectParam("nGetSeqHeaderCount",
                           (unsigned long)(m_AVStatCurr.nGetSeqHeaderCount));
    a_pCmd->SetObjectCounterParam("AudioSamples",
                                  m_AVStatCurr.nAudioSamples);
    a_pCmd->SetObjectCounterParam("VideoFrames",
                                  m_AVStatCurr.nVideoFrames);
    a_pCmd->SetObjectByteCounterParam("RawAudioBytes",
                                      m_AVStatCurr.nRawAudioBytes);
    a_pCmd->SetObjectByteCounterParam("RawVideoBytes",
                                      m_AVStatCurr.nRawVideoBytes);
    a_pCmd->SetObjectByteCounterParam("AudioBytes",
                                      m_AVStatCurr.nAudioBytes);
    a_pCmd->SetObjectByteCounterParam("VideoBytes",
                                      m_AVStatCurr.nVideoBytes);
    m_AVStatLast = m_AVStatCurr;

    // WIS Driver AV Stats
    if(m_nState & OBJ_CONNECTED) {
        m_pVideoCapture->GetAVStats(&m_WISAVStatsCurr);
        a_pCmd->SetObjectParam("WIS-nVacquired",
                               (unsigned long)m_WISAVStatsCurr.V_acquired);
        a_pCmd->SetObjectParam("WIS-nVdropped",
                               (unsigned long)m_WISAVStatsCurr.V_dropped);
        a_pCmd->SetObjectParam("WIS-nVduplicated",
                               (unsigned long)m_WISAVStatsCurr.V_duplicated);
        a_pCmd->SetObjectParam("WIS-nAacquired",
                               (unsigned long)m_WISAVStatsCurr.A_acquired);
        a_pCmd->SetObjectParam("WIS-nAdropped",
                               (unsigned long)m_WISAVStatsCurr.A_dropped);
        a_pCmd->SetObjectParam("WIS-nAduplicated",
                               (unsigned long)m_WISAVStatsCurr.A_duplicated);
        m_WISAVStatsLast = m_WISAVStatsCurr;
    }

    // MPEG TS Mux Stats
    if(m_pMPEGTSMuxFilter) {
        a_pCmd->SetObjectParam("Mux-nPCR",
                               (unsigned long)m_pMPEGTSMuxFilter->m_pStats.nPCR);
        a_pCmd->SetObjectParam("Mux-nMisc",
                               (int)m_pMPEGTSMuxFilter->m_pStats.nMisc);
        a_pCmd->SetObjectParam("Mux-nNull",
                               (int)m_pMPEGTSMuxFilter->m_pStats.nNull);

        a_pCmd->SetObjectParam("Mux-Audio-nPTS",
                               (unsigned long)m_pMPEGTSMuxFilter->streams[STREAM_IDX_AUDIO]->pStats.nPTS);
        a_pCmd->SetObjectParam("Mux-Audio-nFrame",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_AUDIO]->pStats.nFrame);
        a_pCmd->SetObjectParam("Mux-Audio-nPES",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_AUDIO]->pStats.nPES);
        a_pCmd->SetObjectParam("Mux-Audio-nTS",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_AUDIO]->pStats.nTS);
        a_pCmd->SetObjectParam("Mux-Audio-nPad",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_AUDIO]->pStats.nPad);

        a_pCmd->SetObjectParam("Mux-Video-nPTS",
                               (unsigned long)m_pMPEGTSMuxFilter->streams[STREAM_IDX_VIDEO]->pStats.nPTS);
        a_pCmd->SetObjectParam("Mux-Video-nFrame",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_VIDEO]->pStats.nFrame);
        a_pCmd->SetObjectParam("Mux-Video-nPES",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_VIDEO]->pStats.nPES);
        a_pCmd->SetObjectParam("Mux-Video-nTS",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_VIDEO]->pStats.nTS);
        a_pCmd->SetObjectParam("Mux-Video-nPad",
                               (int)m_pMPEGTSMuxFilter->streams[STREAM_IDX_VIDEO]->pStats.nPad);
    }
}
