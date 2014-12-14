#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FTP.h"

#include "StringUtils.h"

int ftp_open_pasv(TCPConnection *ftp_conn) {
	tcp_write(ftp_conn, "PASV\n");

	char *response = (char *) malloc(256 * sizeof(char));
	
	bzero(response, 256 * sizeof(char));
	
	tcp_read(ftp_conn, response);

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

int ftp_retr(TCPConnection *ftp_conn, const char *file_name) {
	char *retr_command = malloc((8 + strlen(file_name)) * sizeof(char));
	
	bzero(retr_command, (8 + strlen(file_name)) * sizeof(char));
	
	strcat(retr_command, "RETR ");
	strcat(retr_command, file_name);
	strcat(retr_command, "\n");
	
	tcp_write(ftp_conn, retr_command);
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	bzero(response, (8 + strlen(file_name)) * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	if (strpos(response, "150") == -1) {
		free(response);
		free(retr_command);
		
		return 1;
	}
	
	free(response);
	free(retr_command);
	
	return 0;
}

int ftp_cwd(TCPConnection *ftp_conn, const char *path) {
	char *cwd_command = malloc((6 + strlen(path)) * sizeof(char));
	
	bzero(cwd_command, (6 + strlen(path)) * sizeof(char));
	
	strcat(cwd_command, "CWD ");
	strcat(cwd_command, path);
	strcat(cwd_command, "\n");
	
	tcp_write(ftp_conn, cwd_command);

	char *response = (char *) malloc(256 * sizeof(char));
	
	bzero(response, 256 * sizeof(char));

	tcp_read(ftp_conn, response);

	free(response);
	free(cwd_command);

	return 0;
}

int ftp_download(TCPConnection *ftp_conn, char *file_name) {
	FILE* file = fopen(file_name, "w");
	
	if (!file) {
		perror("fopen");
		return -1;
	}

	char *receive_buffer = malloc(8 + 256 * sizeof(char));
	
	bzero(receive_buffer, 8 + 256 * sizeof(char));

	int len = 0;

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
		
		free(receive_buffer);
		
		receive_buffer = malloc(8 + 256 * sizeof(char));
		
		bzero(receive_buffer, 8 + 256 * sizeof(char));
	}

	fclose(file);
	
	tcp_close(ftp_conn);		//	Close PASV Connection

	return 0;
}

int ftp_authenticate(TCPConnection *ftp_conn, const char *username, const char *password) {
	//	User Auth
	
	char *user_str = malloc((8 + strlen(username)) * sizeof(char));
	
	bzero(user_str, (8 + strlen(username)) * sizeof(char));
	
	strcat(user_str, "USER ");
	strcat(user_str, username);
	strcat(user_str, "\n");
	
	tcp_write(ftp_conn, user_str);
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	bzero(response, 256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	usleep(500000);
	
	char *passwd_str = malloc((8 + strlen(password)) * sizeof(char));
	
	bzero(passwd_str, (8 + strlen(password)) * sizeof(char));
	
	strcat(passwd_str, "PASS ");
	strcat(passwd_str, password);
	strcat(passwd_str, "\n");
	
	tcp_write(ftp_conn, passwd_str);
	
	free(response);
	
	response = (char *) malloc(256 * sizeof(char));
	
	bzero(response, 256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	if (strpos(response, "230") == -1) {
		free(response);
		
		return 1;
	}
	
	free(response);
	
	return 0;
}
