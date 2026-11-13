#pragma once
#include "utils/logger.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class Client {
public:
  Client(int p_fd) { fd = p_fd; }
  int fd{};
  std::string buffer{};
  uint8_t con_status{0};

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
  bool process_msg(Client client);
};
