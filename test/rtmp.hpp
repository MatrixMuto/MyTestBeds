#ifndef RRTMP_RTMP_H_
#define RRTMP_RTMP_H_

#include <boost/asio.hpp>
#include "message.hpp"

using boost::asio::ip::tcp;

namespace rrtmp {

    class Channel {
        friend
        class RRtmpCli;

    public:
        Channel(
                int csid,
                int max_chunk_size)
                : csid_(csid),
                  max_chunk_size_(max_chunk_size) {}

        void Send(tcp::socket &, const
        Message &);

        void RecvChunk(tcp::socket &,
                       int fmt, Message
                       &);

    private:
        int csid_;
        int chunk_count_;
        int max_chunk_size_;
        Message prev_msg_;
        Message m;
        uint8_t ch_buf_[8192];
    };

    class RRtmpCli {
    public:
        RRtmpCli(
                const RRtmpCli
                &) = delete;

        RRtmpCli &operator=(const RRtmpCli
                            &) = delete;

        RRtmpCli();

        RRtmpCli(std::string
                 url);

        ~RRtmpCli() =
        default;

        void Connect();

        void Connect(std::string url);

        void Disconnect();

        void Play();

        void Publish();

        void Read(Message &);

    private:
        void tcp_connect(std::string);

        void handshake();

        void command_connect();

        void create_stream();

        void release_stream();

        void play();

        void read_one_chunk(Message &);

        void deal_message(Message &);

        Channel *get_rx_channel(int csid) {
            if (rx_channel_map_[csid] == nullptr) {
                rx_channel_map_[csid] = new
                        Channel(csid, rx_max_chunk_size_);
            }
            return rx_channel_map_[csid];
        }

    private:
        boost::asio::io_service io_service_;
        tcp::socket socket_;
        Channel cmd_channel_;
        std::map<int, Channel *> rx_channel_map_;
        std::map<int, Channel *> tx_channel_map_;
        int tx_max_chunk_size_ = 128;
        int rx_max_chunk_size_ = 128;
        std::array<uint8_t, 8192> data_;
        bool wait_for_result_;
        std::string host_;
        std::string app_;
        std::string key_;
    };

    class RRtmp {
    public:
        static RRtmp *Create();

        static RRtmpCli *CreateCli();
    };
} //end namespace rrtmp
#endif
