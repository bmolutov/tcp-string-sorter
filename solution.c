#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_PENDING_CONNECTIONS 5
#define BUFFER_SIZE 1024
#define OFF_COMMAND "OFF"

int comparator(const void *a_, const void *b_) {
  char a = *((char *) a_);
  char b = *((char *) b_);
  if (a < b)
    return 1;
  if (a > b)
    return -1;
  return 0;
}

int main(int argc, char *argv[]) {
  
  if (argc != 2)
    return 1;

  int port = atoi(argv[1]);

  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(port);

  // bind the socket to the specified port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }

  // start listening for incoming connections
  if (listen(server_fd, MAX_PENDING_CONNECTIONS) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
  }

  // Accept incoming connections
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
  }

  while (1) {

    ssize_t bytes_recieved = recv(new_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_recieved < 0)
      break;

    buffer[bytes_recieved] = '\0';

    // exit condition
    if (strcmp(buffer, OFF_COMMAND) == 0)
      break;

    // do sort
    qsort(buffer, bytes_recieved, sizeof(char), comparator);

    // echo message
    send(new_socket, buffer, strlen(buffer), 0);

  }

  close(new_socket);
  close(server_fd);

  return 0;
}

