#include "network/network.hpp"
#include "utils/logger.hpp"
#include <atomic>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <poll.h>
#include <print>
#include <string>
#include <string_view>
#include <sys/poll.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

static constexpr std::array<std::string_view, 5> COMMANDS{
    "CONNECT", "LOOK", "QUIT", "WHO", "GROUP"};
std::atomic<bool> is_running{false};
Server::Server() {
  // AF_INET: IPv4 protocol
  // SOCK_STREAM: 2-way connection-based byte streams
  errno = 0;
  int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
  clients_poll.emplace_back(monitor);
  add_client(monitor.fd);
}

Server::~Server() {
  errno = 0;
  for (pollfd sock : clients_poll) {
    if (close(sock.fd) == -1) {
      std::println(std::cerr, "An error happended when closing fd {}.",
                   sock.fd);
      std::println(std::cerr, "{}", strerror(errno));
    }
  }
}

void Server::add_client(int fd) { clients.try_emplace(fd, fd); }
void Server::remove_client(int fd) {
  // Delete le fd des groupes aussi
  // Et je dois delete aussi dans le clients_poll je pense ?
  clients.erase(fd);
}

void Server::start_listening() {
  errno = 0;
  if (listen(clients_poll.at(0).fd, 5) == -1) {
    throw std::runtime_error("listen() failed.");
  }
  logger.log(LogLevel::Info, "Server is listening.");
  is_running = true;
}

void Server::accept_new_client() {
  // Create a socket and a pollfd for the new client.
  // Ask the kernel to check POLLIN event.
  // Add it to the list so it will be watched at the next poll.
  errno = 0;
  int clientSocket{accept(clients_poll.at(0).fd, nullptr, nullptr)};
  if (clientSocket == -1) {
    const int err = errno;
    logger.log(LogLevel::Critical,
               "An error happened when accepting new client: {}",
               std::generic_category().message(err));
    return;
  }
  logger.log(LogLevel::Info, "New connection request.");
  pollfd pollstruct{};
  pollstruct.events = POLLIN;
  pollstruct.revents = 0;
  pollstruct.fd = clientSocket;
  add_client(clientSocket);
  clients_poll.emplace_back(pollstruct);

  static constexpr std::string_view greeting = "OK hello proto=1\n";
  ::send(clientSocket, greeting.data(), greeting.size(),
         MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "New client accepted.");
}

std::string_view Command::extract_command_type(std::string_view buffer) {
  size_t index{buffer.find_first_of(' ')};
  int i{0};
  if (index == std::string::npos) {
    index = buffer.size() - 1;
  }
  for (std::string_view command : COMMANDS) {
    if (command == buffer.substr(0, index)) {
      return (COMMANDS[i]);
    }
    i++;
  }
  return "unknown";
}

std::string_view Command::extract_command_payload(std::string_view buffer) {
  size_t index{buffer.find_first_of(' ')};
  if (index == std::string::npos) {
    return ("");
  }
  return (buffer.substr(index + 1, buffer.find_first_of('\n') - index - 1));
}

void Server::send_unknown_command(Client &client) {
  static constexpr std::string_view unknown_command{
      "500 Unknown or malformed command."};
  ::send(client.fd, unknown_command.data(), unknown_command.size(),
         MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Warning, "{}", unknown_command);

  // Erase only the unknown_command from the buffer.
  if (client.buffer.contains('\n')) {
    client.buffer.erase(0, client.buffer.find_first_of('\n') + 1);
    return;
  }
  // Erase the whole buffer if it's full without \n
  client.buffer.erase();
}

void Server::process_msg(Client &client) {
  if (client.buffer.contains(
          "\n")) { // will be a while when we remove the command
    // Check if the commands until the first \n is authorized and if its the
    // case, dispatch it to the actual function handling it.
    // And remove the command from the buffer.
    // And do this WHILE there is \n in the buffer.
    Command command{client.buffer};
    if (command.type == "unknown") {
      send_unknown_command(client);
    }
    // if (!has_only_alpha(command.type)) {
    //   send_unknown_command(client);
    //   return;
    // }
    dispatch_command(client, command);
    client.buffer.erase(0, client.buffer.find_first_of('\n') + 1);
  }
  // If there is no authorized command in the buffer:
  // If the buffer is full, send err msg to client
  // Else: do nothing and wait.
  else {
    // if the client's buffer is already full and there is not the delimiter,
    // the command is unknown
    if (client.buffer.size() > BUFFER_SIZE) {
      send_unknown_command(client);
    }
  }
  logger.log(LogLevel::Debug, "Buffer of client after process last msg: {}",
             client.buffer);
  return;
}

