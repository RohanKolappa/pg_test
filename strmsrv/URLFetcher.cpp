/*
 * HTTPUtils.cpp
 *
 *  Created on: May 9, 2011
 *      Author: rkale
 */

#include "strmsrv_compatibility.h"

#ifdef __XPI__
#include "curl/curl.h"
#else
#include <curl/curl.h>
#endif
//#include <openssl/ssl.h>
//#include <openssl/crypto.h>

#include "URLFetcher.hh"


#if 0
// All this only useful for https connections (TBD)
static void pthreads_locking_callback(int mode, int type, const char *file,
         int line)
{
    printf("thread=%4d mode=%s lock=%s %s:%d\n",
            (int) CRYPTO_thread_id(),
            (mode & CRYPTO_LOCK) ? "l":"u",
            (type & CRYPTO_READ) ? "r":"w", file, line);
}

static unsigned long pthreads_thread_id(void) {
    unsigned long ret;
    printf("Thread id requested\n");
    ret = (unsigned long) pthread_self();
    return(ret);
}

#endif

// Define a static initializer class that can initialize libcurl
// during program load time (since it not supposed to be thread safe)
class CCurlInitializer
{
public:
    CCurlInitializer() {
//        SetupOpenSSLLocks();
        curl_global_init(CURL_GLOBAL_ALL);
    }

    ~CCurlInitializer() {
        curl_global_cleanup();
    }


#if 0
    void SetupOpenSSLLocks() {
        int numberOfLocksNeeded = CRYPTO_num_locks();

        printf("Number of needed locks: %d\n", numberOfLocksNeeded);
        SSL_library_init();
        CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
        CRYPTO_set_locking_callback(pthreads_locking_callback);
    }
#endif

    // Current version of libcurl (from Fedora 8 ) seems to have thread safety issues.
    // Stop gap solution: Using a sub-optimal global lock and serializing
    // all calls into libcurl (yikes!!)
    void Lock() {
        m_mutex.Lock();
    }

    void UnLock() {
        m_mutex.Unlock();
    }
private:
    CMutex m_mutex;

};

// Create an object of the above class as a static object
static CCurlInitializer curlIntializer;


// CURLFetcher class implementation
CURLFetcher::CURLFetcher(const char *url) {

    m_url = url;

    m_pBuffer = NULL;
    m_nBufferSize = 0;
    m_nBufferOffset = 0;

    // If url looks like an absolute file system path
    // prepend a "file://" so that curl can handle it
    if (m_url.compare(0, 1, "/") == 0) {
        m_url = "file://";
        m_url.append(url);
    }

}

CURLFetcher::~CURLFetcher()
{
    delete [] m_pBuffer;
}


bool CURLFetcher::FetchToFile(const char *filePath)
{
    int retval = true;

    // First try to open the output file to write the data to
    FILE * fp = fopen(filePath, "wb");
    if (fp == NULL)
        return false;


    // Initialize curl
    CURL *curl = curl_easy_init();

    if (curl == NULL) {
        fclose(fp);
        return false;
    }


    // Setup curl to download the given URL directly to the opened file handle
    // Http 400+ error codes are treated as failure to retrieve data
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());

    // Download
    CURLcode res = curl_easy_perform(curl);
    if (res != 0)
        retval = false;

    // Cleanup
    curl_easy_cleanup(curl);
    fclose(fp);

    return retval;

}



size_t
CURLFetcher::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    CURLFetcher *ourObject = static_cast<CURLFetcher *> (data);
    return ourObject->WriteToMemoryHandler(ptr, realsize);
}

size_t CURLFetcher::WriteToMemoryHandler(void *ptr, size_t size)
{
    if (m_pBuffer == NULL) {
        m_pBuffer = new uint8_t[size + 1];
        m_nBufferSize = size + 1;
    }
    if (m_nBufferOffset + (int)size + 1 > m_nBufferSize) {
        m_nBufferSize = (m_nBufferOffset + size) * 2;
        uint8_t *newBuffer = new uint8_t[m_nBufferSize];
        memcpy(newBuffer, m_pBuffer, m_nBufferOffset);
        delete [] m_pBuffer;
        m_pBuffer = newBuffer;
    }
    memcpy(m_pBuffer + m_nBufferOffset, ptr, size);
    memset(m_pBuffer + m_nBufferOffset + size, 0x0, 1); // Terminate string
    m_nBufferOffset += size;
    return size;
}

bool CURLFetcher::FetchToBuffer(uint8_t * & a_buffer, int & a_size)
{
    int retval = true;

    // Initialize curl
    CURL *curl = curl_easy_init();

    if (curl == NULL) {
        return false;
    }

    // Setup curl to download the given URL into a memory buffer
    // Http 400+ error codes are treated as failure to retrieve data
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLFetcher::WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());

    // Download
    CURLcode res = curl_easy_perform(curl);
    if (res != 0) {
        retval = false;
    }
    else {
        // Return buffer and size info to the caller
        a_buffer = m_pBuffer;
        a_size = m_nBufferOffset;
    }
    // Cleanup
    curl_easy_cleanup(curl);

    return retval;
}


/**********************************************************/

CFileUploader::CFileUploader(const char *url) {
    m_url = url;
    m_strError = "";
}

CFileUploader::~CFileUploader()
{
}

bool CFileUploader::UploadFile(const char *filePath, const char *fileUploadName)
{
    int retval = true;

    curlIntializer.Lock();

    // Initialize curl
    CURL *curl = curl_easy_init();

    if (curl == NULL) {
        curlIntializer.UnLock();
        return false;
    }

    /* Fill in the file upload field */
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, "file",
            CURLFORM_FILE, filePath,
            CURLFORM_FILENAME, fileUploadName,
            CURLFORM_END);

    // Setup curl to upload the file using POST
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());

    // Upload
    CURLcode res = curl_easy_perform(curl);
    if (res != 0) {
        m_strError = curl_easy_strerror(res);
        retval = false;
    }

    // Cleanup
    curl_easy_cleanup(curl);
    curl_formfree(formpost);

    curlIntializer.UnLock();
    return retval;
}

std::string CFileUploader::GetErrorMsg()
{
    return m_strError;
}
