#include "network/network.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <cctype>
#include <print>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

void Server::dispatch_command(Client &client, Command &command) {
  std::println("command type {} payload: {}", command.type, command.payload);
  if (command.type == "CONNECT") {
    connect(client, command);
  } else if (command.type == "QUIT") {
    quit(client);
  }
}

void Server::connect(Client &client, Command &command) {
  static constexpr std::string_view name_used{"ERR 201 NAME_IN_USE"};
  static constexpr std::string_view space_in_name{"ERR 202 WHITESPACE_IN_NAME"};
  static constexpr std::string_view connected{"OK connected"};
  // This part may be optimized with std::find or a binary search if we sort the
  // vector by username.
  // I should check if there is anything else than alnum in the payload instead
  // of just space.
  if (has_whitespace(command.payload)) {
    ::send(client.fd, space_in_name.data(), space_in_name.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Error, "File descriptor {}: {}", client.fd,
               space_in_name);
    return;
  }
  for (Client p_client : clients) {
    if (p_client.player.username == command.payload) {
      ::send(client.fd, name_used.data(), name_used.size(),
             MSG_DONTWAIT | MSG_NOSIGNAL);
      logger.log(LogLevel::Error, "File descriptor {}: {}", client.fd,
                 name_used);
      return;
    }
  }
  client.player.username = command.payload;
  ::send(client.fd, connected.data(), connected.size(),
         MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "{}", connected);
  client.state = ConState::Authenticated;
  logger.log(LogLevel::Debug, "{}", client.player.username);
}

void Server::quit(Client &client) {
  static constexpr std::string_view bye{"OK bye"};
  ::send(client.fd, bye.data(), bye.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "File descriptor {}: {}", client.fd, bye);
}
