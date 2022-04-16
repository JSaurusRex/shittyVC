#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>
#define MAX 4410 //2 extra bytes send for server time, (client/server) latency
#define EXTRABYTES 1
#define TOTALBYTES (EXTRABYTES+MAX)
//server latency means the client has to read the tcp traffic jam

#define MAXCLIENTS 8
#define PORT 8080
#define SA struct sockaddr

typedef struct{
	int online;
	int16_t buff[TOTALBYTES];
	int conn;
	pthread_t thread;
	struct sockaddr_in socket;
}Client;
Client clients[MAXCLIENTS] = {0};
int amountClients = 0;
pthread_mutex_t bufferLock;
int currentClient = 0;
int counting = 0;
clock_t begin;

// Function designed for chat between client and server.
void func()
{
	int counter = 0;
	int delay = 0;
	// infinite loop for chat
	while(1)
	{
		counter++;
		for (int client = 0; client < MAXCLIENTS; client++) {
			if(!clients[client].online)
			{
				//printf(">:(!\n");
				continue;
			}
			//printf("ola!\n");
			//pthread_mutex_lock(&bufferLock);
			bzero(clients[client].buff, sizeof(clients[client].buff));

			// read the message from client and copy it in buffer
			int16_t inBuff [MAX] = {0};
			int result = read(clients[client].conn, inBuff, sizeof(inBuff));
			if(!result)
			{
				//close client
				clients[client].online = 0;
				amountClients--;
				printf("client %i left\n", client);
				// pthread_mutex_unlock(&bufferLock);
				// return;
				continue;
			}
			// print buffer which contains the client contents
			//printf("From client: %s\t To client : ", buff);
			// copy server message in the buffer
			// while ((buff[n++] = getchar()) != '\n')
			// 	;
			// and send that buffer to clientf
			
			memcpy(clients[client].buff, inBuff, sizeof(inBuff));
			//printf("client new frame: %i \n", client);
			int16_t tmpBuff [TOTALBYTES] = {0};
			//printf("  %i  ", buff[client][50]);
			for(int i = 0; i <= amountClients; i++)
			{
				// if(i == client)
				// 	continue;
				// if(clients[i].socket.sin_addr.s_addr == clients[client].socket.sin_addr.s_addr)
				// 	continue;
				if(!clients[i].online)
					continue;
				//printf("imma do it, just you watch");
				for(int j = EXTRABYTES; j < TOTALBYTES; j++)
				{
					tmpBuff[j] += clients[i].buff[j];
					//printf("%i  ", buff[i][j]);
				}
				//printf("  %i  ", buff[i][50]);
				//printf("\n");
			}
			tmpBuff[0] = counter; 
			// printf("time: %i timeBack %i delay %i\n", counter, inBuff[0], counter-inBuff[0]);
			result = write(clients[client].conn, tmpBuff, sizeof(tmpBuff));
			if(!result)
			{
				//close client
				clients[client].online = 0;
				amountClients--;
				printf("client %i left\n", client);
				// pthread_mutex_unlock(&bufferLock);
				// return;
				continue;
			}
			// if msg contains "Exit" then server exit and chat ended.
			currentClient = client;
			//pthread_mutex_unlock(&bufferLock);
			//usleep(5000);
		}
	}

}

// Driver function
int main()
{
	
	int sockfd, connfd1, connfd2, len;
	struct sockaddr_in servaddr;

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
	
	pthread_mutex_init(&bufferLock, NULL) ;

	begin = (clock()/1000);
	pthread_t thread;
	pthread_create(&thread, NULL, func, NULL);
	while(1)
	{
		len = sizeof(clients[0].socket);
		int i = 0;
		for(; i < MAX; i++)
			if(!clients[i].online)
				break;
		// Accept the data packet from client and verification
		clients[i].conn = accept(sockfd, (SA*)&clients[i].socket, &len);
		if (clients[i].conn < 0) {
			printf("server accept failed client: %i \n", i);
			continue;
		}
		else
			printf("server accept the client %i\n", i);
		clients[i].online = 1;
		// Function for chatting between client and server
		
		amountClients++;
	}
	// After chatting close the socket
	close(sockfd);
}
