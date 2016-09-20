#include <cstdio>
#include <iostream>

#include <boost/regex.hpp>
#include "bytestream.hpp"
#include "rtmp.hpp"

using namespace rrtmp;

static RRtmpCli* client;
static std::string url = "rtmp://live.hkstv.hk.lxdns.com/live/hks";

static const char flvHeader[] = { 'F', 'L', 'V', 0x01,
								0x5,
								0, 0, 0, 9,
                                0, 0, 0, 0};
static void dump_to_flv(int n)
{
	FILE *file;
	file = fopen("dump.flv","a+");
	if (file == NULL)
	{
		return;
	}
    int i = n;
    Message msg;
    while (i--) {
		client->Read(msg);
		switch (msg.getType()) {
			case TypeId::VIDEO:
			{
				uint8_t buf[11];
				buf[0] = 0x9;
				ByteStream bs(buf,11);
				bs.put_byte(0x9);
				bs.put_be24(msg.body_.size());
				bs.put_be24(msg.timestamp_);
				bs.put_byte(0);
				bs.put_be32(0);
				
				fwrite(buf,1,sizeof(buf),file);
				fwrite(msg.body_.data(),1, msg.body_.size(),file);
				ByteStream tagSize(buf,4);
				tagSize.put_be32(11 + msg.body_.size());
				fwrite(buf,1,4,file);
				break;
			}
			case TypeId::AUDIO:
			{
				uint8_t buf[11];
				buf[0] = 0x8;
				ByteStream bs(buf,11);
				bs.put_byte(0x8);
				bs.put_be24(msg.body_.size());
				bs.put_be24(msg.timestamp_);
				bs.put_byte(0);
				bs.put_be32(0);
				
				fwrite(buf,1,sizeof(buf),file);
				fwrite(msg.body_.data(),1, msg.body_.size(),file);

				ByteStream tagSize(buf,4);
				tagSize.put_be32(11 + msg.body_.size());
				fwrite(buf,1,4,file);

				break;
			}
			case TypeId::DATA_AMF0:
			{
				break;
			}
			default:
			{
				std::cout << "main: did not handle type: "
					<< msg.getType() << std::endl;
				break;
			}
		}
	}
	fclose(file);
}

static int get_options(int argc, char* argv[])
{
    char* p;
    int i;
    for (i = 1; i < argc; ++i) {
        p = argv[i];
        if (*p++ != '-') {
            return -1;
        }
        
        while (*p) {
            switch (*p++) {
            case 't':
                if (*p) {
                }
                else {
                    url = argv[++i];
                }
                goto next;
                break;
            default:
                break;
            }
        }
    next: 
        continue;
    }

    return 0;
}

static void loop()
{
    char c;
    while(std::cin >> c)
    {
        switch(c) {
        case 'c':
		{
			FILE* file = fopen("dump.flv","w+");
			fwrite(flvHeader, 1, sizeof(flvHeader), file);
			fclose(file);
    		client = RRtmp::CreateCli();
    		client->Connect(url);
            break;
		}
        case 'd':
		{
    		client->Disconnect();
    		delete client;
            break;
		}
        case '\n':
            break;
        case 'p':
        {
			dump_to_flv(100);
			break;
        }
        case 'q':
            return;
        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{
	{
		Message a,b;
		a.body_.push_back(8);
		b = a;
	}
    if ( get_options(argc, argv) )
        return -1;
    std::cout << url << std::endl;

    loop();

    return 0;
}
