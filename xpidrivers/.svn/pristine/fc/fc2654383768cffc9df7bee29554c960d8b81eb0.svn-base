/* DMA Interface class
 * Provides an abstraction layer for the synchronous and asynchronous DMA so application code doesn't have to worry about
   iocbs etc.

 */
#include <stdlib.h>
#include <libaio.h>
#include <list>
#include <linux/types.h>

#ifndef DMA_INTERFACE_H
#define DMA_INTERFACE_H



typedef struct _DMAXfer
{

  unsigned char    *     buffer;
  size_t        size;
  bool          write;
  off_t         offset;


  struct _AsyncXFer               /* < Async Transfer Information */
  {
    struct io_context * io_ctx;
    struct iocb         iocb;
  } AsyncXfer;
   
} DMAXfer;

struct XferData
{
  unsigned char * buffer;
  size_t          size;
  
  size_t          field_size;

  bool            interlaced;

  bool            rx_initialized;
  bool            tx_initialized;
  bool            disk_initialized;


  DMAXfer  rx_frame_xfer; 

  DMAXfer  tx_field1_xfer;
  DMAXfer  tx_field2_xfer;
  int      tx_count;

  DMAXfer  disk_frame_xfer;
};

struct IDMAChannel;

struct IDMAInterface
{
  public:
    virtual ~IDMAInterface()                                                            {};
    virtual IDMAChannel * newDmaChannel(int num, bool fdma, bool write)                 = 0;  
    virtual int Instance()                                                              = 0;
    virtual __u32      ReadRegister(__u32 Bar, __u32 offset)                                 = 0;
    virtual void          WriteRegister(__u32 Bar, __u32 offset,__u32 value)                 = 0;

};

struct IDMAChannel
{
  public:
    virtual ~IDMAChannel()                                                              {};

    virtual ssize_t   MemoryXFer(unsigned char * buffer, off_t offset, size_t size, bool write)                              = 0;
  
    virtual int       MemoryXFerSchedule(unsigned char * buffer, off_t offset, size_t size, bool write, DMAXfer * pXfer)     = 0;
    virtual int       MemoryXFerSchedule(DMAXfer * pXfer)                                             = 0;

    virtual ssize_t   MemoryXFerWait(DMAXfer * pXfer)                                                 = 0;
    virtual ssize_t   MemoryXFerWait(DMAXfer ** ppXfers, int nXfers)                                  = 0;

    virtual int   Start()                                                                             = 0;
    virtual int   Stop()                                                                              = 0;

    virtual bool          isFdma()                                                                    = 0;
    virtual int           getChannelNumber()                                                          = 0;
    virtual bool          SetAsync(bool async)                                                        = 0;
    virtual bool          isValid()                                                                   = 0;


};


#ifdef DMA_INTERFACE_EXPORTS

struct CDMAChannel;

struct CDMAInterface : public IDMAInterface
{
  
  public:
    CDMAInterface(int instance);
    ~CDMAInterface();
    virtual               IDMAChannel * newDmaChannel(int num, bool fdma, bool write);
    virtual int           Instance()  {return m_instance; };
    int                   OpenChannel(int num, bool fdma, bool write);
    struct io_context *   GetIOContext();

    virtual __u32         ReadRegister(__u32 Bar, __u32 offset);
    virtual void          WriteRegister(__u32 Bar, __u32 offset,__u32 value);


  private:
    int   m_instance;
#ifndef NO_UDEV
    struct udev * udev;
#endif
    void                                 PrintDevices();
    std::list<IDMAChannel *>             m_channel_list;
    struct io_context * m_p_io_ctx;    
    void                                 openBar(); //TODO better approach to this - nowhere near as good as it sounds...

    int   m_bar_fd[2];


};

struct CDMAChannel : public IDMAChannel
{
  public:

    CDMAChannel(int num, bool fdma, bool write, CDMAInterface * pDMAInterface);
    ~CDMAChannel();

    virtual ssize_t   MemoryXFer(unsigned char * buffer, off_t offset, size_t size, bool write);
  
    virtual int       MemoryXFerSchedule(unsigned char * buffer, off_t offset, size_t size, bool write, DMAXfer * pXfer);
    virtual int       MemoryXFerSchedule(DMAXfer * pXfer);

    virtual ssize_t   MemoryXFerWait(DMAXfer * pXfer);
    virtual ssize_t   MemoryXFerWait(DMAXfer ** ppXfers, int nXfers);

    virtual int   Start();
    virtual int   Stop();

    virtual bool          isFdma()                                                      {return m_fdma_channel;}
    virtual int           getChannelNumber()                                            {return m_num_channel;}
    virtual bool          SetAsync(bool async);
    virtual bool          isValid()                                                     {return m_valid;}

  private:
    bool          m_fdma_channel;
    int           m_num_channel;
    int           m_fd_channel;
    bool          m_write;
    bool          m_read;
    bool          m_valid;

    CDMAInterface *   m_pDMAInterface;


};
#endif

IDMAInterface *   newDMAInterface(int instance);
void              deleteDMAInterface(IDMAInterface * pDMAInterface);

#endif
