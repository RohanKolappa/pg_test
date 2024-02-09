#ifndef __FILESYSTEMLIB__
#define __FILESYSTEMLIB__

namespace FileSystemLib {
	
	int CreateProjectDir(LPOLESTR projDir);

	int CopyStream(int mediaType, LPOLESTR srcBlobDir, LPOLESTR dstBlobDir, 
		int startMin, int endMin, BOOL allMins);

	int MakeDirIfNotExist(LPOLESTR dirName);	
	int DeleteDir(LPOLESTR dirName);

	int BrowseForFolder(LPOLESTR selectedFolder);
	
} // namespace FileSystemLib

int getDoubleNullTerminatedName(LPOLESTR origName, LPTSTR *doubleNullTerminatedName);

#endif // __FILESYSTEMLIB__