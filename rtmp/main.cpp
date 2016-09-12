#include <cstdio>

#include "rtmp.hpp"

RRtmpCli* client;

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
        case 'p':
            client->Play();
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
    client = RRtmp::CreateCli();
    client->Connect();
    loop();
    client->Disconnect();
    delete client;
    return 0;
}
