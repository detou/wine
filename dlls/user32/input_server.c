// input_server.c
#include "input_server.h"
#include "user_private.h"
#include "wine/debug.h"
#include "wine/server.h"
#include "wine/server_protocol.h"
#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

WINE_DEFAULT_DEBUG_CHANNEL(win);

#define PORT 50000
#define BUFFER_SIZE 1024

static SOCKET server_fd = INVALID_SOCKET; // Global server socket
static volatile int stop_server = 0;      // Global flag to stop the server
static HANDLE threads[FD_SETSIZE];        // Array to keep track of client handler threads
static int thread_count = 0;              // Count of active threads
static HWND window_handle;

typedef struct
{
    HWND hwnd;
} MainWindowSearchData;

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    MainWindowSearchData *search_data = (MainWindowSearchData *)lParam;

    // Check if the window is visible
    FIXME("SOCKET_SERVER: Checking window %p\n", hwnd);
    if (IsWindowVisible(hwnd) && hwnd)
    {
        FIXME("SOCKET_SERVER: Found visible window %p\n", hwnd);
        // We found the main window handle
        search_data->hwnd = hwnd;
        return FALSE; // Stop enumeration
    }

    return TRUE; // Continue enumeration
}

HWND get_main_window_handle(void)
{
    MainWindowSearchData search_data = {NULL};

    FIXME("SOCKET_SERVER: Starting to search for the main window handle\n");

    // Enumerate all top-level windows
    EnumWindows(EnumWindowsProc, (LPARAM)&search_data);

    FIXME("SOCKET_SERVER: Returning window handle: %p\n", search_data.hwnd);
    return search_data.hwnd;
}

void do_mouse(HWND window, DWORD flags, int x, int y)
{
    SERVER_START_REQ(send_hardware_message)
    {
        req->win = window;
        req->flags = 0;
        req->input.type = INPUT_MOUSE;
        FIXME("Coordinates: %d,%d\n", x, y);
        req->input.mouse.x = x;
        req->input.mouse.y = y;
        req->input.mouse.data = 0;
        req->input.mouse.flags = flags;
        req->input.mouse.time = 0;
        req->input.mouse.info = 0;
        // req->flags |= SEND_HWMSG_RAWINPUT;

        wine_server_call(req);
    }
    SERVER_END_REQ;
}

void do_keyboard(HWND window, DWORD flags, unsigned short key)
{
    SERVER_START_REQ(send_hardware_message)
    {
        req->win = window;
        req->flags = 0;
        req->input.type = INPUT_KEYBOARD;
        req->input.kbd.vkey = key;
        req->input.kbd.scan = 0;
        req->input.kbd.flags = flags;
        req->input.kbd.time = 0;
        req->input.kbd.info = 0;
        // req->flags |= SEND_HWMSG_RAWINPUT;

        wine_server_call(req);
    }
    SERVER_END_REQ;
}

void decode_mouse_move_command(const char *command, int *x, int *y)
{
    // Move the pointer to the data part
    const char *data = command + 2;

    // Extract x
    memcpy(x, data, sizeof(int));
    data += sizeof(int);

    // Extract y
    memcpy(y, data, sizeof(int));
}

void decode_mouse_button_command(const char *command, int *x, int *y, int *right_click)
{
    // Move the pointer to the data part
    const char *data = command + 2;

    // Extract x
    memcpy(x, data, sizeof(int));
    data += sizeof(int);

    // Extract y
    memcpy(y, data, sizeof(int));
    data += sizeof(int);

    // Extract rightClick
    memcpy(right_click, data, 1);
}

void mouse_move(int pixel_x, int pixel_y)
{
    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, pixel_x, pixel_y);
}

void mouse_press(int pixel_x, int pixel_y, int right_click)
{
    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | (right_click == 0 ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN), pixel_x, pixel_y);
}

void mouse_release(int pixel_x, int pixel_y, int right_click)
{
    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | (right_click == 0 ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP), pixel_x, pixel_y);
}

void press_key(unsigned short key)
{
    do_keyboard(window_handle, 0, key);
}

void release_key(unsigned short key)
{
    do_keyboard(window_handle, KEYEVENTF_KEYUP, key);
}

void send_key(unsigned short key)
{
    press_key(key);
    Sleep(2);
    release_key(key);
}

