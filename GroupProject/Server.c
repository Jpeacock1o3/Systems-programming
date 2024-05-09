// Group 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#define MAX_DATA_SIZE 1024
#define STAT_OK 1
#define STAT_FAIL -1

#define CMD_SEND 1
#define CMD_RECV 2
#define CMD_RESP 3
#define CMD_DATA 4

struct send_msg { // sent by client to server
    int msg_type; // CMD_SEND or CMD_RECV
    int file_size; // size of file to be sent (if applicable; if not set it to 0)
    char filename[128]; // name of file
};

struct resp_msg { // sent by server to client
    int msg_type;
    int status;
    int file_size;
};

struct data_msg {
    int msg_type; // CMD_DATA
    int data_leng; // length of data in buffer
    char buffer[MAX_DATA_SIZE];
};


int main (int argc, char **argv) {
if (argc == 3) {
    int serverSD, remoteSD;
    FILE *fd;
    struct send_msg send_msg;
    struct resp_msg resp_msg;
    struct data_msg data_msg;
    struct sockaddr_in addressInfoS, addressInfoC;
    struct stat st;
    char data_buffer[MAX_DATA_SIZE];
    char send_msg_buffer[sizeof(send_msg)];
    char resp_msg_buffer[sizeof(resp_msg)];
    char data_msg_buffer[sizeof(data_msg)];

    int bytesWritten = 0;
    int bytesRead = 0;
    int number;

    long address = inet_addr(argv[1]);
    unsigned short port = htons(atoi(argv[2]));

    addressInfoS.sin_family = AF_INET;
    addressInfoS.sin_port = port;
    addressInfoS.sin_addr.s_addr = address;

 
    
    // You might notice step 1 is after step 2. This is because I was trying to follow the steps laid out on canvas, but I could not get the serverSD socket to close if I wasn't doing the step 1 stuff within the loop.

    // 2) infinite loop
    while (1) {
        // 1) create, bind, and listen
        if ( (serverSD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            perror("Server: Something went wrong with socket creation.\n");
            return -1;
        }
        int val = 1;
        if (setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
            perror("Server: Something went wrong with setting socket options.\n");
            return -1;
        }
        if (bind(serverSD, (struct sockaddr*) &addressInfoS, sizeof(addressInfoS)) == -1) {
            perror("Server: Something went wrong with socket binding.\n");
            return -1;
        }
        if (listen(serverSD, 5) == -1) {
            perror("Server: Something went wrong with socket listening.\n");
            return -1;
        }
       
        printf("Server: listening\n");
        // 3) accept incoming connections
        socklen_t addrlenC = sizeof(addressInfoC);
        if ( (remoteSD = accept(serverSD, (struct sockaddr*) &addressInfoC, &addrlenC)) == -1) {
            perror("Server: Something went wrong with socket accept.\n");
            return -1;
        }

        char client_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, (struct sockaddr*) &addressInfoC.sin_addr, client_address, sizeof(client_address));
        int client_port = ntohs(addressInfoC.sin_port);
        printf("Server: Connected to %s, port %d\n", client_address, client_port);

        // 4) read network input stream for client-to-server (CMD_SEND) or server-to-client (CMD_RECV), then send CMD_RESP     
        if (recv(remoteSD, send_msg_buffer, sizeof(struct send_msg), 0) == -1) {
            perror("Server: Something went wrong with receiving message (send_msg).\n");
            return -1;
        } 
        memcpy(&send_msg, send_msg_buffer, sizeof(struct send_msg)); // deserialize received struct; struct holds filename, file_size, msg_type (CMD_SEND or CMD_RECV)
        printf("Server: Received message type %d from %s, port %d:\n", send_msg.msg_type, client_address, client_port);



        resp_msg.msg_type = CMD_RESP;
        resp_msg.status = STAT_OK;
        if (send_msg.msg_type == CMD_RECV) {
            stat(send_msg.filename, &st);
            resp_msg.file_size = st.st_size;
        }
        if (send_msg.msg_type == CMD_SEND) {
            resp_msg.file_size = 0;
        }
        printf("Server sends response: type %d, status %d, and file size %d\n", resp_msg.msg_type, resp_msg.status, resp_msg.file_size);
        memcpy(resp_msg_buffer, &resp_msg, sizeof(struct resp_msg)); // serialize respond msg struct
        if (send(remoteSD, resp_msg_buffer, sizeof(resp_msg_buffer), 0) == -1) {
            perror("Server: Something went wrong with sending resp_msg_buffer.\n");
            return -1;
        }
        printf("Server: response sent\n");

        // 5) if CMD_SEND, read network input for file and write file; if CMD_RECV, read file to network
        if (send_msg.msg_type == CMD_SEND) {
            printf("Server awaits data\n");
            fd = fopen(send_msg.filename, "w");
            printf("Total data for server to receive: %d bytes\n", send_msg.file_size);

            while (bytesWritten < send_msg.file_size) {
                if (recv(remoteSD, data_msg_buffer, sizeof(struct data_msg), 0) == -1) {
                    perror("Server: Something went wrong with receiving message (data_msg).\n");
                    return -1;
                }
                memcpy(&data_msg, data_msg_buffer, sizeof(struct data_msg));

                fprintf(fd, "%.*s", data_msg.data_leng, data_msg.buffer);
                bytesWritten += data_msg.data_leng;
                printf("Data written to file: %d bytes (total = %d)\n", data_msg.data_leng, bytesWritten);
            }
            printf("To server data transfer succeeded\n\n");
            bytesWritten = 0;
        }
        if (send_msg.msg_type == CMD_RECV) {
            fd = fopen(send_msg.filename, "r");
            printf("Total data for server to send: %d bytes\n", resp_msg.file_size);

            while (bytesRead < resp_msg.file_size) {
                int num_bytes;
                num_bytes = fread(data_msg.buffer, sizeof(char), sizeof(data_msg.buffer), fd);
                bytesRead += num_bytes;
                data_msg.data_leng = num_bytes;

                memcpy(data_msg_buffer, &data_msg, sizeof(struct data_msg)); // serialize respond msg struct
                if (send(remoteSD, data_msg_buffer, sizeof(data_msg_buffer), 0) == -1) {
                    perror("Server: Something went wrong with sending data_msg_buffer.\n");
                    return -1;
                }
                printf("Data sent to client: %d bytes (total = %d)\n", num_bytes, bytesRead);

            }
            printf("To client data transfer succeeded\n\n");
            bytesRead = 0;

        }

        // 6) close file and remote socket
        shutdown(serverSD, SHUT_RDWR);
        close(remoteSD);
        fclose(fd);

        printf("Press 1 to keep copying text files.\nPress 2 to exit program.\n");
        fflush(stdout);
        scanf("%d", &number);
        if (number == 2) {
            printf("Exiting now...\n\n");
            return 0;
        }

    }

}
else {
    printf("Incorrect number of command line arguments.\n\n");
}
      
}