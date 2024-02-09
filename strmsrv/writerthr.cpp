#include "strmsrv_compatibility.h"
#include "writerthr.hh"
#include "writer.hh"
#include "imagewriter.hh"
#ifndef WINDOWS
#include "deleter.hh"
#endif
#include "common.hh"

using namespace std;

/// This is the thread code
/// Construtor for the Writer thread
CWriterThr::CWriterThr(const char *pMediaDir) :
        CStreamerThread("Writer")
{
        m_pMediaDir = new char[MAXFILENAMELEN+1];
        sprintf (m_pMediaDir, "%s", pMediaDir);

}

/// Destructor for the Writer thread
CWriterThr::~CWriterThr()
{
        SYSLOG_TRACE("Destructing %s:%s\n", __FILE__, __FUNCTION__);

        delete[] m_pMediaDir;

}

CStreamer *
CWriterThr::handleAdd(CCommand *pCmd)
{
    CStreamer   *pStreamer = NULL;

    switch (pCmd->GetPortType()) {
#ifdef HAVE_DISKIO
        case DMSRECORDERPORT:
        {
            CWriter * pWriter;
            int nTrimLength = 0;
            char pMediaName[80];
            char pMediaDir[MAXFILENAMELEN + 1];
            int bCreateFFTracks = 0;
            int bCreateHLSMedia = 0;
            int bCreatePrimaryThumbnail = 0;
            int nStreamThumbnailFrequency = 0;
            char pStreamType[15];
            char pSDPFilePath[MAXFILENAMELEN + 1];

            strcpy(pMediaName, "");
            pCmd->GetIntVar("trimlength", &nTrimLength);
            pCmd->GetVar("dstname", pMediaName, 80);
            if (pCmd->GetVar("dstdir", pMediaDir, MAXFILENAMELEN) == -1) {
                strcpy(pMediaDir, m_pMediaDir);
            }

            pCmd->GetIntVar("speedtracks", &bCreateFFTracks);
            pCmd->GetIntVar("enableHLS", &bCreateHLSMedia);
            pCmd->GetIntVar("primarythumbnail", &bCreatePrimaryThumbnail);
            pCmd->GetIntVar("streamthumbnails", &nStreamThumbnailFrequency);

            strcpy(pStreamType, "");
            pCmd->GetVar("streamtype", pStreamType, 15);

            strcpy(pSDPFilePath, "");
            pCmd->GetVar("streamURL", pSDPFilePath, MAXFILENAMELEN);

            pWriter = new CWriter (pCmd->GetId(),
                                   pMediaName,
                                   pMediaDir,
                                   nTrimLength,
                                   bCreateFFTracks,
                                   bCreateHLSMedia,
                                   pStreamType,
                                   bCreatePrimaryThumbnail,
                                   nStreamThumbnailFrequency,
                                   pSDPFilePath);

            pStreamer = pWriter;
            break;
        }
#endif

#ifdef HAVE_DISKIO
#ifndef WINDOWS
        case DELETERPORT:
        {
            CDeleter * pDeleter;
            char pMediaDir[MAXFILENAMELEN + 1];

            strcpy(pMediaDir, "/data/DSS/data/portal/site/default/trash");
            pCmd->GetVar("deletedir", pMediaDir, MAXFILENAMELEN);
            pDeleter = new CDeleter (pCmd->GetId(),
                                     pMediaDir);

            pStreamer = pDeleter;

            break;
        }
#endif
#endif

        case IMAGEWRITERPORT:
        {
            char pMediaDir[MAXFILENAMELEN + 1];
            char pPublishURL[MAXURLLEN + 1];
            char pStreamType[15];
            char pSDPFilePath[MAXFILENAMELEN + 1];
            int nIOPort = 0;
            int bEnableTNs = 1;
            int bNative = 0;
            int bTnLarge = 1;
            int bTnSmall = 1;
            int nLargeTNWidth = 640;
            int nLargeTNHeight = 320;
            int nSmallTNWidth = 160;
            int nSmallTNHeight = 160;
            int nTnInterval = 4;

            if (pCmd->GetVar("dstdir", pMediaDir, MAXFILENAMELEN) == -1) {
                strcpy(pMediaDir, m_pMediaDir);
            }

            strcpy(pStreamType, "V2D");
            pCmd->GetIntVar("ioport", &nIOPort);
            pCmd->GetVar("streamtype", pStreamType, 15);

            pCmd->GetIntVar("enabletn", &bEnableTNs);
            pCmd->GetIntVar("native", &bNative);
            pCmd->GetIntVar("tnlarge", &bTnLarge);
            pCmd->GetIntVar("tnsmall", &bTnSmall);

            pCmd->GetIntVar("tnlargewidth", &nLargeTNWidth);
            pCmd->GetIntVar("tnlargeheight", &nLargeTNHeight);
            pCmd->GetIntVar("tnsmallwidth", &nSmallTNWidth);
            pCmd->GetIntVar("tnsmallheight", &nSmallTNHeight);

            pCmd->GetIntVar("tntimeinterval", &nTnInterval);

            strcpy(pSDPFilePath, "");
            pCmd->GetVar("streamURL", pSDPFilePath, MAXFILENAMELEN);

            strcpy(pPublishURL, "");
            pCmd->GetVar("publishURL", pPublishURL, MAXURLLEN);

            CImageWriter * pImageWriter = new CImageWriter (pCmd->GetId(),
                                                            nIOPort,
                                                            pMediaDir,
                                                            pStreamType,
                                                            bEnableTNs,
                                                            bNative,
                                                            bTnLarge,
                                                            bTnSmall,
                                                            nSmallTNWidth,
                                                            nSmallTNHeight,
                                                            nLargeTNWidth,
                                                            nLargeTNHeight,
                                                            nTnInterval,
                                                            pSDPFilePath,
                                                            pPublishURL);


            pStreamer = pImageWriter;
            break;

        }

        default:
            ASSERT(0);
            break;

    }

    return pStreamer;
}

void
CWriterThr::resetGlobalStats()
{
#ifdef HAVE_DISKIO
    CWriter::ResetGlobalStats();
#endif
}

int
CWriterThr::handleUpdateStreamer(CCommand *a_pCmd, CStreamer *a_streamer)
{
    int retval = 0;

    switch (a_pCmd->GetPortType()) {
    case IMAGEWRITERPORT:
        retval = a_streamer->Update(a_pCmd);
        break;
    }

    return retval;
}
