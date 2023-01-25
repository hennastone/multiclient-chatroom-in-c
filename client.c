#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

// Function to recieve data from server
void * doRecieving(void * sockID){
	int clientSocket = *((int *) sockID);
	while(1){
		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
		printf("%s\n",data);
	}
}

int main(){
	// Create a socket and an address
	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	// define the address of the server
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Connect to the server and check for errors
	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");

	// Create a thread to recieve data from server
	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	// Send data to server
	while(1){
		char input[1024];

		// Get input from user
		scanf("%s",input);

		// Check for commands and send data to server

		if(strcmp(input,"SEND") == 0){
			send(clientSocket,input,1024,0);

			scanf("%s",input);
			send(clientSocket,input,1024,0);
			printf("%s isimli kullaniciya giden mesaj: ",input);

			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);
			printf("%s\n",input);

		}
		else if(strcmp(input,"BEGIN") == 0){

			send(clientSocket,input,1024,0);

			scanf("%s",input);
			send(clientSocket,input,1024,0);
		}

		else if(strcmp(input,"END") == 0){

			send(clientSocket,input,1024,0);
			break;
		}

		else{
			printf("Invalid command\n");
		}

	}
	// Close the socket
	close(clientSocket);
	return 0;
}