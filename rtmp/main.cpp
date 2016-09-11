#include <cstdio>

#include "rtmp.hpp"

void loop()
{
    char c;
    while((c = getchar()) != 0)
    {
        switch(c) {
        case 'c':
            break;
        case 'd':
            break;
        case '\n':
            break;
        case 'q':
            return;
        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    RRtmpCli* client = RRtmp::CreateCli();
    client->Connect();
    loop();
    client->Disconnect();
    delete client;
    return 0;
}
