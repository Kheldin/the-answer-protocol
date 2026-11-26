#include "cli/cli.hpp"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  std::string ip = "127.0.0.1";
  int port = 8080;

  if (argc == 3) {
    ip = argv[1];
    try {
      port = std::stoi(argv[2]);
    } catch (const std::exception &) {
      std::cerr << "Error: Invalid port.\n";
      return 1;
    }
  } else if (argc != 1) {
    std::cerr << "Usage: ./cli.out [IP PORT]\n";
    return 1;
  }

  Cli client(ip, port);
  client.run();
  return 0;
}