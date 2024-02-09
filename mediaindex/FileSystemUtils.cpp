#include "FileSystemUtils.hh"

namespace mediaindex {

bool IsDirectory(const string& a_path)
{
    bfs::path aPath(a_path);
    if (bfs::is_directory(aPath)) {
        return true;
    }
    return false;
}

bool DirectoryExists(const string& a_rstrDir)
{
    return bfs::exists(a_rstrDir);
}

string GetFileBaseName(const string& a_rstrAbsFileName)
{
#if defined (__MINGW32__) || defined (__ARM__) 
    return bfs::path(a_rstrAbsFileName).leaf().string();
#else
    return bfs::path(a_rstrAbsFileName).leaf();
#endif
    return string("");
}

bool CreateDbDirectories(const string& a_rstrParentDir)
{
    const string strDbDir = a_rstrParentDir + "/" + DBDIR;
    const string strIpvsDir = strDbDir + "/" + IPVSDEFAULTDIR;

    bool result = false;
    try {
        const bfs::path dbPath(strDbDir);
        const bfs::path ipvsPath(strIpvsDir);

        if (bfs::exists(dbPath)) {
            bfs::remove_all(dbPath);
        }

        result = (bfs::create_directory(dbPath) && bfs::create_directory(
                ipvsPath));
    } catch (...) {
        return false;
    }
    return result;
}

bool GetMetaDataFileList(std::vector<string>& a_rstrFileList, 
        const string& a_rstrExportDir, const string& a_rstrBlobDir)
{
    a_rstrFileList.clear();

    string strDbDir     = a_rstrExportDir + "/" + DBDIR;
    string strIPVSDbDir = strDbDir + "/" + IPVSDEFAULTDIR;

    string strIndexFile = strDbDir + "/" + DBINDEXFILE;
    a_rstrFileList.push_back(strIndexFile);

    string strDbXmlFile = a_rstrExportDir + "/" + BLOBDBFILE;
    a_rstrFileList.push_back(strDbXmlFile);

    return FillFileList(a_rstrFileList, strIPVSDbDir.c_str());
}

bool FillFileList(std::vector<string>& a_rstrFileList, 
                  const bfs::path & a_rstrDbDir)
{
    bfs::path dirPath(a_rstrDbDir);
    if (!bfs::exists(dirPath)) {
        return false;
    }

    bfs::directory_iterator end;
    for (bfs::directory_iterator iter(dirPath); iter != end; ++iter) {
#if defined(__MINGW32__) || defined(__ARM__)
        if (iter->path().leaf().string().compare(0, 1, ".") == 0) {
#else
        if (iter->path().leaf().compare(0, 1, ".") == 0) {
#endif
            continue; // ignore paths that start with .
        }

        if (bfs::is_directory(iter->status())) {
            FillFileList(a_rstrFileList, *iter);
        }
        else {
            if (!bfs::is_regular(iter->status()))
                continue;

#if defined(__MINGW32__) || defined(__ARM__)
            a_rstrFileList.push_back(iter->path().string());
#else
            a_rstrFileList.push_back(iter->path().native_file_string());
#endif
        }
    }

    return true;
}

};
