/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        config_helper.c
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

#include "struct.h"
#include "config_error.h"
#include "config_helper.h"
#include "platform.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// confiuration helper
///////////////////////////////////////////////////////////////////////////////

unsigned char IsFlagsSet(unsigned long test_flags, unsigned long mandetory_flags)
{
	if ( test_flags == 0 ) return 1;	// we treat 0 as 0xFFFFFFFF
	return ( ( (~(test_flags)) & (mandetory_flags) ) == 0 ) ? 1 : 0;
}

unsigned char IsCompatibleWithCurrentTVStandard(TV_STANDARD tv_standard, unsigned long standards_mask)
{
	if ( tv_standard == TVStandard_None )
		return standards_mask == TVStandard_None;

	if ( standards_mask == TVStandard_None )
		return tv_standard == TVStandard_None;
	
	return IsFlagsSet(standards_mask, tv_standard);
}

long FindStreamConfiguration(_VIDEO_CAPABILITIES* caps, TCFGSTREAM* stream)
{
	unsigned long index;

	for ( index = 0 ; index < caps->_num_of_stream_configs ; index ++ )
	{
		TCFGSTREAM* stream_caps = caps->_stream_configs + index;

		// compress mode must be same
		if ( stream->compress_mode != stream_caps->compress_mode ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_MPEG4_MODE ) )
			if ( stream->mpeg4_mode != stream_caps->mpeg4_mode ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_DVD_COMPLIANT ) )
			if ( stream->DVD_compliant != stream_caps->DVD_compliant ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_SEQUENCE_MODE ) )
			if ( stream->sequence != stream_caps->sequence ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_GOP_MODE ) )
			if ( stream->gop_mode != stream_caps->gop_mode ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_GOP_SIZE ) )
			if ( stream->gop_size != stream_caps->gop_size ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_DEINTERLACE_MODE ) )
			if ( stream->deinterlace_mode != stream_caps->deinterlace_mode ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_SEARCH_RANGE ) )
			if ( stream->search_range != stream_caps->search_range ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_GOPHEAD_ENABLE ) )
			if ( stream->gop_head_enable != stream_caps->gop_head_enable ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_SEQHEAD_ENABLE ) )
			if ( stream->seq_head_enable != stream_caps->seq_head_enable ) continue;

		if ( IsFlagsSet( stream_caps->header.flags, FLAGS_STREAM_ASPECT_RATIO ) )
			if ( stream->aspect_ratio != stream_caps->aspect_ratio ) continue;

//		memcpy(stream->header.name, stream_caps->header.name, sizeof(stream->header.name));
//		memcpy(stream->header.desc, stream_caps->header.desc, sizeof(stream->header.desc));

		return index;
	}

	return -1;
}

long FindFrameRateConfiguration(_VIDEO_CAPABILITIES* caps, TCFGFRAMERATE* framerate)
{
	unsigned long index;

	for ( index = 0 ; index < caps->_num_of_framerate_configs ; index ++ )
	{
		TCFGFRAMERATE* framerate_caps = caps->_framerate_configs + index;

		// frame rate and tv standard must be same
		if ( framerate->frame_rate != framerate_caps->frame_rate ) continue; 
		if ( ! IsCompatibleWithCurrentTVStandard(framerate->tv_standard, framerate_caps->tv_standard) ) continue;

		if ( IsFlagsSet( framerate_caps->header.flags, FLAGS_FRAMERATE_DROP_FRAME ) )
			if ( framerate->drop_frame != framerate_caps->drop_frame ) continue; 

		if ( IsFlagsSet( framerate_caps->header.flags, FLAGS_FRAMERATE_IVTC_ENABLE ) )
			if ( framerate->ivtc_enable != framerate_caps->ivtc_enable ) continue;

//		memcpy(framerate->header.name, framerate_caps->header.name, sizeof(framerate->header.name));
//		memcpy(framerate->header.desc, framerate_caps->header.desc, sizeof(framerate->header.desc));

		return index;
	}

	return -1;
}

