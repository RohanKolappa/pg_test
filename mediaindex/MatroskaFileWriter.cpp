#include "MatroskaFileWriter.hh"
#include "MatroskaFileWriterImpl.hh"

#include <iostream>

namespace mediaindex {

CMatroskaFileWriter::CMatroskaFileWriter(
    const string& a_rstrOutFile, 
    const string& a_rstrDbDir,
    const string& a_rstrBlobDir)
{
    m_pWriterImpl = new CMatroskaFileWriterImpl(
        a_rstrOutFile, 
        a_rstrDbDir, 
        a_rstrBlobDir);
}

CMatroskaFileWriter::~CMatroskaFileWriter(void)
{
    delete m_pWriterImpl;
}


int CMatroskaFileWriter::Write()
{
    return m_pWriterImpl->Write();
}

void CMatroskaFileWriter::Silent()
{
    m_pWriterImpl->Silent();
}

void CMatroskaFileWriter::FixedUUIDs()
{
    m_pWriterImpl->FixedUUIDs();
}

void CMatroskaFileWriter::SetEditList(CEditList * editList)
{
    m_pWriterImpl->SetEditList(editList);
}

};
