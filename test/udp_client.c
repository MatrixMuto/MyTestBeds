#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <pthread.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

void test_1() {
    char *data;
    int sockfd;
    sem_t lock;
    struct timespec abstime;

    sem_init(&lock, 0, 0);

    data = (char *) malloc(1000);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in srv_addr;
    size_t cli_add_len;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_addr.s_addr = inet_addr("47.92.27.198");
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    srv_addr.sin_port = htons(9958);


    clock_gettime(CLOCK_REALTIME, &abstime);

    for (int i = 1; i <= 120; i++) {
        printf("%2d ", i);
        if (i % 10 == 0) printf("\n");
        for (int j = 0; j < 100; j++) {
            struct msghdr msg;
            struct iovec iovec1;
            iovec1.iov_base = data;
            iovec1.iov_len = 1000;

            msg.msg_name = &srv_addr;
            msg.msg_namelen = sizeof(srv_addr);

            msg.msg_iov = &iovec1;
            msg.msg_iovlen = 1;


            ssize_t sended = sendmsg(sockfd, &msg, 0);
            if (sended < 0) {
                printf("sendmsg errno %d\n", errno);
            }
        }
        //sendto(sockfd, data, 1000, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));

        //pthread_cond_wait();
        abstime.tv_sec += 1;
        sem_timedwait(&lock, &abstime);
//        sem_wait(&lock);
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {
    test_1();
    return 0;
}
