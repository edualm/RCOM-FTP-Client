all:
	gcc getip.c TCPConnection.c StringUtils.c main.c -o download

clean:
	rm -rf *.o rcom