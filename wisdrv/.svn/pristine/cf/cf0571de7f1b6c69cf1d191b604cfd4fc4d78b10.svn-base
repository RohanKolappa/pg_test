/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        PacGen_7007SB.h
*
*    DESCRIPTION:
*        
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#ifndef _PAC_GEN_7007SB_H_
#define _PAC_GEN_7007SB_H_

#define		TAG_DQVMEC_REG		0xFFE0
#define		TAG_MPEG1_REG		0xFFE1
#define		TAG_MPEG2_REG		0xFFE2
#define		TAG_H263_REG		0xFFE3
#define		TAG_MPEG4_REG		0xFFE4
#define		TAG_MJPEG_REG		0xFFE5
#define		TAG_ICX_SRAM		0xFFE6
#define		TAG_ICY_SRAM		0xFFE7
#define		TAG_IVLC_SRAM		0xFFE8
#define		TAG_T1212_SRAM		0xFFE9
#define		TAG_T3412_SRAM		0xFFEA
#define		TAG_T123_SRAM		0xFFEB
#define		TAG_T343_SRAM		0xFFEC
#define		TAG_TMJ12_SRAM		0xFFED
#define		TAG_TMJ3_SRAM		0xFFEE
#define		TAG_DMA_SRAM		0xFFEF
#define		TAG_SCH_SRAM		0xFFF0
#define		TAG_GPIO_SRAM		0xFFF1
#define		TAG_VLCBOOK_DRAM	0xFFF2
#define		TAG_OSDFONTIP_DRAM	0xFFF3
#define		TAG_OSDFONTIPB_DRAM	0xFFF4
#define		TAG_QTBL12_DRAM		0xFFF5
#define		TAG_QTBLH3_DRAM		0xFFF6
#define		TAG_QTBLM4_DRAM		0xFFF7
#define		TAG_MJPEGHDR_DRAM	0xFFF8
#define		TAG_ADDIO_REG		0xFFF9

#define VLC_SCH_LINE				12
#define SDRAM_REFRESH_SCH_LINE		44
#define SEQUENCE_TO_PACKAGE			0
#define SEQUENCE_TO_BUFFER			1

typedef struct _tpackage {
	UINT16 init_buff[32];
}TPackage;

typedef struct {
	UINT16 tag;
	UINT32 len;
	UINT16 *buf;
}TInitBuffer;

typedef struct {
	SINT16 MB_ADDR;
	SINT16 DQ_ADDR;
	SINT16 SGOP_ADDR;
	SINT16 QD0_ADDR;
	SINT16 QD1_ADDR;
	SINT16 QD2_ADDR;
	SINT16 QD3_ADDR;
	SINT32 SGOP_EXPT_ADDR;
	SINT32 SGOP_PEAK_ADDR;
	SINT32 VBV_ALERT_ADDR;
	SINT32 MIN_LMT_ADDR;
	SINT32 MAX_LMT_ADDR;
	SINT32 COMPLEX0_ADDR;
	SINT32 COMPLEX1_ADDR;
	SINT32 COMPLEX2_ADDR;
	SINT32 COMPLEX3_ADDR;
	SINT32 SGOP_USED_ADDR;
	SINT32 TOTAL_BITS_USED_ADDR;
	SINT32 TOTAL_EXPT_ADDR;
	SINT32 VBV_LEVEL_ADDR;
	SINT16 FRAME_INDEX_ADDR;
	SINT16 SGOP_Q_ADDR;
	SINT16 Q_ADDR;
	SINT16 PQ_ADDR;
	SINT16 MODE0_Q_ADDR;
	SINT16 MODE1_Q_ADDR;
	SINT16 MODE2_Q_ADDR;
	SINT16 MODE3_Q_ADDR;
	SINT32 P_MODE_OFFSET;
	SINT32 RC_SGOP0_ADDR;
	SINT32 RC_SGOP1_ADDR;
	SINT32 RC_SGOP2_ADDR;
	SINT32 RC_SGOP3_ADDR;
	SINT16 SGOP_COEF_ADDR;
	SINT32 SGOP_NEXT_ADDR;
	SINT32 SUM_Q_ADDR;
	SINT32 SUM_USED_ADDR;
	SINT32 SUM_EXPT_ADDR;
	SINT32 SUM_SGOP_ADDR;
} APRIL;

#ifndef _SDK_PACGEN_

