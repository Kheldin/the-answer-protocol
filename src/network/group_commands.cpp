#include "network/network.hpp"
#include "utils/logger.hpp"
#include <string_view>
#include <sys/socket.h>

void Server::group(Client &client, Command &command) {
  if (command.payload.starts_with("CREATE ")) {
    group_create(client, command);
  } else {
    send_unknown_command(client);
  }
}

void Server::group_create(Client &client, Command &command) {
  static constexpr std::string_view already_taken{
      "ERR204 GROUP_NAME_ALREADY_TAKEN"};
  std::string_view name{
      command.payload.substr(7)}; // 7 = "CREATE".size() + whitespace
  for (Group group : groups) {
    if (group.name == name) {
      ::send(client.fd, already_taken.data(), already_taken.size(),
             MSG_DONTWAIT | MSG_NOSIGNAL);
      logger.log(LogLevel::Warning, "Player {}: {}", client.player.username,
                 already_taken);
      return;
    }
  }
  std::string res{"OK group="};
  res += name;
  groups.emplace_back(name);
  ::send(client.fd, res.data(), res.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
  logger.log(LogLevel::Info, "Player {}: {}", client.player.username, res);
}
