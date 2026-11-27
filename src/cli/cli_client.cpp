#include "cli/cli.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

Cli::Cli(const std::string &ip, int port) {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    std::cerr << "Error: socket creation failed.\n";
    isRunning = false;
    return;
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

  if (connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    std::cerr << "Error: connection to server failed.\nTry make-server before.\n";
    close(fd);
    fd = -1;
    isRunning = false;
  } else {
    std::cout << "Connected to server " << ip << ":" << port << "\n";
    listenerThread = std::thread(&Cli::listenToServer, this);
  }
}

Cli::~Cli() {
  isRunning = false;
  if (fd != -1) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
    fd = -1;
  }
  if (listenerThread.joinable()) {
    listenerThread.join();
  }
}

void Cli::run() {
  std::string input;
  while (isRunning) {
    std::cout << "> " << std::flush;
    if (!std::getline(std::cin, input))
      break;

    if (!input.empty()) {
      parseCommand(input);
    }
  }
}

bool Cli::popReceivedMessage(std::string &outMsg) {
  std::lock_guard<std::mutex> lock(queueMutex);
  if (incomingMessages.empty())
    return false;
  outMsg = incomingMessages.front();
  incomingMessages.pop();
  return true;
}

void Cli::parseCommand(const std::string &input) {
  std::istringstream iss(input);
  std::string cmd;
  iss >> cmd;

  std::string args;
  std::getline(iss >> std::ws, args);
  if (cmd == "CONNECT")
    cmdConnect(args);
  else if (cmd == "look")
    cmdLook(args);
  else if (cmd == "move")
    cmdMove(args);
  else if (cmd == "chat")
    cmdChat(args);
  else if (cmd == "take")
    cmdTake(args);
  else if (cmd == "inventory")
    cmdInventory(args);
  else if (cmd == "drop")
    cmdDrop(args);
  else if (cmd == "talk")
    cmdTalk(args);
  else if (cmd == "attack")
    cmdAttack(args);
  else if (cmd == "status")
    cmdStatus(args);
  else if (cmd == "quest")
    cmdQuest(args);
  else if (cmd == "quests")
    cmdQuests(args);
  else if (cmd == "WHO")
    cmdWho(args);
  else if (cmd == "GROUP")
    cmdGroup(args);
  else if (cmd == "QUIT" || cmd == "exit")
    cmdQuit();
  else {
    std::cout << "<" << cmd<< ">" << "Unknown command.\n";
  }
}

void Cli::sendToServer(const std::string &msg) {
  if (fd != -1) {
    std::string packet = msg + "\n";
    send(fd, packet.data(), packet.size(), MSG_NOSIGNAL);
  }
}

void Cli::listenToServer() {
  char buffer[1024];
  while (isRunning) {
    int bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
      buffer[bytesReceived] = '\0';
      std::cout << "\r\033[2K" << buffer << "> " << std::flush;

      std::lock_guard<std::mutex> lock(queueMutex);
      recvBuffer += buffer;
      size_t pos;
      while ((pos = recvBuffer.find('\n')) != std::string::npos) {
        std::string line = recvBuffer.substr(0, pos);
        if (!line.empty() && line.back() == '\r')
          line.pop_back();
        if (!line.empty())
          incomingMessages.push(line);
        recvBuffer.erase(0, pos + 1);
      }
    } else {
      isRunning = false;
      break;
    }
  }
}

void Cli::cmdConnect(const std::string &args) { sendToServer("CONNECT " + args); }
void Cli::cmdLook(const std::string &args)    { sendToServer(args.empty() ? "LOOK" : "LOOK " + args); }
void Cli::cmdMove(const std::string &args)    { if (!args.empty()) sendToServer("MOVE " + args); }
void Cli::cmdChat(const std::string &args)    { if (!args.empty()) sendToServer("CHAT " + args); }
void Cli::cmdTake(const std::string &args)    { if (!args.empty()) sendToServer("TAKE " + args); }
void Cli::cmdInventory(const std::string &args) { (void)args; sendToServer("INVENTORY"); }
void Cli::cmdDrop(const std::string &args)    { if (!args.empty()) sendToServer("DROP " + args); }
void Cli::cmdTalk(const std::string &args)    { if (!args.empty()) sendToServer("TALK " + args); }
void Cli::cmdAttack(const std::string &args)  { if (!args.empty()) sendToServer("ATTACK " + args); }
void Cli::cmdStatus(const std::string &args)  { (void)args; sendToServer("STATUS"); }
void Cli::cmdQuest(const std::string &args)   { sendToServer(args.empty() ? "QUEST" : "QUEST " + args); }
void Cli::cmdQuests(const std::string &args)  { (void)args; sendToServer("QUESTS"); }
void Cli::cmdWho(const std::string &args)     { (void)args; sendToServer("WHO"); }
void Cli::cmdGroup(const std::string &args)   { sendToServer(args.empty() ? "GROUP" : "GROUP " + args); }
void Cli::cmdQuit()                           { sendToServer("QUIT"); isRunning = false; }