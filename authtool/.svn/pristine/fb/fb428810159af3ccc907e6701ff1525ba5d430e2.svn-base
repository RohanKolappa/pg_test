// MP4FileCreator.cpp: implementation of the CMP4FileCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "stdio.h"
#include <fstream>
#include "string.h"
#include <fcntl.h>
#include <io.h>

#include "MP4FileCreator.h"
#include "CommonIncludes.hh"
#include "MediaServer.hh"
#include "MediaHeader.hh"
#include "strcasestr.h"

#include <string>
using std::string;

//Uncomment this when using filecreatortester
//#define FILECREATORTESTER

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMP4FileCreator::CMP4FileCreator(LPOLESTR blobDir, 
									 LPOLESTR mp4OutFile, 
									 const int startMin, 
									 const int endMin, 
									 bool allMins)
{
	m_totalWriteBytes = 0;
	
	m_blobDir = blobDir;
	m_mp4OutFile = mp4OutFile;

	printf("\nIn CMP4FileCreator::constructor()\nm_blobDir -----> %s\nm_mp4OutFile -----> %s\n",m_blobDir,m_mp4OutFile);
	
	m_nStartMin = 0;
	m_nEndMin = 0;
	
	if(!allMins)
	{
		m_nStartMin = startMin;
		m_nEndMin = endMin;
		printf("allMins false start %d end %d\n",m_nStartMin,m_nEndMin); 
	}
	else
		printf("allMins set to true\n");
	
	verbosity = MP4_DETAILS_ERROR;
    createFlags = 0;

	m_mp4File = NULL;
	m_videoTrackId = MP4_INVALID_TRACK_ID;
	m_audioTrackId = MP4_INVALID_TRACK_ID;

	m_pvodec = new CVideoObjectDecoder();
	m_pbitstrmIn = new CInBitStream();
	m_paudiobitstrmIn = new CInBitStream();

	vtimeScale = 0;
	atimeScale = 0;
    mp4Duration = 0;
	aDuration = MP4_INVALID_DURATION;
	vHeight = 0;
	vWidth = 0;
	mp4ProfileLevel = 0;

	configStr = NULL;
	configStr = (char *)malloc(1024);

	aconfigStr = NULL;
	aconfigStr = (char *)malloc(1024);
	
}

CMP4FileCreator::~CMP4FileCreator()
{

 printf("\nIn CMP4FileCreator::destructor()\n");
 free(configStr);
 free(aconfigStr);

#ifndef FILECREATORTESTER

 if(m_paudiobitstrmIn != NULL)
 free(m_paudiobitstrmIn);

 if(m_pvodec != NULL)
 free(m_pvodec);

 if(m_pbitstrmIn != NULL)
 free(m_pbitstrmIn);

#endif

}

int CMP4FileCreator::Initialize()
{
    printf("\nIn CMP4FileCreator::Initialize()\n");

#ifdef FILECREATORTESTER
	string tempStr = (char *)(m_mp4OutFile);
#else
	string tempStr = getMultiByteStr(m_mp4OutFile);
#endif

	if(m_nStartMin > m_nEndMin)
	{
		printf("Invalid minute parameters, m_nStartMin %d, m_nEndMin %d\n",m_nStartMin,m_nEndMin);
		return 1;
	}

	//In some cases the \ is not recognised properly, so manually inserting 
	int pos = 0;
	do{

	 pos = tempStr.find("\\",pos);
	 //printf("pos %d\n",pos);
	 if(pos == string::npos)
		 break;
	 tempStr.replace(pos,1,"\\");
	 pos++;
	}while(pos != string::npos);
	
	m_mp4File = MP4Create((const char*)tempStr.c_str(),verbosity,(createFlags != 0));
	if(!m_mp4File)
		return 1;

    
	if(getConfigStrInfo())
		return 1;

	return 0;
	
}

