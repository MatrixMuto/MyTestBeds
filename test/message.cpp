#include <iostream>


#include "amf0.h"
#include "bytestream.hpp"
#include "message.hpp"

namespace rrtmp 
{ 
/*static*/
Message Message::SetChunkSize(int size)
{
    Message msg;
    msg.csid_ = 2;
    msg.timestamp_ = 0;
    msg.length_ = 4;
    msg.type_ = SET_CHUNK_SIZE;
    msg.stream_id_ = 0;
    
    uint8_t data[4];
    ByteStream bs(data, 4);
    bs.put_be32(size);
    for (int i=0; i<4; i++)
        msg.body_.push_back(data[i]);
    return msg;
}

/*static*/
Message Message::Connect(std::string app, int rpc_number)
{
    uint8_t buf[4096];
    size_t len;
    Message msg;
    amf0_data *data;

	msg.write_string("connect");

    data = amf0_number_new(rpc_number);
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
	amf0_data_free(data);
	
    data = amf0_object_new();
    amf0_object_add(data, "app", amf0_str( app.c_str() ));
    amf0_object_add(data, "type", amf0_str("nonprivate"));
    amf0_object_add(data, "flashVer", amf0_str("FMLE/3.0 (compatible; obs-studio/0.14.2; FMSc/1.0)"));
    amf0_object_add(data, "tcUrl", amf0_str("rtmp://live.hkstv.hk.lxdns.com:1935/live"));
    amf0_object_add(data, "capabilities", amf0_number_new(15));
    amf0_object_add(data, "audioCodecs", amf0_number_new(4071));
    amf0_object_add(data, "videoCodecs", amf0_number_new(252));
    amf0_object_add(data, "videoFunction", amf0_number_new(1));
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
	amf0_data_free(data);

    msg.csid_ = 3;
    msg.type_ = COMMAND_AMF0;  /* Command AMF0 */
    msg.stream_id_ = 0;
    msg.length_ = msg.body_.size();
    return msg;
}

/* static */
Message Message::CreateStream()
{
    uint8_t buf[4096];
    size_t len;
    Message msg;
    amf0_data *data;
	
	msg.write_string("createStream");

    data = amf0_number_new(2);
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
    amf0_data_free(data);

    data = amf0_null_new(); 
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
    amf0_data_free(data);

    msg.csid_ = 3;
    msg.timestamp_ = 0;
    msg.type_ = COMMAND_AMF0;  /* Command AMF0 */
    msg.stream_id_ = 0;
    msg.length_ = msg.body_.size();
    return msg;
}
Message Message::Play(std::string key)
{
    uint8_t buf[4096];
    size_t len;
    Message msg;
    amf0_data *data;
    
	msg.write_string("play");

    data = amf0_number_new(3);
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
    amf0_data_free(data);
    
    data = amf0_null_new(); 
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
    amf0_data_free(data);

	msg.write_string(key);
   
    data = amf0_number_new(-2000);
    len = amf0_data_buffer_write(data, buf, 4096);
    msg.body_.insert(msg.body_.end(), buf, buf+len);
    amf0_data_free(data);

    msg.csid_ = 8;
    msg.timestamp_ = 0;
    msg.type_= COMMAND_AMF0;  /* AMF 0 */
    msg.stream_id_ = 1;
    msg.length_ = msg.body_.size();
    return msg;
}   

size_t Message::write_string(std::string str)
{
	uint8_t buf[4096];
	size_t len;
	amf0_data *data = amf0_str(str.c_str());
    len = amf0_data_buffer_write(data, buf, 4096);
    body_.insert(body_.end(), buf, buf+len);
    amf0_data_free(data);

	return len;
}

}
