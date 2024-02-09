
#include "USBHid.h"

CUSBHID::CUSBHID()
{
	m_nKeyboardFD = -1;
	m_nMouseAbsFD = -1;
	m_nMouseRelFD = -1;
	m_nKeyboardLEDs = 0;
	OpenGadgets();

}

CUSBHID::~CUSBHID()
{
	CloseGadgets();

	m_nKeyboardFD = -1;
	m_nMouseAbsFD = -1;
	m_nMouseRelFD = -1;
	m_nKeyboardLEDs = 0;
}

int CUSBHID::ReadAbsoluteMouse(char *pInbuf, int buflen)
{
	return 1;
}

int CUSBHID::ReadRelativeMouse(char *pInBuf, int buflen)
{
	return 1;
}

int CUSBHID::ReadKeyboard(char *pInBuf, int buflen)
{
	return 1;
}

bool CUSBHID::isFDReady(int fd) {
    fd_set write_fdset;
    struct timeval timeout;

    FD_ZERO(&write_fdset);
    FD_SET(fd, &write_fdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    int nd = select(fd + 1, NULL, &write_fdset, NULL, &timeout);
    if (nd < 0) {
        return false;
    }
    else if (nd == 0) {
        return false;
    }

    if (FD_ISSET(fd, &write_fdset))
        return true;

    return false;
}

int CUSBHID::WriteAbsoluteMouse(char *pInbuf, int buflen)
{
	int nBytes = 0;

    char *buf = (char *) pInbuf;

    if (buflen == 0 || pInbuf == NULL || m_nMouseAbsFD <= 0)
        return 0;

    if (isFDReady(m_nMouseAbsFD)) {
        nBytes = write(m_nMouseAbsFD, buf, buflen);

        if (nBytes <= 0)
            return 0;
    }
    return nBytes;
}

int CUSBHID::WriteRelativeMouse(char *pInBuf, int buflen)
{
	int nBytes = 0;

    if (pInBuf == NULL || buflen <= 0 || m_nMouseRelFD <= 0)
       return 0;

    if (isFDReady(m_nMouseRelFD)) {
        nBytes = write(m_nMouseRelFD, pInBuf, buflen);

        if (nBytes <= 0)
            return 0;
    }
    return nBytes;

}

int CUSBHID::WriteKeyboard(char *pInBuf, int buflen)
{
	int nBytes = 0;

    if (pInBuf == NULL || buflen <= 0 || m_nKeyboardFD <= 0)
        return 0;
	

    if (isFDReady(m_nKeyboardFD)) {
        nBytes =  write(m_nKeyboardFD, pInBuf, buflen);

        if (nBytes <= 0)
            return 0;
    }
    return nBytes;
}

void CUSBHID::OpenGadgets()
{
    int flags;

	m_nKeyboardFD = open(HIDKEYBOARDPATH, O_RDWR);

	if (m_nKeyboardFD <= 0) {
		SYSLOG_DEBUG("-%u- %s : Could not open Keyboard gadget device path %s:\n",__LINE__, __FUNCTION__,HIDKEYBOARDPATH);
	}
	else {
	    flags = fcntl(m_nKeyboardFD, F_GETFL, 0);
	    fcntl(m_nKeyboardFD, F_SETFL, flags | O_NONBLOCK);
	}

	m_nMouseRelFD = open(HIDRELMOUSEPATH, O_RDWR);
	
	if (m_nMouseRelFD <= 0) {
		SYSLOG_DEBUG("-%u- %s : Could not open Relative Mouse Gadget device path %s:\n",__LINE__, __FUNCTION__,HIDRELMOUSEPATH);
	}
	else {
	    flags = fcntl(m_nMouseRelFD, F_GETFL, 0);
	    fcntl(m_nMouseRelFD, F_SETFL, flags | O_NONBLOCK);
	}

	m_nMouseAbsFD = open(HIDABSMOUSEPATH, O_RDWR);
	
	if (m_nMouseAbsFD <= 0) {
		SYSLOG_DEBUG("-%u- %s : Could not open Absolute Mouse Gadget device path %s:\n",__LINE__, __FUNCTION__,HIDABSMOUSEPATH);
    }
    else {
        flags = fcntl(m_nMouseAbsFD, F_GETFL, 0);
        fcntl(m_nMouseAbsFD, F_SETFL, flags | O_NONBLOCK);
    }


}

void CUSBHID::CloseGadgets()
{

    if (m_nKeyboardFD > 0)
       close(m_nKeyboardFD);

    if (m_nMouseRelFD > 0)
       close(m_nMouseRelFD);

    if (m_nMouseAbsFD > 0)
       close(m_nMouseAbsFD);

    m_nKeyboardFD = m_nMouseRelFD = m_nMouseAbsFD = -1;

 
}

int CUSBHID::SetKeyboardLEDsStatus()
{
    int nBytes;
    char  CapslockStatus[2];

    if (m_nKeyboardFD < 0)
        return 0;

    nBytes =  read(m_nKeyboardFD, CapslockStatus, 2);

     if (nBytes < 0)
       return 0;

    m_nKeyboardLEDs = CapslockStatus[0];

    return m_nKeyboardLEDs;
}

int CUSBHID::GetKeyboardLEDStatus()
{
	return m_nKeyboardLEDs;
}

int CUSBHID::GetKeyboardFD()
{
	return m_nKeyboardFD;
}

int CUSBHID::GetAbsoluteMouseFD()
{
	return m_nMouseAbsFD;
}

int CUSBHID::GetRelativeMouseFD()
{
	return m_nMouseRelFD;
}

