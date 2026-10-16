#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main (void) {
  int clientSocket {socket(AF_INET, SOCK_STREAM, 0)};
  
  sockaddr_in serverAdress{};
  serverAdress.sin_port = htons(8080);
  serverAdress.sin_family = AF_INET;
  serverAdress.sin_addr.s_addr = INADDR_ANY;
  connect(clientSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));
  const char* message = "Hello, server!";
  send(clientSocket, message, strlen(message), 0);
  close(clientSocket);
  return 0;
}
