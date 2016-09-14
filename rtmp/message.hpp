#ifndef RRTMP_MESSAGE_H_
#define RRTMP_MESSAGE_H_

#include <vector>
#include <cstdint>

namespace rrtmp
{
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

    bool completed(){
        return is_completed_;
    }
    bool Check(){
        is_completed_ = length_ == body_.size();
        return is_completed_;
    }
    void clear() {
        body_.clear();
        is_completed_ = false;
    }
public:
    uint16_t csid_;
    uint32_t timestamp_;
    uint32_t length_;
    uint8_t  type_;
    uint32_t stream_id_;
    std::vector<uint8_t> body_;
    bool is_completed_;
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

}
#endif