long FindResolutionConfiguration(_VIDEO_CAPABILITIES* caps, TCFGRESOLUTION* resolution)
{
	unsigned long index;

	for ( index = 0 ; index < caps->_num_of_resolution_configs ; index ++ )
	{
		TCFGRESOLUTION* resolution_caps = caps->_resolution_configs + index;

		// width, height and tv standard must be same
		if ( resolution->width != resolution_caps->width ) continue; 
		if ( resolution->height != resolution_caps->height ) continue; 
		if ( ! IsCompatibleWithCurrentTVStandard(resolution->tv_standard, resolution_caps->tv_standard) ) continue;

		if ( IsFlagsSet( resolution_caps->header.flags, FLAGS_RESOLUTION_SCALE_OFFSET ) )
		{
			if ( resolution->h_sub_offset != resolution_caps->h_sub_offset ) continue; 
			if ( resolution->v_sub_offset != resolution_caps->v_sub_offset ) continue; 
			if ( resolution->h_scale_enb != resolution_caps->h_scale_enb ) continue; 
			if ( resolution->v_scale_enb != resolution_caps->v_scale_enb ) continue; 
		}

//		memcpy(resolution->header.name, resolution_caps->header.name, sizeof(resolution->header.name));
//		memcpy(resolution->header.desc, resolution_caps->header.desc, sizeof(resolution->header.desc));
		resolution->max_bitrate = resolution_caps->max_bitrate;
		resolution->min_bitrate = resolution_caps->min_bitrate;

		return index;
	}

	return -1;
}

