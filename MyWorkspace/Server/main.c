#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int portnum = 8080;
socklen_t clilen;
char read_buffer[256], write_buffer[256];
struct sockaddr_in serv_addr, cli_addr;
char client_ip[256];
int n;


void echoMessage(int newsockfd)
{
    while (1)
    {
        // Echo message back to client.
        bzero(read_buffer, 256);
        n = read(newsockfd, read_buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Server (Me) Received: %s\n", read_buffer);

        bzero(write_buffer, 256);
        sprintf(write_buffer, "Echo: %s\0", read_buffer);

        n = write(newsockfd, write_buffer, sizeof(write_buffer)-1);
        if (n < 0)
            error("ERROR writing to socket");
        printf("Server (Me) Sent: %s\n", write_buffer);
    }

    close(newsockfd);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    int pid;
    printf("I am the server program.\n");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnum);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        inet_ntop(AF_INET, &(cli_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Accepted client at IP address: %s\n", client_ip);

        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid < 5)
        {
            close(sockfd);
            echoMessage(newsockfd);
            exit(0);
        }
        else
            close(newsockfd);
    } /* end of while */





    return 0;

}
