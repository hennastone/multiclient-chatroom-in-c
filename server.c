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
#include <stdbool.h>
#include <time.h>

// Declaration of necessary global variables
int clientCount = 0;

// Structure for client
struct client{
    char name[20];
    char messages[10][1024];
    int messageCount;
    int sockID;
    struct sockaddr_in clientAddr;
    int len;
    bool online;
};

// Declaration of necessary arrays
struct client Client[1024];
pthread_t thread[1024];

// Function to handle client
void * doNetworking(void * ClientDetail){

    // Declaration of some client variables
    struct client* clientDetail = (struct client*) ClientDetail;
    int clientSocket = clientDetail -> sockID;
    clientDetail -> online = true;

    // Informing client about valid commands
    send(clientSocket,"Valid commands are: BEGIN <name>, SEND <reciever> <message>, END\n",1024,0);

    while(1){
        // Declaration of necessary variables for communication
        char data[1024];
        char name[20];
        char output[1024];
        bool found = false;

        // Receiving data from client
        int read = recv(clientSocket,data,1024,0);
        data[read] = '\0';

        // Checking the data received

        if(strcmp(data,"BEGIN") == 0){

            // Receiving name from client
            read = recv(clientSocket,data,1024,0);
            data[read] = '\0';

            // Storing name in client structure
            strcpy(clientDetail -> name,data);

            // Sending welcome message to client
            sprintf(output,"Welcome %s\n",data);
            send(clientSocket,output,1024,0);
            printf("Client %s connected.\n", data);

            for(int i = 0 ; i < clientCount ; i ++){
                // checking for every client object with name same as client
                if(strcmp(Client[i].name,clientDetail->name) == 0){
                    found = true;
                    // Checking if client has messages
                    if( Client[i].messageCount > 0){

                        // Sending all messages that client has
                        char message[1024];
                        sprintf(message, "You have %d messages\n", Client[i].messageCount);
                        send(clientSocket, message, 1024, 0);
                        for (int j = 0; j < Client[i].messageCount; j++) {
                            send(clientSocket, Client[i].messages[j], 1024, 0);
                        }
                        // Updating client's message count
                        Client[i].messageCount = 0;
                        break;
                    }
                    else
                        send(clientSocket,"You have 0 messages\n",20,0);
                        break;
                }
                else
                    found = false;
            }
            if(found==false)
                send(clientSocket,"You have 0 messages\n",20,0);
            
        }

        else if(strcmp(data,"SEND") == 0){

            // Receiving name from client
            read = recv(clientSocket,data,1024,0);
            data[read] = '\0';

            // Storing name in name variable
            strcpy(name,data);

            // Receiving message from client
            read = recv(clientSocket,data,1024,0);
            data[read] = '\0'; 

            // Calculating local time
            time_t seconds;
            struct tm *t;
            seconds = time(NULL);
            t = localtime(&seconds);

            // Storing message in output variable
            sprintf(output,"%d:%d:%d-%s: %s\n",t->tm_hour,t->tm_min,t->tm_sec,clientDetail->name,data);

            // Finding client with name same as name variable 
            for(int i = clientCount ; i >=0 ; i --){
                if(strcmp(Client[i].name,name) == 0){

                    // Checking if client is online
                    if(Client[i].online == true){

                        // Sending message to client if client is online
                        send(Client[i].sockID,output,1024,0);
                        Client[i].messageCount = 0;
                        break;
                    }
                    else{
                        // Storing message in client's message array if client is offline
                        strcpy(Client[i].messages[Client[i].messageCount],output);

                        // Updating client's message count
                        Client[i].messageCount ++;

                        Client[i].messages[Client[i].messageCount][0] = '\0';
                    }
                }
            }
        }

        else if(strcmp(data,"END") == 0){

            // Sending message to client that he is disconnected
            send(clientSocket,"You are disconnected\n",21,0);
            
            printf("Client %s disconneted\n", clientDetail->name);

            // Updating client's online status
            clientDetail->online = false;
            break;
        }

        else{
            // Sending error message to client if data received is not valid
            send(clientSocket,"Invalid command\n",16,0);
        }

        // Clearing output variable
        output[0] = '\0';
    }
    return NULL;
}

int main(){
    // Declaration of necessary variables for socket
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

    // Binding and listening to socket, if error occurs then program terminates
    if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1){
        printf("Bind error.\n");
        return 0;
    }
    if(listen(serverSocket,1024) == -1){
        printf("Listen error.\n");
        return 0;
    }

    printf("Server started listenting ...........\n");

    // Accepting clients and creating threads for them
    while(1){
        Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
        pthread_create(&thread[clientCount], NULL, doNetworking, (void *) &Client[clientCount]);
        // Updating client count
        clientCount ++;
    }
    // Joining all threads
    for(int i = 0 ; i < clientCount ; i ++){
        pthread_join(thread[i],NULL);
    }
}