#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 5120
#define OFF_COMMAND "OFF\n"

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
    if (argc != 2) {
        return 1;
    }

    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    char buffer[MAX_MESSAGE_SIZE];
    while (1) {
      struct sockaddr_in client_addr;
      socklen_t client_addr_len = sizeof(client_addr);

      ssize_t bytes_received = recvfrom(
        sockfd, buffer, sizeof(buffer), 0,
                                         (struct sockaddr *)&client_addr, &client_addr_len);
      if (bytes_received == -1) {
          perror("recvfrom");
          close(sockfd);
          return 1;
      }

      buffer[bytes_received] = '\0';

      if (strcmp(buffer, OFF_COMMAND) == 0) {
          break;
      }

      printf("%s\n", buffer);
    }

    close(sockfd);

    return 0;
}