void process_command(const char *init_command, int read_size)
{
    // Read first two characters of command, they determine the command type
    const char *command = init_command;
    int left = read_size;

    FIXME("SOCKET_SERVER: Read size: %d\n", read_size);

    while (left > 1)
    {
        char command_type[3];
        memcpy(command_type, command, 2);
        command_type[2] = '\0';

        // Compare it with "mm", "mp", "mr"
        if (strcmp(command_type, "mm") == 0)
        {
            int x, y;
            decode_mouse_move_command(command, &x, &y);
            FIXME("SOCKET_SERVER: Mouse move: %d, %d\n", x, y);
            mouse_move(x, y);
            command += 10;
            left -= 10;
        }
        else if (strcmp(command_type, "mp") == 0)
        {
            int x, y, right_click;
            decode_mouse_button_command(command, &x, &y, &right_click);
            FIXME("SOCKET_SERVER: Mouse press: %d, %d, %d\n", x, y, right_click);
            mouse_press(x, y, right_click);
            command += 11;
            left -= 11;
        }
        else if (strcmp(command_type, "mr") == 0)
        {
            int x, y, right_click;
            decode_mouse_button_command(command, &x, &y, &right_click);
            FIXME("SOCKET_SERVER: Mouse release: %d, %d, %d\n", x, y, right_click);
            mouse_release(x, y, right_click);
            command += 11;
            left -= 11;
        }
        else
        {
            FIXME("SOCKET_SERVER: Unknown command type: %s\n", command_type);
        }
    }

    // mouse_click(508, 428);
    // Sleep(100);
    // press_key(VK_SHIFT);
    // send_key('A');
    // release_key(VK_SHIFT);
    // char myChar = *command;
    // // if (strncmp(command, "key:", 4) == 0)
    // // {
    // //     char key = command[4];
    // //     INPUT ip;
    // //     ip.type = INPUT_KEYBOARD;
    // //     ip.ki.wVk = key;
    // //     ip.ki.dwFlags = 0; // Key press
    // //     SendInput(1, &ip, sizeof(INPUT));

    // //     ip.ki.dwFlags = KEYEVENTF_KEYUP; // Key release
    // //     SendInput(1, &ip, sizeof(INPUT));
    // // }
    // // Add more command processing as needed
    // // FIXME("SOCKET_SERVER: Received command: %s\n", command);

    // printf("SOCKET_SERVER: Received command: %s\n", command);
    // test_click(508, 585);
    // test_click(508, 561);
}

DWORD WINAPI client_handler_thread(LPVOID lpParam)
{
    SOCKET client_socket = *(SOCKET *)lpParam;
    free(lpParam); // Free the allocated memory for the socket

    char buffer[BUFFER_SIZE] = {0};

    FIXME("SOCKET_SERVER: Client connected: socket %ld\n", client_socket);

    while (!stop_server)
    {
        int read_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (read_size <= 0)
        {
            FIXME("SOCKET_SERVER: Client disconnected: socket %ld\n", client_socket);
            closesocket(client_socket);
            return 0;
        }
        buffer[read_size] = '\0';
        FIXME("SOCKET_SERVER: Received command from socket %ld: %s\n", client_socket, buffer);
        process_command(buffer, read_size);
    }

    closesocket(client_socket);
    return 0;
}

DWORD WINAPI socket_server_thread()
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    FIXME("SOCKET_SERVER: Socket server started, listening on port %d\n", PORT);

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
            FIXME("SOCKET_SERVER: Accept failed\n");
            free(new_socket);
            continue;
        }

        if (thread_count < FD_SETSIZE)
        {
            threads[thread_count] = CreateThread(NULL, 0, client_handler_thread, new_socket, 0, NULL);
            thread_count++;
        }
        else
        {
            FIXME("SOCKET_SERVER: Maximum thread limit reached. Closing new connection.\n");
            closesocket(*new_socket);
            free(new_socket);
        }
    }

    FIXME("SOCKET_SERVER: Socket server stopped\n");
    return 0;
}

void start(LPVOID lpParam)
{
    // Make sure we have a window
    Sleep(15000);
    if ((window_handle = get_main_window_handle()) == 0)
    {
        FIXME("SOCKET_SERVER: Main window not found\n");
        return;
    }
    if (stop_server)
    {
        FIXME("SOCKET_SERVER: Socket server already stopped\n");
        return;
    }
    FIXME("SOCKET_SERVER: Main window handle: %p\n", window_handle);

    HANDLE thread_handle;
    struct sockaddr_in address;
    int opt = 1;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        FIXME("SOCKET_SERVER: WSAStartup failed\n");
        return;
    }

    FIXME("SOCKET_SERVER: WSAStartup succeeded\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        FIXME("SOCKET_SERVER: Socket creation failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    FIXME("SOCKET_SERVER: Socket created\n");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == SOCKET_ERROR)
    {
        FIXME("SOCKET_SERVER: Setsockopt failed with error %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    FIXME("SOCKET_SERVER: Setsockopt succeeded\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
    {
        FIXME("SOCKET_SERVER: Bind failed with error %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    FIXME("SOCKET_SERVER: Bind succeeded\n");

    if (listen(server_fd, 3) == SOCKET_ERROR)
    {
        FIXME("SOCKET_SERVER: Listen failed with error %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return;
    }

    FIXME("SOCKET_SERVER: Listen succeeded\n");

    socket_server_thread(NULL);
}

void start_socket_server()
{
    HANDLE thread_handle = CreateThread(NULL, 0, start, NULL, 0, NULL);
    if (thread_handle == NULL)
    {
        FIXME("SOCKET_SERVER: Thread creation failed\n");
    }
    else
    {
        CloseHandle(thread_handle); // Optionally close the handle as we don't need it here
    }
}

void stop_socket_server()
{
    FIXME("SOCKET_SERVER: Stopping socket server\n");

    stop_server = 1;
    if (server_fd != INVALID_SOCKET)
    {
        FIXME("SOCKET_SERVER: Closing server socket\n");
        closesocket(server_fd);
        server_fd = INVALID_SOCKET;
    }
    else
    {
        FIXME("SOCKET_SERVER: Server socket already closed\n");
    }

    // Wait for all threads to finish
    for (int i = 0; i < thread_count; i++)
    {
        FIXME("SOCKET_SERVER: Waiting for thread %d to finish\n", i);
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
        FIXME("SOCKET_SERVER: Thread %d finished\n", i);
    }

    WSACleanup();
    FIXME("SOCKET_SERVER: Socket server stopped\n");
}