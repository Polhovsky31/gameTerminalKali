#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_addr) {
    int number_to_guess = rand() % 100 + 1; // Загаданное число от 1 до 100
    int guess, attempts = 0;
    char buffer[BUFFER_SIZE];
    char client_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_address, INET_ADDRSTRLEN);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("%s: Client disconnected.\n", client_address);
            break;
        }

        guess = atoi(buffer);
        attempts++;

        printf("%s: %s\n", client_address, buffer);

        if (guess < number_to_guess) {
            send(client_socket, "Загаданное число больше ", strlen("Загаданное число больше "), 0);
        } else if (guess > number_to_guess) {
            send(client_socket, "Загаданное число меньше ", strlen("Загаданное число меньше "), 0);
        } else {
            snprintf(buffer, BUFFER_SIZE, "Поздравляем! Вы угадали число %d за %d попыток.", number_to_guess, attempts);
            send(client_socket, buffer, strlen(buffer), 0);
            printf("%s: %s\n", client_address, buffer);
            break;
        }
    }

    close(client_socket);
}

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    // Создание сокета
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязка сокета к адресу
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при привязке сокета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Прослушивание входящих соединений
    if (listen(server_socket, 5) < 0) {
        perror("Ошибка при прослушивании");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d...\n", PORT);

    while (1) {
        // Принятие входящего соединения
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Ошибка при принятии соединения");
            continue;
        }

        char client_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_address, INET_ADDRSTRLEN);
        printf("%s: Client connected.\n", client_address);

        handle_client(client_socket, client_addr);
    }

    close(server_socket);
    return 0;
}
