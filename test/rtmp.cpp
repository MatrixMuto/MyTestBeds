#include <iostream>
#include <boost/asio.hpp>
#include <boost/regex.hpp>

#include "amf0.h"
#include "bytestream.hpp"
#include "rtmp.hpp"

using boost::asio::ip::tcp;

namespace rrtmp {
    RRtmp *RRtmp::Create() {
        return new RRtmp;
    }

    RRtmpCli *RRtmp::CreateCli() {
        return new RRtmpCli();
    }

    RRtmpCli::RRtmpCli()
            : io_service_(),
              socket_(io_service_),
              cmd_channel_(2, 128),
              tx_max_chunk_size_(128),
              rx_max_chunk_size_(128) {
    }

    RRtmpCli::RRtmpCli(std::string url)
            : RRtmpCli() {
        boost::regex e("^rtmp://([^/]+)/([^/]+)/([^/]+)$");

        boost::match_results <std::string::const_iterator> what;

        std::cout << "regex: " << what.size() << std::endl;
        boost::smatch matches;
        if (boost::regex_match(url, matches, e)) {
            std::cout << matches[1] << std::endl;
        }


        //if (boost::regex_search( url, what, e)) {
        /*
        std::string base_url( what[1].first, what[1].second );
        std::cout << base_url << std::endl;
        */
        //}
    }

    void RRtmpCli::Connect(std::string url) {
        boost::regex e("^rtmp://([^/]+)/([^/]+)/([^/]+)$");

        boost::match_results <std::string::const_iterator> what;

        boost::smatch matches;
        if (boost::regex_match(url, matches, e)) {
            std::cout << matches[1] << std::endl;
            std::cout << matches[2] << std::endl;
            std::cout << matches[3] << std::endl;
            host_ = matches[1];
            app_ = matches[2];
            key_ = matches[3];
        } else {
            std::cerr << "Please check Url" << std::endl;
            return;
        }

        if (boost::regex_search(url, what, e)) {
            std::cout << "regex: " << what.size() << std::endl;
            std::string base_url(what[1].first, what[1].second);
            std::cout << base_url << std::endl;
        }
        try {
            tcp_connect(host_);

            handshake();

            command_connect();

            create_stream();

            play();
        }
        catch (std::exception &e) {
            std::cerr << "Exception:" << e.what() << std::endl;
        }
    }

    void RRtmpCli::Connect() {
    }

    void RRtmpCli::Disconnect() {
        for (auto pair: rx_channel_map_) {
            if (pair.second != nullptr) {
                std::cout << "free: " << pair.second->csid_ << std::endl;
                delete static_cast<Channel *>(pair.second);
            }
        }
    }

    void RRtmpCli::Play() {
    }

    void RRtmpCli::Publish() {
    }

    void RRtmpCli::Read(Message &msg) {
        msg.is_completed_ = false;
        for (;;) {
            read_one_chunk(msg);
            deal_message(msg);
            if (msg.completed()) {
                if (msg.type_ == TypeId::VIDEO) {
                    std::cout << "Video:" << msg.timestamp_ << std::endl;
                } else if (msg.type_ == TypeId::AUDIO) {
                    std::cout << "Audio:" << msg.timestamp_ << std::endl;
                } else if (msg.type_ == DATA_AMF0) {
                    std::cout << "Meta:" << msg.csid_ << std::endl;
                } else
                    continue;
                break;
            }
        }
    }

    void RRtmpCli::tcp_connect(std::string host) {
        boost::asio::ip::address addr;
        tcp::resolver resolver(io_service_);
        tcp::resolver::query query(host, "");
        for (tcp::resolver::iterator i = resolver.resolve(query);
             i != tcp::resolver::iterator();
             ++i) {
            boost::asio::ip::tcp::endpoint end = *i;
            //	addr = boost::asio::ip::address::from_string(ip);
            tcp::endpoint serv(end.address(), 1935);
            socket_.connect(serv);
            std::cout << end.address() << std::endl;
            break;
        }

    }

    void RRtmpCli::handshake() {
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
        socket_.send(boost::asio::buffer(&s0_s1[1], 1536));

    }

    void RRtmpCli::read_one_chunk(Message &msg) {
        using boost::asio::buffer;
        uint8_t data[1];
        boost::asio::read(socket_, buffer(data, 1));
        int fmt = data[0] >> 6;
        int csid = data[0] & 0x3F; /* TODO: Complete Chunk Stream ID format */

        Channel *ch = get_rx_channel(csid);
        ch->RecvChunk(socket_, fmt, msg);
    }

