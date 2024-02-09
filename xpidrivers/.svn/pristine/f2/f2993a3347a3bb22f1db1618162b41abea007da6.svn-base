/*! \file omnitek_dma.h
 * \brief Omnitek - Barco NetViz Interface Header
 *  This header file details various functions provided to support the driver
 * for the Barco Netviz project
 */

/*! \struct reg_request 
 \brief Used for reading/writing registers
 */
struct reg_request {
  /*! Register address */
  u32 address;    
  /*! Read/Write Data */
  u32 data;     
};

/*! \struct ringbuffer_desc 
  \brief Specifies the details of the ringbuffer for use with event FDMAs
 */
struct ringbuffer_desc {
  /*! Address of Buffer */
  u8 * address;   
  /*! Size of Buffer */
  u32  size;      
  /*! Number of slices (ignored for write EFDMA channels) */
  u8   nslices;   
};

#if 0

// /*! \struct efdma_segment
 // \brief Details of a segment of an EFDMA transfer
 // */
// struct efdma_segment {
  // /*! Address of Segment*/
  // u8 * address;   
  // /*! Size of Segment */
  // u32  size;  
// };

// /*! \struct efdma_request
 // \brief Details of buffers presented to or returned from the event DMA channels
 // */
// struct efdma_request {
  // /*! segment array (max 2) */
  // struct efdma_segment segment[2];  
  
  // /*! nsegments - Number of segments for this request */
  // u8 nsegments;                     
  
  // /*! sequence_number is a driver supplied number that indicates the order the buffers have been allocated, the buffers must be submitted for
       // * transmission in this order.
      // */
  // u32 sequence_number;
// };
 
// /*! DMATransfer struct
// \brief Details of an FDMA Transfer
// */
// struct DMATransfer {
  // /*! Buffer for data */
  // u8  * buffer;
  // /*! Size of Buffer */
  // u32 size;  
  // /*! true when this transfer is a write operation */
  // bool  write;
// };  
#endif

/*! \struct PQAData
 \brief PQA Data will be returned in this struct
 */
 struct PQAData {
  /*! Buffer for data */
  struct {
    u32   y_value_h;    /*! Value for Y channel  - high 32 bits*/
    u32   u_value_h;    /*! Value for U channel  - high 32 bits*/
    u32   v_value_h;    /*! Value for V channel  - high 32 bits*/
    u16   y_value_l;    /*! Value for Y channel  - low 16 bits*/
    u16   u_value_l;    /*! Value for U channel  - low 16 bits*/
    u16   v_value_l;    /*! Value for V channel  - low 16 bits*/
    //18 bytes
    u32   t_b_d         /*! to be defined  - PQA Status values - yellow boxes etc */
    //22bytes    
    u32   jiffies       /*! System time for this data point */    
    //26bytes
    u32   bit_error_count /*! Bit error count for this frame */
    //30 bytes
    u16   pad           /*! Pad to 32 bytes for data element */
    
  } * pqa_data;
  
  u32 nElements;            /*! Number of entries returned */
  bool discont;              /*! Will be set if the buffer overflowed */
  
};

/*! \struct FDMA_Request 
 \brief Details of a VFDMA Transfer
 */
struct FDMA_Request {
  /*! Buffers for data */
  u32 ** buffers;
  /*! Size of Buffers */
  u32 ** sizes;
  /*! Number of buffers in request */
  u32 nBuffers;
  // /*! true when this transfer is a write operation */
  // bool  write;
  /*! Driver IO information */
  struct iocb * iocb;  
};


/* IOCTL Definitions */
/*! 
\def OMNITEK_DMA_IOC_MAGIC
\brief IOCTL Magic Number - This is not used as of 2.6.31 kernel 
*/
#define OMNITEK_DMA_IOC_MAGIC 0xF5    

/*!  
 \def OMNITEK_DMA_IOCREADREGISTER
 \brief Read from Register  
 \details  Read a value from the register using the address specified in the supplied struct.  Value will be stored in the struct,
*/
#define OMNITEK_DMA_IOCREADREGISTER	    _IOR(OMNITEK_DMA_IOC_MAGIC,1,struct reg_request *)    
/*! 
 \def OMNITEK_DMA_IOCWRITEREGISTER
 \brief Write to register 
  \details  Write a value to a register using the information supplied in the struct.
*/
#define OMNITEK_DMA_IOCWRITEREGISTER	  _IOW(OMNITEK_DMA_IOC_MAGIC,2,struct reg_request *)    

/*! 
\def OMNITEK_DMA_IOCDMASTART
\brief Start DMA 
\details  For FDMA channels this will start the DMA engine processing queued requests
*/
#define OMNITEK_DMA_IOCDMASTART         _IOR(OMNITEK_DMA_IOC_MAGIC,3,int)                     
/*! 
\def OMNITEK_DMA_IOCDMASTOP
\brief Stop DMA 
\details  for FDMA channels this will stop the DMA engine.  Outstanding requests may be completed or cancelled.
*/
#define OMNITEK_DMA_IOCDMASTOP          _IOR(OMNITEK_DMA_IOC_MAGIC,4,int)                     
/*! 
\def OMNITEK_DMA_IOCDMAABORT
\brief Abort DMA (shouldn't be needed )
\details  this will cause an immediate shutdown of the DMA channel, leaving it in a potentially unstable state.  This should not be used in practice.
*/
#define OMNITEK_DMA_IOCDMAABORT         _IOR(OMNITEK_DMA_IOC_MAGIC,5,int)                     

/*! 
\def OMNITEK_DMA_IOCSETRINGBUFFER
\brief Set the ring buffer for a read event FDMA 
\details  The ringbuffer_desc struct passed in will be used to specify the ringbuffer for use with an event FDMA.  For write channels the slices part
is ignored.
*/
#define OMNITEK_DMA_IOCSETRINGBUFFER    _IOW(OMNITEK_DMA_IOC_MAGIC,6,struct ringbuffer_desc *)
/*! 
\def OMNITEK_DMA_IOCCLEARRINGBUFFER
\brief Clear the ringbuffer - return 0 on success
\details  Clear the exisiting ring buffer, can only be performed when the channel has stopped (no outstanding requests).
*/
#define OMNITEK_DMA_IOCCLEARRINGBUFFER  _IOR(OMNITEK_DMA_IOC_MAGIC,7,int)                     
/*! 
\def OMNITEK_DMA_IOCGETNEXTEVENT
\brief Request the next event from the event FDMA 
\details  Get the next event from the DMA Channel.  Will block if one is not available.  Result will be stored in the provided struct
*/
#define OMNITEK_DMA_IOCGETNEXTEVENT     _IOR(OMNITEK_DMA_IOC_MAGIC,8,struct fdma_request *)  
/*! 
\def OMNITEK_DMA_IOCSENDEVENT
\brief Send number of bytes via event FDMA 
\details  This will cause the driver to instruct the DMA engine to transfer the specified number of bytes from the ring buffer.
*/
#define OMNITEK_DMA_IOCSENDEVENT        _IOW(OMNITEK_DMA_IOC_MAGIC,9,int)


///\def ASYNC_RING_IOCTL_MAXNR
///\brief Largest IOCtl number used
#define ASYNC_RING_IOCTL_MAXNR 9
