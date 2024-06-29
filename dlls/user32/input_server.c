// input_server.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <pthread.h>
#include "input_server.h"

#define PORT 12345
#define BUFFER_SIZE 1024

static SOCKET server_fd = INVALID_SOCKET; // Global server socket
static volatile int stop_server = 0;      // Global flag to stop the server

void process_command(const char *command)
{
    // if (strncmp(command, "key:", 4) == 0)
    // {
    //     char key = command[4];
    //     INPUT ip;
    //     ip.type = INPUT_KEYBOARD;
    //     ip.ki.wVk = key;
    //     ip.ki.dwFlags = 0; // Key press
    //     SendInput(1, &ip, sizeof(INPUT));

    //     ip.ki.dwFlags = KEYEVENTF_KEYUP; // Key release
    //     SendInput(1, &ip, sizeof(INPUT));
    // }
    // Add more command processing as needed
    print("Received command: %s\n", command);
}

void *client_handler_thread(void *arg)
{
    SOCKET client_socket = *(SOCKET *)arg;
    free(arg); // Free the allocated memory for the socket

    char buffer[BUFFER_SIZE] = {0};

    printf("SOCKET_SERVER: Client connected: socket %ld\n", client_socket);

    while (!stop_server)
    {
        int read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (read_size <= 0)
        {
            printf("SOCKET_SERVER: Client disconnected: socket %ld\n", client_socket);
            closesocket(client_socket);
            return NULL;
        }
        buffer[read_size] = '\0';
        printf("SOCKET_SERVER: Received command from socket %ld: %s\n", client_socket, buffer);
        process_command(buffer);
    }

    closesocket(client_socket);
    return NULL;
}

void *socket_server_thread(void *arg)
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    printf("SOCKET_SERVER: Socket server started, listening on port %d\n", PORT);

    while (!stop_server)
    {
        SOCKET *new_socket = malloc(sizeof(SOCKET));
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET)
        {
            if (stop_server)
            {
                free(new_socket);
                break;
            }
            printf("SOCKET_SERVER: Accept failed\n");
            free(new_socket);
            continue;
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handler_thread, new_socket);
        pthread_detach(thread_id); // Optional: detach thread to clean up resources automatically
    }

    printf("SOCKET_SERVER: Socket server stopped\n");
    return NULL;
}

void start_socket_server()
{
    pthread_t thread_id;
    struct sockaddr_in address;
    int opt = 1;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("SOCKET_SERVER: WSAStartup failed\n");
        return;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("SOCKET_SERVER: Socket creation failed\n");
        WSACleanup();
        return;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == SOCKET_ERROR)
    {
        printf("SOCKET_SERVER: Setsockopt failed\n");
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        printf("SOCKET_SERVER: Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR)
    {
        printf("SOCKET_SERVER: Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    pthread_create(&thread_id, NULL, socket_server_thread, NULL);
    pthread_detach(thread_id); // Optional: detach thread to clean up resources automatically
}

void stop_socket_server()
{
    stop_server = 1;
    if (server_fd != INVALID_SOCKET)
    {
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
        WSACleanup();
    }
    printf("SOCKET_SERVER: Socket server stopped\n");
}