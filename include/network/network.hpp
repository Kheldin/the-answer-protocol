#pragma once
#include "utils/logger.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define BUFFER_SIZE 1023

class Client {
public:
  Client(int p_fd) {
    fd = p_fd;
    buffer.reserve(
        BUFFER_SIZE); // 1023 "effective" bytes to keep the 1024th for the \0
  }
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;
  Client(Client &&) noexcept = default;
  Client &operator=(Client &&) noexcept = default;
  int fd{};
  std::string buffer{}; // This buffer should not exceed 1024 bytes
  uint8_t con_status{0};
  bool alive{true};

private:
};

class Server {
public:
  Server();
  ~Server();
  int getfd() const { return sockfd; }
  void start_listening();
  void run();
  std::vector<pollfd> clients_poll{};
  int sockfd{-1};
  Logger logger{"logfile"};
  std::vector<Client> clients{};

private:
  void accept_new_client();
  bool receive_msg(size_t i);
  bool process_msg(Client &client);
};
