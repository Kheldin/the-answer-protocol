#include "network/network.hpp"
#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include <cctype>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

void Server::dispatch_command(Client &client, Command &command) {
  logger.log(LogLevel::Debug, "{} {} ", command.type, command.payload);
  if (command.type != "CONNECT" && client.state != ConState::Authenticated) {
    send_not_auth_err(client.fd);
    return;
  }
  if (command.type == "CONNECT") {
    connect(client, command);
  } else if (command.type == "QUIT") {
    quit(client, command);
  } else if (command.type == "WHO") {
    who(client, command);
  } else if (command.type == "GROUP") {
    group(client, command);
  } else {
    send_unknown_command(client);
  }
  return;
}

void Server::send_not_auth_err(int fd) {
  static constexpr std::string_view err{"ERR 205 NOT_AUTHENTICATED"};
  ::send(fd, err.data(), err.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Warning, "File descriptor {}: {}", fd, err);
}

void Server::connect(Client &client, Command &command) {
  static constexpr std::string_view name_used{"ERR 201 NAME_IN_USE"};
  static constexpr std::string_view space_in_name{"ERR 202 WHITESPACE_IN_NAME"};
  static constexpr std::string_view empty_payload{"ERR 204 EMPTY_NAME"};
  static constexpr std::string_view already_connected{
      "ERR 203 ALREADY_CONNECTED"};
  static constexpr std::string_view connected{"OK connected"};
  if (client.state == ConState::Authenticated) {
    ::send(client.fd, already_connected.data(), already_connected.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Warning, "Player {}: {}", client.player.username,
               already_connected);
  }
  if (command.payload == "") {
    ::send(client.fd, empty_payload.data(), empty_payload.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Warning, "File descriptor {}: {}", client.fd,
               empty_payload);
    return;
  };
  if (has_whitespace(command.payload)) {
    ::send(client.fd, space_in_name.data(), space_in_name.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Warning, "File descriptor {}: {}", client.fd,
               space_in_name);
    return;
  }
  for (const auto &[fd, p_client] : clients) {
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
  logger.log(LogLevel::Info, "Player {}: {}", client.player.username,
             connected);
  client.state = ConState::Authenticated;
}

void Server::quit(Client &client, Command &command) {
  if (!command.payload.empty()) {
    send_unknown_command(client);
  }
  static constexpr std::string_view bye{"OK bye"};
  ::send(client.fd, bye.data(), bye.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: {}", client.player.username, bye);
}

void Server::who(Client &client, Command &command) {
  if (!command.payload.empty()) {
    send_unknown_command(client);
    return;
  }
  std::string res{"OK players="};
  res += std::to_string(clients.size() - 1);
  ::send(client.fd, res.data(), res.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: {}", client.player.username, res);
}
