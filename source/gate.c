#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"

#define SOCKET_ERROR        -1
#define QUEUE_SIZE          5

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize = sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;

    if(argc < 2)
    {
	    printf("\nUsage: server host-port\n");
	    return 0;
    }
    else
    {
	    nHostPort = atoi(argv[1]);
    }
    /* make a socket */
    printf("Starting server at port %d...\n", nHostPort);
    hServerSocket = socket(AF_INET,SOCK_STREAM,0);
    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }
    /* fill address struct */
    Address.sin_addr.s_addr = INADDR_ANY;
    Address.sin_port = htons(nHostPort);
    Address.sin_family = AF_INET;


    /* bind to a port */
    if(bind(hServerSocket, (struct sockaddr*)&Address, sizeof(Address)) == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }

    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }

    signal(SIGCHLD, SIG_IGN);

    for(;;)
    {
        /* get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);

        if(fork() == 0)
        {
            char * resp;
            pBuffer[read(hSocket,pBuffer,BUFFER_SIZE)] = '\0';
            printf("%s", pBuffer);
            /*
            strcpy(pBuffer, "HTTP/1.1 200 OK\nDate: Thu, 10 Mar 2011 15:31:59 GMT\nContent-Type: text/html;charset=ISO-8859-1\nContent-Length: 13\n\nHello, world!");
            write(hSocket, pBuffer, strlen(pBuffer));
            */
            resp = respond(pBuffer);
            write(hSocket, resp, strlen(resp));
            free(resp);
            close(hSocket);
            exit(0);
        }

        close(hSocket);
    }
}