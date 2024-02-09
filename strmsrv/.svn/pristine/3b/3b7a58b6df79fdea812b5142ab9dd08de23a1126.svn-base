/*
 * CFFSnapshot.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: durga
 */

#include "strmsrv_compatibility.h"
#include "FFSnapshot.hh"
#include "ThumbnailGenerator.h"
#include "SnapshotHandler.hh"
#include "sframe.hh"
#include "timekeeper.hh"
#include "bitmap.hh"
#include "FFMediaProcessorErrorHandler.hh"

#ifndef WINDOWS
#ifdef __XPI__
#include "jpeglib.h"
#else
#include <jpeglib.h>
#endif
#include <setjmp.h>
#endif

#include "utils.hh"
#include "colortransform.h"
#include "bitmap.hh"

//For jpeg decompressing.
#ifndef WINDOWS
static void init_source (j_decompress_ptr cinfo) {}

static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
    return TRUE ;
}

static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    jpeg_source_mgr* src = cinfo->src;

    src->next_input_byte += (size_t) num_bytes;
    src->bytes_in_buffer -= (size_t) num_bytes;
}

static void term_source (j_decompress_ptr cinfo) {}

struct decompress_error_mgr {
    struct jpeg_error_mgr pub;    /* "public" fields */

    jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct decompress_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}
#endif
//Jpeg decompressing end.

CFFSnapshot::CFFSnapshot(int nId, std::vector<int> & trackSpeeds,
                         bool bShowProgress, bool bNativeOutput,
                         bool bCreateTracks)
: CFFCodec("SNAPSHOT", bShowProgress, false, bNativeOutput)
{
    m_strMediaDestDir = "";

    m_nSnapshotWidth = 0;
    m_nSnapshotHeight = 0;

    m_pDecodedBuffer = NULL;
}

CFFSnapshot::~CFFSnapshot()
{
    if (m_pDecodedBuffer != NULL) {
        delete [] m_pDecodedBuffer;
    }
    m_pDecodedBuffer = NULL;
}

void CFFSnapshot::ProcessFrame(CSFrame *sFrame, timestamp_t nMediaOffset)
{
    if (sFrame == NULL) {
        return;
    }

    m_nElapsed = nMediaOffset;

    if (sFrame->GetMessageType() != CSFrame::Normal) {
        return;
    }

    if (sFrame->GetData() == NULL || sFrame->GetLength() <= 0) {
        return;
    }

    m_nProcessedBytes += sFrame->GetLength();

    PrintProgress();
    CreateOutput((unsigned char*)sFrame->GetData(), sFrame->GetLength(),
                 sFrame->GetMediaFlag());
}

void CFFSnapshot::CreateOutput(unsigned char* a_pInputData,
                               int a_nInBuffSize,
                               unsigned long a_nFlags)
{
    CreateThumbnails(a_pInputData, a_nInBuffSize, a_nFlags);
    CreateStandardMedia(a_pInputData, a_nInBuffSize, a_nFlags);
}

void CFFSnapshot::CreateThumbnails(unsigned char* a_pInputData,
                                   int a_nInBuffSize,
                                   unsigned long a_nFlags)
{
    CThumbnailHandler *pThumbnailHandler = GetThumbnailHandler();
    if (pThumbnailHandler == NULL) {
        return;
    }

    if (pThumbnailHandler->IsStreamTNEnabled() == false &&
            pThumbnailHandler->IsPrimaryTNEnabled() == false)
        return;

    uint64_t nOutSize = -1;
    int nErrorCode = 0;
    if ((a_nFlags & IMAGETYPE_MASK) == IMAGETYPE_BMP) {
    	nErrorCode = HandleBmpInput(a_pInputData, a_nInBuffSize, nOutSize);
    }
    else if ((a_nFlags & IMAGETYPE_MASK) == IMAGETYPE_JPEG) {
    	nErrorCode = HandleJpegInput(a_pInputData, a_nInBuffSize, nOutSize);
    }
    else {
        //Right now we are supporting only JPEG and BMP formats.
        return;
    }

    if (pThumbnailHandler->IsTNDirectoryCreated() == false) {
        pThumbnailHandler->CreateTNDirectory(m_strMediaDestDir);
    }

    if (nErrorCode == 0) {
        CreatePrimaryTN(m_pDecodedBuffer, nOutSize);
    }
    else {
        CFFMediaProcessorErrorHandler::SetThumbnailCreationErrorForSnapshot(nErrorCode);
    }
}

void CFFSnapshot::CreateStandardMedia(unsigned char * a_pInBuffer, int a_InBuffSize,
                                      unsigned long a_nFlags)
{
    CSnapshotHandler *pSnapshotHandler = dynamic_cast<CSnapshotHandler*>
                                        (GetStandardMediaHandler());
    if (pSnapshotHandler == NULL) {
        return;
    }

    if (pSnapshotHandler->CreateOutput(a_pInBuffer,
                                       a_InBuffSize,
                                       a_nFlags) == true) {
        m_nBytesWritten += a_InBuffSize;
    }
}

