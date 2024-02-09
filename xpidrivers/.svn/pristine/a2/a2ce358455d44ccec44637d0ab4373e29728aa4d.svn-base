// File        : EthernetDefinitions.h
// Author      : Eldridge M. Mount IV
// Description : XML parsing event handler for AVB configuration files.  This
//               handler provides the "glue" between an XML file defining the
//               clock domains, Talker, and Listener streams an endpoint will
//               manage and the hardware abstraction objects which will manage
//               the platform's AVB hardware.
//
// Copyright (c) 2010 Lab X Technologies.  All rights reserved.

#ifndef _ETHERNET_DEFINITIONS_H_
#define _ETHERNET_DEFINITIONS_H_

// Size, in bytes, of various header fields
#define MAC_ADDRESS_BYTES  (6)
#define LTF_BYTES          (2)

// Offsets of various header fields
#define DEST_MAC_OFFSET  (0)
#define SRC_MAC_OFFSET   (DEST_MAC_OFFSET + MAC_ADDRESS_BYTES)
#define LTF_OFFSET       (SRC_MAC_OFFSET + MAC_ADDRESS_BYTES)

// Size, in bytes, of various components of Ethernet MAC overhead
#define ETH_HEADER_SIZE    ((2 * MAC_ADDRESS_BYTES) + LTF_BYTES)
#define VLAN_TAG_SIZE      (4)
#define FCS_SIZE           (4)
#define IFG_SIZE          (12)
#define PREAMBLE_SFD_SIZE  (8)

#endif
