/*! \file omnitek_dma_api.h
 * \brief Omnitek - Barco NetViz Interface API
 *  \details This header file details the interface API provided by Omnitek 
 *  for use with the Omnitek DMA Driver.  The API provides helper
 *  functions that support the main DMA operations of the Driver.
 */

#include "omnitek_dma.h"
 
/// \class IOmnitek_DMA_Interface
/// \brief Omnitek DMA Interface base class
/*! 
 * \details This class provides a basic interface to the DMA.  It supports
 * the core functions such as resource acquisition and locking,
 * and single-transfer DMA operations.
 
 \msc 
  "User Application", "DMA Interface", "Device Driver";
  "User Application"=>"DMA Interface"[label="DMARead()"];
  "DMA Interface"=>"Device Driver" [label = "DMA READ IOCtl"];
  "DMA Interface"<<"Device Driver" [label = "return"];
  "DMA Interface">>"User Application" [label = "return"];
 ...;
  "User Application"=>"DMA Interface"[label="DMAWrite()"];
  "DMA Interface"=>"Device Driver" [label = "DMA Write IOCtl"];
  "DMA Interface"<<"Device Driver" [label = "return"];
  "DMA Interface">>"User Application" [label = "return"];
 \endmsc
 */
class IOmnitek_DMA_Interface
{
  public:
  
    /// Create a DMA Interface for a specific device
    /// \param [in] dev path to device as string - this will usually be one of the \e /dev/netviz/dma/mdmaX devices or the \e /dev/netviz/dma/video device
    /// \return null if the specified device is not a DMA device.
    virtual IOmnitek_DMA_Interface(char * dev)    = 0;

    ///Shutdown and delete the DMA Interface
    /// \return
    virtual ~IOmnitek_DMA_Interface()             = 0;
    
    /// Read via DMA (single blocking transfer)
    /// \details this call will block until the read is completed.  If the timeout is reached the transfer will be cancelled and the call will return an error.
    /// \param [in] buffer pointer to transfer buffer - Must be 4 byte aligned for 32 bit DMA, 8 byte aligned for 64 bit DMA
    /// \param [in] size of transfer - for MDMA channels this can be a number of bytes, up to 4GiB.  For FDMA channels the number
    /// of bytes must correspond to the FIFO width - e.g. a multiple of 4 bytes for a 32 bit channel.  Maximum transfer size is 4GiB.
    /// \param [in] deviceaddr local address (for MDMA Channels) to start reading from
    /// \param [in] timeout_ms for transfer
    /// \return number of bytes read or negative error code
    virtual int DMARead(u8  * buffer,
                        u32 size,
                        u32 deviceaddr = 0,
                        int timeout_ms = 5000
                        ) = 0 ;
    
    /// Write via DMA (single blocking transfer)
    /// \param [in] buffer pointer to transfer buffer - Must be 4 byte aligned for 32 bit DMA, 8 byte aligned for 64 bit DMA
    /// \param [in] size of transfer - for MDMA channels this can be a number of bytes, up to 4GiB.  For FDMA channels the number
    /// of bytes must correspond to the FIFO width - e.g. a multiple of 4 bytes for a 32 bit channel.  Maximum transfer size is 4GiB.
    /// \param [in] deviceaddr local address (for MDMA Channels) to start writing to
    /// \param [in] timeout_ms for transfer
    /// \return number of bytes written or negative error code
    virtual int DMAWrite(u8  * buffer,
                        u32 size,
                        u32 deviceaddr = 0,
                        int timeout_ms = 5000                        
                        ) = 0 ;
    
};

