#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main(void) {
  int clientSocket{socket(AF_INET, SOCK_STREAM, 0)};

  sockaddr_in serverAdress{};
  serverAdress.sin_port = htons(8080);
  serverAdress.sin_family = AF_INET;
  serverAdress.sin_addr.s_addr = INADDR_ANY;
  connect(clientSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));
  const char *message = "Hello Server";
  send(clientSocket, message, strlen(message), 0);
  const char *message2 = "SUIIIIIII MPBAPPER";
  send(clientSocket, message2, strlen(message2), 0);
  // The msgs above are sent in one packet;
  close(clientSocket);
}
