#ifndef __TCP_CONNECTION__

#define __TCP_CONNECTION__

#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>

typedef struct TCPConnection {

	int sockfd;

	struct sockaddr_in server_addr;

} TCPConnection;

TCPConnection *tcp_open(const char *ip_addr, int port);

int tcp_write(TCPConnection *conn, const char *buf);

int tcp_read(TCPConnection *conn, char *response);

void tcp_close(TCPConnection *conn);

#endif