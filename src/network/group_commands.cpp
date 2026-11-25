#include "network/network.hpp"
#include "utils/logger.hpp"
#include <format>
#include <string_view>
#include <sys/socket.h>
#include <unordered_map>

void Server::group(Client &client, Command &command) {
  if (command.payload.starts_with("CREATE ")) {
    group_create(client, command);
  } else if (command.payload.starts_with("INVITE ")) {
    group_invite(client, command);
  } else {
    send_unknown_command(client);
  }
}

void Server::group_create(Client &client, Command &command) {
  static constexpr std::string_view already_taken{
      "ERR204 GROUP_NAME_ALREADY_TAKEN"};
  std::string name{
      command.payload.substr(7)}; // 7 = "CREATE".size() + whitespace
  if (groups.contains(name)) {
    ::send(client.fd, already_taken.data(), already_taken.size(),
           MSG_DONTWAIT | MSG_NOSIGNAL);
    logger.log(LogLevel::Warning, "Player {}: {}", client.player.username,
               already_taken);
    return;
  }
  std::string res{"OK group="};
  res += name;
  groups.try_emplace(name, name, client.fd);
  client.player.group = name;
  ::send(client.fd, res.data(), res.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: {}", client.player.username, res);
}

int Server::retrieve_fd_by_username(std::string_view username) {
  for (const auto [fd, client] : clients) {
    if (client.player.username == username) {
      return (fd);
    }
  }
  return (-1);
}

void Server::send_invite(Client &client, Client &target) {
  std::string msg{std::format("{} invited you to join {} group.",
                              client.player.username, client.player.group)};
  ::send(target.fd, msg.data(), msg.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: {}", target.player.username, msg);
}

void Server::group_invite(Client &client, Command &command) {
  static constexpr std::string_view player_not_connected{
      "ERR 206 PLAYER_NOT_CONNECTED"};
  static constexpr std::string_view not_in_group{"ERR 207 NOT_IN_GROUP"};

  if (client.player.group.empty()) {
    ::send(client.fd, not_in_group.data(), not_in_group.size(),
           MSG_NOSIGNAL | MSG_DONTWAIT);
    logger.log(LogLevel::Warning, "Player {}: {}", client.player.username,
               not_in_group);
    return;
  }
  int fd = retrieve_fd_by_username(command.payload);
  if (fd == -1) {
    ::send(client.fd, player_not_connected.data(), player_not_connected.size(),
           MSG_NOSIGNAL | MSG_DONTWAIT);
    return;
  }
  Client target{clients.at(fd)};
  ::send(client.fd, "OK", sizeof("OK"), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: OK", client.player.username);
  send_invite(client, target);
}
