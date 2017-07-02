#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>    //for inet_ntop

inline void debug_dump_addr(char *desc, struct sockaddr_in *addr) {
	int port;
	char buf[255];
	const char *ip;

	ip = inet_ntop(addr->sin_family, &addr->sin_addr, buf, 255);
	port = ntohs(addr->sin_port);

	printf("%s: ip:%s port:%d\n", desc, ip, port);
}

inline void debug_dump_arg(int argc, char** argv) {
	int i;
	for (i = 0; i < argc; ++i) {
		printf("[%d] [%s]\n", i, argv[i]);
	}
}

int open_udp_socket(int port) {
	int socket_fd, res;
	struct sockaddr_in addr;

	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		perror("socket: socket_fd");
	}

	memset((void*) &addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	debug_dump_addr("server", &addr);

	res = bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr));
	if (res == -1) {
		perror("bind:");
	}

	return socket_fd;
}

void test_udp_client(char *ip, int port, char *cip, int port2) {
	int sock, addr_from_len, len;
	int err;
	struct sockaddr_in addr, addr_from, b_addr;

	addr_from_len = sizeof(addr_from);

	sock = open_udp_socket(port);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10012);
	err = inet_pton(AF_INET, ip, &addr.sin_addr);
//	err = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	memset((void*) &b_addr, 0, sizeof(b_addr));
	b_addr.sin_family = AF_INET;
	b_addr.sin_port = htons(port2);
	err = inet_pton(AF_INET, cip, &b_addr.sin_addr);
//	err = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	debug_dump_addr("dst", &addr);

	char buf[255] = { 0 };
	for (;;) {
		buf[0] = getchar();
		err = sendto(sock, buf, 1, 0, (const struct sockaddr *) &addr,
				sizeof(addr));
		err = sendto(sock, buf, 1, 0, (const struct sockaddr *) &b_addr,
				sizeof(addr));
		len = recvfrom(sock, buf, 255, 0, (struct sockaddr*) &addr_from,
				&addr_from_len);
		printf("%s  <-----", buf);
		util_dump_addr("from", &addr_from);
	}
}

int main(int argc, char* argv[]) {
	debug_dump_arg(argc, argv);
	if (argc < 4) {
		printf("Invalid arg\n");
		return 0;
	}
	test_udp_client(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
	return 0;
}
