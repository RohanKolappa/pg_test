/*
 * HTTPUtils.hh
 *
 *  Created on: May 9, 2011
 *      Author: rkale
 */

#ifndef URLFETCHER_HH_
#define URLFETCHER_HH_

#include <string>
#include "mutex.hh"


class CURLFetcher {
public:
    CURLFetcher(const char *url);
    ~CURLFetcher();

    bool FetchToFile(const char *filePath);
    bool FetchToBuffer(uint8_t * & a_buffer, int & a_size);


private:
    std::string m_url;
    uint8_t *m_pBuffer;
    int m_nBufferSize;
    int m_nBufferOffset;

    size_t WriteToMemoryHandler(void *ptr, size_t size);

    static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);

};

/**********************************************************/

class CFileUploader {
public:
    CFileUploader(const char *url);
    ~CFileUploader();

    bool UploadFile(const char *filePath, const char *fileUploadName);

    std::string GetErrorMsg();
private:
    std::string m_url;
    std::string m_strError;
};

#endif /* URLFETCHER_HH_ */
