#include <stdio.h> /* for printf() and fprintf() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <netdb.h>
#include <time.h>
#define RCVBUFSIZE 1000000 /* Size of receive buffer */
#define BUFFER_SIZE 100000
void DieWithError(char *errorMessage); /* Error handling function */ 
char* build_get_request(char* hostname, char* path);
int create_response_file();
int get_index(char* string, char c);

//timepassedms = 1000.0 * (endtime.tv_sec - starttime.tv_sec) + 0.001 * (endtime.tv_usec - starttime.tv_usec);

int main(int argc, char* argv[]) {
    // Handle arguments
    char* options = "";
    char* server_url;
    char* port_number;
    if (argc != 3 && argc != 4) {
        DieWithError("Invalid Arguments");
    }
    if (argc == 3) {
        server_url = argv[1];
        port_number = argv[2];
    }
    if (argc == 4) {
        options = argv[1];
        server_url = argv[2];
        port_number = argv[3];
    }
    printf("Input: %s, %s, %s\n", options, server_url, port_number);
    
    // Split the URL
    printf("Starting to Split\n");
    char hostname[BUFFER_SIZE];
    char pathname[BUFFER_SIZE];
    int divider;
    if ((divider = get_index(server_url, '/')) != 1) {
        strncpy(hostname, server_url,(size_t) divider);
        server_url += divider;
        strncpy(pathname, server_url,(size_t) BUFFER_SIZE - divider);
        server_url -= divider;
    }
    else {
        strcpy(hostname, server_url); 
        pathname[0] = '/';
        pathname[1] = '\0';
    }

    printf("Host Name: %s\nPath Name: %s\n", hostname, pathname);
    
    int sock;
    // Make Socket Stream
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        DieWithError("socket() failed");
    }
    else {
        printf("socket() succeeded");
    }

    // Connect to Server    
    struct addrinfo* server_info = malloc(sizeof(struct addrinfo));
    getaddrinfo(hostname, port_number, 0, &server_info);
    struct addrinfo* cursor = server_info;
    int connection;
    while (cursor != NULL) {
        connection = connect(sock, (struct sockaddr*) cursor->ai_addr, cursor->ai_addrlen);
        if (connection == 0) {
            // Successful Connection
            break;
        }
        cursor = cursor->ai_next;
    }
    if (connection != 0) {
        // Handle Error
        DieWithError("connect() failed\n");
    }
    else {
        printf("connect() succeeded\n");
    }

    // Make GET Request
    char* get_request = build_get_request(hostname, pathname);
    printf("-GET request start:-\n%s\n-GET request end-\n", get_request);
    
    double start = clock();
    FILE * sock_fd = fdopen(sock, "r+");
    int sent = fputs(get_request, sock_fd);
    if (sent < 0) {
        DieWithError("fputs() failed");
    }
    printf("GET request sent successfully\n");


    double end;
    // Receive data
    char* response = NULL;
    size_t RESPONSESIZE = 100000;

    FILE * output_fd = fopen("output", "w");
    int read;
    printf("\n-Start of Response-\n");
    while ((read = getline(&response, &RESPONSESIZE, sock_fd)) > 0) {
        if (strncmp(response, "HTTP/1.1 200 OK", 15)) {
            end = clock();
        }
        printf("%s", response);
        fprintf(output_fd, "%s", response);
        if (response == NULL || response[RESPONSESIZE] == EOF) {
            break;
        }
        if ((clock() - start) >= 5000) {
            break;
        }
    }
    if (response == NULL) {
        DieWithError("Failed to receive response");
    }
    printf("\n-End of Response-\n");

    double rtt = end - start;
    if (strcmp(options, "-p") == 0) {
        printf("RTT: %f\n", rtt);
    }
    // Shut everything down
    fclose(sock_fd);
    fclose(output_fd);
    close(sock);
    free(response);
    freeaddrinfo(server_info);
    free(get_request);
    exit(0);
}

void DieWithError(char* errorMessage) {
    printf(errorMessage);
    printf("\n");
    exit(1);
}

char* build_get_request (char* hostname, char* path) {
    char* request = (char*) malloc(1000 * sizeof(char));
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);
    return request;
}

int create_response_file() {
  int fd = open("response.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("open() failed\n");
    exit(1);
  }
  return fd;
}

// Source: https://stackoverflow.com/questions/3217629/how-do-i-find-the-index-of-a-character-within-a-string-in-c
//Returns the index of the first occurence of char c in char* string. If not found -1 is returned.
int get_index(char* string, char c) {
    char *e = strchr(string, c);
    if (e == NULL) {
        return -1;
    }
    return (int)(e - string);
}