/// \class IOmnitek_FDMA_Write_interface
/// \brief This class provides access to the vectorized DMA interface
/*! \details This provides a flexible DMA implementation capable of handling four types of IO:
 *    -   Synchronous (blocking) single transfers
 *    -   Asynchronous (non-blocking) single transfers
 *    -   Synchronous (blocking) vector transfers
 *    -   Asynchronous (non-blocking) vector transfers
 *  Vector transfers refers to transfers that are composed of multiple transfers to separate buffers.
 * Vectored IO submissions will be chained together, so a number of buffers will be processed automatically by the DMA engine
 * The driver will maintain a list of outstanding requests.  Requests will have a timeout associated with them, which will cause the 
 * request to return (for blocking) or to be completed (for non-blocking) with an error.  If an interface is closed all requests for that
 * interface will be deleted.
 * \note  This is not intended for production but rather to provde example code and documentation for the vector capable FDMA channels.
 * Access to these functions should be performed through standard Linux system calls (read, readv, and the io_submit/io_getevents systems)
 \msc
 "User application","FDMA Write Interface","Driver Sync IO Layer","Driver Async IO Layer";
 "FDMA Write Interface"=>"Driver Sync IO Layer" [label = "open device"];
 "FDMA Write Interface"<="Driver Sync IO Layer" [label = "return"];
 --- [label = "Single blocking Write"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersSync"];
 "FDMA Write Interface"=>"Driver Sync IO Layer" [label = "Request Sync DMA Transfer"];
 "Driver Sync IO Layer"=>"Driver Async IO Layer" [label = "Request Async DMA Transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer"[label = "start DMA transfer"];
 "Driver Async IO Layer"->"Driver Sync IO Layer" [label = "submitted"];
 "Driver Sync IO Layer"=>"Driver Sync IO Layer" [label = "Wait for completion"];
 ...;
 "Driver Sync IO Layer"<-"Driver Async IO Layer" [label = "Transfer Complete"];
 "User application"<="Driver Sync IO Layer" [label = "return bytes_transferred"];
 ...;
 \endmsc
 \msc
 "User application","FDMA Write Interface","Driver Sync IO Layer","Driver Async IO Layer";
 ...;
 --- [label = "Single Non-blocking Write"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersAsync()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Request Async DMA transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "start DMA transfer"];
 "Driver Async IO Layer"->"User application" [label = "submitted"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWaitAsyncRequest()"];
 ...;
 "FDMA Write Interface"<-"Driver Async IO Layer" [label = "Transfer Complete"];
 "User application"<="FDMA Write Interface" [label = "return bytes_transferred"];
...;
 \endmsc
 \msc
 "User application","FDMA Write Interface","Driver Sync IO Layer","Driver Async IO Layer";
 ...;
  --- [label = "Multiple Non-blocking Write"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersAsync()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Request Async DMA transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "start DMA transfer"];
 "Driver Async IO Layer"->"User application" [label = "submitted"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersAsync()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Request Async DMA transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "queue DMA transfer"];
 "Driver Async IO Layer"->"User application" [label = "submitted"]; 
 "User application"=>"FDMA Write Interface" [label = "FDMAWaitAsyncRequest()"];
 ...;
 "FDMA Write Interface"<-"Driver Async IO Layer" [label = "Transfer Complete"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "Start next request in queue"];
 "User application"<="FDMA Write Interface" [label = "return bytes_transferred"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWaitAsyncRequest()"];
 "FDMA Write Interface"<-"Driver Async IO Layer" [label = "Transfer Complete"];
 "User application"<="FDMA Write Interface" [label = "return bytes_transferred"];
 ...;
 \endmsc
 \msc
 "User application","FDMA Write Interface","Driver Sync IO Layer","Driver Async IO Layer";
 ...;
 --- [label = "Multiple Non-blocking Write - Cancellation example"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersAsync()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Request Async DMA transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "start DMA transfer"];
 "Driver Async IO Layer"=>"User application" [label = "submitted"];
 "User application"=>"FDMA Write Interface" [label = "FDMAWriteBuffersAsync()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Request Async DMA transfer"];
 "Driver Async IO Layer"=>"Driver Async IO Layer" [label = "start DMA transfer"];
 "Driver Async IO Layer"=>"User application" [label = "submitted"];
 "User application"=>"FDMA Write Interface"[label = "FDMAWaitAsyncCancel()"];
 "User application"=>"FDMA Write Interface"[label = "FDMAWaitAsyncCancel()"];
 "FDMA Write Interface"=>"Driver Async IO Layer" [label = "Cancel Requests"];
 "FDMA Write Interface"=>"FDMA Write Interface" [label = "Wait for request completions"];
 ...;
 "FDMA Write Interface"<-"Driver Async IO Layer" [label = "Transfer Complete"];
 "FDMA Write Interface"<-"Driver Async IO Layer" [label = "Transfer Cancelled"];
 "FDMA Write Interface"=>"Driver Sync IO Layer" [label = "Close interface"];
 "FDMA Write Interface"<="Driver Sync IO Layer" [label = "return"];
 "FDMA Write Interface"=>"User application" [label = "return"];
 
\endmsc
 
 */
class IOmnitek_FDMA_Write_interface
{
    ///Create a FDMA Interface for a specific device
    /// param [in] path to device as string - this will usually be one of the debug ( \e /dev/netviz/dma/debug or \e debug_tx)
    /// or the ethernet transmit(\e /dev/netviz/dma/net_tx)  devices.
    /// \return null if the specified device is not a FDMA device.
    virtual IOmnitek_FDMA_Write_Interface(char * dev)    = 0;

    ///Destructor - will cancel any outstanding requests and ensure that they complete
    virtual ~IOmnitek_FDMA_Write_Interface()             = 0;
    
    // ///Check that this is a read/write FDMA
    // /// \return true if this is a write FDMA channel
    // virtual bool VFDMAIsWriteChannel() = 0;
    
