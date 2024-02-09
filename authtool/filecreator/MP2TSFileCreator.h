// MP2TSFileCreator.h: interface for the CMP2TSFileCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP2TSFILECREATOR_H__681C7E51_9DC9_4858_A17E_D1EC57B0281A__INCLUDED_)
#define AFX_MP2TSFILECREATOR_H__681C7E51_9DC9_4858_A17E_D1EC57B0281A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IFileCreator.h"

class CMP2TSFileCreator : public IFileCreator  
{
public:
	CMP2TSFileCreator(LPOLESTR blobDir, LPOLESTR tsOutFile, const int startMin, const int endMin, bool allMins);
	virtual ~CMP2TSFileCreator();
	
	virtual int Initialize();
	virtual int CreateFile();
	
	
private:
	int m_nFile;
	int m_nStartMin;
	int m_nEndMin;
	int m_totalWriteBytes;
	LPOLESTR m_blobDir;
	LPOLESTR m_tsOutFile;
	
	
};

#endif // !defined(AFX_MP2TSFILECREATOR_H__681C7E51_9DC9_4858_A17E_D1EC57B0281A__INCLUDED_)
