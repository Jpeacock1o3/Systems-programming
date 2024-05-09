// Group 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>

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
if (argc == 5) {
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
    int copy_message_type;
    char filename[128];
    strcpy(filename, argv[4]);

    int bytesWritten = 0;
    int bytesRead = 0;

    long address = inet_addr(argv[1]);
    unsigned short port = htons(atoi(argv[2]));

    addressInfoS.sin_family = AF_INET;
    addressInfoS.sin_port = port;
    addressInfoS.sin_addr.s_addr = address;
    
    if (strcmp(argv[3], "-s") == 0) {copy_message_type = CMD_SEND; }
    else if (strcmp(argv[3], "-r") == 0) {copy_message_type = CMD_RECV; }
    else {
        printf("Incorrect argument [%s] entered.\n", argv[3]);
        return -1;
    }

 
    // create socket
    if ( (remoteSD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("Client: Something went wrong with socket creation.\n");
        return -1;
    }
    // client connect to server
    if ( (connect(remoteSD, (struct sockaddr*) &addressInfoS, sizeof(struct sockaddr_in))) == -1) {
        perror("Client: Something went wrong with socket connection.\n");
        return -1;
    }
    printf("Client: Connected to %s, port %s\n", argv[1], argv[2]);

    if (copy_message_type == CMD_SEND) {
        printf("Client file name: %s\n", filename);
        stat(filename, &st);
        strcpy(send_msg.filename, filename);
        send_msg.msg_type = CMD_SEND;
        send_msg.file_size = st.st_size;
    }
    if (copy_message_type == CMD_RECV) {
        printf("Server file name: %s\n", filename);
        strcpy(send_msg.filename, filename);
        send_msg.msg_type = CMD_RECV;
        send_msg.file_size = 0;
    }

    // 2) send server [send_msg]
    memcpy(send_msg_buffer, &send_msg, sizeof(struct send_msg)); // serialize respond msg struct
    if (send(remoteSD, send_msg_buffer, sizeof(send_msg_buffer), 0) == -1) {
        perror("Client: Something went wrong with sending resp_msg_buffer.\n");
        return -1;
    }
    printf("Client sends command to server: type %d, filesize %d, file name %s\n", send_msg.msg_type, send_msg.file_size, send_msg.filename);

    // 3) await server response
    if (recv(remoteSD, resp_msg_buffer, sizeof(struct resp_msg), 0) == -1) {
        perror("Client: Something went wrong with receiving message (resp_msg).\n");
        return -1;
    } 
    memcpy(&resp_msg, resp_msg_buffer, sizeof(struct resp_msg)); // deserialize received struct
    printf("Client: response recv'd, type = %d, status = %d, filesize = %d\n", resp_msg.msg_type, resp_msg.status, resp_msg.file_size);

    // 4) if status == OK, enter loop to write to server or to read from server
    if (copy_message_type == CMD_RECV) {
        fd = fopen(filename, "w");
        printf("Total data for client to receive: %d bytes\n", resp_msg.file_size);

        while (bytesWritten < resp_msg.file_size) {
            if (recv(remoteSD, data_msg_buffer, sizeof(struct data_msg), 0) == -1) {
                perror("Client: Something went wrong with receiving message (data_msg).\n");
                return -1;
            }
            memcpy(&data_msg, data_msg_buffer, sizeof(struct data_msg));

            fprintf(fd, "%.*s", data_msg.data_leng, data_msg.buffer);
            bytesWritten += data_msg.data_leng;
            printf("Client writes %d bytes to file (total = %d)\n", data_msg.data_leng, bytesWritten);
        }
        printf("To client data transfer succeeded\n\n");
    }
    if (copy_message_type == CMD_SEND) {
        fd = fopen(filename, "r");
        printf("Total data for client to send: %d bytes\n", send_msg.file_size);

        while (bytesRead < send_msg.file_size) {
            int num_bytes;
            num_bytes = fread(data_msg.buffer, sizeof(char), sizeof(data_msg.buffer), fd);
            bytesRead += num_bytes;
            data_msg.data_leng = num_bytes;

            memcpy(data_msg_buffer, &data_msg, sizeof(struct data_msg)); // serialize respond msg struct
            if (send(remoteSD, data_msg_buffer, sizeof(data_msg_buffer), 0) == -1) {
                perror("Client: Something went wrong with sending data_msg_buffer.\n");
                return -1;
            }
            printf("Client sends %d bytes (total = %d)\n", num_bytes, bytesRead);
        }
        printf("To server data transfer succeeded\n\n");
    }


    // 5) close file and socket
    fclose(fd);
    close(remoteSD);
    return 0;
}
else {
    printf("Incorrect number of command line arguments.\n\n");
}
      
}