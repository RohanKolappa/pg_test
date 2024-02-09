//Defined the class CMatroskaFileWriter which uses the arguments entered .
#ifndef MATROSKA_FILE_WRITER_H
#define MATROSKA_FILE_WRITER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "libmediaconverter.hh"


namespace mediaindex {

class CEditList;

class CMatroskaFileWriter {
public:
    CMatroskaFileWriter(const string& a_rstrOutFile, const string& a_rstrDbDir,
            const string& a_rstrBlobDir);
    ~CMatroskaFileWriter();

    int Write();
    void Silent();
    void FixedUUIDs();

    void SetEditList(CEditList * editList);

private:
    CMatroskaFileWriter(const CMatroskaFileWriter &a_rWriter);
    CMatroskaFileWriter &operator=(const CMatroskaFileWriter &a_rWriter);

    CMatroskaFileWriterImpl* m_pWriterImpl;
};

};

#endif //MATROSKA_FILE_WRITER_H

