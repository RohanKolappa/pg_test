/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        PackGen.c
*
*    DESCRIPTION:
*        This file generates the packets for communicating with firmware
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/


#include "platform.h"
#include "PackGen.h"
#include "PacGen_7007SB.h"
#include "CfgFileParser.h"
#include "function.h"


void FormatSysFileName(SINT8 *FileName);


#define GET_FIX_CONFIG_INT(Struct,Field)		\
{												\
	SINT32 Value;								\
	if(GetNextConfigInt(fp, #Field, &Value)==0)	\
		Struct->Field = Value;					\
}

void DumpFixConfig(TVIDEOCFGFIX *fix_config)
{
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- osdcfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DoOSD                   : %d",	fix_config->osdcfg.DoOSD));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDyc0                  : %d",	fix_config->osdcfg.OSDyc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDyc1                  : %d",	fix_config->osdcfg.OSDyc1));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDuc0                  : %d",	fix_config->osdcfg.OSDuc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDuc1                  : %d",	fix_config->osdcfg.OSDuc1));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDvc0                  : %d",	fix_config->osdcfg.OSDvc0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("OSDvc1                  : %d",	fix_config->osdcfg.OSDvc1));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- thcfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THACCoeffSet0           : %d",	fix_config->thcfg.THACCoeffSet0));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THACCoeffStartpoint     : %d",	fix_config->thcfg.THACCoeffStartpoint));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THedge                  : %d",	fix_config->thcfg.THedge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THmotion                : %d",	fix_config->thcfg.THmotion));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THblending              : %d",	fix_config->thcfg.THblending));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THbigedge               : %d",	fix_config->thcfg.THbigedge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THsmalledge             : %d",	fix_config->thcfg.THsmalledge));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THedgestatistics        : %d",	fix_config->thcfg.THedgestatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THmotionstatistics      : %d",	fix_config->thcfg.THmotionstatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THbigedgestatistics     : %d",	fix_config->thcfg.THbigedgestatistics));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("THsmalledgestatistics   : %d",	fix_config->thcfg.THsmalledgestatistics));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- deintercfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Motion   : %d",	fix_config->deintercfg.DeInter_Always_Motion));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Blending : %d",	fix_config->deintercfg.DeInter_Always_Blending));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DeInter_Always_Weave    : %d",	fix_config->deintercfg.DeInter_Always_Weave));

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("  --------- elsecfg ---------"));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("HPIBufferCapacity       : %d",	fix_config->elsecfg.HPIBufferCapacity));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("clockrate               : %d",	fix_config->elsecfg.clockrate));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("DRAM                    : %d",	fix_config->elsecfg.DRAM));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("ivtc_holding_time       : %d",	fix_config->elsecfg.ivtc_holding_time));
	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("HasAudio                : %d",	fix_config->elsecfg.HasAudio));
}

