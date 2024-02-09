#ifndef _SPI_MASTER_H_
#define _SPI_MASTER_H_

// System headers
#include <list>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace LabX {

  /**
   * Wrapper class for an SPI master making use of the spidev driver
   * interface.  Each instance abstracts the connection to one of
   * potentially several slaves controlled by an SPI master, which
   * are uniquely identified by an spidev minor number.
   *
   * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
   *
   * @author Chris Wulff
   * @author Albert Hajjar
   */
  class SpiMaster {

    // Constructor / Destructor
  public:
    
    /**
     * Creates an instance associated with the passed device.  
     * @param friendlyName - "Friendly" name for the SPI master
     * @param deviceName   - Device name, e.g. "/dev/spidev0"
     */
    SpiMaster(const std::string& friendlyName, 
              const std::string& deviceName);

    /**
     * Virtual destructor
     */
    virtual ~SpiMaster(void);
    
    // Public interface methods
  public:
    
    /**
     * Accessor for the friendly name
     * @return - The "friendly name" specified at construction
     */
    virtual const std::string& getName(void) const;
    
    /**
     * Accessor for the device name
     * @return - The "device name" specified at construction
     */
    virtual const std::string& getDeviceName(void) const;
    
    /**
     * Opens the instance and prepares the master for use
     */
    virtual void open(void);
    
    /**
     * Closes the instance, disabling the master
     */
    virtual void close(void);

    // Protected helper methods
  protected:

    /**
     * Transfer data over SPI.
     * @param txBuffer      - Transmit data buffer
     * @param rxBuffer      - Receive data buffer
     * @param length        - Message length
     * @param finalTransfer - True if this is the last transfer (slave select released)
     * @return              - The value returned by the system call
     */
    virtual int32_t transferData(const uint8_t *txBuffer, 
                                 uint8_t *rxBuffer,
                                 uint32_t length, 
                                 bool finalTransfer);

    // Private helper methods
  private:
    
    /**
     * Sends an I/O control command to the encapsulated device
     * @param ioCommand   - Enumerated command understood by the device driver
     * @param ioParameter - Command-specific parameter data
     * @return            - The value returned by the system call
     */
    int32_t deviceIoControl(uint32_t ioCommand, void *ioParameter);

    // Private attributes
  private:
    
    /**
     * Friendly name of the instance
     */
    const std::string friendlyName;
    
    /**
     * Device driver name
     */
    const std::string deviceName;
    
    /**
     * Device file handle
     */
    int32_t deviceHandle;
  };
}

#endif // _SPI_MASTER_H_

