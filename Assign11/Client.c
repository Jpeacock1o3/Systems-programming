#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MESSAGE "Hello world!"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    printf("Client: send message to %s, port %d\n", server_ip, server_port);

    ssize_t sent_len = sendto(sockfd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_len < 0) {
        perror("sendto");
        exit(1);
    }

    printf("Client: message sent\n");

    close(sockfd);
    return 0;
}