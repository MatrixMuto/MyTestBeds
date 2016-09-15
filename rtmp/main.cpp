#include <cstdio>
#include <iostream>

#include <boost/regex.hpp>

#include "rtmp.hpp"


using namespace rrtmp;

static RRtmpCli* client;
static char *url = "not spec";

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
    while((c = getchar()) != 0)
    {
        switch(c) {
        case 'c':
		{
    		client->Connect(url);
            break;
		}
        case 'd':
		{
    		client->Disconnect();
            break;
		}
        case '\n':
            break;
        case 'p':
        {
            int i = 100;
            Message msg;
            while (i--) {
                client->Read(msg);
				switch (msg.getType()) {
					case TypeId::VIDEO:
					case TypeId::AUDIO:
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
    if ( get_options(argc,argv) )
        return -1;
    std::cout << url << std::endl;
    client = RRtmp::CreateCli();
    loop();
    delete client;
    return 0;
}
