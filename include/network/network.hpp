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
  void run();
  void handle_events(); // Not implemented yet: basically a switch case for each
                        // Poll event constant.
  pollfd monitor{};
  std::vector<pollfd> clients_fds{};
  int sockfd{-1};

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
