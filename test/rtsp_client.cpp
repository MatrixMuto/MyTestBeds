#include <iostream>
#include <cstring>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>

static const char *dstIps[] = {
        "218.204.223.237"
};
static const short ports[] = {
        554
};

using boost::asio::ip::tcp;

class MuRTSssssClient {

};

void *rtsp_client(void *para) {
    boost::asio::io_service io_service;
    tcp::socket s(io_service);
    tcp::resolver resolver(io_service);
    return NULL;
}

void *rtsp_client_2(void *para) {
    std::cout << "thread in" << std::endl;
    // Address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, dstIps[0], &serv_addr.sin_addr);
    serv_addr.sin_port = htons(ports[0]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int err;
    err = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (err < 0) {
        std::cout << "connect error." << std::endl;
    } else {
        char buf[255] = {0};
        size_t len = sizeof(buf);

        send(sockfd, "xxxxxx\r\n\r\n\r\n", 13, 0);
        recv(sockfd, buf, len, 0);

        std::cout << buf << std::endl;
    }

    shutdown(sockfd, SHUT_RDWR);

    std::cout << "thread out" << std::endl;
    return NULL;
}

pthread_t thread;

int create_thread(void *(*func)(void *)) {
    pthread_create(&thread, NULL, func, NULL);
    return 0;
}

void join_thread() {
    pthread_join(thread, NULL);
}

int main(int argc, char *argv[]) {
    create_thread(rtsp_client);
    join_thread();
    return 0;
}
