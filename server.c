#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
void func(int connfd1, int connfd2)
{
	char buff1[MAX];
	char buff2[MAX];
	// infinite loop for chat
	for (;;) {
		bzero(buff1, MAX);
		bzero(buff2, MAX);

		// read the message from client and copy it in buffer
		read(connfd1, buff1, sizeof(buff1));
		read(connfd2, buff2, sizeof(buff2));
		// print buffer which contains the client contents
		//printf("From client: %s\t To client : ", buff);
		// copy server message in the buffer
		// while ((buff[n++] = getchar()) != '\n')
		// 	;

		// and send that buffer to client
		write(connfd1, buff2, sizeof(buff2));
		write(connfd2, buff1, sizeof(buff1));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff1, 4) == 0 || strncmp("exit", buff1, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd1, connfd2, len;
	struct sockaddr_in servaddr, cli1, cli2;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli1);
	len = sizeof(cli2);

	// Accept the data packet from client and verification
	connfd1 = accept(sockfd, (SA*)&cli1, &len);
	connfd2 = accept(sockfd, (SA*)&cli2, &len);
	if (connfd1 < 0 || connfd2 < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the clients...\n");

	// Function for chatting between client and server
	func(connfd1, connfd2);

	// After chatting close the socket
	close(sockfd);
}
