// MP4FileCreator.h: interface for the CMP4FileCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP4FILECREATOR_H__B9145303_0DC0_48B0_A16C_B01C80D6CF2B__INCLUDED_)
#define AFX_MP4FILECREATOR_H__B9145303_0DC0_48B0_A16C_B01C80D6CF2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFileCreator.h"


#include "typeapi.h"
#include "mode.hpp"
#include "bitstrm.hpp"
#include "vopses.hpp"
#include "vopsedec.hpp"
#include "codehead.h"

#include "mp4.h"


class CMP4FileCreator : public IFileCreator  
{
public:
	CMP4FileCreator(LPOLESTR blobDir, LPOLESTR mp4OutFile, const int startMin, const int endMin, bool allMins);
	virtual ~CMP4FileCreator();
	
	virtual int Initialize();
	virtual int CreateFile();


private:
	int m_nStartMin;
	int m_nEndMin;
	int m_totalWriteBytes;
	LPOLESTR m_blobDir;
	LPOLESTR m_mp4OutFile;
	char *configStr;
	char *aconfigStr;

    u_int32_t vtimeScale;
	u_int32_t atimeScale;
    MP4Duration mp4Duration;
	MP4Duration aDuration;
	u_int16_t vHeight;
	u_int16_t vWidth;
	int mp4ProfileLevel;



	u_int32_t verbosity;
	int createFlags;

	MP4FileHandle m_mp4File;
	MP4TrackId m_videoTrackId;
	MP4TrackId m_audioTrackId;
	CVideoObjectDecoder *m_pvodec;
	CInBitStream *m_pbitstrmIn;
	CInBitStream *m_paudiobitstrmIn;

	
	int getConfigStrInfo(/*char *cfgStr*/);
	int getConfigStr();
    uint8_t tohex (char a);
	int findStartCode( int dont_loop );
	int getAudioConfigStr(char *audioConfig);

};

#endif // !defined(AFX_MP4FILECREATOR_H__B9145303_0DC0_48B0_A16C_B01C80D6CF2B__INCLUDED_)
