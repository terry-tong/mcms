/*
Terry Tong
Multi-client Messaging System
Server Code
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
#define LISTEN_PORT 8080

int main(int argc, char *argv[]) {

	int sock_listen, sock_recv;
	int i, addr_size, bytes_received;
	int incoming_len;
	int recv_msg_size;
	struct sockaddr_in my_addr, recv_addr;
	int select_ret;
	fd_set readfds;
	struct timeval timeout={0,0};
	struct sockaddr remote_addr;
	char buf[BUF_SIZE];
	int status;
	
	/* create socket for listening */
	sock_listen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_listen < 0) {
		printf("socket() failed\n");
		exit(0);
	}

	/* make local address structure */
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons((unsigned short)LISTEN_PORT);

	/* bind socket to the local address */
	i = bind(sock_listen, (struct sockaddr *) &my_addr, sizeof(my_addr));
	if (i < 0) {
		printf("bind() failed\n");
		exit(0);
	}

	/* listen */
	i = listen(sock_listen, 5);
	if (i < 0) {
		printf("listen() failed\n");
		exit(0);
	}
	int pid;
	while (1) {
		addr_size = sizeof(recv_addr);
		sock_recv = accept(sock_listen, (struct sockaddr *) &recv_addr, &addr_size);
		if (sock_recv < 0) {
			close(sock_recv);
			printf("accept() failed\n");
		}
		pid = fork();
		if (pid == -1) {
			close(sock_recv);
			printf("fork() failed\n");
			exit(0);
		}
		else if (pid == 0) {
			/* child process */
			printf("A client has joined!\n");
			while (1) {
				bytes_received = recv(sock_recv, buf, BUF_SIZE, 0);
				buf[bytes_received] = 0;
				printf("Received: %s\n", buf);
				if (strcmp(buf, "quit") == 0) {
					printf("A client has left!\n");
					close(sock_recv);
					break;
				}
				else if (strcmp(buf, "shutdown") == 0) {
					printf("Server is shutting down.\n");
					kill(pid, SIGTERM);
					exit(0);
				}
			}
		}
	}
	close(sock_listen);
	close(sock_recv);
}
