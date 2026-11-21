#pragma once
#include "utils/logger.hpp"
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define BUFFER_SIZE 22 // 1023 "effective" bytes to keep the 1024th for the \0

enum class ConState { Disconnected, Connected, Authenticated, Terminated };

class Player {
public:
  std::string username{};

private:
};

class Client {
public:
  Client(int p_fd) {
    fd = p_fd;
    buffer.reserve(BUFFER_SIZE);
  }
  // Client(const Client &) = delete;
  // Client &operator=(const Client &) = delete;
  // Client(Client &&) noexcept = default;
  // Client &operator=(Client &&) noexcept = default;
  int fd{};
  std::string buffer{};
  ConState state{ConState::Connected};
  Player player{};

private:
};

class Command {
public:
  Command(std::string_view command) {
    type = extract_command_type(command);
    payload = extract_command_payload(command);
  }
  std::string_view type{};
  std::string_view payload{};
  std::string_view extract_command_type(std::string_view buffer);
  std::string_view extract_command_payload(std::string_view buffer);

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
  void process_msg(Client &client);
  void send_unknown_command(Client &client);
  void dispatch_command(Client &client, Command &command);
  void connect(Client &client, Command &command);
  void quit(Client &client);
};