/*
long FindBitRateConfiguration(_VIDEO_CAPABILITIES* caps, TCFGBRCTRL* bitrate)
{
	unsigned long index;

#define FLAGS_BITRATE_VBV_BUFFER			0x00000010
#define FLAGS_BITRATE_CONVERGE_SPEED		0x00000020
#define FLAGS_BITRATE_LAMBDA				0x00000040
#define FLAGS_BITRATE_IPBQ					0x00000100

	for ( index = 0 ; index < caps->_num_of_bitrate_configs ; index ++ )
	{
		TCFGBRCTRL* bitrate_caps = caps->_bitrate_configs + index;

		// target bitrate and Q must be same
		if ( bitrate->target_bitrate != bitrate_caps->target_bitrate ) continue; 
		if ( bitrate->Q != bitrate_caps->Q ) continue; 

		if ( IsFlagsSet( bitrate_caps->header.flags, FLAGS_BITRATE_PEAK ) )
			if ( bitrate->peak_bitrate != bitrate_caps->peak_bitrate ) continue;

		if ( IsFlagsSet( bitrate_caps->header.flags, FLAGS_BITRATE_VBV_BUFFER ) )
			if ( bitrate->vbv_buffer != bitrate_caps->vbv_buffer ) continue;

		if ( IsFlagsSet( bitrate_caps->header.flags, FLAGS_BITRATE_CONVERGE_SPEED ) )
			if ( bitrate->converge_speed != bitrate_caps->converge_speed ) continue;

		if ( IsFlagsSet( bitrate_caps->header.flags, FLAGS_BITRATE_LAMBDA ) )
			if ( bitrate->lambda != bitrate_caps->lambda ) continue;

		if ( IsFlagsSet( bitrate_caps->header.flags, FLAGS_BITRATE_IPBQ ) )
		{
			if ( bitrate->IQ != bitrate_caps->IQ ) continue;
			if ( bitrate->PQ != bitrate_caps->PQ ) continue;
			if ( bitrate->BQ != bitrate_caps->BQ ) continue;
		}

		memcpy(bitrate->header.name, bitrate_caps->header.name, sizeof(bitrate->header.name));
		memcpy(bitrate->header.desc, bitrate_caps->header.desc, sizeof(bitrate->header.desc));

		return index;
	}

	return -1;
}
*/
enum CONFIG_ERROR Config_IsAllowedByAssociations(
			_VIDEO_CAPABILITIES* caps, 
			TCFGASSOCIATION* assc)
{
	unsigned long index;
	for ( index = 0 ; index < caps->_num_of_associations ; index ++ )
	{
		TCFGASSOCIATION* association = caps->_associations + index;
		if ( association->_master_type != assc->_master_type ) continue;
		if ( association->_slave_type != assc->_slave_type ) continue; 
		if ( association->_master_id != assc->_master_id && association->_master_id != ASSOCIATION_ALL ) continue;
		if ( association->_slave_id != assc->_slave_id && association->_slave_id != ASSOCIATION_ALL ) continue;
		return ( association->_associate_type == ASSOCIATION_TYPE_ALLOW ) ? ERROR_NONE : ERROR_OTHERS;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// by default we allow this association
	///////////////////////////////////////////////////////////////////////////////////////
	return ERROR_NONE;
}

/*
enum CONFIG_ERROR Config_EnumVideoConfigurations(_VIDEO_CAPABILITIES* caps, unsigned long system, unsigned long stream)
{
	unsigned long res_index, fps_index;
	TCFGASSOCIATION association;

	caps->_num_of_configurations = 0;
	if ( caps->_configurations != NULL ) FREE_MEM(caps->_configurations);
	caps->_configurations = ALLOC_MEM( caps->_num_of_bitrate_configs * caps->_num_of_framerate_configs * caps->_num_of_resolution_configs * sizeof(TVCFG_ENTRY) );
	if ( caps->_configurations == NULL ) return ERROR_OUT_OF_MEMORY;

	///////////////////////////////////////////////////////////////////////////////////////
	// for each possible configuration combinations, prescreen invalid configurations:
	// 1 check the associations to check sanity
	// 2 check the tv standards to prevent incompatible framerate and resolution configurations
	// 3 check the bitrate range to prevent incompatible bitrate configurations
	///////////////////////////////////////////////////////////////////////////////////////
	for ( fps_index = 0 ; fps_index < caps->_num_of_framerate_configs ; fps_index ++ )
	{
		if ( caps->_framerate_configs[fps_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;

		for ( res_index = 0 ; res_index < caps->_num_of_resolution_configs ; res_index ++ )
		{
			if ( caps->_resolution_configs[res_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;

			///////////////////////////////////////////////////////////////////////////////////////
			// is the combination allowed by our association? : 
			// system <-> stream <-> fps_index <-> res_index <-> brc_index 
			///////////////////////////////////////////////////////////////////////////////////////
			association._master_type = TYPE_STREAM_CONFIG;
			association._master_id = stream;
			association._slave_type = TYPE_FRAMERATE_CONFIG;
			association._slave_id = fps_index;
			if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

			association._master_type = TYPE_STREAM_CONFIG;
			association._master_id = stream;
			association._slave_type = TYPE_RESOLUTION_CONFIG;
			association._slave_id = res_index;
			if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

			association._master_type = TYPE_FRAMERATE_CONFIG;
			association._master_id = fps_index;
			association._slave_type = TYPE_RESOLUTION_CONFIG;
			association._slave_id = res_index;
			if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

			PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("valid configuration: system %d, stream %d, framerate %d, resolution %d !!!", 
				system, stream, fps_index, res_index));

			caps->_configurations[caps->_num_of_configurations].framerate_index = fps_index;
			caps->_configurations[caps->_num_of_configurations].resolution_index = res_index;
			caps->_configurations[caps->_num_of_configurations].bitrate_index = -1;
			caps->_num_of_configurations ++;
		}
	}

	if ( caps->_num_of_configurations <= 0 ) // there is no valid configurations
		return ERROR_CONFIG_VIDEO_CAPABILITY;

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("total valid configuration: %d", caps->_num_of_configurations));

	return ERROR_NONE;
}

enum CONFIG_ERROR Config_EnumVideoConfigurationsEx(_VIDEO_CAPABILITIES* caps, unsigned long system, unsigned long stream)
{
	unsigned long brc_index, res_index, fps_index;
	TCFGASSOCIATION association;

	caps->_num_of_configurations = 0;
	if ( caps->_configurations != NULL ) FREE_MEM(caps->_configurations);
	caps->_configurations = ALLOC_MEM( caps->_num_of_bitrate_configs * caps->_num_of_framerate_configs * caps->_num_of_resolution_configs * sizeof(TVCFG_ENTRY) );
	if ( caps->_configurations == NULL ) return ERROR_OUT_OF_MEMORY;

	///////////////////////////////////////////////////////////////////////////////////////
	// for each possible configuration combinations, prescreen invalid configurations:
	// 1 check the associations to check sanity
	// 2 check the tv standards to prevent incompatible framerate and resolution configurations
	// 3 check the bitrate range to prevent incompatible bitrate configurations
	///////////////////////////////////////////////////////////////////////////////////////
	for ( fps_index = 0 ; fps_index < caps->_num_of_framerate_configs ; fps_index ++ )
	{
		if ( caps->_framerate_configs[fps_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;

		for ( res_index = 0 ; res_index < caps->_num_of_resolution_configs ; res_index ++ )
		{
			if ( caps->_resolution_configs[res_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;

			for ( brc_index = 0 ; brc_index < caps->_num_of_bitrate_configs ; brc_index ++ )
			{
				if ( ( caps->_bitrate_configs[brc_index].target_bitrate != 0 ) &&
					( caps->_bitrate_configs[brc_index].target_bitrate > caps->_resolution_configs[res_index].max_bitrate
					|| caps->_bitrate_configs[brc_index].target_bitrate < caps->_resolution_configs[res_index].min_bitrate ) )
					continue;

				///////////////////////////////////////////////////////////////////////////////////////
				// is the combination allowed by our association? : 
				// system <-> stream <-> fps_index <-> res_index <-> brc_index 
				///////////////////////////////////////////////////////////////////////////////////////
				association._master_type = TYPE_STREAM_CONFIG;
				association._master_id = stream;
				association._slave_type = TYPE_FRAMERATE_CONFIG;
				association._slave_id = fps_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_STREAM_CONFIG;
				association._master_id = stream;
				association._slave_type = TYPE_RESOLUTION_CONFIG;
				association._slave_id = res_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_STREAM_CONFIG;
				association._master_id = stream;
				association._slave_type = TYPE_BITRATE_CONFIG;
				association._slave_id = brc_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_FRAMERATE_CONFIG;
				association._master_id = fps_index;
				association._slave_type = TYPE_RESOLUTION_CONFIG;
				association._slave_id = res_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_FRAMERATE_CONFIG;
				association._master_id = fps_index;
				association._slave_type = TYPE_BITRATE_CONFIG;
				association._slave_id = brc_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_RESOLUTION_CONFIG;
				association._master_id = res_index;
				association._slave_type = TYPE_BITRATE_CONFIG;
				association._slave_id = brc_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("valid configuration: system %d, stream %d, framerate %d, resolution %d, bitrate %d !!!", 
					system, stream, fps_index, res_index, brc_index));

				caps->_configurations[caps->_num_of_configurations].framerate_index = fps_index;
				caps->_configurations[caps->_num_of_configurations].resolution_index = res_index;
				caps->_configurations[caps->_num_of_configurations].bitrate_index = brc_index;
				caps->_num_of_configurations ++;
			}
		}
	}

	if ( caps->_num_of_configurations <= 0 ) // there is no valid configurations
		return ERROR_CONFIG_VIDEO_CAPABILITY;

	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("total valid configuration: %d", caps->_num_of_configurations));

	return ERROR_NONE;
}
*/
enum CONFIG_ERROR Config_EnumVideoConfigurationsExx(_VIDEO_CAPABILITIES* caps, unsigned long system)
{
	unsigned long str_index, res_index, fps_index;
	TCFGASSOCIATION association;

	caps->_num_of_configurations = 0;
	if ( caps->_configurations != NULL ) FREE_MEM(caps->_configurations);
	caps->_configurations = (TVCFG_ENTRY*)ALLOC_MEM( caps->_num_of_stream_configs * 
										caps->_num_of_framerate_configs * 
										caps->_num_of_resolution_configs * 
										sizeof(TVCFG_ENTRY) );
	if ( caps->_configurations == NULL ) return ERROR_OUT_OF_MEMORY;

	///////////////////////////////////////////////////////////////////////////////////////
	// for each possible configuration combinations, prescreen invalid configurations:
	// 1 check the associations to check sanity
	// 2 check the tv standards to prevent incompatible framerate and resolution configurations
	// 3 check the bitrate range to prevent incompatible bitrate configurations
	///////////////////////////////////////////////////////////////////////////////////////
	for ( str_index = 0 ; str_index < caps->_num_of_stream_configs ; str_index ++ )
	{
		for ( fps_index = 0 ; fps_index < caps->_num_of_framerate_configs ; fps_index ++ )
		{
			if ( caps->_framerate_configs[fps_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;

			for ( res_index = 0 ; res_index < caps->_num_of_resolution_configs ; res_index ++ )
			{
				if ( caps->_resolution_configs[res_index].tv_standard != caps->_system_configs[system].tv_standard ) continue;


				///////////////////////////////////////////////////////////////////////////////////////
				// is the combination allowed by our association? : 
				// system <-> stream <-> fps_index <-> res_index <-> brc_index 
				///////////////////////////////////////////////////////////////////////////////////////
				association._master_type = TYPE_STREAM_CONFIG;
				association._master_id = str_index;
				association._slave_type = TYPE_FRAMERATE_CONFIG;
				association._slave_id = fps_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_STREAM_CONFIG;
				association._master_id = str_index;
				association._slave_type = TYPE_RESOLUTION_CONFIG;
				association._slave_id = res_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

				association._master_type = TYPE_FRAMERATE_CONFIG;
				association._master_id = fps_index;
				association._slave_type = TYPE_RESOLUTION_CONFIG;
				association._slave_id = res_index;
				if ( Config_IsAllowedByAssociations(caps, &association ) != ERROR_NONE ) continue;

#ifdef DRIVER
				//	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("valid configuration ex: system %d, stream %d, framerate %d, resolution %d!!!", 
				//			system, str_index, fps_index, res_index));
#endif
				caps->_configurations[caps->_num_of_configurations].stream_index = str_index;
				caps->_configurations[caps->_num_of_configurations].framerate_index = fps_index;
				caps->_configurations[caps->_num_of_configurations].resolution_index = res_index;
				caps->_num_of_configurations ++;
			}
		}
	}

	if ( caps->_num_of_configurations <= 0 ) // there is no valid configurations
		return ERROR_CONFIG_VIDEO_CAPABILITY;

//	PrintMsg(DEBUG_CONFIGURATION|DEBUG_INFORMATION_LEVEL, ("total valid configuration: %d", caps->_num_of_configurations));

	return ERROR_NONE;
}

SINT32	INVERSE32(SINT32 x)
{
	UINT8 *code = (UINT8*)&x, r[4];
	r[3] = code[0]; r[2] = code[1]; r[1] = code[2]; r[0] = code[3];
	return *((SINT32*)r);
}
 
unsigned long FormatGOStreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader)
{

#define MAX_GO_SEQ_HEADER_LEN 200
	static char go_seqheader[MAX_GO_SEQ_HEADER_LEN];
	unsigned long go_seqheader_length = 0;

	*((unsigned long*)(go_seqheader +  0)) = 0xF1010000;

	// char	interlace;		// 0 for progressive or 1 for interlace orignal input or 2 for interlace source and interlacing coding
	*((char *)(go_seqheader +  4)) = 1;// (pcfg->syscfg.sensor_pformat == UYVYI ? (pcfg->strcfg.interlace_coding == 1 ? 2 : 1) : 0);

	// char	mode;			// 'MultiMedia.H': EVideoFormat
	*((char *)(go_seqheader +  5)) = extension->_stream.compress_mode;

	// char	seq;			// see ESequenceMode
	*((char *)(go_seqheader +  6)) = extension->_stream.sequence; 

	// SINT32	cols;			// count of macroblocks in horizontal
	*((SINT32*)(go_seqheader +  7)) = INVERSE32(extension->_resolution.width >> 4); 

	// SINT32	rows;			// count of macroblocks in vertical
	*((SINT32*)(go_seqheader + 11)) = INVERSE32(extension->_resolution.height >> 4);

	// REAL64	fps;			// frame rate of current stream
	*((SINT32*)(go_seqheader + 15)) = INVERSE32(extension->_bitrate.target_bitrate);

	//	char	uvmode;			// see EVideoFormat (for uv motion vectors)
	*((char *)(go_seqheader + 19)) = extension->_stream.compress_mode - GO;

	// char	dqmode;			// see EVideoFormat (for DCTQ mismatch control)
								//   GO (0x40) indicates no iQ-mismatch-control and use JDCT
	*((char *)(go_seqheader + 20)) = GO;

	//	char	fpmode;			// see EFractionLevel
								//   by default we only use FLHALF
	*((char *)(go_seqheader + 21)) = 1;

	// char	acpred;			// If do AC prediction in intra macro-block
	*((char *)(go_seqheader + 22)) = 0;

	// char	userq;			// if use user customized quantization table(1: Yes, 0: No)
	*((char *)(go_seqheader + 23)) = 0;

	go_seqheader_length = 24;

	*ppSeqHeader = go_seqheader;
	return go_seqheader_length;
}

unsigned long FormatMPEG1StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader)

{

#define MAX_MPEG1_SEQ_HEADER_LEN 200
	static char mpeg1_seqheader[MAX_MPEG1_SEQ_HEADER_LEN];

	static unsigned long ratetab[8] = { 24000, 24024, 25025, 30000, 30030, 50050, 60000, 60060 };
	SINT32 j, frame_rate_code, aspect_ratio_code;
	unsigned long mpeg1_seqheader_length = 12;
	for ( j = 0 ; j < 8 ; j ++ )
		if(ratetab[j] == extension->_framerate.frame_rate ) break;
	frame_rate_code = ( j == 8 ) ? 4 : j + 1;
	aspect_ratio_code = extension->_stream.DVD_compliant ? extension->_stream.aspect_ratio : 2;

	mpeg1_seqheader[0] = 0;
	mpeg1_seqheader[1] = 0;
	mpeg1_seqheader[2] = 1;
	mpeg1_seqheader[3] = (char)0xB3;

	mpeg1_seqheader[4] = (char)( extension->_resolution.width >> 4);
	mpeg1_seqheader[5] = (char)( ( ( extension->_resolution.width << 4 ) & 0xF0 ) + ( ( extension->_resolution.height >> 8 ) & 0x0F ) );
	mpeg1_seqheader[6] = (char)( extension->_resolution.height & 0xFF);
	mpeg1_seqheader[7] = (char)( (aspect_ratio_code << 4 ) + frame_rate_code);
 
	mpeg1_seqheader[8] = (char)0xFF;
	mpeg1_seqheader[9] = (char)0xFF;
	mpeg1_seqheader[10] = (char)0xF2;
	mpeg1_seqheader[11] = (char)0xA8;

	*ppSeqHeader = mpeg1_seqheader;
	return mpeg1_seqheader_length;
}

unsigned long FormatMPEG2StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader)
{
	
	#define MAX_MPEG2_SEQ_HEADER_LEN 200
	static char mpeg2_seqheader[MAX_MPEG2_SEQ_HEADER_LEN];

	static unsigned long ratetab[8] = { 24000, 24024, 25025, 30000, 30030, 50050, 60000, 60060 };
	SINT32 j, frame_rate_code, aspect_ratio_code, interlace_coding;
	unsigned long mpeg2_seqheader_length = 22;
	for ( j = 0 ; j < 8 ; j ++ )
		if(ratetab[j] == extension->_framerate.frame_rate) break;
	frame_rate_code = ( j == 8 ) ? 4 : j + 1;

	aspect_ratio_code = extension->_stream.DVD_compliant ? extension->_stream.aspect_ratio : 2;
	interlace_coding = extension->_stream.deinterlace_mode == 2;

	mpeg2_seqheader[0] = 0;
	mpeg2_seqheader[1] = 0;
	mpeg2_seqheader[2] = 1;
	mpeg2_seqheader[3] = (char)0xB3;

	mpeg2_seqheader[4] = (char)( extension->_resolution.width >> 4);
	mpeg2_seqheader[5] = (char)( ( ( extension->_resolution.width << 4 ) & 0xF0 ) + ( ( extension->_resolution.height >> 8 ) & 0x0F ) );
	mpeg2_seqheader[6] = (char)( extension->_resolution.height & 0xFF);
	mpeg2_seqheader[7] = (char)( (aspect_ratio_code << 4 ) + frame_rate_code);

	mpeg2_seqheader[8] = (char)0x17;	// variable bit rate value (9.8MB/s) + 1 bit marker
	mpeg2_seqheader[9] = (char)0xED;
	mpeg2_seqheader[10] = (char)((1 << 5) + ((112 & 0x03E0) >> 5));
	mpeg2_seqheader[11] = (char)(((112 & 0x1F) << 3) +		// vbv_buffer_size_value 112
						  (0 << 2) +				// constrained_parameters_flag
						  (0 << 1) +				// load_intra_quantizer_matrix
						  0);						// load_inter_quantizer_matrix;

	mpeg2_seqheader[12] = (char)0x0;
	mpeg2_seqheader[13] = (char)0x0;
	mpeg2_seqheader[14] = (char)0x1;
	mpeg2_seqheader[15] = (char)0xB5;

	mpeg2_seqheader[16] = (char)0x14;
	mpeg2_seqheader[17] = (UINT8) (8 << 4) +	// level indication
					  (extension->_stream.DVD_compliant? 0:((UINT8) ((1 - interlace_coding) << 3))) +	// progressive/interlace sequence
					  (UINT8) (1 << 1);		// chroma_format 
	mpeg2_seqheader[18] = (char)0x0;
	mpeg2_seqheader[19] = (char)0x1;

	mpeg2_seqheader[20] = (char)0x0;
	mpeg2_seqheader[21] = (char)0x0;

	*ppSeqHeader = mpeg2_seqheader;
	return mpeg2_seqheader_length;
}

