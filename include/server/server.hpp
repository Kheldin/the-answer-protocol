#pragma once
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
  pollfd monitor{};
  std::vector<pollfd> clients_fds{};
  int sockfd{-1};

private:
};

// class Monitor {
// public:
//   Monitor() = default;
//   ~Monitor();
//   int get_nb_fds() { return nb_fds; }
//   std::vector<pollfd> get_fds() { return fds; }
//   void run_monitor();
//
// private:
//   int nb_fds{};
//   std::vector<pollfd> clients{};
// };