// Return false if the client terminated the connection
bool Server::receive_msg(int fd) {
  ssize_t check{0};
  char buffer[BUFFER_SIZE]{0};
  Client &client{clients.at(fd)};
  // Check contains the number of bytes received.
  errno = 0;
  check = ::recv(clients.at(fd).fd, buffer, sizeof(buffer), MSG_DONTWAIT);
  if (check == 0) {
    logger.log(LogLevel::Info, "Player {} disconnected properly.",
               client.player.username);
    if (!client.player.group.empty()) {
      groups.at(client.player.group).members_fd.erase(fd);
    }
    client.state = ConState::Terminated;
    return (false);
  } else if (check < 0) {
    // We have to check these errors even if check event with poll() because of
    // spurious wakeup
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return (true);
    }
    logger.log(LogLevel::Error, "Client: {}.", fd);
    logger.log(LogLevel::Error, "{}", std::strerror(errno));
    return (false);
  } else {
    client.buffer.append(buffer, check);
    process_msg(clients.at(fd));
  }
  // Print every fd in all groups
  // for (const auto &[name, group] : groups) {
  //   std::println("{}", group.members_fd);
  // }
  return (true);
}

void Server::run() {
  while (is_running) {
    // std::vector::data() return a pointer to the first elem of the
    // container. poll() is a system call. The kernel will fill the
    // pollfd.revents with what happended. poll() return:
    //  The number of fds that received an events/errors.
    //  0 means the system calls timed out before any fds is ready.
    //  -1 = errors and errno is set to indicate the error.
    // nb: Explain in readMe the choice of poll instead of epoll.
    errno = 0;
    if (::poll(clients_poll.data(), clients_poll.size(), -1) < 0) {
      if (errno == EINTR) {
        logger.log(LogLevel::Warning,
                   "A signal occurred before any requested event");
      } else {
        const int err = errno;
        logger.log(LogLevel::Critical, "Error on poll(): {}",
                   std::generic_category().message(err));
        is_running = false;
      }
    }

    // We go through the list to see which fd moved
    for (size_t i{0}; i < clients_poll.size(); ++i) {
      const int err{errno};
      if (clients_poll[i].revents & (POLLERR | POLLNVAL | POLLHUP)) {
        if (clients_poll[i].revents & POLLHUP) {
          logger.log(LogLevel::Info, "Connection terminated for fd {}",
                     clients_poll[i].fd);
        } else {
          logger.log(LogLevel::Critical, "Error on fd {}: {}",
                     clients_poll[i].fd, std::generic_category().message(err));
        }
        close(clients_poll[i].fd);
        clients_poll.erase(clients_poll.begin() + i);
        remove_client(clients_poll[i].fd);
        i--;
      } else if (clients_poll[i].revents & POLLIN) {
        // The index 0 is the server socket.
        // If the server socket is readable, it means that a client wants to
        // connect to the server.
        if (i == 0) {
          accept_new_client();
        } else {
          if (!receive_msg(clients_poll.at(i).fd)) {
            close(clients_poll[i].fd);
            clients_poll.erase(clients_poll.begin() + i);
            remove_client(clients_poll[i].fd);
            i--;
          }
        }
      }
    }
  }
}

void handle_sigint(int) {
  is_running = false;
  const char *msg = "\nCTRL+C CAPTURED: Stopping the server cleanly.\n";
  write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void) {
  // Handle CTRL+C
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &handle_sigint;
  sigfillset(&sa.sa_mask);
  if (sigaction(SIGINT, &sa, nullptr) == -1) {
    std::cerr << "sigaction error" << "\n";
    return (EXIT_FAILURE);
  }
  // try {
  Server server{};
  server.start_listening();
  server.run();
  return (EXIT_SUCCESS);
  // } catch (std::exception &e) {
  //   std::println(std::cerr, "A standard exception was caught, with message:
  //   {}",
  //                e.what());
  // }
}
