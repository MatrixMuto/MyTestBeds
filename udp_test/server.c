#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <pthread.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

void test_1()
{
    char *data;
    int sockfd;
    sem_t lock;
    struct timespec abstime;

    sem_init(&lock, 0, 0);

    data = (char*) malloc(1000);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in cli_addr;
    size_t cli_add_len;
//    bzero(&srv_addr,sizeof(srv_addr));
//    srv_addr.sin_addr.s_addr = inet_addr("47.92.27.198");
//    srv_addr.sin_port = htons(9958);

    for (;;) {
        recvfrom(sockfd, data, sizeof(data), 0, &cli_addr, &cli_add_len);
    }

    clock_gettime(CLOCK_REALTIME, &abstime);

    for (int i=0; i < 60; i++) {
        printf("%d \n", i);

        for (int j=0; j< 50; j++)
            sendto(sockfd, data, 1000, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));

        //pthread_cond_wait();
        abstime.tv_sec += 1;
        sem_timedwait(&lock, &abstime);
//        sem_wait(&lock);
    }




    close(sockfd);
}

int main(int argc, char* argv[])
{
    test_1();
    return 0;
}
