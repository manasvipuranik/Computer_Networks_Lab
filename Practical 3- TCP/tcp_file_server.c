#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 10090
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    FILE *fp;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(1);
    }
    printf("Server listening on port %d...\n", PORT);

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(1);
    }
    printf("Client connected!\n");

    char filename[256];
    ssize_t name_len = recv(new_socket, filename, sizeof(filename) - 1, 0);
    if (name_len <= 0) {
        perror("Failed to receive filename or connection closed");
        exit(1);
    }
    filename[name_len] = '\0';
    printf("Receiving file: %s\n", filename);

    fp = fopen(filename, "wb");
    if (!fp) {
        perror("File open failed for writing");
        exit(1);
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }

    if (bytes_received < 0) {
        perror("Error receiving file content");
    } else {
        printf("\n\nFile '%s' received successfully.\n", filename);
    }

    fclose(fp);
    close(new_socket);
    close(server_fd);
    return 0;
}
