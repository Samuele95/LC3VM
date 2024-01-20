/* Server code */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define BUFSIZE 1024
#define PORT_NUMBER     1031
#define SERVER_ADDRESS  "192.168.124.207"


int main(int argc, char **argv)
{
    int server_socket;
    int peer_socket;
    socklen_t       sock_len;
    ssize_t len;
    struct sockaddr_in      server_addr;
    struct sockaddr_in      peer_addr;
    int fd;
    int sent_bytes = 0;
    char file_size[256];
    char file_path[100];
    struct stat file_stat;
    off_t offset;
    int remain_data;

    /* Create server socket */
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        fprintf(stderr, "Error creating socket --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Zeroing server_addr struct */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Construct server_addr struct */
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDRESS, &(server_addr.sin_addr));
    server_addr.sin_port = htons(PORT_NUMBER);

    /* Bind */
    if ((bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))) == -1)
    {
        fprintf(stderr, "Error on bind --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Listening to incoming connections */
    if ((listen(server_socket, 5)) == -1)
    {
        fprintf(stderr, "Error on listen --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    printf("\nWrite the path of the file --> ");
    scanf("%s", file_path);

    fd = open(file_path, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening file --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Get file stats */
    if (fstat(fd, &file_stat) < 0)
    {
        fprintf(stderr, "Error fstat --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "File Size: \n%d bytes\n", file_stat.st_size);

    sock_len = sizeof(struct sockaddr_in);
    /* Accepting incoming peers */
    peer_socket = accept(server_socket, (struct sockaddr *)&peer_addr, &sock_len);
    if (peer_socket == -1)
    {
        fprintf(stderr, "Error on accept --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Accept peer --> %s\n", inet_ntoa(peer_addr.sin_addr));

    sprintf(file_size, "%d", file_stat.st_size);

    /* Sending file size */
    len = send(peer_socket, file_size, sizeof(file_size), 0);
    if (len < 0)
    {
        fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server sent %d bytes for the size\n", len);

    offset = 0;
    remain_data = file_stat.st_size;
    /* Sending file data */
    while (((sent_bytes = sendfile(peer_socket, fd, &offset, BUFSIZ)) > 0) && (remain_data > 0))
    {
        fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
        remain_data -= sent_bytes;
        fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
    }

    close(peer_socket);
    close(server_socket);

    return 0;
}