int CMP4FileCreator::getConfigStr()
{
	printf("\nIn CMP4FileCreator::getConfigStr()\n");

#ifdef FILECREATORTESTER
	string blob_str = (char *)m_blobDir;
#else
	string blob_str = getMultiByteStr(m_blobDir);
#endif
		
	//strip the last part out
	int i = blob_str.find_last_of("\\",blob_str.length());
	//printf("length %d, last pos \\ found %d\n",blob_str.length(),i);
	
	string db_file = blob_str.substr(i,blob_str.length() - i);
	//printf("db_file %s\n",db_file.c_str());

	string db_string = blob_str.substr(0,i - strlen("\\blob"));
	//printf("db_string %s\n",db_string.c_str());

    db_string += "\\db";	
	db_string += db_file;

	printf("DB file Located at -----> %s\n",db_string.c_str());


   std::ifstream inSdp(db_string.c_str());
   string lineSdp;
   string::size_type pos;


    while(std::getline(inSdp,lineSdp))
	{
	  pos = lineSdp.find("config=",0);
      if(pos != string::npos)
	  {
		  printf("lineSdp %s\nSIZE of lineSdp %d\n",lineSdp.c_str(),lineSdp.length());
		  pos = lineSdp.find("config=",0);
	      string::size_type pos2 = lineSdp.find(";",pos);
		  string cString = lineSdp.substr(pos,pos2-pos).c_str();
		  strcpy(configStr,(char*)cString.c_str());
	  }

	  pos = lineSdp.find("a=rtpmap:97",0);
	  if(pos != string::npos)
	  {
		  string::size_type pos2 = lineSdp.find_first_of("/",pos+strlen("a=rtpmap:97")+1);
		  string::size_type pos3 = lineSdp.find_first_of("\t",pos+strlen("a=rtpmap:97")+1);
		  string audioTimescale = lineSdp.substr(pos2+1,pos3 - pos2);
		  atimeScale = atoi(audioTimescale.c_str());
		  printf("Audio timescale = %d\n",atimeScale);
		  string::size_type pos4 = lineSdp.find("config=",pos3);
		  string::size_type pos5 = lineSdp.find_first_of(";",pos4);
		  string audioConfig = 	lineSdp.substr(pos4 + strlen("config="),pos5 - pos4 - strlen("config="));	  
		  audioConfig = "config=" + audioConfig;
		  printf("Audio config = %s\n",audioConfig.c_str());
		  getAudioConfigStr((char*)audioConfig.c_str());
	  }
	  
	}

	return 0;
}

int CMP4FileCreator::getAudioConfigStr(char *audioConfig)
{

	uint8_t* buffer;
   int ascii = 1;
   uint32_t len = strlen(audioConfig);
   unsigned char *bufptr;
 
   buffer = (uint8_t *)malloc(256);

   printf("Audio configStr %s\n",audioConfig);
   
   for(int j = 0; j<256;j++)
  	buffer[j] = 0x00;


  if (ascii == 1) {
    const char *config = strcasestr(audioConfig, "config=");
    if (config == NULL) {
		printf(" Error:: config == NULL\n");
      return 1;
    }
    config += strlen("config=");
    const char *end;
    end = config;
    while (isxdigit(*end)) end++;
    if (config == end) {
		printf(" Error:: config == end\n");
      return 1;
    }
    // config string will run from config to end
    len = end - config;

    // make sure we have even number of digits to convert to binary
    if ((len % 2) == 1) 
	{
      printf(" Error:: (len % 2) == 1\n");
	  return 1;
	}
    uint8_t *write;
    write = buffer;
    // Convert the config= from ascii to binary
    for (uint32_t ix = 0; ix < len/2; ix++) {
      *write = 0;
      *write = (tohex(*config)) << 4;
      config++;
      *write += tohex(*config);
      config++;
      write++;
    }
    len /= 2;
    bufptr = (unsigned char *)&buffer[0];
  } else {
    bufptr = (unsigned char *)audioConfig;
  }

  m_paudiobitstrmIn->set_buffer(bufptr,len);

  return 0;


}

