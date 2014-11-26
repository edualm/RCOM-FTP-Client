#ifndef __FTP__
#define __FTP__

#include "TCPConnection.h"

int ftp_open_pasv(TCPConnection *ftp_conn);
int ftp_cwd(TCPConnection *ftp_conn, const char *path);
int ftp_retr(TCPConnection *ftp_conn, const char *file_name);
int ftp_download(TCPConnection *ftp_conn, const char *file_name);
int ftp_authenticate(TCPConnection *ftp_conn, const char *username, const char *password);

#endif