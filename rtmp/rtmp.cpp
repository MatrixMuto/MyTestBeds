#include <sys/types.h>
#include <sys/socket.h>

#include <boost/asio.hpp>

#include "rtmp.hpp"
#include <iostream>

using boost::asio::ip::tcp;

RRtmp* RRtmp::Create()
{
    return new RRtmp;
}

RRtmpCli* RRtmp::CreateCli()
{
    return new RRtmpCli();
}

RRtmpCli::RRtmpCli()
    : io_service_(),socket_(io_service_)
{
}

void RRtmpCli::Connect(char *ip)
{
    try {
        tcp_connect(ip);
        
        handshake();
       
        command_connect();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void RRtmpCli::Connect()
{
    Connect("122.228.237.24");
}
void RRtmpCli::Disconnect()
{
}

void RRtmpCli::Play()
{

}

void RRtmpCli::Publish()
{
}

void RRtmpCli::tcp_connect(char* ip)
{
    boost::asio::ip::address addr;
    addr = boost::asio::ip::address::from_string(ip);
    tcp::endpoint serv(addr, 1935);

    socket_.connect(serv);
}     

void RRtmpCli::handshake()
{
    char c0[1] = {0x3};
    char c1[1536] = {0x0};
        
    socket_.send(boost::asio::buffer(c0));
    socket_.send(boost::asio::buffer(c1));

    char buf[3073];
    boost::system::error_code error;
    size_t len = socket_.read_some(boost::asio::buffer(buf),error);
        
    /* send S2 */
    socket_.send(boost::asio::buffer(&buf[1],1536));
}

void RRtmpCli::command_connect()
{
    /* ------ command message (connect) ------> */
    
   // socket_.send();
   Message message;
   chunking_.Send(socket_, message);


}

void RRtmpCli::create_stream()
{
}

void RRtmpCli::release_stream()
{
}

Control::Control(int type, int para)
{
    type_ = type;
    para_ = para;
}


Chunking::Chunking()
{
    chunk_size_ = 128;
}

void Chunking::Send(tcp::socket& socket, Message& msg)
{
    char set_chunk_size[] = { 
        0x2, 
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x4,
        0x1,
        0x0,0x0,0x0,0x0,
        0x0,0x0,0x10,0x0 };
    socket.send(boost::asio::buffer(set_chunk_size));
    /* basic header */
    /* message header 0,1,2,3 */
    /* extended timestam */
    /* chunk data */

    /*
    int size = msg.size();
    while (size > 0) {
        if (size >= chunk_size_) {
            socket.send();
            size -= chunk_size_;
        }
        else {
            size = 0;
        }
    }
    */
}
