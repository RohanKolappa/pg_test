#include "stdafx.h"

#include <shellapi.h>
#include <shlobj.h>

#include "AxAuthor_Defines.h"
#include "FileSystemLib.h"

#include "IFileCreator.h"

int FileSystemLib::MakeDirIfNotExist(LPOLESTR dirName)
{
	
	BOOL rc = CreateDirectory(dirName, NULL);
	if(!rc)
	{
		DWORD lastErr = GetLastError();
		if(lastErr != ERROR_ALREADY_EXISTS)
			return DIRCREATE_FAILED;
	}

	return 0;	
}

int FileSystemLib::DeleteDir(LPOLESTR dirName)
{
	int rc = 0;
	SHFILEOPSTRUCT structFileOp;

	LPTSTR dirName2 = NULL;
	getDoubleNullTerminatedName(dirName, &dirName2);

	ZeroMemory(&structFileOp, sizeof(structFileOp));
	structFileOp.hwnd   = NULL;
	structFileOp.wFunc  = FO_DELETE;
	structFileOp.pFrom  = dirName2; // must be double null termin.
	structFileOp.fFlags = 0;
	structFileOp.fFlags = FOF_ALLOWUNDO | FOF_SILENT; // | FOF_NOCONFIRMATION; This is very dangerous flag- 

	rc = SHFileOperation(&structFileOp); //SHFileOperation fails on any path prefixed with "\\?\".

	delete [] dirName2;

	return (rc || structFileOp.fAnyOperationsAborted) ? DIRDELETE_FAILED : 0;	
}

int FileSystemLib::CreateProjectDir(LPOLESTR projDir)
{
	int rc = 0;

	rc = FileSystemLib::MakeDirIfNotExist(projDir);
	if (rc > 0)
		return rc;

	// Create blob and db dirs too
	size_t projDirNameLen = wcslen(projDir);

	size_t blobDirNameLen = projDirNameLen + 5; // \blob
	LPOLESTR blobDir = new OLECHAR[blobDirNameLen + 1]; 

	size_t dbDirNameLen = projDirNameLen + 3; // \db
	LPOLESTR dbDir   = new OLECHAR[dbDirNameLen + 1];	

	rc = _snwprintf(blobDir, blobDirNameLen, L"%s\\blob", projDir);
	if(rc > 0)
	{
		blobDir[blobDirNameLen] = L'\0';
		rc = FileSystemLib::MakeDirIfNotExist(blobDir);
		if(rc)
		{
			delete [] blobDir;
			return rc;
		}
	}

	rc = _snwprintf(dbDir, dbDirNameLen, L"%s\\db", projDir);
	if(rc > 0)
	{
		dbDir[dbDirNameLen] = L'\0';
		rc = FileSystemLib::MakeDirIfNotExist(dbDir);
		if(rc)
		{
			delete [] blobDir;
			delete [] dbDir;
			return rc;
		}
	}
	
	delete [] blobDir;
	delete [] dbDir;
	
	return 0;
}

int FileSystemLib::CopyStream(int mediaType, LPOLESTR srcBlobDir, LPOLESTR dstBlobDir, 
							  int startMin, int endMin, BOOL allMins)
{
	int rc = 0;
	SHFILEOPSTRUCT structFileOp;

	if( (mediaType == MEDIATYPE_MPEGTS) || (mediaType == MEDIATYPE_MPEG4) )
	{
		IFileCreator* fc = IFileCreator::CreateObj(mediaType,srcBlobDir, dstBlobDir, startMin, endMin, allMins);		
		if(fc == NULL)
		{
			wprintf(L" No FileCreator object created \n");
			rc = OUTOFMEMORY;
		}
		
		if(fc)
		{
			int ret = fc->Initialize();
			if(ret)
			{
				wprintf(L"Init failed\n");
				return MEDIACOPY_FAILED;
			}
			
			ret = fc->CreateFile();
			if(ret)
			{
				wprintf(L" CreateFile failed\n");
				return MEDIACOPY_FAILED;
			}

			delete fc;
		}
		
	}
	else if(mediaType == MEDIATYPE_V2D)
	{		
		LPTSTR srcBlobDir2 = NULL;
		LPTSTR dstBlobDir2 = NULL;
		
		getDoubleNullTerminatedName(srcBlobDir, &srcBlobDir2);
		getDoubleNullTerminatedName(dstBlobDir, &dstBlobDir2);

		if(allMins)
		{
			ZeroMemory(&structFileOp, sizeof(structFileOp));
			structFileOp.hwnd   = NULL;
			structFileOp.wFunc  = FO_COPY;
			structFileOp.pFrom  = srcBlobDir2; // must be double null termin.
			structFileOp.pTo    = dstBlobDir2;
			structFileOp.fFlags = 0;
			structFileOp.fFlags = FOF_ALLOWUNDO; 
			
			rc = SHFileOperation(&structFileOp); //SHFileOperation fails on any path prefixed with "\\?\".
						
			if (rc || structFileOp.fAnyOperationsAborted)
				rc = MEDIACOPY_FAILED;						
		}	

		delete [] srcBlobDir2;
		delete [] dstBlobDir2;
	}
	else
	{
		// Unknown media type!!!
		return MEDIATYPE_INVALID;
	}

	return rc;
}

int FileSystemLib::BrowseForFolder(LPOLESTR selectedFolder)
{
	OLECHAR relFolderName[MAX_PATH];	
	OLECHAR displayStr[] = L"Select a Directory";

	BROWSEINFO bInfo;
	bInfo.hwndOwner = NULL;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = relFolderName;
	bInfo.lpszTitle = displayStr;
	bInfo.ulFlags = 0;
	bInfo.ulFlags |= BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	bInfo.lpfn = NULL;

	LPITEMIDLIST  pidList = SHBrowseForFolder(&bInfo);
	
	BOOL rc = FALSE;
	if(pidList)
	{
		rc = SHGetPathFromIDList(pidList, selectedFolder);	
		CoTaskMemFree(pidList);
		pidList = NULL;
	}

	return (rc == TRUE) ? 0 : -1;
}

// Caller's responsibility to free memory
int getDoubleNullTerminatedName(LPOLESTR origName, LPTSTR *doubleNullTerminatedName)
{
	size_t nameLen = wcslen(origName);
	
	*doubleNullTerminatedName = new OLECHAR[nameLen + 2]; // 2 terminating NULL chars
	
	int rc = _snwprintf(*doubleNullTerminatedName, nameLen+1, L"%s", origName);
	if(rc < 0)
	{
		return STRMANIP_FAILED;
	}
	LPTSTR x = *doubleNullTerminatedName;
	x[nameLen+1] = L'\0';	

	return 0;
}