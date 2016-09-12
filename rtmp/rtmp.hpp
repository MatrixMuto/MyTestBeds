#ifndef RRTMP_RTMP_H_
#define RRTMP_RTMP_H_ 

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
struct ByteStream
{
    ByteStream(uint8_t* buf, size_t size) {
        begin = buf;
        end   = begin + size;
        cur   = begin;
    }

    uint32_t get_be32() {
		uint32_t val = *cur++ << 24 | *cur++ << 16 | *cur++ << 8 | *cur++;
		return val;
	}
	uint32_t get_be24() {
		return *cur++ << 16 | *cur++ << 8 | *cur++;
	}
	uint8_t get_byte() {
		return *cur++;
	}
    void put_be32(uint32_t val) {
       *cur++ = val >> 24;
       *cur++ = val >> 16;
       *cur++ = val >> 8;
       *cur++ = val;
    }

    void put_be24(uint32_t val) {
        *cur++ = val >> 16;
        *cur++ = val >> 8;
        *cur++ = val;
    }

    void put_be16(uint16_t val) {
        *cur++ = val >> 8;
        *cur++ = val;
    }

    void put_byte(uint8_t val) {
        *cur++ = val;
    }
    
    void put_buffer(uint8_t* val, size_t size)
    {
        memcpy(cur, val, size);
        cur += size;
    }

    inline size_t size() { 
        return cur - begin;
    };

    inline uint8_t* buf() {
        return begin;
    }
private:
    uint8_t* begin;
    uint8_t* end;
    uint8_t* cur;
};

class Message
{
public:
    static Message SetChunkSize(int size);
    static Message Connect(std::string);
    Message() =default;
    Message(uint32_t length)
       :length_(length) 
    {
        body_ = (uint8_t*) malloc(length_);
    }
    Message(uint16_t csid, uint32_t timestamp, uint32_t size)
       :csid_(csid) 
    {

    }
public:
    uint16_t csid_;
    uint32_t timestamp_;
    uint32_t length_;
    uint8_t  type_;
    uint32_t stream_id_;
    uint8_t* body_;
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

class Channel 
{
public:
    Channel(int csid) :csid_(csid),chunk_size_(128) {}
    void Send(tcp::socket&, const Message&);

    void SetChunkSize(int size)
    {
        chunk_size_ = size;
    }
public:
    int csid_;
    int chunk_size_;
    uint8_t fmt;
    uint32_t last_timestamp_;
};

class RRtmpCli
{
public:
    RRtmpCli(const RRtmpCli&) = delete;
    RRtmpCli& operator=(const RRtmpCli&) = delete;
    RRtmpCli();
    void Connect();
    void Connect(std::string ip);
    void Disconnect();
    
    void Play();

    void Publish();

private:
    void tcp_connect(std::string);
    void handshake();
    void command_connect();
    void create_stream();
    void release_stream();

private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    Channel channel_;
};

class RRtmp
{
public:
    static RRtmp* Create();
    static RRtmpCli* CreateCli();
};

#endif
