// input_server.h
#ifndef INPUT_SOCKET_H
#define INPUT_SOCKET_H

void process_command(const char *command);
void *socket_server_thread(void *arg);
void start_socket_server();
void stop_socket_server();
#endif