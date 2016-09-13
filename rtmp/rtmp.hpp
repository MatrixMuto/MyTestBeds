#ifndef RRTMP_RTMP_H_
#define RRTMP_RTMP_H_ 

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
class Message
{
public:
    static Message SetChunkSize(int size);
    static Message Connect(std::string);
    static Message WindowAcknowledgementSize(uint32_t);
    static Message CreateStream();
    static Message Play();
    static Message SetBufferLength();
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
    bool completed(){
        return body2_.size() == length_;
    }
public:
    uint16_t csid_;
    uint32_t timestamp_;
    uint32_t length_;
    uint8_t  type_;
    uint32_t stream_id_;
    uint8_t* body_;
    std::vector<uint8_t> body2_;
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
    Channel(int csid)
       : csid_(csid), chunk_size_(128)
    {}
    void Send(tcp::socket&, int, const Message&);
    void ReadOneChunk(tcp::socket&);
    /*
    int GetTxMaxChunkSize() {
        return tx_max_chunk_size_;
    }
    void SetTxMaxChunkSize(int size) {
       tx_max_chunk_size_ = size; 
    }
    int GetRxMaxChunkSize() {
        return rx_max_chunk_size_;
    }
    void SetRxMaxChunkSize(int size) {
       rx_max_chunk_size_ = size; 
    }
    */
private:
    int csid_;
    int chunk_size_;
    uint8_t fmt;
    uint32_t last_timestamp_;
};
class Packet
{
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
    void Read();
private:
    void tcp_connect(std::string);
    void handshake();
    void command_connect();
    void create_stream();
    void release_stream();
    void play();
    void read_one_chunk();
    void deal_message();
    /*
    Channel get_peer_channel(int chunk_stream_id) {
        for (auto channel : peer_channel_) {
            if (channel.id() == chunk_stream_id)
                return channel;
        }
        return 
    }
    */
private:
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    Channel cmd_channel_;
    Message prev_message;
    //Channel peer_channel_[10];
    int recv_max_chunk_size = 128;
    int tx_max_chunk_size_;
    int rx_max_chunk_size_;
    std::vector<Packet> packet_;
    std::array<uint8_t,8192> data_;
    bool wait_for_result_;
};

class RRtmp
{
public:
    static RRtmp* Create();
    static RRtmpCli* CreateCli();
};

#endif
