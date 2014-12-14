all:
	gcc -Wall -Wextra getip.c TCPConnection.c StringUtils.c FTP.c main.c -o download

clean:
	rm -rf *.o rcom