int GetFixConfig(PDEVICE_EXTENSION_COMMON pdxc, TVIDEOCFGFIX *fix_config)
{
	char Str[256];
	FILE *fp;

	GetConfigFilePath(Str);
	strcat(Str, "fix_setting.txt");

	fp = fopen(Str,"r");
	if(fp==NULL)
	{
		return 1;
	}

	// osd setting

	GET_FIX_CONFIG_INT(fix_config, osdcfg.DoOSD);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDyc0);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDyc1);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDuc0);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDuc1);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDvc0);
	GET_FIX_CONFIG_INT(fix_config, osdcfg.OSDvc1);

	// Threshold

	GET_FIX_CONFIG_INT(fix_config, thcfg.THACCoeffSet0);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THACCoeffStartpoint);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THedge);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THmotion);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THblending);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THbigedge);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THsmalledge);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THedgestatistics);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THmotionstatistics);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THbigedgestatistics);
	GET_FIX_CONFIG_INT(fix_config, thcfg.THsmalledgestatistics);

	// Deinterlace

	GET_FIX_CONFIG_INT(fix_config, deintercfg.DeInter_Always_Motion);
	GET_FIX_CONFIG_INT(fix_config, deintercfg.DeInter_Always_Blending);
	GET_FIX_CONFIG_INT(fix_config, deintercfg.DeInter_Always_Weave);
		
	// IIP setting

	GET_FIX_CONFIG_INT(fix_config, iipcfg.Bit_Length);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_R_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_R_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_R_2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_R_3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_G_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_G_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_G_2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_G_3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_B_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_B_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_B_2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.CCM_B_3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.R_Gain);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.G_Gain);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.B_Gain);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.R_Offset);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.G_Offset);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.B_Offset);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P0_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P0_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P1_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P1_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P2_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P2_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P3_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P3_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P4_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P4_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P5_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P5_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P6_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P6_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P7_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P7_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P8_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_R_P8_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P0_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P0_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P1_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P1_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P2_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P2_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P3_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P3_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P4_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P4_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P5_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P5_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P6_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P6_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P7_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P7_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P8_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_G_P8_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P0_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P0_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P1_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P1_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P2_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P2_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P3_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P3_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P4_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P4_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P5_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P5_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P6_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P6_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P7_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P7_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P8_C);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Gamma_B_P8_Y);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Black_P);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.White_P);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_WIN_L);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_WIN_T);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_WIN_R);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_WIN_B);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_R1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_G1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_B1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_N_VLD_W1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_R2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_G2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_B2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_N_VLD_W2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_R3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_G3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_B3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_N_VLD_W3);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_R4);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_G4);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_B4);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_N_VLD_W4);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_R5);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_G5);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_B5);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AWB_N_VLD_W5);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AE_WIN_L);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AE_WIN_T);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AE_WIN_R);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.AE_WIN_B);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.MAX_Y_CTR);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.MIN_Y_CTR);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_Y_CTR);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.VALID_NUM_CTR);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.MAX_Y_BK);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.MIN_Y_BK);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SUM_Y_BK);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.VALID_NUM_BK);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.B_Strgth);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.B_Thrshld);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.SAT);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.HUE);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Y_Gain);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.Y_Offset);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_Switch);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Y_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Y_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Y_2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cr_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cr_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cr_2);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cb_0);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cb_1);
	GET_FIX_CONFIG_INT(fix_config, iipcfg.LPF_COEF_Cb_2);

	// audio setting
	{
		AUDIO_CONFIG* audio_config;

		audio_config = Config_GetCurrentAudioConfig(&pdxc->config_manager);
		DumpAudioConfig(audio_config);

		if(pdxc->BoardInterface->pIBoardAudio != NULL)
			pdxc->BoardInterface->pIBoardAudio->GetAudioChipConfig(pdxc->BoardContext,(AVSTREAM_COOKIE)pdxc,&(fix_config->audiocfg), audio_config);
		else
			PIFillMem(&(fix_config->audiocfg), sizeof(TCFGAUDIO), 0);
	}

	// else setting

	GET_FIX_CONFIG_INT(fix_config, elsecfg.clockrate);
	GET_FIX_CONFIG_INT(fix_config, elsecfg.DRAM);
	GET_FIX_CONFIG_INT(fix_config, elsecfg.ivtc_holding_time);

	fclose(fp);

	if(pdxc->BoardInfo.BusMode == UM_USB_EZUSB)
		fix_config->elsecfg.HPIBufferCapacity = 7;
	else // UM_ONCHIP
		fix_config->elsecfg.HPIBufferCapacity = 0;

	fix_config->elsecfg.HasAudio = pdxc->DoAudio;

	DumpFixConfig(fix_config);

	return 0;
}

void BitrateControlPatch(TCFGVIDEOEX* video_config)
{
	if(video_config->strcfg.compress_mode == 1)	// MPEG2
	{
		if(video_config->strcfg.DVD_compliant == 1)
        {
            video_config->ctlcfg.peak_bitrate = 8000000;
        }
		else
        {
            video_config->ctlcfg.peak_bitrate = 
	            video_config->ctlcfg.target_bitrate * 12 / 10;
        }

        video_config->ctlcfg.vbv_buffer = 900000;
	}

	if(video_config->strcfg.compress_mode == 0 &&	// MPEG1
	   video_config->ctlcfg.vbv_buffer == 327680)	// VCD complaint
	{
		video_config->ctlcfg.peak_bitrate = 1000000;
		video_config->ctlcfg.vbv_buffer = 163840;
	}
}

int GenInitialPacket(PDEVICE_EXTENSION_COMMON pdxc, UINT8 *pPack, SINT32 *PackLen)
{
	int r=0;

#if 1

	TCFGVIDEOEX* video_config;
	TVIDEOCFGFIX fix_config;

	SINT8 error_info[256];
	UINT8 *iniBuffer; 
	int buflen;
	char FileName[256] = "go7007sb.bin";

	buflen = 256*1024;
	iniBuffer = PIAllocVirtualMem(buflen);

	FormatSysFileName(FileName);
	if(ReadFileToBuffer(FileName, iniBuffer, &buflen)>0)
	{
		PIFreeVirtualMem(iniBuffer);
		return 1;
	}

	GetFixConfig(pdxc, &fix_config);

	video_config = Config_GetCurrentVideoConfig(&pdxc->config_manager);
	
	BitrateControlPatch(video_config);
	DumpVideoConfig(video_config);

	r = gen_init_packages_7007SB(iniBuffer,
						 buflen,
						 video_config,
						 &fix_config,
						 error_info,
						 pPack,
						 PackLen,
						 pdxc->pBlankPFrame);

	PIFreeVirtualMem(iniBuffer);

	PrintMsg(DEBUG_PACKGEN, ("Pac Gen: return: %d, len: %d, mesg: %s",r,*PackLen,error_info));
//	WriteBufferToFile("\\??\\c:\\init_pack_sdk.bin",pPack,*PackLen);

#else

#if defined(_WINDOWS)
	char InitPackFileName[256] = "\\??\\c:\\init_pack_sdk.bin";
#elif defined(_LINUX)
	char InitPackFileName[256] = "/root/init_pack_sdk.bin";
#endif

	if(ReadFileToBuffer(InitPackFileName, pPack, PackLen)>0)
	{
		PrintMsg(DEBUG_PACKGEN, ("InitializeDevice : Read init pack file fail"));
		return 1;
	}

#endif

	return r;
}

