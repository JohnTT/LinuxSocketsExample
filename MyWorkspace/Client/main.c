#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>

int sockfd, n;
char stdio_buffer[256];
int portnum = 8080;
char hostname[INET_ADDRSTRLEN];
struct sockaddr_in serv_addr;
struct hostent *server;

char read_buffer[256];
int msg_count = 0;

pthread_t msgcount_tid;
pthread_t readmsg_tid;
int err;

void* readMsg(void *arg)
{
	while (1)
	{
		// Read Response from Server
		bzero(read_buffer, 256);
			
		n = read(sockfd, read_buffer, 255);
		
		if (n < 0) 
			error("ERROR reading from socket");
				 
				 
		printf("Client (Me) Received: %s\n", read_buffer);
		
	}
	
	return NULL;	
}

void* sendMsgCount(void *arg)
{
	char write_buffer[100];
    pthread_t id = pthread_self();
	
	while (1)
	{
		// Send Message Count to Server
		msg_count++;
		sprintf(write_buffer, "Message Count: %d", msg_count);
		
		n = write(sockfd, write_buffer, strlen(write_buffer));
	
		if (n < 0) 
			 error("ERROR writing to socket");
		printf("Client (Me) Sent: %s\n", write_buffer);
		
		
		
		// Sleep for 1 second
		sleep(1);
	}

    return NULL;
}


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	
	printf("I am the client program.\n");
	
	printf("Please enter the server's IP address: ");
	scanf("%s", stdio_buffer);
	memcpy(hostname, stdio_buffer, sizeof(stdio_buffer));
	server = gethostbyname(hostname);
	
	printf("Please enter the server's port number: ");
	scanf("%s", stdio_buffer);
	portnum = atoi(stdio_buffer);
		
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
		
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portnum);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
		
	// Create Sent Message Count Thread
	err = pthread_create(&(msgcount_tid), NULL, &sendMsgCount, NULL);
	
	// Create Read Server Response Thread
	err = pthread_create(&(readmsg_tid), NULL, &readMsg, NULL);

		
	while (1) 
	{
	}
	
    close(sockfd);
    return 0;
}