#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<syslog.h>

#include "KbmIPCUtils.hh"
#include "common.hh"

CKbmIpc::CKbmIpc()
{
    InitializeVariables();
    OpenFIFOFile();

}

CKbmIpc::~CKbmIpc()
{
	CloseFIFOFile();

}

void CKbmIpc::InitializeVariables()
{
    m_nKeyboardfifofd = -1;
    m_nMouseAbsfifofd = -1;
    m_nWidth = 0;
    m_nHeight = 0;
}

void CKbmIpc::OpenFIFOFile()
{

    m_nKeyboardfifofd = open(KBSFIFOPATH, O_RDWR);
    if (m_nKeyboardfifofd < 0)
        SYSLOG_DEBUG("KBMIPCUTILS: Could not open the Keyboard fifo file %s:\n",
                     KBSFIFOPATH);
    else
        FlushKBMData(m_nKeyboardfifofd);

    m_nMouseAbsfifofd = open(ABSMSFIFOPATH, O_RDWR);
    if (m_nMouseAbsfifofd < 0)
        SYSLOG_DEBUG("KBMIPCUTILS: Could not open the Abs Mouse fifo file %s:\n",
                     ABSMSFIFOPATH);
    else
        FlushKBMData(m_nMouseAbsfifofd);

}

void CKbmIpc::CloseFIFOFile()
{
    if (m_nKeyboardfifofd >= 0)
        close(m_nKeyboardfifofd);

    if (m_nMouseAbsfifofd >= 0)
        close(m_nMouseAbsfifofd);

    m_nMouseAbsfifofd = -1;
    m_nKeyboardfifofd = -1;
}

/* flush any data available in KBM fifo */
void CKbmIpc::FlushKBMData(int fd )
{
    int nBytes;
    int buffer[5];
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    do {
        nBytes = read(fd, (void *)&buffer, sizeof(buffer));
    } while (nBytes > 0);
}

int CKbmIpc::GetKbData(unsigned int *key, unsigned int *down)
{
    int nBytes;
    unsigned int buffer[2];

    if (m_nKeyboardfifofd < 0)
        return 0;

    nBytes = read(m_nKeyboardfifofd, (void *)&buffer, sizeof(buffer));
    if (nBytes != sizeof(buffer))
        return 0;

    *key = buffer[0];
    *down = buffer[1];


    return nBytes;

}

int CKbmIpc::GetMsData(int *buttonMask, int *VncAbsWidth, int *VncAbsHeight)
{

    int nBytes;
    int buffer[5];

    if (m_nMouseAbsfifofd < 0)
        return 0;

    nBytes = read(m_nMouseAbsfifofd, (void *)&buffer, sizeof(buffer));
    if (nBytes != sizeof(buffer))
       return 0;

    *buttonMask   = buffer[0];
    *VncAbsWidth  = buffer[1];
    *VncAbsHeight = buffer[2];
    m_nWidth      = buffer[3];
    m_nHeight     = buffer[4];


    return nBytes;

}

int CKbmIpc::GetScreenWidth()
{

    return m_nWidth;

}

int CKbmIpc::GetScreenHeight()
{

    return m_nHeight;

}
