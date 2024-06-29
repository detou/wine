// input_server.c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <windows.h>
#include "input_server.h"

#define PORT 12345
#define BUFFER_SIZE 1024

static int server_fd = -1;           // Global server socket file descriptor
static volatile int stop_server = 0; // Global flag to stop the server

void process_command(const char *command)
{
    if (strncmp(command, "key:", 4) == 0)
    {
        char key = command[4];
        INPUT ip;
        ip.type = INPUT_KEYBOARD;
        ip.ki.wVk = key;
        ip.ki.dwFlags = 0; // Key press
        SendInput(1, &ip, sizeof(INPUT));

        ip.ki.dwFlags = KEYEVENTF_KEYUP; // Key release
        SendInput(1, &ip, sizeof(INPUT));
    }
    // Add more command processing as needed
}

void *socket_server_thread(void *arg)
{
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    printf("SOCKET_SERVER : Server started!\n");

    while (!stop_server && (new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        int read_size = read(new_socket, buffer, BUFFER_SIZE);
        buffer[read_size] = '\0';
        printf("SOCKET_SERVER : Received command: %s\n", buffer);
        process_command(buffer);
        close(new_socket);
    }

    return NULL;
}

void start_socket_server()
{
    pthread_t thread_id;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("SOCKET_SERVER : socket failed");
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("SOCKET_SERVER : setsockopt failed");
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("SOCKET_SERVER : bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("SOCKET_SERVER : listen failed");
        close(server_fd);
        return;
    }

    pthread_create(&thread_id, NULL, socket_server_thread, NULL);
    pthread_detach(thread_id); // Optional: detach thread to clean up resources automatically
}

void stop_socket_server()
{
    stop_server = 1;
    if (server_fd != -1)
    {
        close(server_fd);
        server_fd = -1;
    }
}
