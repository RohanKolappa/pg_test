/*
 * MulticastUtils.hh
 *
 *  Created on: Oct 7, 2009
 *      Author: rkale
 */

#ifndef MULTICASTUTILS_HH_
#define MULTICASTUTILS_HH_

#include <string>

#include "command.hh"
#include "v2d.h"


// Forward Declarations
class CSocketDgram;
class CInternetAddress;


class CMulticastUtils {
public:
    static void RegisterMulticastRecvInterfaces(CSocketDgram & a_socket,
            const CInternetAddress & a_source, const intlist & a_interfaceList,
            std::string & a_interfaceInfo);
    static void RegisterMulticastSendInterface(CSocketDgram & a_socket,
            const CInternetAddress & a_destination, int a_interface,
            std::string & a_interfaceInfo);
    static void RegisterMulticastRecvInterfaces(V2DRx_t * a_RxSession,
            const intlist & a_interfaceList,
            std::string & a_interfaceInfo);
    static void RegisterMulticastSendInterface(V2DTx_t * a_TxSession,
            int a_interface,
            std::string & a_interfaceInfo);
private:
    static bool CheckInterfaceList(const intlist & a_interfaceList, int port);
};

#endif /* MULTICASTUTILS_HH_ */
