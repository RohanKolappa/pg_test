#ifndef __H_ADDRESS__
#define __H_ADDRESS__


#include <string>

/// Base class for all "address" types. We only have InternetAddress type
/// so far
class CAddress {

public:

    /// enumeration of different address types supported
    enum AddressType {
        InternetAddress = 1,
        UnixDomainAddress,
        FileAddress,
    };

    /// Constructor. Expects the type of address and the size
    CAddress(AddressType a_type, int a_size);

    /// Destructor
    virtual ~CAddress();

    /// Return the size of address. That is the size of the opaque object
    /// that GetAdress() would return
    int GetSize() const { return m_size; };

    /// Get the address "type" enumerated constant
    AddressType GetType() const { return m_type; };

    /// Get the string representation of the address. Virtual method
    /// that subclasses must define.
    virtual const std::string GetHost(void) const = 0;

    /// Get a pointer to the native operating system dependent
    /// address struct. Virtual method that subclasses must define.
    virtual void * GetAddress(void) const = 0;

protected:
    AddressType             m_type;
    int                     m_size;

};

/// Class representing the Internet Address type. Wrapper around the
/// struct sockaddr_in. Stores the IP address and port
class CInternetAddress : public CAddress {
public:

    /// Default constructor. Creates the INADDR_ANY IP address with port 0
    CInternetAddress();

    /// Constructor. Creates the INADDR_ANY IP address with given port.
    CInternetAddress(int a_portNumber);

    /// Constructor. Performs the necessary conversion from host name
    /// into IP address and associates it with the given port
    CInternetAddress(const char *a_hostName, int a_portNumber);

    /// Constructor. Performs the necessary conversion from host name
    /// into IP address and associates it with port 0
    CInternetAddress(const char *a_hostName);

    /// Constructor. Creates an address form the supplied sockaddr_in struct
    CInternetAddress(const sockaddr_in &a_inetAddr);

    /// Destructor.
    ~CInternetAddress();

    /// Setup the address to the INADDR_ANY IP address with given port.
    int SetAddress(int a_portNumber);

    /// Setup the address from the given hostname and port.
    int SetAddress(const char *a_hostName, int a_portNumber);

    /// Return a pointer to internal representation of the address (ie pointer to
    /// struct sockaddr_in
    void * GetAddress(void) const { return (void *) &m_addr; };

    /// Get the string representation of the address.
    const std::string GetHost(void) const;

    /// Retrieves the port of the address.
    int GetPort(void) const;

    /// Test if the address is zero. If default constructor is used
    /// and SetAddress() has not been invoked this will return true
    bool IsNull(void) const { return (m_addr.sin_addr.s_addr == 0); };

    /// Test if the address is a Multicast address
    bool IsMulticast(void) const;

    bool EqualsAddress(CInternetAddress & a_address) {
        if (m_addr.sin_addr.s_addr == a_address.m_addr.sin_addr.s_addr)
            return true;
        return false;

    };

private:
    struct sockaddr_in              m_addr;
    void SetupDefaultAddress(void);


};


#endif /* __H_ADDRESS__ */
