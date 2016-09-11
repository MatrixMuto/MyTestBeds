#ifndef RRTMP_RTMP_H_
#define RRTMP_RTMP_H_ 

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class Message
{
private:
	char header;
	char body;
};

class Control : public Message
{
public:
    Control(int type, int para);
private:
    int type_;
    int para_;
};

class Command : public Message
{
};

class Chunking
{
public:
    Chunking();
    void Send(tcp::socket&, Message&);
private:
    int chunk_size_;
    char fmt[3];
};

class RRtmpCli
{
public:
    RRtmpCli(const RRtmpCli&) = delete;
    RRtmpCli& operator=(const RRtmpCli&) = delete;
    RRtmpCli();
    void Connect();
    void Connect(char *ip);
    void Disconnect();
    
    void Play();

    void Publish();

private:
    void tcp_connect(char* ip);
    void handshake();
    void command_connect();
    void create_stream();
    void release_stream();

private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    Chunking chunking_;
};

class RRtmp
{
public:
    static RRtmp* Create();
    static RRtmpCli* CreateCli();
};

#endif
