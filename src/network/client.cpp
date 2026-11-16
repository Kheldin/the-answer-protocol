#include <cstring>
#include <netinet/in.h>
#include <print>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
int main(void) {
  int clientSocket{socket(AF_INET, SOCK_STREAM, 0)};
  char buffer[32]{0};

  sockaddr_in serverAdress{};
  serverAdress.sin_port = htons(8080);
  serverAdress.sin_family = AF_INET;
  serverAdress.sin_addr.s_addr = INADDR_ANY;
  connect(clientSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress));
  const char *message = "QUIT\n";
  send(clientSocket, message, strlen(message), 0);
  ssize_t check = ::recv(clientSocket, buffer, sizeof(buffer), 0);
  std::string command{buffer, (size_t)check};
  std::println("response: {}", command);
  close(clientSocket);
}
