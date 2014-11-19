#ifndef __TCP_CONNECTION__

#define __TCP_CONNECTION__

typedef struct TCPConnection {

	int sockfd;

	struct sockaddr_in server_addr;

} TCPConnection;

TCPConnection *tcp_open(const char *ip_addr, int port);

int tcp_write(TCPConnection *conn, const char *buf);

void tcp_close(TCPConnection *conn);

#endif