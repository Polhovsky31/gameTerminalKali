#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr);

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при подключении к серверу");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\nВведите ваше предположение (или 'exit' для выхода): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        send(client_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        printf("Сервер: %s", buffer); 
    }

    close(client_socket);
    return 0;
}