int CMP4FileCreator::getConfigStrInfo(/*char *cfgStr*/)
{
   printf("\nIn CMP4FileCreator::getConfigStrInfo()\n");
   
   if(getConfigStr())
   {
	   printf("getConfigStr failed\n");
	   return 1;
   }

   uint8_t* buffer;
   int ascii = 1;
   uint32_t len = strlen(configStr);
   unsigned char *bufptr;
 
   buffer = (uint8_t *)malloc(256);

   printf("configStr %s\n",configStr);
  
   for(int j = 0; j<256;j++)
  	buffer[j] = 0x00;


  if (ascii == 1) {
    const char *config = strcasestr(configStr, "config=");
    if (config == NULL) {
		printf(" Error:: config == NULL\n");
      return 1;
    }
    config += strlen("config=");
    const char *end;
    end = config;
    while (isxdigit(*end)) end++;
    if (config == end) {
		printf(" Error:: config == end\n");
      return 1;
    }
    // config string will run from config to end
    len = end - config;

    // make sure we have even number of digits to convert to binary
    if ((len % 2) == 1) 
	{
      printf(" Error:: (len % 2) == 1\n");
	  return 1;
	}
    uint8_t *write;
    write = buffer;
    // Convert the config= from ascii to binary
    for (uint32_t ix = 0; ix < len/2; ix++) {
      *write = 0;
      *write = (tohex(*config)) << 4;
      config++;
      *write += tohex(*config);
      config++;
      write++;
    }
    len /= 2;
    bufptr = (unsigned char *)&buffer[0];
  } else {
    bufptr = (unsigned char *)configStr;
  }

  m_pbitstrmIn->set_buffer(bufptr,len);

  printf("***** Printing Decoded VOL Body *****\n");
  
  int video_object_sequence_start_code = m_pbitstrmIn->getBits(32);
  printf("video_object_sequence_start_code 0x%x\n",video_object_sequence_start_code);

  int profile_and_level_indication = m_pbitstrmIn->getBits(8);
  mp4ProfileLevel = profile_and_level_indication;
  printf("profile_and_level_indication 0x%x\n",profile_and_level_indication);

 

  if(findStartCode(0) == EOF)
  {
	  printf("Find Start Code failed\n");
	  return 1;
  }

  

  int video_object_layer_start_code = m_pbitstrmIn->getBits(32);
  printf("video_object_layer_start_code 0x%x\n",video_object_layer_start_code);

  int random_accessible_vol = m_pbitstrmIn->getBits(1);
  printf("random_accessible_vol 0x%x\n",random_accessible_vol);

  int video_object_type_indication = m_pbitstrmIn->getBits(8);
  printf("video_object_type_indication 0x%x\n",video_object_type_indication);

  int is_object_layer_identifier = m_pbitstrmIn->getBits(1);
  printf("is_object_layer_identifier 0x%x\n",is_object_layer_identifier);
  if(is_object_layer_identifier & 0x1)
	m_pbitstrmIn->getBits(7);

  int aspect_ratio_info = m_pbitstrmIn->getBits(4);
  printf("aspect_ratio_info 0x%x\n",aspect_ratio_info);
  if((aspect_ratio_info & 0xF) == 0xF)//extended_PAR
	  m_pbitstrmIn->getBits(16);

  int vol_control_parameters = m_pbitstrmIn->getBits(1);
  printf("vol_control_parameters 0x%x\n",vol_control_parameters);
  if(vol_control_parameters & 0x1)
  {
	  int chroma_format = m_pbitstrmIn->getBits(2);
	  printf("chroma_format 0x%x\n",chroma_format);
	  int low_delay = m_pbitstrmIn->getBits(1);
	  printf("low_delay 0x%x\n",low_delay);
	  int vbv_parameters = m_pbitstrmIn->getBits(1);
	  printf("vbv_parameters 0x%x\n",vbv_parameters);
	  if(vbv_parameters)
	  {
		m_pbitstrmIn->getBits(79);
	  }
  }


  int video_object_layer_shape = m_pbitstrmIn->getBits(2);
  printf("video_object_layer_shape 0x%x\n",video_object_layer_shape);

  int marker_bit = m_pbitstrmIn->getBits(1);
  printf("marker_bit 0x%x\n",marker_bit);

  int vop_time_increment_resolution = m_pbitstrmIn->getBits(16);
  vtimeScale = vop_time_increment_resolution;
  printf("vop_time_increment_resolution 0x%x %d\n",vop_time_increment_resolution,vop_time_increment_resolution);

  marker_bit = m_pbitstrmIn->getBits(1);
  printf("marker_bit 0x%x\n",marker_bit);

  int fixed_vop_rate = m_pbitstrmIn->getBits(1);
  printf("fixed_vop_rate 0x%x\n",fixed_vop_rate);

  int no_of_bits = 0;
  while(vop_time_increment_resolution)
  {
	no_of_bits++;
    vop_time_increment_resolution = vop_time_increment_resolution >> 1;
  }

  printf("no of bits %d\n",no_of_bits);
  
  if(fixed_vop_rate & 0x1)
  {
	 int fixed_vop_time_increment = m_pbitstrmIn->getBits(no_of_bits);
	 mp4Duration = fixed_vop_time_increment;
	 printf("fixed_vop_time_increment 0x%x %d\n",fixed_vop_time_increment,fixed_vop_time_increment);
  }

  if(video_object_layer_shape != 2)//"binary only"
  {
	  if(video_object_layer_shape == 0)//"rectangular"
	  {
		marker_bit = m_pbitstrmIn->getBits(1);
	    printf("marker_bit 0x%x\n",marker_bit);

	    int video_object_layer_width = m_pbitstrmIn->getBits(13);
	    printf("video_object_layer_width 0x%x %d\n",video_object_layer_width,video_object_layer_width);

	    marker_bit = m_pbitstrmIn->getBits(1);
	    printf("marker_bit 0x%x\n",marker_bit);

	    int video_object_layer_height = m_pbitstrmIn->getBits(13);
	    printf("video_object_layer_height 0x%x %d\n",video_object_layer_height,video_object_layer_height);

	    marker_bit = m_pbitstrmIn->getBits(1);
	    printf("marker_bit 0x%x\n",marker_bit);

		vHeight = video_object_layer_height;
	    vWidth = video_object_layer_width;

	  }
  }

  printf("***** End of Decoded VOL Body *****\n\n");

  	return 0;
}

