#pragma once
#include "utils/logger.hpp"
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
class Server {
public:
  Server();
  ~Server();
  int getfd() const { return sockfd; }
  void start_listening();
  void run();
  std::vector<pollfd> clients_fds{};
  int sockfd{-1};
  Logger logger{"logfile"};

private:
  void accept_new_client();
  bool receive_msg(size_t i);
};

class Client {
public:
  Client();
  ~Client() { close(fd); }
  int fd{-1};

private:
};
