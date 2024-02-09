/*
 * strmplayer.h
 *
 *  Created on: Jul 5, 2010
 *      Author: rkale
 */

#ifndef STREAMINGSERVER_HH_
#define STRMEAINGSERVER_HH_

#include <string>

class IStreamingServer {

public:
    virtual ~IStreamingServer() {};

    virtual bool Start() = 0;
    virtual bool IsRunning() = 0;
    virtual void SetRequestPort(int port) = 0;
    virtual void SetNotifyPort(int port) = 0;
    virtual void SetHTTPPort(int port) = 0;
    virtual void SetRequestFile(const std::string & a_fileName) = 0;
    virtual void SetRecoveryFile(const std::string & a_fileName) = 0;
    virtual bool ExecuteCommand(const std::string & a_command) = 0;
};

class CStreamingServer : public IStreamingServer {

public:
    CStreamingServer(bool bLog=true);
    ~CStreamingServer();

    bool Start();
    bool IsRunning();
    void SetRequestPort(int port);
    void SetNotifyPort(int port);
    void SetHTTPPort(int port);
    void SetRequestFile(const std::string & a_fileName);
    void SetRecoveryFile(const std::string & a_fileName);

    bool ExecuteCommand(const std::string & a_command);

    // Stop Gap method to retrieve SDP file from a URL and parse it
    // This helper function can be used to setup a RTP stream playback till we
    // have a real implementation in StreamingServer
    static bool RetrieveSDPFile(const std::string & a_sdpURL, const std::string & a_savePath,
            int & a_videoPort, int & a_audioPort, std::string & a_ipAddress);

private:
    struct CStreamingServerImpl;
    CStreamingServerImpl * m_pimpl;
};

#endif /* STREAMINGSERVER_HH_ */
