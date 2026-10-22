#include "server/server.hpp"
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

Server::Server() {
  try {
    // AF_INET: IPv4 protocol
    // SOCK_STREAM: 2-way connection-based byte streams
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      throw std::runtime_error("socket() failed.");
    }
    sockaddr_in serverAdress{};
    // sin_family: The address family for the transport address. This member
    // should always be set to AF_INET.
    serverAdress.sin_family = AF_INET;
    // sin_port: A transport protocol port number.
    // htons: Convert a uint16 host endian into the network endian.
    serverAdress.sin_port = htons(8080);
    // sin_addr: An IN_ADDR structure that contains an IPv4 transport address.
    // INADDR_ANY: A wildcard.
    serverAdress.sin_addr.s_addr = INADDR_ANY;
    // Assign the adress to the socket.
    if (bind(sockfd, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) ==
        -1) {
      throw std::runtime_error("bind() failed.");
    }
  } catch (std::exception &e) {
    std::cout << "An error occured during the server initialization: "
              << e.what() << "\n";
  }

  monitor.fd = sockfd;
  monitor.events = POLLIN;
  monitor.revents = 0;
  clients_fds.push_back(monitor);
}

void Server::start_listening() { listen(sockfd, 5); }

Server::~Server() { close(sockfd); }

int main(void) {
  Server server{};
  server.start_listening();

  // the buffer who'll store the request
  char buffer[1024]{0};

  int clientSocket{0};
  int res{0};
  int check{0};
  bool run{true};

  // the server infinite loop
  while (run) {
    // start the radar with -1 means 'block infinitily while a thing move'
    // fds.data() allows to give the C array from the vector to the C fonction
    // poll()
    res = poll(server.clients_fds.data(), server.clients_fds.size(), -1);

    if (res < 0) {
      std::cerr << "Error on poll()\n";
      break;
    }

    // we go through the list to see who fd moved
    for (size_t i{0}; i < server.clients_fds.size(); ++i) {
      if (server.clients_fds[i].revents & POLLIN) {
        if (i == 0) {
          clientSocket = accept(server.getfd(), nullptr, nullptr);

          std::cout << "NEW PLAYER CONNECTED" << "\n";
          pollfd client{};
          client.fd = clientSocket;
          client.events = POLLIN;
          client.revents = 0;

          server.clients_fds.push_back(client);
        } else {
          check = recv(clientSocket, buffer, sizeof(buffer), 0);
          // looking for errors or deconnections
          if (check <= 0) {
            close(server.clients_fds[i].fd);
            server.clients_fds.erase(server.clients_fds.begin() + i);
            --i; // compensate for the vector offset
            std::cout << "The player " << server.clients_fds[i].fd
                      << " is deconnected or crash.\n";
          } else {
            std::string msg(buffer);

            // rm the last char while is not a normal char
            while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r')) {
              msg.pop_back();
            }

            if (msg == "QUIT") {
              close(server.clients_fds[i].fd);
              server.clients_fds.erase(server.clients_fds.begin() + i);
              --i; // compensate for the vector offset
              std::cout << "The player " << server.clients_fds[i].fd
                        << " leave the game.\n";
            } else if (msg == "SHUTDOWN") {
              run = false;
              break;
            } else {
              std::cout << "|" << buffer << "|" << "\n";
            }
          }
        }
      }
    }
  }
  return 0;
}