    void RRtmpCli::deal_message(Message &message) {
        int type;
        if (!message.completed()) {
            return;
        }
        type = message.type_;
        uint8_t *p = message.body_.data();
        std::vector <uint8_t> &body = message.body_;
        switch (type) {
            case 0x01: /* Set Chunk Size */
            {
                ByteStream bs(p, 4);
                rx_max_chunk_size_ = bs.get_be32();
                std::cout << "set rx chunk size to " << rx_max_chunk_size_
                          << std::endl;
                break;
            }
            case 0x04: /* User Control Message */
                break;
            case 0x05: /* Window Ack Size */
                break;
            case 0x06: /* Set Peer Bandwidth */
                break;
            case 0x14: /* AMF0 Message */
            {
                amf0_data *data = amf0_data_buffer_read(body.data(), body.size());
                std::cout << "amf0 size " << amf0_data_size(data) << std::endl;
                if (AMF0_TYPE_STRING == amf0_data_get_type(data)) {
                    size_t len = amf0_string_get_size(data);
                    uint8_t *method = amf0_string_get_bytes(data);
                    std::string key;
                    for (size_t i = 0; i < len; ++i) {
                        key += static_cast<char>(method[i]);
                    }
                    std::cout << amf0_string_get_bytes(data) << std::endl;
                    if (key == "_result") {
                        wait_for_result_ = false;
                    }
                }
                if (data) amf0_data_free(data);
                break;
            }
            case 0x8:
            case 0x9: {
                break;
            }
            default: {
                std::cout << "unhandled message type:" << type << std::endl;
                break;
            }
        }
    }

    void RRtmpCli::command_connect() {
        using boost::asio::buffer;

        Message message = Message::SetChunkSize(4096);
        cmd_channel_.Send(socket_, message);
        tx_max_chunk_size_ = 4096;
        cmd_channel_.max_chunk_size_ = 4096;

        /* ------ command message (connect) ------> */
        message = Message::Connect(app_, 1);
        cmd_channel_.Send(socket_, message);

        Message msg;
        wait_for_result_ = true;
        while (wait_for_result_) {
            read_one_chunk(msg);
            deal_message(msg);
        }
    }

    void RRtmpCli::create_stream() {
        Message msg = Message::CreateStream();
        cmd_channel_.Send(socket_, msg);

        wait_for_result_ = true;
        Message msg2;
        while (wait_for_result_) {
            read_one_chunk(msg2);
            deal_message(msg2);
        }
    }

    void RRtmpCli::release_stream() {
    }

    void RRtmpCli::play() {
        Message msg = Message::Play(key_);
        cmd_channel_.Send(socket_, msg);
    }

    void Channel::Send(tcp::socket &socket, const Message &msg) {
        /* basic header */
        /* message header, type is 0, 1, 2, or 3 */
        /* extended timestam */
        /* chunk data */
        uint8_t buf[128];

        int size = msg.length_;
        const uint8_t *p = msg.body_.data();
        bool first = true;
        while (size > 0) {
            ByteStream bs(buf, 128);
            if (first) {
                bs.put_byte(0x3F & msg.csid_);     /* basic header */
                bs.put_be24(msg.timestamp_);
                bs.put_be24(msg.length_);
                bs.put_byte(msg.type_);
                std::cout << "TX --->" << "stream_id_:" << msg.stream_id_ << std::endl;
                bs.put_le32(msg.stream_id_);
                first = false;
            } else {
                bs.put_byte(0xC0 | msg.csid_);
            }
            socket.send(boost::asio::buffer(bs.buf(), bs.size()));
            int to_send = size > max_chunk_size_ ? max_chunk_size_ : size;
            socket.send(boost::asio::buffer(p, to_send));
            size -= to_send;
            p += to_send;
        }
    }

    void Channel::RecvChunk(tcp::socket &socket, int fmt, Message &msg) {
        using boost::asio::buffer;
        uint8_t *p = ch_buf_;

        if (m.completed()) {
            m.clear();
        }

        m.csid_ = csid_;

        if (fmt == 0) {
            /*
             * MUST
             * the start of a chunk stream, and whenever the stream timestamp goes backward.
             */
            boost::asio::read(socket, buffer(p, 11));
            ByteStream bs(p, 11);
            m.timestamp_ = bs.get_be24();
            m.length_ = bs.get_be24();
            m.type_ = bs.get_byte();
            m.stream_id_ = bs.get_be32();
            p += 11;
        } else if (fmt == 1) {
            /*
             * SHOULD used for the first chunk of each new message after the first.
             */
            boost::asio::read(socket, buffer(p, 7));
            ByteStream bs(p, 7);
            m.timestamp_ = prev_msg_.timestamp_ + bs.get_be24();
            m.length_ = bs.get_be24();
            m.type_ = bs.get_byte();
            m.stream_id_ = prev_msg_.stream_id_;
            p += 7;
        } else if (fmt == 2) {
            boost::asio::read(socket, buffer(p, 3));
            ByteStream bs(p, 3);
            m.timestamp_ = bs.get_be24();
            m.length_ = prev_msg_.length_;
            m.stream_id_ = prev_msg_.stream_id_;
            m.type_ = prev_msg_.type_;
            p += 3;
        } else if (fmt == 3) {
            m.timestamp_ = prev_msg_.timestamp_;
            m.length_ = prev_msg_.length_;
            m.stream_id_ = prev_msg_.stream_id_;
            m.type_ = prev_msg_.type_;
        }
        std::cout << "RX <----" << "fmt:" << fmt << ", csid:" << csid_
                  << ", type:" << (int) m.type_ << ", size:" << m.length_
                  << std::endl;
        int size = m.length_ - m.body_.size();
        int to_read = size > max_chunk_size_ ? max_chunk_size_ : size;
        boost::asio::read(socket, buffer(p, to_read));

        m.body_.insert(m.body_.end(), p, p + to_read);
        if (m.Check()) {
            msg = m;
        }
        prev_msg_ = m; /* copy this message */
    }
}//end of namespace rrtmp
