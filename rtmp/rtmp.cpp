#include <sys/types.h>
#include <sys/socket.h>

#include <boost/asio.hpp>

#include "rtmp.hpp"
#include <iostream>

using boost::asio::ip::tcp;

RRtmp* RRtmp::Create()
{
    return new RRtmp;
}

RRtmpCli* RRtmp::CreateCli()
{
    return new RRtmpCli();
}

void RRtmpCli::Connect(char *ip)
{
    try {
        boost::asio::io_service io_service;

        boost::asio::ip::address addr;
        addr = boost::asio::ip::address::from_string(ip);
        tcp::endpoint serv(addr, 1935);

        tcp::socket socket(io_service);
        socket.connect(serv);
        /* Handshake */
        Handshake();

        for(;;)
        {
            boost::asio::buffer buf2();
            boost::asio::write(socket,buf);
            boost::array<char,128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf),error);
            if (error == boost::asio::error::eof)
                break;
            else
                throw boost::system::system_error(error);

            std::cout.write(buf.data(),len);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void RRtmpCli::Connect()
{
    Connect("122.228.237.24");
}
void RRtmpCli::Disconnect()
{
}

void RRtmpCli::Play()
{

}

void RRtmpCli::Publish()
{
}

