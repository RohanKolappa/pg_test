#ifndef MEDIACONVERTER_FILESYSTEM_UTILS_H_
#define MEDIACONVERTER_FILESYSTEM_UTILS_H_ 

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include "libmediaconverter.hh"

#include <iostream>
#include <string>
using namespace std;


namespace mediaindex {

#define DBDIR "db"
#define IPVSDEFAULTDIR "IPVSc_default"
#define DBINDEXFILE "_index"
#define BLOBDBFILE "db.xml"

namespace bfs = boost::filesystem;

bool IsDirectory(const string& a_path);

bool DirectoryExists(const string& a_rstrDir);

string GetFileBaseName(const string& a_rstrAbsFileName);

bool CreateDbDirectories(const string& a_rstrParentDir);

bool GetMetaDataFileList(std::vector<string>& a_rstrFileList, 
        const string& a_rstrExportDir, const string& a_rstrBlobDir);

bool FillFileList(std::vector<string>& a_rstrFileList, const bfs::path& a_strDir);

};

#endif // MEDIACONVERTER_FILESYSTEM_UTILS_H_