unsigned long FormatMPEG4StreamSEQHeader(_VIDEO_CAPABILITIES* caps,
								TCFG_FORMAT_EXTENSION* extension,
								char** ppSeqHeader)
{

	#define MAX_MPEG4_SEQ_HEADER_LEN 200
	static char mpeg4_seqheader[MAX_MPEG4_SEQ_HEADER_LEN];

	unsigned long mpeg4_seqheader_length = 18;

	mpeg4_seqheader[0] = (char)0x0;
	mpeg4_seqheader[1] = (char)0x0;
	mpeg4_seqheader[2] = (char)0x1;
	mpeg4_seqheader[3] = (char)0x0;

	mpeg4_seqheader[4] = (char)0x0;
	mpeg4_seqheader[5] = (char)0x0;
	mpeg4_seqheader[6] = (char)0x1;
	mpeg4_seqheader[7] = (char)0x20;

	mpeg4_seqheader[8] = (char)0x0;
	mpeg4_seqheader[9] = (char)0x84;
	mpeg4_seqheader[10] = (char)0x5D;
	mpeg4_seqheader[11] = (char)0x4C;

	mpeg4_seqheader[12] = (char)(0x28 + ( ( extension->_resolution.width >> 10 ) & 0x7 ));
	mpeg4_seqheader[13] = (char)(extension->_resolution.width >> 2);
	mpeg4_seqheader[14] = (char)(0x20 + ( ( extension->_resolution.height >> 8 ) & 0x1F ));
	mpeg4_seqheader[15] = (char)(extension->_resolution.height & 0xFF);

	mpeg4_seqheader[16] = (char)0xA3;
	mpeg4_seqheader[17] = (char)0x1F;

	*ppSeqHeader = mpeg4_seqheader;
	return mpeg4_seqheader_length;
}

/****************************** end of config_helper.c ***********************/

