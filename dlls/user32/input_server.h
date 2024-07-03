// input_server.h
#ifndef INPUT_SERVER_H
#define INPUT_SERVER_H

void start_socket_server();
void stop_socket_server();
void process_command(const char *command, int read_size);

#endif // INPUT_SERVER_H
