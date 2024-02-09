
#include "strmsrv_compatibility.h"
#include "portuse.hh"


bool
CPortUse::ReservePort(int a_port)
{
    CScopedLock lock(m_mutex);

    if (m_portMap[a_port] == false) {
        m_portMap[a_port] = true;
        return true;
    }
    return false;
}


void
CPortUse::ReleasePort(int a_port)
{
    CScopedLock lock(m_mutex);

    m_portMap[a_port] = false;
}

bool
CPortUse::PortAvailable(int a_port)
{
    return m_portMap[a_port];
}
