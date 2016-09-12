#include <sys/types.h>
#include <sys/socket.h>

#include <boost/asio.hpp>

#include <iostream>

#include "amf0.h"

#include "rtmp.hpp"
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
    : io_service_(),socket_(io_service_),channel_(2)
{
}

void RRtmpCli::Connect(std::string ip)
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
    Message message = Message::Connect("live");
    chunking_.Send(socket_, message);
}

void RRtmpCli::Publish()
{
}

void RRtmpCli::tcp_connect(std::string ip)
{
    boost::asio::ip::address addr;
    addr = boost::asio::ip::address::from_string(ip);
    tcp::endpoint serv(addr, 1935);

    socket_.connect(serv);
}     

void RRtmpCli::handshake()
{
    using boost::asio::buffer;
    /* TODO: Handshake C1 */
    char c0[1] = {0x3};
    char c1[1536] = {0x0};
        
    socket_.send(boost::asio::buffer(c0));
    socket_.send(boost::asio::buffer(c1));

    char s0_s1[1537];
    char s2[1536];
    boost::system::error_code error;
    size_t len = boost::asio::read(socket_, buffer(s0_s1), error);
    len = boost::asio::read(socket_, buffer(s2), error);

    /* send C2, C2 must same with S1*/
    socket_.send(boost::asio::buffer(&s0_s1[1],1536));

}

void RRtmpCli::command_connect()
{
    using boost::asio::buffer;
    /* ------ command message (connect) ------> */
    
   // socket_.send();
    Message message = Control::SetChunkSize(4096);
    chunking_.Send(socket_, message);
    chunking_.chunk_size_ = 4096;
    message = Command::Connect("live");
    chunking_.Send(socket_, message);
    uint8_t msg[1024]; 
    boost::asio::read(socket_, buffer(msg));

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
/*static*/
Message Message::SetChunkSize(int size)
{
    Message msg(4);
    msg.csid_ = 2;
    msg.timestamp_ = 0;
    msg.length_ = 4;
    msg.type_ = 1;
    msg.stream_id_ = 0;
    
    ByteStream bs(msg.body_, 4);
    bs.put_be32(size);

    return msg;
}

/*static*/
/*
Message Message::Connect(std::string app)
{
    Message msg(4096);


    return msg;
}
*/
/*static*/
Message Message::Connect(std::string app)
{
    Message msg(4096);
    amf0_data *data;
    data = amf0_data_new(AMF0_TYPE_STRING);
    data = amf0_str("connect");
    std::cout << amf0_string_get_size(data) << std::endl;  
    
    size_t len = amf0_data_buffer_write(data, msg.body_, 4096);
    std::cout << len << std::endl;

    data = amf0_number_new(1);
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;

    data = amf0_object_new();
    amf0_object_add(data, "app", amf0_str("live"));
    amf0_object_add(data, "type", amf0_str("nonprivate"));
    amf0_object_add(data, "flashVer", amf0_str("FMLE/3.0 (compatible; obs-studio/0.14.2; FMSc/1.0)"));
    amf0_object_add(data, "swfUrl", amf0_str("rtmp://172.17.196.3/live"));
    amf0_object_add(data, "tcUrl", amf0_str("rtmp://172.17.196.3/live"));
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;
    msg.csid_ = 3;
    msg.timestamp_ = 0;
    msg.type_ = 20;  /* AMF 0 */
    msg.stream_id_ = 0;
    msg.length_ = len;
    return msg;
}
Channel::Channel()
{
    chunk_size_ = 128;
}

void Channel::Send(tcp::socket& socket, const Message& msg)
{
    uint8_t buf[128];
    ByteStream bs(buf,128);
    bs.put_byte(msg.csid_);     /* basic header */
    
    bs.put_be24(msg.timestamp_);
    bs.put_be24(msg.length_);
    bs.put_byte(msg.type_);
    bs.put_be32(msg.stream_id_);

    /* basic header */
    /* message header, type is 0, 1, 2, or 3 */
    /* extended timestam */
    /* chunk data */

    int size = msg.length_;
    uint8_t* p = msg.body_;

    while (size > chunk_size_) {
        socket.send(boost::asio::buffer(bs.buf(),bs.size()));
        socket.send(boost::asio::buffer(p,chunk_size_));
        size -= chunk_size_;
        p += chunk_size_;
    }
    if (size > 0) {
        socket.send(boost::asio::buffer(bs.buf(),bs.size()));
        socket.send(boost::asio::buffer(p,size));
    }
}