void CFFSnapshot::CreatePrimaryTN(unsigned char* a_pInputData, int a_nInBuffSize)
{
    char strFileName[255];
    if (a_pInputData == NULL)
        return;
    sprintf(strFileName,"%s/tn.jpg",
            GetThumbnailHandler()->GetTNDirectory().c_str());

    CreateSingleImage(a_pInputData, strFileName,
            GetThumbnailHandler()->GetPrimaryTNWidth(),
            GetThumbnailHandler()->GetPrimaryTNHeight());

    GetThumbnailHandler()->IncrementTNCount();
}

bool CFFSnapshot::CreateSingleImage(unsigned char * a_pInputBuffer, char * a_pStrFileName,
                                    int a_nThumbnailWidth, int a_nThumbnailHeight)
{
    GetThumbnailHandler()->ReCreateThumbnailGenerator(
            m_nSnapshotWidth, m_nSnapshotHeight, m_ePixelFormat);

    const int tnSize = a_nThumbnailWidth * a_nThumbnailHeight * 3;
    unsigned char *pTNBuffer = new unsigned char[tnSize];
    memset(pTNBuffer, 0x00, tnSize);

    GetThumbnailHandler()->SetThumbnailResolution(a_nThumbnailWidth, a_nThumbnailHeight);

    int nTNSize = GetThumbnailHandler()->CreateImage(&a_pInputBuffer, &pTNBuffer, tnSize);

    if (nTNSize <= 0) {
        delete [] pTNBuffer;
        return false;
    }

    GetThumbnailHandler()->CreateImageFile(&pTNBuffer, nTNSize, a_pStrFileName);

    delete [] pTNBuffer;

    m_nBytesWritten += nTNSize;

    return true;
}

void CFFSnapshot::StopStream()
{
}

int CFFSnapshot::HandleJpegInput (unsigned char *a_pInBuffer, int a_InBuffSize,
                                  uint64_t & a_OutBuffSize)
{
    m_nBytesPerPixel = 3;
    m_ePixelFormat = eRGB24;

    return (DecodeJpegImage(a_pInBuffer, a_InBuffSize, a_OutBuffSize));
}

int CFFSnapshot::HandleBmpInput (unsigned char *a_pInBuffer, int a_InBuffSize,
                                 uint64_t & a_OutBuffSize)
{
    m_ePixelFormat = eBGRX32;

    CBitmap oSrcBmp;
    int nErrorCode = oSrcBmp.LoadFromBuffer(a_pInBuffer, a_InBuffSize);
    if (nErrorCode != 0)
        return nErrorCode;

    m_nSnapshotWidth = oSrcBmp.GetWidth();
    m_nSnapshotHeight = oSrcBmp.GetHeight();

    a_OutBuffSize = m_nSnapshotWidth * m_nSnapshotHeight * 4;

    delete [] m_pDecodedBuffer;
    m_pDecodedBuffer = new unsigned char[a_OutBuffSize];

    unsigned int nSize = a_OutBuffSize;

    return (oSrcBmp.GetBitsBGRA((void*)m_pDecodedBuffer, nSize) ? 0 : 10);
}

int CFFSnapshot::DecodeJpegImage(unsigned char * a_pInBuffer, int a_InBuffSize,
                                 uint64_t & a_OutBuffSize)
{
#ifndef WINDOWS
    struct jpeg_decompress_struct cinfo;

    struct decompress_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        return 21;
    }

    jpeg_create_decompress(&cinfo);

    if (cinfo.src == NULL) {
        cinfo.src = (struct jpeg_source_mgr *) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
                     sizeof(struct jpeg_source_mgr));
    }

    cinfo.src->init_source = init_source;
    cinfo.src->fill_input_buffer = fill_input_buffer;
    cinfo.src->skip_input_data = skip_input_data;
    cinfo.src->term_source = term_source;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart;
    cinfo.src->bytes_in_buffer = a_InBuffSize;
    cinfo.src->next_input_byte = a_pInBuffer;

    (void) jpeg_read_header(&cinfo, TRUE);

    cinfo.out_color_space = JCS_RGB;
    cinfo.out_color_components = m_nBytesPerPixel;

    (void) jpeg_start_decompress(&cinfo);

    int nRow_stride = cinfo.output_width * cinfo.output_components;

    m_nSnapshotWidth = cinfo.output_width;
    m_nSnapshotHeight = cinfo.output_height;

    delete [] m_pDecodedBuffer;
    m_pDecodedBuffer = new unsigned char[cinfo.image_width * cinfo.image_height * cinfo.output_components];

    JSAMPARRAY pRowBuffer = new JSAMPROW[1];

    int i = 0;
    while(cinfo.output_scanline < cinfo.output_height) {
        pRowBuffer[0] = (JSAMPROW)((m_pDecodedBuffer) + nRow_stride * i);
        (void) jpeg_read_scanlines(&cinfo, pRowBuffer, 1);
        i++;
    }

    delete  [] pRowBuffer;

    (void) jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    return 0;
#endif

    return 0;
}

void CFFSnapshot::PrintStats()
{
    char buffer[64];
    printf("Processed: %.2f%% %s\n", m_PercentComplete, GetTimeString(m_nElapsed, buffer));
}
