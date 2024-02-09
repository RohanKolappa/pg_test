// IFileCreator.h: interface for the IFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_)
#define AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (defined(__WIN32__) || defined(_WIN32))
#include <windows.h>
#include <wtypes.h>
#endif
#include <iostream>
using namespace std;

#include "CommonIncludes.hh"

typedef struct TrParams
{
	uint16_t    nTargetFps ;
	uint64_t    nStartOffset ;
	uint64_t    nDurationTime ;
    std::string      strAVMode;
}StTranscodeOpts;

class IFileCreator  
{
public:
	IFileCreator();
	virtual ~IFileCreator();
	
	static IFileCreator* CreateObj(std::string& a_rStrBlobDir,
                                   std::string& a_rStrOutFile,
                                   StTranscodeOpts& a_rStArgList);
		 
	virtual int Initialize() = 0;
	virtual int CreateFile() = 0;
	virtual int SetTargetFps(const int a_nTargetFps) = 0;
	
};

#if (defined(__WIN32__) || defined(_WIN32))
char *getMultiByteStr(LPOLESTR wStr);
#endif

#endif // !defined(AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_)
