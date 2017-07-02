#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int setnonblocking(int fd) {
    int fdflags;

    if ((fdflags = fcntl(fd, F_GETFL, 0)) == -1)
        return -1;
    fdflags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, fdflags) == -1)
        return -1;
    return 0;
}

int open_listen_socket() {
    int socket_fd, res;
    struct sockaddr_in addr;

    socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1) {
        perror("socket: socket_fd");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);

    res = bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr));
    if (res == -1) {
        perror("bind:");
    }

    res = listen(socket_fd, 255);
    if (res == -1) {
        perror("listen:");
    }

    return socket_fd;
}

int accept_connect(int listen_sock) {
    int addr_len, conn_sock;
    struct sockaddr_in local;
    addr_len = sizeof(local);
    bzero(&local, addr_len);

    conn_sock = accept(listen_sock, (struct sockaddr *) &local, &addr_len);
    if (conn_sock == -1) {
        perror("accept: conn_sock");
    }
    return conn_sock;
}

int epoll_loop() {
#define MAX_EVENTS 10
    int epollfd, nfds, n, listen_sock, conn_sock;
    struct epoll_event ev, events[MAX_EVENTS];

    epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd == -1) {
        return -1;
    }

    listen_sock = open_listen_socket();
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        //exit(EXIT_FAILURE);
    }

    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                conn_sock = accept_connect(listen_sock);
                if (setnonblocking(conn_sock) < 0) {
                    perror("setnonblocking");
                }
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {

                }
            } else {
                int fd = events[n].data.fd;
                char buf[255];
                int len = recv(fd, buf, 255, MSG_DONTWAIT);
                buf[len] = '\0';
                printf("%s\n", buf);
            }
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    epoll_loop();
    return 0;
}
