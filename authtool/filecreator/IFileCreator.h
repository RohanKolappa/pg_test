// IFileCreator.h: interface for the IFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_)
#define AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <wtypes.h>

class IFileCreator  
{
public:
	IFileCreator();
	virtual ~IFileCreator();
	
	static IFileCreator* CreateObj(
		int mediaType,
		LPOLESTR srcBlobDir, 
		LPOLESTR outFile, 
		const int startMin, 
		const int endMin, 
		const bool allMins);
	
	virtual int Initialize() = 0;
	virtual int CreateFile() = 0;
	
};

char *getMultiByteStr(LPOLESTR wStr);

#endif // !defined(AFX_IFILECREATOR_H__8BD3F610_81B8_4F44_A420_F44F3ACB5DFE__INCLUDED_)