    // ///Perform a read operation, this call will block until either the read is completed or the timeout is reached.  If this request is 
    // /// placed in a queue it will not return until all previous entries, and this entry, have been completed
    // /// \param [in] transfer element details pointer
    // /// \param [in] timeout_ms (optional) timeout in ms- default to 1000
    // /// \return number of bytes transferred on success or negative error value
    // virtual int VFDMAReadBlocking(
                                  // struct VFDMA_Request * transfer, 
                                  // int timeout_ms = 1000) = 0;
    
    ///Perform a write operation, this call will block until either the write is completed or the timeout is reached.  If this request is 
    /// placed in a queue it will not return until all previous entries, and this entry, have been completed
    /// \param [in] transfer element details pointer
    /// \param [in] timeout_ms (optional) timeout in ms- default to 1000
    /// \return number of bytes transferred on success or negative error value
    virtual int FDMAWriteBuffersSync(
                                   struct FDMA_Request * transfer, 
                                   int timeout_ms = 1000) = 0;
                                   
    // ///Perform a read operation, this call will return immediately
    // /// \param [in] transfer element details pointer
    // /// \return 0 on queueing success or negative error value
    // virtual int VFDMAReadNonBlocking(
                                  // struct VFDMA_Request * transfer) = 0;
    
    ///Perform a write operation, this call will return immediately
    /// \param [in] transfer element details pointer
    /// \return 0 on queueing success or negative error value
    virtual int FDMAWriteBuffersAsync(struct FDMA_Request * transfer) = 0;
                                      
    ///Wait for a non-blocking transfer to complete
    /// \param [in] transfer request that was submitted with a non-blocking call
    /// \param [in] timeout_ms (optional) timeout in ms- default to 1000
    /// \return number of bytes transmitted or negative error value
    virtual int FDMAWaitAsyncRequest(struct FDMA_Request * transfer, 
                                 int timeout_ms = 1000) = 0;
    
    ///Attempt to cancel a submitted request.  Requests that have already been 
    /// \param [in] transfer request to cancel
    /// \return 0 if the request was cancelled or negative error code.
    virtual int FDMAAsyncCancel(struct FDMA_Request * transfer) = 0;    

    /// Attempt to cancel all outstanding requests on this channel
    /// \param [in] timeout_ms max time to wait for cancel
    /// \return 0 if succesful, negative error code otherwise
    virtual int FDMAAsyncCancelAll(int timeout_ms = 1000) = 0;
};
    
 
/// \class IOmnitek_FDMA_Read_Interface
/// \brief This class provides the FDMA read interface
/*!
 * \details The read channel requires a buffer to be provided that is used as a ring buffer.  This buffer
 * is broken up into multiple slices.  Read requests will return a pointer into this ring buffer and an amount
 * of data that has been written to that buffer.  If the read request wraps past the end of the buffer the
 * returned structure will contain two pointers, representing the portions of data at the end of the buffer
 * and at the start respectively
 * Any previous buffer is automatically returned to the driver when the next transfer is requested, so the data 
 * is only guaranteed not to change
 * during the time between the buffer request call returning and the next buffer request call being made.
 *
 * \image latex "Event FDMA - normal operation.jpg" "FDMA Read Operation" width=\textwidth
 *
 \msc
 "User Application", "FDMA Read Interface", "Device Driver";
  "User Application"=>"FDMA Read Interface"[label = "FDMASetRingBuffer()"];
  "FDMA Read Interface"=>"Device Driver"[label = "Configure Buffers"];
  "FDMA Read Interface"=>"Device Driver"[label = "Start DMA Channel"];
  ...;
  --- [label = "Main Loop"];
  "User Application"=>"FDMA Read Interface" [label = "FDMARequestNextTransfer()"];
  --- [label = "If Previous buffer has been requested"];
  "FDMA Read Interface"=>"Device Driver"[label = "Release Previous Buffer"];
  "Device Driver"=>"Device Driver"[label = "Re-queue slice if possible"];
  --- [label = "endif"];
  "FDMA Read Interface"=>"Device Driver" [label = "Request next transfer"];
  ...;
  "User Application"<="Device Driver"[label = "return transfer size"];
  ---[label = "End Loop"];
  "User Application"=>"FDMA Read Interface"[label = "FDMARemoveRingBuffer()"];
  "FDMA Read Interface"=>"Device Driver"[label = "Stop DMA Channel"];
  "FDMA Read Interface"=>"Device Driver"[label = "Remove Ring Buffer"]; 
 \endmsc
 * FDMA Read Call Sequence
 */
class IOmnitek_FDMA_Read_Interface : public IOmnitek_DMA_Interface
{
  public:
    ///Create a FDMA channel object
    ///Create a FDMA Interface for a specific device
    /// param [in] path to device as string - this will be the \e /dev/netviz/dma/net_rx device
    /// \return null if the specified device is not an event FDMA Read device.
    virtual IOmnitek_FDMA_Read_Interface(char * dev)       = 0;

