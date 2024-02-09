// File        : SpiMaster.cpp
// Author      : Albert M. Hajjar, Chris Wulff
// Description : Wrapper class for an SPI master making use of the spidev 
//               driver interface.  Each instance abstracts the connection
//               to one of potentially several slaves controlled by an SPI 
//               master, which are uniquely identified by an spidev minor number.
//
// Copyright (c) 2011 Lab X Technologies.  All rights reserved.

// System headers
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// Library headers
#include "Debug.h"
#include "Exception.h"
#include "SpiMaster.h"

// Namespace using directives
using LabX::SpiMaster;
using LabX::Exception;
using std::cout;
using std::endl;
using std::hex;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;

// Constructor / Destructor

SpiMaster::SpiMaster(const string &friendlyName, const string &deviceName) :
  friendlyName(friendlyName),
  deviceName(deviceName),
  deviceHandle(-1) {
}

SpiMaster::~SpiMaster(void) {
  // Ensure that we are closed
  close();
}

// Public interface methods

const string& SpiMaster::getName(void) const {
  return(friendlyName);
}

const string& SpiMaster::getDeviceName(void) const {
  return(deviceName);
}

void SpiMaster::open(void) {
  // Guard against multiple calls
  if(deviceHandle < 0) {
    // SPI master not yet opened, proceed with the attempt
    DebugNoteVa("Opening SPI master \"%s\"", deviceName.c_str());
    if((deviceHandle = ::open(deviceName.c_str(), O_RDWR)) < 0) {
      stringstream excStream;
      excStream << "Couldn't open SPI Master device file "
        << deviceName
        << " for writing "
        << errno;
      throw(new Exception(excStream.str()));
    }
  }
}

void SpiMaster::close(void) {
  if(deviceHandle >= 0) {
    // Close the device file and null it
    DebugNoteVa("Closing SPI Master \"%s\"", deviceName.c_str());
    ::close(deviceHandle);
    deviceHandle = -1;
  }
}

// Protected interface methods

int32_t SpiMaster::transferData(const uint8_t *txBuffer, 
                                uint8_t *rxBuffer,
                                uint32_t length, 
                                bool finalTransfer) {

  struct spi_ioc_transfer transfer = { };

  transfer.tx_buf        = (uintptr_t) txBuffer,
  transfer.rx_buf        = (uintptr_t) rxBuffer,
  transfer.len           = length;
  transfer.delay_usecs   = 0;
  transfer.speed_hz      = 10000000;
  transfer.bits_per_word = 8;
  transfer.cs_change     = (finalTransfer) ? 0 : 1;

  // Send a single SPI transfer
  return(deviceIoControl(SPI_IOC_MESSAGE(1), &transfer));
}

// Private helper methods

int32_t SpiMaster::deviceIoControl(uint32_t ioCommand, void *ioParameter) {
  // Presume success
  int returnValue = 0;

  // Ensure the device is open at all
  if(deviceHandle < 0) {
    stringstream excStream;
    excStream << "Attempted I/O control of SPI Master \""
      << getName()
      << "\" prior to call to open()";
    throw(new Exception(excStream.str()));
  }

  // Invoke the system call and pass its error code on up; client code
  // is responsible for handling it.
  if(ioctl(deviceHandle, ioCommand, ioParameter) < 0) returnValue = errno;

  return(returnValue);
}
