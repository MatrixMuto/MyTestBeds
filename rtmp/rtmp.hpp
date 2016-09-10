#ifndef RRTMP_RTMP_H_
#define RRTMP_RTMP_H_ 

#include <boost/asio.hpp>

class RRtmpCli
{
public:
    RRtmpCli() = default;
    void Connect();
    void Connect(char *ip);
    void Disconnect();
    
    void Play();

    void Publish();

private:
    void Handshake();


private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket;
};

class RRtmp
{
public:
    static RRtmp* Create();
    static RRtmpCli* CreateCli();
};

#endif
