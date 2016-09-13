#include <sys/types.h>
#include <sys/socket.h>

#include <boost/asio.hpp>

#include <iostream>

#include "amf0.h"
#include "bytestream.hpp"
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
    : io_service_(),
      socket_(io_service_),
      cmd_channel_(2),
      tx_max_chunk_size_(128),
      rx_max_chunk_size_(128)
{
}

void RRtmpCli::Connect(std::string ip)
{
    try {
        tcp_connect(ip);
        
        handshake();
       
        command_connect();

        create_stream();

        play();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void RRtmpCli::Connect()
{
    Connect("122.228.237.24");
    //Connect("127.0.0.1");
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
void RRtmpCli::Read()
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
void RRtmpCli::read_one_chunk()
{
    using boost::asio::buffer;
	//size_t len = socket_.read_some(buffer(data_));
    uint8_t data[4096];
    boost::asio::read(socket_, buffer(data,1));
	//std::cout << "readed:" << len << std::endl;
	uint8_t *p = data,*end = data_.end();

    int type;
    int size;
    int fmt = *p >> 6;
    /* TODO: Complete Chunk Stream ID format */
    int csid = *p & 0x3F;
    std::cout << "fmt=" << fmt << ",csid:" << csid << std::endl;
    p++;
    if (prev_message.completed()) {
        prev_message.body2_.clear();
    }

    if (fmt == 0) {
        boost::asio::read(socket_, buffer(p,11));
        ByteStream bs(p,11);
        bs.get_be24();
        size = bs.get_be24();
        type = bs.get_byte();
        bs.get_be32();
        prev_message.csid_ = csid;
        prev_message.length_ = size;
        prev_message.type_ = type;
        p += 11;
    }
    else if(fmt ==1) {
        boost::asio::read(socket_, buffer(p,7));
	    ByteStream bs(p,7);
    	bs.get_be24();
        size = bs.get_be24();
        type = bs.get_byte();
        prev_message.csid_ = csid;
        prev_message.length_ = size;
        prev_message.type_ = type;
        p += 7;	
    }
    else if (fmt == 2) {
        boost::asio::read(socket_, buffer(p,3));
        p += 3;
    }
    else if (fmt == 3) {
        size = prev_message.length_ - prev_message.body2_.size();
        type = prev_message.type_;
    }
    std::cout << "type:" << type << ",size:" << size << std::endl;
    int parsed = size > rx_max_chunk_size_ ? rx_max_chunk_size_ : size;
    boost::asio::read(socket_, buffer(p,parsed));
    
     for(int i=0; i<parsed; ++i)
        prev_message.body2_.push_back(p[i]);
}

void RRtmpCli::deal_message()
{
    int type;
    if (!prev_message.completed()) {
        return ;
    }
    type = prev_message.type_;
    uint8_t* p = prev_message.body_;
    ByteStream bs(p,4);
    std::vector<uint8_t>& body = prev_message.body2_;
    switch (type) {
        case 0x01: /* Set Chunk Size */
            //rx_max_chunk_size_ = bs.get_be32();
            rx_max_chunk_size_ = 4096; 
            std::cout << "rx:set chunk size to " << rx_max_chunk_size_ 
                << std::endl; 
            break;
		case 0x04: /* User Control Message */
			break;
		case 0x05: /* Window Ack Size */
			break;
		case 0x06: /* Set Peer Bandwidth */
			break;
		case 0x14: /* AMF0 Message */
        {
            amf0_data *data = amf0_data_buffer_read(body.data(),body.size());
            std::cout << "amf0 size " << amf0_data_size(data) << std::endl;
            if ( AMF0_TYPE_STRING == amf0_data_get_type(data)) {
                size_t len = amf0_string_get_size(data);
                uint8_t *method = amf0_string_get_bytes(data);
                std::string key;
                for(int i=0;i<len;++i)
                {
                   key += static_cast<char>(method[i]); 
                }
                std::cout << amf0_string_get_bytes(data) << std::endl;
                if (key == "_result") {
                    wait_for_result_ = false;
                } 
            }
			break;
        }
		default:
			break;
    }
}
void RRtmpCli::command_connect()
{
    using boost::asio::buffer;
    /* ------ command message (connect) ------> */
    
    // socket_.send();
    Message message = Control::SetChunkSize(4096);
    cmd_channel_.Send(socket_, tx_max_chunk_size_, message);
    tx_max_chunk_size_ = 4096; 
   
    message = Command::Connect("live");
    cmd_channel_.Send(socket_, tx_max_chunk_size_, message);

    wait_for_result_ = true;
    while(wait_for_result_) 
    {
        read_one_chunk();
        deal_message();
    }
}

void RRtmpCli::create_stream()
{
    Message msg = Command::CreateStream();
    cmd_channel_.Send(socket_, tx_max_chunk_size_, msg);

    wait_for_result_ = true;
    while(wait_for_result_){
        read_one_chunk();
        deal_message();
    }
}

void RRtmpCli::release_stream()
{
}
void RRtmpCli::play()
{
    Message msg = Command::Play();
    cmd_channel_.Send(socket_, tx_max_chunk_size_, msg);
    wait_for_result_ = true;
    while(wait_for_result_){
        read_one_chunk();
        deal_message();
    }
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
/* static */
Message Message::CreateStream()
{
    Message msg(4096);
    amf0_data *data;
    data = amf0_data_new(AMF0_TYPE_STRING);
    data = amf0_str("createStream");
    std::cout << amf0_string_get_size(data) << std::endl;  
    
    size_t len = amf0_data_buffer_write(data, msg.body_, 4096);
    std::cout << len << std::endl;

    data = amf0_number_new(2);
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;
    data = amf0_null_new(); 
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;
    
    msg.csid_ = 3;
    msg.timestamp_ = 0;
    msg.type_ = 20;  /* AMF 0 */
    msg.stream_id_ = 0;
    msg.length_ = len;
    return msg;
}
Message Message::Play()
{
    Message msg(4096);
    amf0_data *data;
    
    data = amf0_str("play");
    //std::cout << amf0_string_get_size(data) << std::endl;  
    size_t len = amf0_data_buffer_write(data, msg.body_, 4096);
    std::cout << len << std::endl;

    data = amf0_number_new(3);
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;
    
    data = amf0_null_new(); 
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;

    data = amf0_str("hks");
    //std::cout << amf0_string_get_size(data) << std::endl;  
    len =len+ amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;
   
    data = amf0_number_new(-2000);
    len = len + amf0_data_buffer_write(data, msg.body_+len, 4096);
    std::cout << len << std::endl;

    msg.csid_ = 8;
    msg.timestamp_ = 0;
    msg.type_ = 20;  /* AMF 0 */
    msg.stream_id_ = 1;
    msg.length_ = len;
    return msg;
}   

void Channel::Send(tcp::socket& socket, int max_chunk_size, const Message& msg)
{
    /* basic header */
    /* message header, type is 0, 1, 2, or 3 */
    /* extended timestam */
    /* chunk data */
    uint8_t buf[128];

    int size = msg.length_;
    uint8_t* p = msg.body_;
    bool first = true;
    while (size > 0) {
        ByteStream bs(buf,128);
        if (first) {
            bs.put_byte(0x3F & msg.csid_);     /* basic header */
            bs.put_be24(msg.timestamp_);
            bs.put_be24(msg.length_);
            bs.put_byte(msg.type_);
            std::cout << "stream_id_:" << msg.stream_id_ << std::endl;
            bs.put_le32(msg.stream_id_);
            first = false;
        }
        else {
            bs.put_byte(0xC0 | msg.csid_);
        }
        socket.send(boost::asio::buffer(bs.buf(),bs.size()));
        int to_send = size > max_chunk_size ? max_chunk_size: size;
            socket.send(boost::asio::buffer(p,to_send));
        size -= to_send;
        p += to_send;
    }
}