void	gen_init_packages(void *packBuffer, SINT32 *pack_no, UINT8 *iniBuffer, SINT32 buflen,
						  SINT32 width, SINT32 height, SINT8 fgs_elbp,
						  CHIP_CONFIG chip_con, OSD_CONFIG osd_con, IIP_CONFIG iip_con, MOTION_DETECTION_CONFIG_t md_con,
						  DEINTERLACE_CONFIG deinterlace_con, AUDIOS_CONFIG audio_con);

static SINT32 quantizer_scale_matrix[16] = {
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	0, 1, -1, 1, -1, 2, -1, 1, -2, 1, 1, -1, 1, -2, 1, -1
};

static SINT32 ACCoeff_threshold_matrix[16] = {
	0, 0, 1, 2, 1, 3, 2, 1, 0, 1, 2, 3, 1, 2, 0, 3
//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#else

typedef enum {
	DRAM7007U_4M = 1,
	DRAM7007U_8M = 2
} E7007U_DRAM;

typedef enum {
	GO7007U_X_SCALE	= 1,
	GO7007U_Y_SCALE	= 2,
	GO7007U_XYSCALE	= 3,
	GO7007U_NOSCALE	= 0
} E7007U_PictureScale;

typedef enum {
	GO7007U_MIDIAN	= 1,
	GO7007U_LOWPASS	= 2,
	GO7007U_NOFILTER = 0
} E7007U_PreFilter;

typedef struct {
	SINT32 main_divider;
	SINT32 BCLK_divider;
	SINT32 OKI_mode;
	SINT32 OKI_shortframe_mode_clock_gen;
	SINT32 clock_generator_enable;
	SINT32 I2S_master_mode;
	SINT32 I2S_mode;
	SINT32 invert_phase_BCLK;
	SINT32 OKI_shortframe_mode_input_config;
	SINT32 word_length;
	SINT32 auto_SOF_generation;
	SINT32 left_channel_only;
	SINT32 bit8_filter;
	SINT32 AC97_enable;
	SINT32 audio_signature_bit;
	SINT32 buffer_config;
	SINT32 sample_mapping_reg1;
	SINT32 sample_mapping_reg2;
	SINT32 sample_mapping_reg3;
	SINT32 sample_mapping_reg4;
	SINT32 sample_mapping_reg5;
	SINT32 sample_mapping_reg6;
	SINT32 sample_mapping_reg7;
	SINT32 adpcm_enable;
	SINT32 adpcm_mode;
	SINT32 bytes_per_block;
	SINT32 predictor;
	SINT32 delta_left;
	SINT32 delta_right;
	SINT32 coef1_left;
	SINT32 coef1_right;
	SINT32 coef2_left;
	SINT32 coef2_right;
	SINT32 sample_per_block;
	SINT32 reserved;
} AUDIOS_CONFIG;

typedef struct {
	UINT32 DoOSD;
	UINT32 OSDyc0;
	UINT32 OSDyc1;
	UINT32 OSDuc0;
	UINT32 OSDuc1;
	UINT32 OSDvc0;
	UINT32 OSDvc1;
	UINT32 reserved;
} OSD_CONFIG;

typedef struct {
	SINT32 THedge;
	SINT32 THmotion;
	SINT32 THblending;
	SINT32 THbigedge;
	SINT32 THsmalledge;
	SINT32 THedgestatistics;
	SINT32 THmotionstatistics;
	SINT32 THbigedgestatistics;
	SINT32 THsmalledgestatistics;
	SINT32 DeInter_Always_Motion;
	SINT32 DeInter_Always_Blending;
	SINT32 DeInter_Always_Weave;
	SINT32 reserved;
} DEINTERLACE_CONFIG;

typedef struct {
	SINT32 wis_proprietary_stream;
	SINT32 is_closed_gop;
	SINT32 compress_mode;
	SINT32 nSubGOP;
	SINT32 nGOP;
	SINT32 dramType;
	SINT32 seqMode;
	SINT32 iniQScale;
	SINT32 inideQScale;
	SINT32 iniPQScale;
	SINT32 iniPdeQScale;
	SINT32 iniBQScale;
	SINT32 iniBdeQScale;
	SINT32 dram_cas;
	SINT32 if_read_param;
	SINT32 dump_to_dram;
	SINT32 do_bitrate_control;
	SINT32 brctrl_dup_enable;
	SINT32 brctrl_mb_enable;
	SINT32 brctrl_qmin;
	SINT32 brctrl_qmax;
	SINT32 brctrl_window_size;
	SINT32 brctrl_peak_rate;
	SINT32 target_bitrate;
	SINT32 brctrl_vbv;
	SINT32 brctrl_converge_speed;
	SINT32 brctrl_Q;
	SINT32 brctrl_LAMBDA;
	SINT32 control_period;
	SINT32 ivtc_holding_time;
	SINT32 hv_resync_enable;
	SINT32 DVD_compliant;
	SINT32 aspect_ratio;
	SINT32 cfg_vbi_enable;
	SINT32 SAA7113_vbi_enable;
	SINT32 cfg_yuvj_rgb;
	SINT32 cfg_interlace;
	SINT32 cfg_sensor_noise_enable;
	SINT32 cfg_user_Qtable;
	SINT32 cfg_bayer_pattern;
	SINT32 cfg_y_sub_window;
	SINT32 cfg_x_sub_window;
	SINT32 cfg_y_scale_enb;
	SINT32 cfg_x_scale_enb;
	SINT32 cfg_v_filter_enb;
	SINT32 cfg_h_filter_enb;
	SINT32 cfg_v_filter_format;
	SINT32 cfg_h_filter_format;
	SINT32 cfg_filter_nAX;
	SINT32 cfg_filter_nBX;
	SINT32 cfg_filter_nCX;
	SINT32 cfg_filter_nAY;
	SINT32 cfg_filter_nBY;
	SINT32 cfg_filter_nCY;
	SINT32 cfg_drop_frame;
	SINT32 cfg_sub_sample;
	SINT32 cfg_y_sub_offset;
	SINT32 cfg_y_sub_count;
	SINT32 cfg_x_sub_offset;
	SINT32 cfg_x_sub_count;
	SINT32 cfg_mode_656;
	SINT32 cfg_eof_delay;
	SINT32 cfg_sensor_desc;
	SINT32 cfg_deinterlace;
	SINT32 cfg_iip_enable;
	SINT32 cfg_ivtc_enable;
	SINT32 cfg_osd_enable;
	SINT32 cfg_sub_mode;
	SINT32 cfg_four_channel;
	SINT32 search_range;
	SINT32 gop_header_enable;
	SINT32 seq_header_enable;
	SINT32 Frame_rate;
	SINT32 scene_change_quota;
	SINT32 THACCoeffSet0;
	SINT32 THACCoeffStartpoint;
	SINT32 test_quant_change_in_mb;
	SINT32 HPIBufferCapacity;
	SINT32 cfg_av_sync_enable;
	SINT32 clock_rate;
	SINT32 myFOURCC;
	SINT32 has_audio_package;
	SINT32 video_format;
	SINT32 cfg_motion_detection_enable;
	SINT32 v_sync_bitplane;
	SINT32 mp4_repeat_seqhead;
	SINT32 reserved;
} CHIP_CONFIG;

typedef struct {
	SINT32 Bit_Length;
	SINT32 CCM_R_0;
	SINT32 CCM_R_1;
	SINT32 CCM_R_2;
	SINT32 CCM_R_3;
	SINT32 CCM_G_0;
	SINT32 CCM_G_1;
	SINT32 CCM_G_2;
	SINT32 CCM_G_3;
	SINT32 CCM_B_0;
	SINT32 CCM_B_1;
	SINT32 CCM_B_2;
	SINT32 CCM_B_3;
	SINT32 R_Gain;
	SINT32 G_Gain;
	SINT32 B_Gain;
	SINT32 R_Offset;
	SINT32 G_Offset;
	SINT32 B_Offset;
	SINT32 Gamma_R_P0_C;
	SINT32 Gamma_R_P0_Y;
	SINT32 Gamma_R_P1_C;
	SINT32 Gamma_R_P1_Y;
	SINT32 Gamma_R_P2_C;
	SINT32 Gamma_R_P2_Y;
	SINT32 Gamma_R_P3_C;
	SINT32 Gamma_R_P3_Y;
	SINT32 Gamma_R_P4_C;
	SINT32 Gamma_R_P4_Y;
	SINT32 Gamma_R_P5_C;
	SINT32 Gamma_R_P5_Y;
	SINT32 Gamma_R_P6_C;
	SINT32 Gamma_R_P6_Y;
	SINT32 Gamma_R_P7_C;
	SINT32 Gamma_R_P7_Y;
	SINT32 Gamma_R_P8_C;
	SINT32 Gamma_R_P8_Y;
	SINT32 Gamma_G_P0_C;
	SINT32 Gamma_G_P0_Y;
	SINT32 Gamma_G_P1_C;
	SINT32 Gamma_G_P1_Y;
	SINT32 Gamma_G_P2_C;
	SINT32 Gamma_G_P2_Y;
	SINT32 Gamma_G_P3_C;
	SINT32 Gamma_G_P3_Y;
	SINT32 Gamma_G_P4_C;
	SINT32 Gamma_G_P4_Y;
	SINT32 Gamma_G_P5_C;
	SINT32 Gamma_G_P5_Y;
	SINT32 Gamma_G_P6_C;
	SINT32 Gamma_G_P6_Y;
	SINT32 Gamma_G_P7_C;
	SINT32 Gamma_G_P7_Y;
	SINT32 Gamma_G_P8_C;
	SINT32 Gamma_G_P8_Y;
	SINT32 Gamma_B_P0_C;
	SINT32 Gamma_B_P0_Y;
	SINT32 Gamma_B_P1_C;
	SINT32 Gamma_B_P1_Y;
	SINT32 Gamma_B_P2_C;
	SINT32 Gamma_B_P2_Y;
	SINT32 Gamma_B_P3_C;
	SINT32 Gamma_B_P3_Y;
	SINT32 Gamma_B_P4_C;
	SINT32 Gamma_B_P4_Y;
	SINT32 Gamma_B_P5_C;
	SINT32 Gamma_B_P5_Y;
	SINT32 Gamma_B_P6_C;
	SINT32 Gamma_B_P6_Y;
	SINT32 Gamma_B_P7_C;
	SINT32 Gamma_B_P7_Y;
	SINT32 Gamma_B_P8_C;
	SINT32 Gamma_B_P8_Y;
	SINT32 Black_P;
	SINT32 White_P;
	SINT32 AWB_WIN_L;
	SINT32 AWB_WIN_T;
	SINT32 AWB_WIN_R;
	SINT32 AWB_WIN_B;
	SINT32 SUM_R1;
	SINT32 SUM_G1;
	SINT32 SUM_B1;
	SINT32 AWB_N_VLD_W1;
	SINT32 SUM_R2;
	SINT32 SUM_G2;
	SINT32 SUM_B2;
	SINT32 AWB_N_VLD_W2;
	SINT32 SUM_R3;
	SINT32 SUM_G3;
	SINT32 SUM_B3;
	SINT32 AWB_N_VLD_W3;
	SINT32 SUM_R4;
	SINT32 SUM_G4;
	SINT32 SUM_B4;
	SINT32 AWB_N_VLD_W4;
	SINT32 SUM_R5;
	SINT32 SUM_G5;
	SINT32 SUM_B5;
	SINT32 AWB_N_VLD_W5;
	SINT32 AE_WIN_L;
	SINT32 AE_WIN_T;
	SINT32 AE_WIN_R;
	SINT32 AE_WIN_B;
	SINT32 MAX_Y_CTR;
	SINT32 MIN_Y_CTR;
	SINT32 SUM_Y_CTR;
	SINT32 VALID_NUM_CTR;
	SINT32 MAX_Y_BK;
	SINT32 MIN_Y_BK;
	SINT32 SUM_Y_BK;
	SINT32 VALID_NUM_BK;
	SINT32 B_Strgth;
	SINT32 B_Thrshld;
	SINT32 SAT;
	SINT32 HUE;
	SINT32 Y_Gain;
	SINT32 Y_Offset;
	SINT32 LPF_Switch;
	SINT32 LPF_COEF_Y_0;
	SINT32 LPF_COEF_Y_1;
	SINT32 LPF_COEF_Y_2;
	SINT32 LPF_COEF_Cr_0;
	SINT32 LPF_COEF_Cr_1;
	SINT32 LPF_COEF_Cr_2;
	SINT32 LPF_COEF_Cb_0;
	SINT32 LPF_COEF_Cb_1;
	SINT32 LPF_COEF_Cb_2;
	SINT32 reserved;
} IIP_CONFIG;

extern MOTION_DETECTION_CONFIG_t sMotionDetectionConfiguration;





#include <Board.h>

SINT32 gen_init_packages_7007SB(UINT8 *iniBuffer,
								SINT32 buflen,
								TCFGVIDEOEX *conf_sdk,
								TVIDEOCFGFIX *conf2_sdk,
								char *error_info,
								char *memory,
								SINT32 *memory_size,
								UINT8 *osd_font,
								SINT32 osd_font_size,
								UINT8 *dup_mem,
								UINT8 *seqheader_mem,
								SINT8 reset_type);
								// reset_type: 0 = normal reset, 1 = quick reset

#endif

SINT32	get_init_buffer(TInitBuffer *tBuff, UINT8 *iniBuffer, SINT32 buflen);
SINT32	find_init_buffer(TInitBuffer *tBuff, UINT16 tag);
void	free_init_buffer(TInitBuffer *tBuff);
void	gen_dqvmec_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	gen_icxy_ivlc_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	gen_dma_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	gen_sch_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode, SINT32 clock_rate);
void	gen_mpegx_reg_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode);
void	gen_mpegx_tbl_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode);
void	gen_vlcbook_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	gen_qdeq_tbl_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 compress_mode, 
						  SINT8 default_iq, SINT8 default_niq, UINT8 *intra_quant_matrix, UINT8 *inter_quant_matrix);