int CMP4FileCreator::findStartCode( int dont_loop )
{
	printf("\nIn CMP4FileCreator::findStartCode()\n");
	// ensure byte alignment
	m_pbitstrmIn->flush ();

	unsigned int min_video_object_layer_start_code = 0x00000120;
	unsigned int max_video_object_layer_start_code = 0x0000012f;

	while(1)
	{
		if(
			(m_pbitstrmIn->peekBits(NUMBITS_START_CODE_PREFIX + 8) >= min_video_object_layer_start_code) &&
			(m_pbitstrmIn->peekBits(NUMBITS_START_CODE_PREFIX + 8) <= max_video_object_layer_start_code)
		  )
		  break;

		int temp = m_pbitstrmIn->getBits(8);
		if(m_pbitstrmIn->eof()==EOF)
				return EOF;
	}

	return 0;
}

uint8_t CMP4FileCreator::tohex (char a)
{ 
  if (isdigit(a))
    return (a - '0');
  return (tolower(a) - 'a' + 10);
}

int CMP4FileCreator::CreateFile()
{
	printf("\nIn CMP4FileCreator::CreateFile()\n");
	printf("vtimeScale %d, atimeScale %d\n",vtimeScale,atimeScale);
	printf("mp4Duration %d, aDuration %d\n",mp4Duration,aDuration);
	printf("vWidth %d, vHeight %d\n",vWidth,vHeight);


	int frames = 0,vframes = 0,aframes = 0;
	int vframeslost = 0,aframeslost = 0;
	bool isAudioTrackPresent = false;
	bool isVideoTrackPresent = false;
	CCFrame cframe;		

	bool rc = true;
	bool is_fixed_vop_rate = false;
	if(mp4Duration)
		is_fixed_vop_rate = true;

#ifdef FILECREATORTESTER
	char *mbStrBlobDir = (char*)m_blobDir;
#else
	char *mbStrBlobDir = getMultiByteStr(m_blobDir);
#endif
	
	CMediaServer *ms = new CMediaServer(mbStrBlobDir);
	if (ms == NULL) 
	{
        printf("Could not create MediaServer object!!\n");
		MP4Close(m_mp4File);
        return 1;
	}
	
	if (ms->Init() != MSRC_OK) 
	{
        printf("MediaServer Init Failed: %s\n",
			ms->GetErrorString());
		MP4Close(m_mp4File);
        return 1;
	}
	
	if (ms->Start(m_nStartMin) != MSRC_OK) 
	{
        printf("MediaServer Start Failed: %s\n",
			ms->GetErrorString());
		MP4Close(m_mp4File);
        return 1;
	}
	
	uint64_t vcurrTimestamp = 0;
	uint64_t vprevTimestamp = 0;
	uint64_t acurrTimestamp = 0;
	uint64_t aprevTimestamp = 0;

    uint64_t tfirstTimestamp = 0;
	uint64_t difftstamp = 0;

	uint64_t fileSize = 0;

	uint32_t sampleLength = 0;
	uint64_t sampleTimeStamp = 0;
	uint32_t sampleMediaFlag = 0;

	while(1) 
	{
		if (ms->GetFrame(cframe) != MSRC_OK) 
		{
			printf("MediaServer Get Frame Failed: %s\n",
				ms->GetErrorString());
			break;
		}

		sampleLength = cframe.GetLength();
		sampleTimeStamp = cframe.GetTimeStamp();
		sampleMediaFlag = cframe.GetMediaFlag();

		//This is the current limitation in MP4 library.
		if((fileSize + sampleLength) >= 0xFFFFFFFF)
		{
			printf("FileSize %I64u Greater than 0x%X , breaking out..\n",fileSize,0xFFFFFFFF);
			break;
		}

		fileSize += sampleLength;
			
		if(tfirstTimestamp == 0)
				tfirstTimestamp = sampleTimeStamp;

		if((sampleTimeStamp - tfirstTimestamp) < (m_nStartMin * 60000))
			continue;
				
		if(m_nEndMin)
		{
			difftstamp = sampleTimeStamp - tfirstTimestamp;
			//if(difftstamp >= totalClipDuration)
			if(difftstamp >= (m_nEndMin * 60000))
				break;
			//printf("currentTimestamp %I64u tfirstTimestamp %I64u difftstamp %I64u\n",sampleTimeStamp,tfirstTimestamp,difftstamp);
		}
				
		
		if ((sampleMediaFlag & FRAMETYPE_VIDEO) == FRAMETYPE_VIDEO)
		{
			vframes++;
			if(!isVideoTrackPresent)
			{
				m_videoTrackId = MP4AddVideoTrack(m_mp4File,vtimeScale,mp4Duration,vWidth,vHeight,MP4_MPEG4_VIDEO_TYPE);
				if (m_videoTrackId == MP4_INVALID_TRACK_ID)
					break;
				isVideoTrackPresent = true;
				MP4SetVideoProfileLevel(m_mp4File,mp4ProfileLevel);
			}
			else
			{
				//vcurrTimestamp = cframe.GetTimeStamp() - vprevTimestamp;
				vcurrTimestamp = sampleTimeStamp - vprevTimestamp;
			}

			//ie. if fixed_vop_rate is zero we need to find the 
			//difference in time between the current and prev frame 
			//and divide by the timescale
			if(!is_fixed_vop_rate)
			{
				//vprevTimestamp = cframe.GetTimeStamp();
				vprevTimestamp = sampleTimeStamp;
				mp4Duration = vcurrTimestamp;
				mp4Duration = mp4Duration/vtimeScale;
			}

					
			//rc = MP4WriteSample(m_mp4File,m_videoTrackId,cframe.GetData(),cframe.GetLength(),mp4Duration);
			rc = MP4WriteSample(m_mp4File,m_videoTrackId,cframe.GetData(),sampleLength,mp4Duration);
			if(!rc)
				vframeslost++;
		} 

		if ((sampleMediaFlag & FRAMETYPE_AUDIO) == FRAMETYPE_AUDIO)
		{
			aframes++;
			
			if(!isAudioTrackPresent)
			{
				m_audioTrackId = MP4AddAudioTrack(m_mp4File,atimeScale,aDuration,MP4_MPEG4_AUDIO_TYPE);
				if (m_audioTrackId == MP4_INVALID_TRACK_ID)
					break;
				isAudioTrackPresent = true;
			
				uint8_t aCfg[2];
				aCfg[0] = m_paudiobitstrmIn->getBits(8);
				aCfg[1]= m_paudiobitstrmIn->getBits(8);
				//printf("aCfg[0] 0x%x, aCfg[1] 0x%x\n",aCfg[0],aCfg[1]);
				//printf("aCfg[0] %d, aCfg[1] %d\n",aCfg[0],aCfg[1]);
				MP4SetTrackESConfiguration(m_mp4File,m_audioTrackId,aCfg,2);
			}
			else
			{
				//acurrTimestamp = cframe.GetTimeStamp() - aprevTimestamp;	
				acurrTimestamp = sampleTimeStamp - aprevTimestamp;	
			}

			//aprevTimestamp = cframe.GetTimeStamp();
			aprevTimestamp = sampleTimeStamp;

			aDuration = acurrTimestamp * atimeScale/1000;
									
			//rc = MP4WriteSample(m_mp4File,m_audioTrackId,cframe.GetData(),cframe.GetLength(),aDuration);
			rc = MP4WriteSample(m_mp4File,m_audioTrackId,cframe.GetData(),sampleLength,aDuration);
			if(!rc)
				aframeslost++;
			
		}
		
		frames++;
    }

	
    printf("tfirstTimestamp %I64u currentTimestamp %I64u difftstamp %I64u\n",tfirstTimestamp,sampleTimeStamp,difftstamp);
	printf("End of CreateFile()\nFound vframes %d, vframeslost %d aframes %d, aframeslost %d, total frames %d, totalWriteBytes %d\n",
		vframes,vframeslost,aframes,aframeslost,frames,m_totalWriteBytes);
	
    ms->Stop();

    
  	MP4Close(m_mp4File);

	printf("\nRunning MP4FileInfo on %s\n",(const char*)m_mp4OutFile);

	char* info = MP4FileInfo((const char*)m_mp4OutFile);
	if (!info) 
	     printf("Can't open %s\n",m_mp4File);
    else
		 fputs(info, stdout);

	delete ms;
	
	return 0;
	
}
