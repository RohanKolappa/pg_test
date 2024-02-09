/*
 * MulticastUtils.cpp
 *
 *  Created on: Oct 7, 2009
 *      Author: rkale
 */
#include "strmsrv_compatibility.h"
#include <sstream>

#include "MulticastUtils.hh"
#ifndef WINDOWS
#ifdef HAVE_V2O
#include "if_list_config.h"
#endif
#endif
#include "socket.hh"

using namespace std;

/// Returns true if the given port is found in the list of interfaces
bool CMulticastUtils::CheckInterfaceList(const intlist & a_interfaceList,
        int port)
{
    for (int i = 0; i < (int) a_interfaceList.size(); i++) {
        if (a_interfaceList[i] == port)
            return true;
    }
    return false;
}

/// Registers all interfaces that need to support multicast receiving
/// with the given socket.
void CMulticastUtils::RegisterMulticastRecvInterfaces(CSocketDgram & a_socket,
        const CInternetAddress & a_source, const intlist & a_interfaceList,
        string & a_interfaceInfo)
{
#ifndef HAVE_V2O
    return;
#else
    // No need to do anything if source is not multicast
    if (!a_source.IsMulticast()) {
        a_interfaceInfo.append("");
        return;
    }

    // Get current network interface config
    struct if_list_config if_list_cfg;
    get_if_list_config(&if_list_cfg);

    // Register each interface that is enabled.
    // If caller has not provided a list of interfaces, register
    // all interface that have "multicast" enabled in the config
    // Else register the exact list of interfaces the caller wants
    a_socket.UseMcastRecvInterfaceList();
    int count = 0;
    for (int i = 0; i < if_list_cfg.number_of_interfaces; i++) {
        if (if_list_cfg.interface[i].enable == 1) {
            if (a_interfaceList.size() == 0) {
                if (if_list_cfg.interface[i].multicast != 1)
                    continue;
            }
            else {
                if (!CheckInterfaceList(a_interfaceList,
                        if_list_cfg.interface[i].port))
                    continue;
            }

            // OK to register this interface
            a_socket.AddMcastReceiveInterface(CInternetAddress(
                    if_list_cfg.interface[i].ipaddress));
            ostringstream os;
            if (count != 0)
                os << ",";
            os << "LAN" << if_list_cfg.interface[i].port;
            a_interfaceInfo.append(os.str());
            count++;
        }
    }
#endif
}

/// Registers the interface that need to support multicast sending
/// with the given socket.
void CMulticastUtils::RegisterMulticastSendInterface(CSocketDgram & a_socket,
        const CInternetAddress & a_destination, int a_interface,
        string & a_interfaceInfo)
{
#ifndef HAVE_V2O
    return;
#else
    // No need to do anything if destination is not multicast
    if (!a_destination.IsMulticast()) {
        a_interfaceInfo.append("");
        return;
    }

    // Get current network interface config
    struct if_list_config if_list_cfg;
    get_if_list_config(&if_list_cfg);

    // Find the lowest numbered interface that is enabled and supports multicast
    // If an interface is forced on us, use that interface as long as the
    // interface is enabled
    int use_index = -1;
    int port = MAX_NUMBER_OF_INTERFACES;
    for (int i = 0; i < if_list_cfg.number_of_interfaces; i++) {
        if (if_list_cfg.interface[i].enable == 1) {
            if (a_interface > 0) {
                // forced
                if (if_list_cfg.interface[i].port == a_interface) {
                    use_index = i;
                    break;
                }
            }
            else {
                // find minimum
                if (if_list_cfg.interface[i].multicast == 1) {
                    if (port > if_list_cfg.interface[i].port) {
                        port = if_list_cfg.interface[i].port;
                        use_index = i;
                    }
                }
            }
        }
    }

    // If matching interface found, register it
    if (use_index >= 0) {
        a_socket.SetMcastSendInterface(CInternetAddress(
                if_list_cfg.interface[use_index].ipaddress));
        ostringstream os;
        os << "LAN" << if_list_cfg.interface[use_index].port;
        a_interfaceInfo.append(os.str());
    }
    else {
        a_interfaceInfo.append("Default");
    }
#endif
}

/// Registers all interfaces that need to support multicast receiving
/// with the given socket.
void CMulticastUtils::RegisterMulticastRecvInterfaces(V2DRx_t * a_RxSession,
        const intlist & a_interfaceList,
        string & a_interfaceInfo)
{
#ifdef WINDOWS
    return;
#else
    // The functionality is already implemented in another context in
    // a function above.
    // Reuse the implementation by mocking up a socket and a multicast source.
    // Then collect list from the socket and insert into V2DRx_t session

    CSocketDgram socket;
    CInternetAddress source("224.1.1.1");
    RegisterMulticastRecvInterfaces(socket, source, a_interfaceList, a_interfaceInfo);

    Rx_UseMcastRecvInterfaceList(a_RxSession);

    for (int i = 0; i < socket.GetMcastReceiveInterfaceCount(); i++) {
        CInternetAddress address = socket.GetMcastReceiveInterface(i);
        Rx_AddMcastReceiveInterface(a_RxSession,
                ((struct sockaddr_in *) address.GetAddress())->sin_addr);
    }
#endif
}

void CMulticastUtils::RegisterMulticastSendInterface(V2DTx_t * a_TxSession,
        int a_interface,
        string & a_interfaceInfo)
{
#ifdef WINDOWS
    return;
#else
    // The functionality is already implemented in another context in
    // a function above.
    // Reuse the implementation by mocking up a socket and a multicast source.
    // Then collect send interface from socket and set into V2DRTx_t session

    CSocketDgram socket;
    CInternetAddress source("224.1.1.1");
    RegisterMulticastSendInterface(socket, source, a_interface, a_interfaceInfo);

    if (socket.IsMcastSendInterfaceSet()) {
        CInternetAddress address = socket.GetMcastSendInterface();
        Tx_SetMcastSendInterface(a_TxSession,
                ((struct sockaddr_in *) address.GetAddress())->sin_addr);
    }
#endif
}