void	gen_osdfont_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT32 seqMode);
void	gen_osdfont_pack2(TPackage *ex_package,SINT32 *pack_no,UINT8 *osd_font,SINT32 osd_font_size,SINT32 seqMode);
void	gen_mjpeghdr_to_package(TPackage *ex_package, SINT32 *pack_no, SINT32 cols, SINT32 rows);
void	gen_mjpeghdr_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	gen_quantchange_pack(TPackage *ex_package, SINT32 *pack_no, SINT8 test_quant_change_in_mb);
void	gen_gpio_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff, SINT8 fgs_elbp, CHIP_CONFIG *chip_con);
void	gen_osdindex_pack(TPackage *ex_package, SINT32 *pack_no, SINT32 height);
void	config_package(TPackage *ex_package, SINT32 *pack_no, CHIP_CONFIG *chip_con, DEINTERLACE_CONFIG *deinterlace_con, SINT32 width, SINT32 height);
void	final_package(TPackage *ex_package, SINT32 *pack_no, SINT8 fgs_elbp, SINT32 width, SINT32 height, CHIP_CONFIG *chip_con);
void	osddeinter_to_package(TPackage *ex_package, SINT32 *pack_no, DEINTERLACE_CONFIG *deinterlace_con, OSD_CONFIG *osd_con);
void	iip_config_convert(SINT32 *count, UINT16 *iip_Configuration, IIP_CONFIG *iip_con, SINT32 x_sub_count, SINT32 y_sub_count);
void	iip_to_package(TPackage *ex_package, SINT32 *pack_no, CHIP_CONFIG *chip_con, IIP_CONFIG *iip_con);
void	md_to_package(TPackage *ex_package, SINT32 *pack_no, MOTION_DETECTION_CONFIG_t *md_con);
void	audio_to_package(TPackage *ex_package, SINT32 *pack_no, AUDIOS_CONFIG *audio_config);
void	gen_addio_pack(TPackage *ex_package, SINT32 *pack_no, TInitBuffer *tBuff);
void	brctrl_to_package(TPackage *ex_package, SINT32 *pack_no, APRIL fool, UINT16 *dupframe_length);
void	dupframe_to_package(TPackage *ex_package, SINT32 *pack_no, SINT32 width, SINT32 height, CHIP_CONFIG *chip_con, UINT16 *dupframe_length);
void	dupfrmMP4_to_package(TPackage *ex_package, SINT32 *pack_no, SINT32 width, SINT32 height, CHIP_CONFIG *chip_con, UINT16 *dupframe_length);
void	blank_Pframe(SINT32 width, SINT32 height, CHIP_CONFIG *chip_con, UINT8 *dup_mem);
void	seqhead_to_package(SINT8 seqhead_choice,
							TPackage *ex_package,
							SINT32 *pack_no,
							UINT8 *seqhead,
							SINT32 *seqlen,
							CHIP_CONFIG *chip_con,
							SINT32 width,
							SINT32 height);
void	Setup(APRIL *fool,	SINT32 ttarget_rate, SINT32 ppeak_rate,
			  SINT32 vvbv, SINT32 cconverge_speed, SINT32 LLAMBDA,
			  SINT32 QQ, SINT32 ffps, SINT32 ff_area, SINT32 ssgop_mode,
			  SINT32 qmin, SINT32 qmax, SINT32 window_size);
void	generator(TPackage *package_buf, SINT32 *pack_no, TInitBuffer *tBuff,
				  CHIP_CONFIG *chip_con, OSD_CONFIG *osd_con, IIP_CONFIG *iip_con, MOTION_DETECTION_CONFIG_t *md_con,
				  DEINTERLACE_CONFIG *deinterlace_con, AUDIOS_CONFIG *audio_con,
				  SINT32 width, SINT32 height, SINT8 fgs_elbp, SINT8 additional_io,
				  UINT8 *osd_font, SINT32 osd_font_size, SINT8 reset_type);

#endif

/****************************** end of PacGen_7007SB.h ***********************/
