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
    std::cout << "An error occured during the socket initialization: "
              << e.what() << "\n";
  }

  monitor.fd = sockfd;
  monitor.events = POLLIN;
  monitor.revents = 0;
  clients_fds.push_back(monitor);
}

Server::~Server() { close(sockfd); }

void Server::start_listening() { listen(sockfd, 5); }

void Server::accept_new_client(int &clientSocket) {
  // Create a socket and a pollfd for the new client.
  // Ask the kernel to check POLLIN event.
  // Add it to the list so it will be watched at the next poll.
  clientSocket = accept(getfd(), nullptr, nullptr);
  std::cout << "New connection request." << "\n";
  pollfd client{};
  client.fd = clientSocket;
  client.events = POLLIN;
  client.revents = 0;
  clients_fds.push_back(client);
}

int Server::receive_msg(int i, int &clientSocket) {
  int check{0};
  char buffer[1024]{0};
  check = recv(clientSocket, buffer, sizeof(buffer), 0);
  std::cout << "check == " << i << " " << check << "\n";
  // looking for errors or deconnections
  if (check <= 0) {
    close(clients_fds[i].fd);
    clients_fds.erase(clients_fds.begin() + i);
    --i; // compensate for the vector offset
    std::cout << "The player " << clients_fds[i].fd
              << " is deconnected or crash.\n";
    return (1);
  } else {
    std::string msg(buffer);
    // rm the last char while is not a normal char
    while (!msg.empty() && (msg.back() == '\n' || msg.back() == '\r')) {
      msg.pop_back();
    }

    if (msg == "QUIT") {
      close(clients_fds[i].fd);
      clients_fds.erase(clients_fds.begin() + i);
      --i; // compensate for the vector offset
      std::cout << "The player " << clients_fds[i].fd << " leave the game.\n";
    } else if (msg == "SHUTDOWN") {
      return (1);
    } else {
      std::cout << "|" << buffer << "|" << "\n";
    }
    return (0);
  }
}

void Server::run() {
  bool run{true};
  int clientSocket{0};
  while (run) {
    // std::vector::data() return a pointer to the first elem of the
    // container. poll() is a system call. The kernel will fill the
    // pollfd.revents with what happended. poll() return:
    //  The number of fds that received an events/errors.
    //  0 means the system calls timed out before any fds is ready.
    //  -1 = errors and errno is set to indicate the error.
    // nb: Explain in readMe the choice of poll instead of epoll.
    if (poll(clients_fds.data(), clients_fds.size(), -1) < 0) {
      std::cerr << "Error on poll()\n";
      break;
    }

    // We go through the list to see which fd moved
    for (size_t i{0}; i < clients_fds.size(); ++i) {
      if (clients_fds[i].revents & POLLIN) {
        // The index 0 is the server socket.
        // If the server socket is readable, it means that a client wants to
        // connect to the server.
        if (i == 0) {
          accept_new_client(clientSocket);
        } else {
          if (receive_msg(i, clientSocket) == 1) {
            break;
          }
        }
      }
    }
  }
}

int main(void) {
  Server server{};
  server.start_listening();
  server.run();
  return 0;
}
