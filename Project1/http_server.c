#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define BUFFER_SIZE 1024
void DieWithError(char *errorMessage); /* Error handling function */
int HandleTCPClient(int client_sock); /* TCP client handling function */

int main(int argc, char *argv[]) { 
    int server_sock; /*Socket descriptor for server */
    int client_sock; /* Socket descriptor for client */
    struct sockaddr_in server_address; /* Local address */
    struct sockaddr_in client_address; /* Client address */
    unsigned short server_port; /* Server port */
    unsigned int client_length; /* Length of client address data structure */

    if (argc != 2) /* Test for correct number of arguments */ {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
        exit(1);
    }
    server_port = atoi(argv[1]); /* First arg: local port */
    /* Create socket for incoming connections */
    if ((server_sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("socket() failed\n"); 
    }
    else {
        printf("socket() succeeded\n");
    }

    /* Construct local address structure */
    memset(&server_address, 0, sizeof(server_address)); /* Zero out structure */
    server_address.sin_family = AF_INET; /* Internet address family */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_address.sin_port = htons(server_port); /* Local port */
    
    /* Bind to the local address */
    if (bind (server_sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        DieWithError("bind() failed\n");
    }
    else {
        printf("bind() succeeded\n");
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen (server_sock, MAXPENDING) < 0) {
        DieWithError("listen() failed\n");
    }
    else {
        printf("listen() succeeded\n");
    }

    for (;;) /* Run forever */ {
        /* Set the size of the in-out parameter */
        client_length = sizeof(client_address); /* Wait for a client to connect */
        if ((client_sock = accept (server_sock, (struct sockaddr *) &client_address, &client_length)) < 0) {
            DieWithError("accept() failed\n");
        }
        /* client_sock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(client_address.sin_addr));
        if (HandleTCPClient(client_sock) == -1) {
            break;
        }
    }
    exit(0);
}

void DieWithError(char *errorMessage) {
	printf(errorMessage);

}

/* TCP client handling function */
int HandleTCPClient(int client_sock) {
    // Read Socket
    char buf[BUFFER_SIZE]; // Main Buffer
    FILE* client_fd;
    if ((client_fd = fdopen(client_sock, "r+")) <= 0) {
        DieWithError("Failure opening socket\n");
    }
    // Read File
    recv(client_sock, buf, BUFFER_SIZE, 0);
    FILE* file_fd;
    
    char * path;
    path = strtok(buf, " ");
    path = strtok(NULL, " ");
    

    path++;

    // Check for kill signall
    if (strcmp(path, "SIGTERM") == 0) {
        fclose(client_fd);
        return -1;
    }

    if (strcmp(path, "") || strcmp(path, "index.html") == 0) {
        path = "TMDG_files/TMDG.html";
    }

    if ((file_fd = fopen(path, "r")) != NULL) {
        char* response = NULL;
        size_t RESPONSESIZE = 100000;
        fputs("HTTP/1.1 200 OK", client_fd);
        while ((getline(&response, &RESPONSESIZE, file_fd)) != -1) {
            fputs(response, client_fd);
        }
        fputc(EOF, client_fd);
        if (response) { 
            free(response); 
            fclose(file_fd);
        }
    }

    else {
        if (fputs("404 Not Found", client_fd) < 0) {
            DieWithError("Client write failed\n");
        }
        fclose(client_fd);
        return 0;
    }
    
    fclose(client_fd);
    return 0;
}
