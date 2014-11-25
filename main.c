#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TCPConnection.h"
#include "StringUtils.h"

#include "getip.h"

#include "Secret.h"

int open_pasv(TCPConnection *ftp_conn) {
	tcp_write(ftp_conn, "PASV\n");
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	char *ptr = strstr(response, "(");
	
	ptr++;
	
	char **arr = str_split(ptr, ',');
	
	char *current = NULL;
	
	int curr_pos = 0;
	
	int port = 0;
	
	while (( current = *(arr++) )) {
		int pos;
		
		printf("Part: %s\n", current);
		
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

int main() {
	const char *ip_addr = get_ip_address_with_hostname("gnomo.fe.up.pt");
	
	TCPConnection *ftp_conn = tcp_open(ip_addr, 21);
	
	printf("Connected to gnomo.fe.up.pt (%s).\n", ip_addr);
	
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
	
	//	PASV
	
	int pasv_port = open_pasv(ftp_conn);
	
	printf("PASV Port: %d\n", pasv_port);
	
	TCPConnection *pasv = tcp_open(ip_addr, pasv_port);
	
	tcp_write(ftp_conn, "LIST");
	
	char *ls = malloc(256 * sizeof(char));
	
	tcp_read(pasv, ls);
	
	printf("%s\n", ls);
	
	free(ls);
	
	tcp_close(ftp_conn);
	
	return 0;
}