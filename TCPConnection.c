#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "TCPConnection.h"

TCPConnection *tcp_open(const char *ip_addr, int port) {
	TCPConnection *conn = (TCPConnection *) malloc(sizeof(TCPConnection));
	
	bzero((char *) &(conn->server_addr), sizeof(conn->server_addr));
	
	conn->server_addr.sin_family = AF_INET;
	conn->server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	conn->server_addr.sin_port = htons(port);
	
	if ( (conn->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket()");
		
		return NULL;
	}
	
	if (connect(conn->sockfd, (struct sockaddr *) &(conn->server_addr), sizeof(conn->server_addr)) < 0) {
		perror("connect()");
		
		return NULL;
	}
	
	return conn;
}

int tcp_write(TCPConnection *conn, const char *buf) {
	return write(conn->sockfd, buf, strlen(buf));
}

int tcp_read(TCPConnection *conn, char *response) {
	return read(conn->sockfd, response, 16777216);
	
	/*int cur_size = bytes_read;
	
	char *ptr = response;
	
	while (bytes_read == 255) {
		response = realloc(response, cur_size += 255);
		
		ptr += 255;
		
		bytes_read = read(conn->sockfd, response, 255);
	}
	
	return bytes_read;*/
}

void tcp_close(TCPConnection *conn) {
	close(conn->sockfd);
}
