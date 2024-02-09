#ifndef MINUTE_FILE_WRITER_H
#define MINUTE_FILE_WRITER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using std::string;

#include <list>
using std::list;

#include "libmediaconverter.hh"


namespace mediaindex {

class CEditList;
class CMediaProcessorInterface;

class CMinuteFileWriter
{
public:

    CMinuteFileWriter(const string& a_rstrInFile,
        const string& a_rstrDbDir,
        const string& a_rstrBlobDir);

    ~CMinuteFileWriter();

    int Write();
    void Silent();
    void FixedUUIDs();
    void SetEditList(CEditList * editList);
    void AddMediaProcessor(CMediaProcessorInterface *a_pMediaProcessor);

private:
    struct Impl;
    Impl* m_pimpl;

    CMinuteFileWriter(const CMinuteFileWriter &a_rReader);
    CMinuteFileWriter &operator=(const CMinuteFileWriter &a_rReader);

    void Initialize(const string& a_rstrInFile,
                    const string& a_rstrDbDir,
                    const string& a_rstrBlobDir);
};

};

#endif //MINUTE_FILE_WRITER_H