    virtual ~IOmnitek_FDMA_Read_Interface()                = 0;
     
    ///Specify the ring buffer
    /// \param [in] buffer pointer to buffer - Must be 4 byte aligned for 32 bit DMA, 8 byte aligned for 64 bit DMA
    /// \param [in] size of buffer - Must be multiple of 4 bytes
    /// \param [in] numslices number of slices
    /// \return size of buffer if succesful, error code if not.
    virtual int FDMASetRingBuffer(u32 * buffer,
                                   u32 size,
                                   u8 numslices = 8
                                  )   = 0;
 
    /*!
             * Will stop DMA, cause any pending requests to be cancelled, the ring buffer pages unlocked and the 
             *  ringbuffer references in the kernel to be deleted
             * \return 0 on success or negative error code on failure.
             */
    virtual int FDMARemoveRingBuffer()  = 0;
 
    ///Get the next request from the buffer (blocking)
    ///The resulting buffer must be processed in a short enough timespan to prevent the 
    /// ringbuffer overflowing.  If the FDMARemoveRingBuffer() function is called and outstanding call will
    /// be cancelled and return with an error.  The returned buffer will be valid until a subsequent call to RequestNextTransfer is made
    /// at which point the buffer may be re-used by the driver.
    /// \param [in] request pointer to fdma_request struct that will contain details of the returned buffers
    /// \param [in] timeout_ms number of milliseconds to wait for a request
    /// \return  0 on success, negative error code on failure
    virtual int FDMARequestNextTransfer(struct FDMA_Request * request,
                                        int timeout_ms = 5000) = 0;
};

/// \class IOmnitek_PQA_Interface
/// \brief This class supports the PQA functionality built in to the decoder.
/// \details The PQA module provides the sum of the error squared for each pixel in the frame, compared to a reference test pattern.
/// this allows measurements of the overall quality of the images transferred to be made.  PQA Data is generated for each frame, and will be 
/// stored in a circular buffer by the driver.  The buffer will contain 128 entries (each entry is 32 bytes, giving 32*128 = 4096 bytes), which is just
/// over 2 seconds worth of data at 60 fps.  When the PQAGetData function is called the driver will return the data points received during the specified
/// interval since the last data point returned to the user.  It is possible for the buffer to overflow if the data is not requested fast enough.
/// \msc
/// "User Application", "PQA Interface", "Device Driver";
/// "User Application"=>"PQA Interface" [label="PQAGetData()"];
/// "PQA Interface"=>"Device Driver" [label="request data"];
/// "Device Driver"=>"Device Driver" [label="wait for time period to expire"];
/// "PQA Interface"<="Device Driver" [label="return data"];
/// "User Application"<="PQA Interface" [label="return"];
/// \endmsc


class IOmnitek_PQA_Interface
{
  public:
    /// \brief Create an interface to the PQA device
    /// \param [in] dev the device to use (usually /dev/netviz/pqa)
    virtual IOmnitek_PQA_Interface(char * dev) = 0;
    
    virtual ~IOmnitek_PQA_Interface() = 0;
   
    ///\brief Get data from driver
    ///\details will block until the interval specified by PQASetInterval has expired
    /// \param [in] data struct that contains the results from the driver.
    /// \param [in] interval in ms that the returned data will cover.  If zero all data that has not been previously
    /// transferred will be returned.
    /// \return 0 on success, or negative error code
    virtual int PQAGetData(struct PQAData * data, int interval) = 0;
    
    ///\brief Load the line pattern using the data in the supplied buffer.
    /// \details This will load one of the pattern memories with data from the specified buffer. The data buffer will
    /// contain 2048 entries, each entry having 30 bits of data (10 bits each YUV/RGB).  If the video standard has a
    /// width of less than 2048 pixels the excess at the end will not be displayed (e.g. the pattern must be scaled to 
    /// the line length of the current video standard)
    /// \param [in] bank number (1 - 3) to load
    /// \param [in] data buffer with pixel data.
    /// \return 0 on success, negative error code otherwise
    virtual int PQALoadPattern(int bank, u32 * data) = 0 ;
    
    /// \brief Load a predefined pattern
    /// \details This will preload one of several pre-determined line patterns.
    /// \param [in] bank number
    /// \param [in] linewidth in pixels
    /// \param [in] preset line pattern number (patterns to be defined).
    /// \return 0 on success, negative error code otherwise
    virtual int PQALoadPattern(int bank, int linewidth, int preset) = 0;
    
    /// \brief Load a logo
    /// \details This will load a logo into the memory and set it's size
    /// \param [in] width of the logo
    /// \param [in] height of the logo
    /// \param [in] data buffer containing logo data
    /// \return 0 on success, negative error code otherwise
    virtual int PQALoadLogo(int width, int height, u32 * data) = 0;
};