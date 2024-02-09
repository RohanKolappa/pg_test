// ProjectManager.cpp : Implementation of CProjectManager
#include "stdafx.h"
#include "IPVSAxAuthor.h"
#include "ProjectManager.h"

#include "FileSystemLib.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectManager

CProjectManager::CProjectManager()
: m_bstrProjectDir(L"")
{
}

CProjectManager::~CProjectManager()
{
}

STDMETHODIMP CProjectManager::get_ProjectDir(BSTR *pVal)
{
	// TODO: Add your implementation code here
	int rc = FileSystemLib::BrowseForFolder(*pVal);
	return S_OK;
}

STDMETHODIMP CProjectManager::put_ProjectDir(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_bstrProjectDir = newVal;

	int rc = FileSystemLib::CreateProjectDir(m_bstrProjectDir.m_str);
	if(!rc)
	{
		MessageBox(NULL, L"Created Dir.",  L"IPVSAxAuthor", MB_OK | MB_SETFOREGROUND);
	}
	else
	{
		MessageBox(NULL, L"Failed to create Dir.",  L"IPVSAxAuthor", MB_OK | MB_SETFOREGROUND);
	}

	return (!rc) ? S_OK : E_FAIL;
}



STDMETHODIMP CProjectManager::DeleteDir(BSTR dirName)
{
	// TODO: Add your implementation code here
	int rc = FileSystemLib::DeleteDir(dirName);

	return (!rc) ? S_OK : E_FAIL;
}

// Pushkar: Why is the rc undefined in JS???
STDMETHODIMP CProjectManager::CopyStream(INT mediaType, BSTR srcBlobDir, BSTR dstBlobDir, INT startMin, INT endMin, BOOL allMins)
{
	// TODO: Add your implementation code here
	int rc = FileSystemLib::CopyStream(
		mediaType, 
		srcBlobDir, dstBlobDir, 
		startMin, endMin, allMins);	
	return (!rc) ? S_OK : E_FAIL;
}

STDMETHODIMP CProjectManager::BrowseForFolder(BSTR *selectedFolder)
{
	// TODO: Add your implementation code here
	OLECHAR oleFolder[MAX_PATH];
	
	int rc = FileSystemLib::BrowseForFolder(oleFolder);
	if(!rc)
	{
		CComBSTR bstrFolder(oleFolder);
		*selectedFolder = bstrFolder.Copy();
	}
	return (!rc) ? S_OK : E_FAIL;
}
