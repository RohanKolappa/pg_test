#ifndef __PORTUSE_H__
#define __PORTUSE_H__

#include <map>

#include "mutex.hh"


class CPortUse {
public:
    bool ReservePort(int a_port);
    void ReleasePort(int a_port);
    bool PortAvailable(int a_port);

private:
    std::map <int, bool> m_portMap;
    CMutex m_mutex;

};

#endif                          // __PORTUSE_H__
