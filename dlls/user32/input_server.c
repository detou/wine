// input_server.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <unistd.h>
#include "input_server.h"
#include "wine/server.h"
#include "wine/debug.h"
#include "wine/server_protocol.h"
#include "user_private.h"

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
    // INPUT input;
    // input.type = INPUT_MOUSE;
    // input.mi.dx = x;
    // input.mi.dy = y;
    // input.mi.mouseData = 0;
    // input.mi.dwFlags = flags;
    // input.mi.time = 0;
    // input.mi.dwExtraInfo = 0;

    // SendInput(1, &input, sizeof(INPUT));
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
        req->flags |= SEND_HWMSG_RAWINPUT;

        wine_server_call(req);
    }
    SERVER_END_REQ;
}

void test_click(int pixel_x, int pixel_y)
{
    FIXME("SOCKET_SERVER: Clicking on window %p\n", window_handle);

    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, pixel_x, pixel_y);
    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pixel_x, pixel_y);
    do_mouse(window_handle, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pixel_x, pixel_y);

    FIXME("SOCKET_SERVER: Mouse click simulated at (%lu, %lu)\n", pixel_x, pixel_y);
}

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
    // FIXME("SOCKET_SERVER: Received command: %s\n", command);

    printf("SOCKET_SERVER: Received command: %s\n", command);
    test_click(508, 585);
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
        process_command(buffer);
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