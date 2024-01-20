/* Client code */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

/**
 * Splits the ip and the port from the string ipPort
*/
void splitIpPort(char *ipPort, char *ip, int *port) {
    char  *token;

    if((token = strtok(ipPort, ":")) == NULL) {
        fprintf(stderr, "Error on strtok (ip)");
        exit(EXIT_FAILURE);
    }
    strcpy(ip, token);
    
    if((token = strtok(NULL, ":")) == NULL) {
        fprintf(stderr, "Error on strtok (port)");
        exit(EXIT_FAILURE);
    }
    *port = atoi(token);
}

int main(int argc, char **argv)
{
    int client_socket;
    ssize_t len;
    struct sockaddr_in remote_addr;
    char buffer[BUFSIZ];
    int file_size;
    FILE *received_file;
    int remain_data = 0;

    /* Max char count: 255.255.255.255 (15 chars + end null = 16) */
    char server_ip[16];
    /* server port */
    int server_port;

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <ip>:<port> <received_file_path>\n", argv[0]);

        exit(EXIT_FAILURE);
    }

    splitIpPort(argv[1], server_ip, &server_port);

    /* Zeroing remote_addr struct */
    memset(&remote_addr, 0, sizeof(remote_addr));

    /* Construct remote_addr struct */
    remote_addr.sin_family = AF_INET;

    if(inet_pton(AF_INET, server_ip, &(remote_addr.sin_addr)) == 0)
    {
        fprintf(stderr, "Ip address is not valid --> %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    remote_addr.sin_port = htons(server_port);

    /* Create client socket */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Connect to the server */
    if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Receiving file size */
    recv(client_socket, buffer, BUFSIZ, 0);
    file_size = atoi(buffer);
    //fprintf(stdout, "\nFile size : %d\n", file_size);

    received_file = fopen(argv[2], "wb");
    if (received_file == NULL)
    {
        fprintf(stderr, "Failed to open file %s --> %s\n", argv[2], strerror(errno));

        exit(EXIT_FAILURE);
    }

    printf("Received file size: %d\n", file_size);

    remain_data = file_size;

    while ((remain_data > 0) && ((len = recv(client_socket, buffer, BUFSIZ, 0)) > 0))
    {
        fwrite(buffer, sizeof(char), len, received_file);
        remain_data -= len;
        fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
    }

    fclose(received_file);
    close(client_socket);

    return 0;
}
