#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "TCPConnection.h"
#include "StringUtils.h"
#include "FTP.h"

#include "getip.h"

const char * get_cwd() {
	char *cwd = getcwd (0, 0);
	
	if (!cwd) {
		fprintf(stderr, "getcwd failed: %s\n", strerror (errno));
		
		return NULL;
	}
	
	return cwd;
}

int array_len(char **array) {
	int counter = 0;
	
	for (; array[counter] != NULL; counter++);
	
	return counter;
}

char ** parse_ftp_link(char *url) {
	char *username = NULL, *password = NULL, *host = NULL, *path = NULL, *ptr = strstr(url, "ftp://");
	
	if (ptr == NULL) {
		printf("URL Parse Error.\n");
		
		return NULL;
	}
	
	ptr += 6;
	
	char **auth_rest = str_split(ptr, '@');
	
	if (auth_rest[0] && auth_rest[1]) {
		//	There's a username here, at least!
		
		char *auth = auth_rest[0];
		
		int username_len = 0;
		
		if ((username_len = strpos(auth, ":")) == -1) {
			printf("Invalid URL format: Username found, but no password. Aborting...\n");
			
			return NULL;
		}
		
		char **userpwd = str_split(auth, ':');
		
		username = malloc(strlen(userpwd[0]) * sizeof(char));
		
		strcpy(username, userpwd[0]);
		
		password = malloc(strlen(userpwd[1]) * sizeof(char));
		
		strcpy(password, userpwd[1]);
		
		ptr = auth_rest[1];
	}
	
	char **host_path = str_split(ptr, '/');
	
	host = malloc(strlen(host_path[0]) * sizeof(char));
	
	strcpy(host, host_path[0]);
	
	path = malloc(256 * sizeof(char));
	
	bzero(path, 256 * sizeof(char));
	
	int len = array_len(host_path);
	
	int i = 1;
	
	for (; i < len; i++) {
		strcat(path, host_path[i]);
		strcat(path, "/");
	}
	
	path[strlen(path) - 1] = '\0';
	
	char **ret;
	
	if (username && password)
		ret = malloc(5 * sizeof(char *));
	else
		ret = malloc(3 * sizeof(char *));
	
	int counter = 0;
	
	ret[counter++] = host;
	ret[counter++] = path;
	
	if (username && password) {
		ret[counter++] = username;
		ret[counter++] = password;
	}
	
	ret[counter] = NULL;
	
	return ret;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		char progname[512];
		strcpy(progname, argv[0]);
		
		printf("Usage: %s ftp://[<user>:<password>@]<host>/<file-path>\n", basename(progname));
		
		return 0;
	}
	
	char **parsed = parse_ftp_link(argv[1]);
	
	if (parsed == NULL)
		return 1;
	
	char *host = parsed[0], *path = parsed[1], *username = parsed[2], *password = parsed[3];
	
	const char *ip_addr = get_ip_address_with_hostname(host);
	
	if (ip_addr == NULL) {
		printf("Can't resolve host %s. Aborting...\n", host);
		
		return 1;
	}
	
	TCPConnection *ftp_conn = tcp_open(ip_addr, 21);
	
	printf("Connected to %s (%s).\n", host, ip_addr);
	
	char *response = (char *) malloc(256 * sizeof(char));
	
	tcp_read(ftp_conn, response);
	
	free(response);
	
	if (username != NULL) {
		if (ftp_authenticate(ftp_conn, username, password)) {
			printf("FTP Authentication Failure. Aborting...\n");
			
			return 1;
		}
		
		printf("Logged in as \"%s\".\n", username);
	} else {
		printf("No login details provided - attempting to login as anonymous:guest...\n");
		
		if (ftp_authenticate(ftp_conn, "anonymous", "guest")) {
			printf("FTP Authentication Failure. Aborting...\n");
			
			return 1;
		}
		
		printf("Logged in as anonymous.\n");
	}
	
	int pasv_port = ftp_open_pasv(ftp_conn);
	
	if (pasv_port == -1) {
		printf("Couldn't get a passive mode port. Aborting...\n");
		
		return 1;
	}
	
	TCPConnection *pasv_conn = tcp_open(ip_addr, pasv_port);
	
	char *file_name = NULL;
	
	char **path_array = str_split(path, '/');
	
	int pa_len = array_len(path_array);
	
	int i = 0;
	
	for (; i < pa_len - 1; i++) {
		printf("Performing CWD to %s...\n", path_array[i]);
		
		ftp_cwd(ftp_conn, path_array[i]);
	}
	
	file_name = path_array[pa_len - 1];
	
	if (ftp_retr(ftp_conn, file_name)) {
		printf("RETR command failed. Aborting...\n");
		
		return 1;
	}	
	
	const char *cwd = get_cwd();
	
	if (cwd == NULL) {
		printf("Couldn't get current working directory. Aborting...\n");
		
		return 1;
	}
	
	int dl_path_mem = sizeof(cwd) + sizeof(file_name) + 16 * sizeof(char);
	
	char *dl_path = malloc(1024 * sizeof(char));
	
	bzero(dl_path, dl_path_mem);
	
	strcat(dl_path, cwd);
	
	if (dl_path[strlen(dl_path) - 1] != '/')
		strcat(dl_path, "/");
	
	strcat(dl_path, file_name);
	
	printf("Performing download on %s...\n", dl_path);
	
	if (ftp_download(pasv_conn, dl_path)) {
		printf("Download failed. Aborting...\n");
		
		return 1;
	}
	
	tcp_close(ftp_conn);
	
	printf("All done! Quitting...\n");
	
	return 0;
}
