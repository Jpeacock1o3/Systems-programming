#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
int main(int argc, char *argv[]) {
char *filename = argv[1];
int fd;
if ((fd = open(filename, O_RDONLY)) == -1) {
perror("Error opening file");
}
printf("File opened successfully.\n");
lseek(fd, 0, SEEK_CUR);
printf("Current position: 0\n");
char buffer[1024];
long position;
ssize_t bytes_read;
while (1) {
printf("Position has been set to 0, enter a new position (negative to
quit):");
if (scanf("%ld", &position) != 1) {
fprintf(stderr, "Error: Please enter a valid number.\n");
continue;
}
if (position < 0)
break;
if (lseek(fd, position, SEEK_SET) == -1) {
perror("Error seeking position");
break;
}
long new_position = lseek(fd, 0, SEEK_CUR);
printf("New position: %ld\n", new_position);
printf("Enter the number of bytes to read: ");
size_t num_bytes;
if (scanf("%zu", &num_bytes) != 1) {
fprintf(stderr, "Error: Please enter a valid number.\n");
break;
}
bytes_read = read(fd, buffer, num_bytes);
if (bytes_read == -1) {
perror("Error reading file");
break;
}
printf("Bytes read: %.*s\n", (int)bytes_read, buffer);
printf("Bytes actually read: %zd\n", bytes_read);
// Reposition to location 0
if (lseek(fd, 0, SEEK_SET) == -1) {
perror("Error repositioning to location 0");
break;
}
}
close(fd);
return 1;}