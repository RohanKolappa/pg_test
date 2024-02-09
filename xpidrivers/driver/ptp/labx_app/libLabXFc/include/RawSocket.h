#ifndef __RAW_SOCKET_H__
#define __RAW_SOCKET_H__

// System headers
#include <string.h>
#include <stdint.h>
#include <vector>

// Library headers
#include "Thread.h"
#include "Semaphore.h"
#include "MacAddress.h"

// Locate within the "LabX" namespace
namespace LabX {

  class RawSocketObserver;

  /**
   * Wrapper class for raw sockets.  This encapsulates details 
   * of socket and packet handling and provides an observer interface
   * for received packets.
   *
   * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
   * @author Chris Pane
   */
  class RawSocket : public Thread
  {
  public:
    /**
     * Packet class for incoming and outgoing data buffering.
     *
     * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
     * @author Chris Pane
     */
    class Packet
    {
      // Constructor / Destructor
    public:
      /**
       * Create a new packet attached to a socket.
       * @param socket - the attached socket
       */
      Packet(RawSocket &socket, uint16_t etherType, const MacAddress &ourMacAddress);

      /**
       * Destroy a packet
       */
      ~Packet(void);

      /**
       * Increate the reference count.
       */
      void AddRef(void);

      /**
       * Decrease the reference count. The packet will be placed
       * back in the free pool for the attached socket when the
       * reference count goes to zero.
       */
      void Release(void);

      /**
       * Clear packet contents and reset all pointers and ref counts in
       * preparation for placing in the free pool.
       */
      void Clear(void);

      static const size_t kMaxPacketLength=1500;
      static const int kDataStart=14;
      static const int kDstMacAddress=0;
      static const int kSrcMacAddress=6;

      size_t size(void) { return m_dptr; }
      size_t dataSize(void) { return m_dptr-kDataStart; }
      size_t freeSpace(void) { return Packet::kMaxPacketLength-m_dptr; }
      uint8_t* data(void) { return &m_buf[kDataStart]; }
      uint8_t* dstMacAddress(void) { return &m_buf[kDstMacAddress]; }
      uint8_t* srcMacAddress(void) { return &m_buf[kSrcMacAddress]; }
      uint8_t* bufStart(void) { return &m_buf[0]; };
      int dptr(void) { return m_dptr;}
      void rewind(int dptr) { m_dptr = dptr;}
      bool WriteBuffer(const uint8_t *data, size_t size);

      /**
       * Print debug information for the packet.
       */
      void print(void);

      /**
       * Print the packet data
       */
      void printData(void);

    private:
      /**
       * Current data index (into m_buf)
       */
      int        m_dptr;

      /**
       * Current reference count.
       */
      int        m_refCount;

      /**
       * Packet data buffer.
       */
      uint8_t   *m_buf;

      /**
       * Socket class that owns this packet.
       */
      RawSocket &m_socket;
    };
 
  public:
    static const int defPoolSize=10;

    /**
     * Attach an observer for received packets.
     * @param o - observer to attach
     */
    virtual void Attach(RawSocketObserver* o);

    /**
     * Detach an observer for received packets.
     * @param o - observer to detach
     */
    virtual void Detach(RawSocketObserver* o);

    /**
     * Notify all attached observers that a packet was received.
     * @param p - received packet
     */
    virtual void Notify(Packet* p);

    // Constructor / Destructor
  public:
    /**
     * Create a new RawSocket. Init must be called before the socket
     * is ready for use.
     */
    RawSocket(void);

    /**
     * Destroy a RawSocket.
     */
    ~RawSocket(void);

  public:
    /**
     * Initialize the socket. This opens the socket and creates the packet pool.
     * @param etherType     - ethertype for packets on this socket
     * @param poolSize      - number of packets in the pool
     * @param interfaceName - interface to send packets on
     */
    void Init(uint16_t etherType, int poolSize, const std::string& interfaceName);

    /**
     * Subscribe to an ethernet level multicast address. This only affects
     * local filtering of multicast traffic.
     * @param mac - MAC address to allow received packets on
     */
    bool SubscribeMcast(const MacAddress& mac);

    /**
     * Send a packet.
     * @param p - the packet to send
     * @param toMacAddress - the destination MAC to send the packet to
     * @return - true if the packet was sent successfully
     */
    bool Send(RawSocket::Packet *p, const MacAddress &toMacAddress);

    /**
     * Mark a packet as free an put it back in the pool.
     * @param p - Packet to free
     */
    void FreePacket(Packet *p);

    /**
     * Get a packet from the free pool.
     * @return - new packet from the pool.
     */
    Packet * GetPacket(void);

    /**
     * Packet receive thread main function
     * @param The return status for the thread
     */
    virtual int32_t run(void);

    /**
     * Get the source MAC address for the interface this socket is attached to.
     * @return - our source mac
     */
    const MacAddress& GetMacAddress(void) const { return m_ourMacAddress; }

  private:
    static const int kMaxIfNamesize = 256;

    /**
     * File descriptor for this socket.
     */
    int m_socketfd;

    /**
     * Ethertype of this socket.
     */
    uint16_t m_etherType;

    /**
     * The source MAC address of this socket.
     */
    MacAddress m_ourMacAddress;

    /**
     * Pool of free packet objects.
     */
    std::vector<Packet *> m_freePool;

    /**
     * Attached observers for incoming packets.
     */
    std::vector<RawSocketObserver*> m_observers;

    /**
     * Interface name that this socket is bound to.
     */
    std::string m_interfaceName;

    /**
     * The receive thread state
     */
    bool m_threadRunning;

    /**
     * Semaphore protecting accecss to the socket.
     */
    Semaphore m_socketSem;

    /**
     * Semaphore protecting access to the packet pool.
     */
    Semaphore m_poolSem;		
  };

  /**
   * Packet observer interface class. Observers must derive from this class.
   *
   * Copyright (c) 2010 Lab X Technologies.  All rights reserved.
   * @author Chris Pane
   */
  class RawSocketObserver
  {
  public: 
    /**
     * Callback for received packets.
     * Make sure to call p->AddRef() if you want to keep the packet past the callback.
     *
     * @param p - Packet received.
     */
    virtual void PacketReceived(RawSocket::Packet *p) = 0;

    /**
     * Constructor and virtual destructor
     */
    RawSocketObserver(void) {};
    virtual ~RawSocketObserver(void) {};
  };

} // Namespace LabX

#endif // __RAW_SOCKET_H__

