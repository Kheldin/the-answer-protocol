#include "network/network.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

Server::Server() {
  // AF_INET: IPv4 protocol
  // SOCK_STREAM: 2-way connection-based byte streams
  sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  int opt{1};
  if (sockfd == -1 ||
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
    throw std::runtime_error("socket() failed.");
  }
  sockaddr_in serverAddress{};
  // sin_family: The address family for the transport address. This member
  // should always be set to AF_INET.
  serverAddress.sin_family = AF_INET;
  // sin_port: A transport protocol port number.
  // htons: Convert a uint16 host endian into the network endian.
  serverAddress.sin_port = htons(8080);
  // sin_addr: An IN_ADDR structure that contains an IPv4 transport address.
  // INADDR_ANY: A wildcard.
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  // Assign the Address to the socket.
  if (bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) ==
      -1) {
    throw std::runtime_error("bind() failed.");
  }
  pollfd monitor{};
  monitor.fd = sockfd;
  monitor.events = POLLIN;
  monitor.revents = 0;
  clients_fds.push_back(monitor);
}

Server::~Server() { close(sockfd); }

void Server::start_listening() {
  if (listen(sockfd, 5) == -1) {
    throw std::runtime_error("listen() failed.");
  }
}

void Server::accept_new_client() {
  // Create a socket and a pollfd for the new client.
  // Ask the kernel to check POLLIN event.
  // Add it to the list so it will be watched at the next poll.
  int clientSocket{accept(getfd(), nullptr, nullptr)};
  if (clientSocket == -1) {
    std::cerr << "An error happended when accepting new client: "
              << std::strerror(errno) << "\n";
    return;
  }
  std::cout << "New connection request." << "\n";
  pollfd client{};
  client.fd = clientSocket;
  client.events = POLLIN;
  client.revents = 0;
  clients_fds.push_back(client);
}

bool Server::receive_msg(size_t i) {
  ssize_t check{0};
  char buffer[1024]{0};
  bool is_client_alive{true};

  // Check contains the number of bytes received.
  check = recv(clients_fds[i].fd, buffer, sizeof(buffer), 0);
  if (check == 0) {
    std::cout << "Client " << clients_fds[i].fd << " disconnected properly."
              << "\n";
    is_client_alive = false;
  } else if (check < 0) {
    std::cout << "Client: " << clients_fds[i].fd
              << " Error on recv: " << std::strerror(errno) << "\n";
    is_client_alive = false;
  } else {
    // Read only the number of bytes received
    std::string command{buffer, (size_t)check};
    if (command == "QUIT") {
      std::cout << "The player " << clients_fds[i].fd << " leave the game.\n";
      is_client_alive = false;
    } else {
      std::cout << "|" << command << "|" << "\n";
    }
  }
  return (is_client_alive);
}

void Server::run() {
  bool run{true};
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
          accept_new_client();
        } else {
          if (!receive_msg(i)) {
            close(clients_fds[i].fd);
            clients_fds.erase(clients_fds.begin() + i);
            i--;
          }
        }
      }
    }
  }
}

int main(void) {
  try {
    Server server{};
    server.start_listening();
    server.run();
  } catch (std::exception &e) {
    std::cout << "An error occurred: " << e.what() << "\n";
    std::cout << "Reason: " << strerror(errno) << "\n";
  }

  return 0;
}
