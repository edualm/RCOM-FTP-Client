#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TCPConnection.h"
#include "StringUtils.h"

#include "getip.h"

#include "Secret.h"

#define HOSTNAME "gnomo.fe.up.pt"

int open_pasv(TCPConnection *ftp_conn) {
	tcp_write(ftp_conn, "PASV\n");
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);
	
	char *ptr = strstr(response, "(");
	
	ptr++;
	
	char **arr = str_split(ptr, ',');
	
	char *current = NULL;
	
	int curr_pos = 0;
	
	int port = 0;
	
	while (( current = *(arr++) )) {
		int pos;
		
		if ( (pos = strpos(current, ")")) != -1 )
			current[pos] = '\0';
		
		if (curr_pos == 4)
			port = atoi(current) * 256;
		else if (curr_pos == 5)
			port += atoi(current);
		
		curr_pos++;
	}
	
	free(response);
	
	return port;
}

int download(TCPConnection *ftp_conn, const char *file_name) {
	FILE* file = fopen(file_name, "w");
	
	if (!file) {
		perror("fopen");
		return -1;
	}
	
	char *receive_buffer = malloc(256 * sizeof(char));
	
	int len;
	
	while ((len = tcp_read(ftp_conn, receive_buffer))) {
		if (len < 0) {
			perror("read");
			return len;
		}
		
		int error = fwrite(receive_buffer, len, 1, file);
	
		if (error < 0) {
			perror("fwrite");
			return error;
		}
	}
	
	fclose(file);
	
	return 0;
}

int main() {
	const char *ip_addr = get_ip_address_with_hostname(HOSTNAME);
	
	TCPConnection *ftp_conn = tcp_open(ip_addr, 21);
	
	printf("Connected to %s (%s).\n", HOSTNAME, ip_addr);
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);
	
	free(response);
	
	//	User Auth
	
	tcp_write(ftp_conn, "USER ei12018\n");
	
	response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);
	
	free(response);
	
	//	Password Auth
	
	tcp_write(ftp_conn, PASSWD_COMMAND);
	
	response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);
	
	free(response);
	
	/*tcp_write(ftp_conn, "CWD projeto-1\n");
	
	response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);*/
	
	/*
	
	//	PASV
	
	int pasv_port = open_pasv(ftp_conn);
	
	printf("PASV Port: %d\n", pasv_port);
	
	TCPConnection *pasv = tcp_open(ip_addr, pasv_port);
	
	printf("Opened PASV connection...\n");
	
	//	LS
	
	tcp_write(ftp_conn, "LIST\n");
	
	printf("Wrote LIST to ftp_conn...\n");
	
	char *ls = malloc(16777216 * sizeof(char));
	
	printf("Reading PASV connection...\n");
	
	tcp_read(pasv, ls);
	
	printf("%s\n", ls);
	
	response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	printf("%s\n", response);
	
	free(ls);
	
	tcp_close(pasv);
	
	*/
	
	//	PASV (again...)
	
	int pasv_port = open_pasv(ftp_conn);
	
	printf("PASV Port: %d\n", pasv_port);
	
	TCPConnection *pasv = tcp_open(ip_addr, pasv_port);
	
	printf("Opened PASV connection...\n");
	
	//	RETR
	
	tcp_write(ftp_conn, "RETR pfs.png\n");
	
	printf("Wrote RETR to ftp_conn...\n");
	
	//char *pasterino = malloc(16777216 * sizeof(char));
	
	printf("Reading PASV connection...\n");
	
	//	tcp_read(pasv, pasterino);
	
	download(pasv, "/Users/MegaEduX/pasterino.png");
	
	//printf("Pasterino Result:\n\n%s\n", pasterino);
	
	free(pasv);
	
	//	Close Connection
	
	tcp_close(ftp_conn);
	
	return 0;
}