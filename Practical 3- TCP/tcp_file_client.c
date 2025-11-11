#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 10090 // The port number for the server
#define BUFFER_SIZE 1024 // The buffer size for reading/sending data

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    char filename[256]; // File to send

    printf("Enter the filename to send: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        perror("Error reading filename");
        exit(1);
    }
    filename[strcspn(filename, "\n")] = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    send(sock, filename, strlen(filename), 0);
    printf("Sending file: %s\n", filename);

    fp = fopen(filename, "rb");
    if (!fp) {
        perror("File open failed");
        exit(1);
    }

    ssize_t bytes;
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        send(sock, buffer, bytes, 0);
    }

    printf("\nFile sent successfully.\n");

    fclose(fp);
    close(sock);
    return 0;
}
