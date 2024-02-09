/*
 * File        : ptp_demo.cpp
 * Author      : Eldridge M. Mount IV
 * Description : Demonstration of the PTP component in isolation
 *
 * Copyright (c) 2011, Lab X Technologies, LLC
 * All rights reserved
 */

// System headers
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <termios.h>
#include <vector>

// Lab X Foundation Class Library headers
#include "Exception.h"
#include "LabX.h"
#include "Thread.h"

// Lab X AVB Library headers
#include "AvbPort.h"
#include "PtpDomain.h"

// Namespace using directives
using Avb::AvbPort;
using Avb::PtpDomain;
using LabX::Exception;
using LabX::Thread;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// Simple Thread-derived class to abstract the main thread.
// Various Lockable-derived objects within the application rely on
// a Thread object to implement multiple-lock semantics.
class PtpDaemon : public Thread {

  // Constructor / destructor
public:

  /**
   * Constructor
   * @param appName - Name of the application executable
   */
  PtpDaemon(const string &appName) :
    Thread("PtpDaemon"),
    appName(appName) {
  }

  /**
   * Destructor
   */
  ~PtpDaemon(void) {}

  // Public overrides from class Thread
public:

  /**
   * Main thread of control, runs the demo
   */
  virtual int32_t run(void) {
    // Presume success
    int32_t returnValue(Thread::k_Success);

    // Announce the demonstration and begin creating the objects to
    // abstract the AVB hardware to be demonstrated
    cout << "<<< PTP Daemon started >>>" << endl;

    try {
      // Create the new PTP domain, adding all AVB ports first
      avbPorts.push_back(new AvbPort("eth0"));
 //     avbPorts.push_back(new AvbPort("eth1"));
      string domainName("AVB Domain");
      string ptpDevice("/dev/ptp0");
      PtpDomain *ptpDomain = new PtpDomain(domainName, ptpDevice, avbPorts);
  
      // Use 802.1AS profile defaults
      int16_t currentUtcOffset = IEEE_802_1_AS_UTC_OFFSET;
      uint8_t grandmasterPriority1 = IEEE_802_1_AS_GRANDMASTER_PRIORITY1;
      uint8_t grandmasterPriority2 = IEEE_802_1_AS_GRANDMASTER_PRIORITY2;
      uint8_t clockClass = IEEE_802_1_AS_CLOCK_CLASS;
      uint8_t clockAccuracy = IEEE_802_1_AS_CLOCK_ACCURACY;
      uint16_t offsetLogVariance = IEEE_802_1_AS_SCALED_LOG_VARIANCE;

      // Start the PTP domain with the desired parameters and the requested
      // delay mechanism (Peer-to-Peer, e.g. 802.1AS or End-to-End, classic PTP)
    //  PtpDomain::DelayMechanism delayMechanism = PtpDomain::DELAY_MECHANISM_P2P; // PtpDomain::DELAY_MECHANISM_E2E
      PtpDomain::DelayMechanism delayMechanism = PtpDomain::DELAY_MECHANISM_E2E; // PtpDomain::DELAY_MECHANISM_E2E
      ptpDomain->open(currentUtcOffset, grandmasterPriority1, 
                      grandmasterPriority2, clockClass, clockAccuracy,
                      offsetLogVariance, delayMechanism);

      // Modify the coefficients used by the PTP loop based upon whether point-to-point
      // or end-to-end mode has been selected.  Specifically, the gain can be higher in
      // point-to-point mode (i.e. 802.1AS profile) since the error due to jitter is
      // largely eliminated.
      if(delayMechanism == PtpDomain::DELAY_MECHANISM_P2P) {
        ptpDomain->setCoefficients(0xFFF00000, 0x80000000, 0x80000000);
      } else {
        ptpDomain->setCoefficients(0xF8000000, 0xFC000000, 0x00000000);
      }
    } catch(Exception *exception) {
      cout << "Caught: " << exception->toString() << endl;
      delete(exception);
      returnValue = Thread::k_CaughtException;
    } catch(...) {
      cout << "Caught unknown exception" << endl;
      returnValue = Thread::k_CaughtException;
    } // try {}

    // Return an appropriate error code
    return(returnValue);
  } // PtpDaemon::run()

  // Private attributes

private:

  /**
   * Name of the application
   */
  const string appName;

  /**
   * Vector of AVB port abstractions
   */
  vector<Avb::AvbPort*> avbPorts;

  /**
   * PTP domain object
   */
  PtpDomain *ptpDomain;

};

/**
 * Main point of entry for the AVB Control Daemon
 */
int main(int argc, char* argv[]) {
  int returnValue = Thread::k_Success;
  const string appName(argv[0]);

  // Create the PTP instance with the selected parameters
  PtpDaemon ptpDaemon(appName);
  returnValue = ptpDaemon.attachCurrent();

  // Return the exit status
  return(returnValue);
}
