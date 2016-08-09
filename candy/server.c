#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>    //for inet_ntop
int open_udp_socket(int port)
{
	int socket_fd, res, err;
	struct sockaddr_in addr;

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		perror("socket: socket_fd");
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
//	err = inet_pton(AF_INET, "104.194.67.119", &addr.sin_addr);

	util_dump_addr("server", &addr);

	res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
	if (res == -1){
		perror("bind:");
	}

	return socket_fd;
}

void util_dump_addr(char *desc, struct sockaddr_in *addr)
{
	int af;
	char buf[255];
	char *ip;
	int port;
	af = addr->sin_family;

	ip = inet_ntop(af, &addr->sin_addr, buf, 255);
	port = ntohs(addr->sin_port);
	printf("%s: ip:%s port:%d\n",desc, ip, port);

}
void test_udp()
{
	int sock,sock2,len,addr_from_len;
	char buffer[4096];
	struct sockaddr_in addr_from;
	addr_from_len = sizeof(addr_from);
	sock = open_udp_socket(8890);
	sock2 = open_udp_socket(8900);
	for(;;) {
		len = recvfrom(sock, buffer, 4096, 0, &addr_from, &addr_from_len);
		printf("[%s ]<----",buffer);
		util_dump_addr("client", &addr_from);
		len = sendto(sock2, buffer, len, 0, &addr_from, addr_from_len);
	}
}

int main(int argc, char* argv[])
{
	printf("test_udp 2\n");
	test_udp();
	return 0;
}
