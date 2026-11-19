#include "network/network.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <cctype>
#include <string_view>
#include <sys/socket.h>

void Server::dispatch_command(Client &client, Command &command) {
  if (command.type == "CONNECT") {
    connect(client, command);
  }
}

void Server::connect(Client &client, Command &command) {
  static constexpr std::string_view name_used{"ERR 201 NAME_IN_USE"};
  static constexpr std::string_view space_in_name{"ERR 202 WHITESPACE_IN_NAME"};
  static constexpr std::string_view connected{"OK connected"};
  // This part may be optimized with std::find or a binary search if we sort the
  // vector by username.
  // I should check if there is anything else than alnum in the content instead
  // of just space.
  if (has_whitespace(command.content)) {
    ::send(client.fd, space_in_name.data(), space_in_name.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Error, "File descriptor {}: {}", client.fd,
               space_in_name);
    return;
  }
  for (Client p_client : clients) {
    if (p_client.player.username == command.content) {
      ::send(client.fd, name_used.data(), name_used.size(),
             MSG_DONTWAIT | MSG_NOSIGNAL);
      logger.log(LogLevel::Error, "File descriptor {}: {}", client.fd,
                 name_used);
      return;
    }
  }
  client.player.username = command.content;
  ::send(client.fd, connected.data(), connected.size(),
         MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "{}", connected);
  client.con_status = 1;
  logger.log(LogLevel::Debug, "{}", client.player.username);
}
