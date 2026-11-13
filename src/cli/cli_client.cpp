#include "cli.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// --- CLI CLASS IMPLEMENTATION ---

Cli::Cli(const std::string& ip, int port) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cerr << "Error: socket creation failed.\n";
        return;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error: connection to server failed.\n";
        close(fd);
        fd = -1;
    } else {
        std::cout << "Connected to server " << ip << ":" << port << "\n";
        listenerThread = std::thread(&Cli::listenToServer, this);
    }
}

Cli::~Cli() {
    isRunning = false;
    if (fd != -1) {
        close(fd);
    }
    if (listenerThread.joinable()) {
        listenerThread.join();
    }
}

void Cli::run() {
    std::string input;
    while (isRunning) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, input)) break;

        if (!input.empty()) {
            parseCommand(input);
        }
    }
}

void Cli::parseCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;

    std::string args;
    std::getline(iss >> std::ws, args);

    if (cmd == "connect") cmdConnect(args);
    else if (cmd == "look") cmdLook(args);
    else if (cmd == "move") cmdMove(args);
    else if (cmd == "chat") cmdChat(args);
    else if (cmd == "take") cmdTake(args);
    else if (cmd == "inventory") cmdInventory(args);
    else if (cmd == "drop") cmdDrop(args);
    else if (cmd == "talk") cmdTalk(args);
    else if (cmd == "attack") cmdAttack(args);
    else if (cmd == "status") cmdStatus(args);
    else if (cmd == "quest") cmdQuest(args);
    else if (cmd == "quests") cmdQuests(args);
    else if (cmd == "who") cmdWho(args);
    else if (cmd == "group") cmdGroup(args);
    else if (cmd == "quit" || cmd == "exit") cmdQuit();
    else {
        std::cout << "Unknown command.\n";
    }
}

void Cli::sendToServer(const std::string& msg) {
    if (fd != -1) {
        std::string packet = msg + "\n";
        send(fd, packet.c_str(), packet.length(), MSG_NOSIGNAL);
    } else {
        std::cerr << "Error: connection failed.\n";
    }
}

void Cli::listenToServer() {
    char buffer[1024];
    while (isRunning) {
        int bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "\r\033[2K" << buffer;
            std::cout << "> " << std::flush;
        }
        else if (bytesReceived == 0) {
            // FIX: Removed the backslash before the [ to fix the ANSI escape sequence
            std::cout << "\r\033[2K[Server disconnected]\n";
            isRunning = false;
            break;
        }
        else {
            break;
        }
    }
}

void Cli::cmdConnect(const std::string& args) {
    sendToServer("connect " + args);
}

void Cli::cmdLook(const std::string& args) {
    sendToServer(args.empty() ? "look" : "look " + args);
}

void Cli::cmdMove(const std::string& args) {
    if (args.empty()) std::cout << "Please specify a direction (e.g., move north).\n";
    else sendToServer("move " + args);
}

void Cli::cmdChat(const std::string& args) {
    if (args.empty()) std::cout << "Please specify a message (e.g., chat hello!).\n";
    else sendToServer("chat " + args);
}

void Cli::cmdTake(const std::string& args) {
    if (args.empty()) std::cout << "Please specify an item (e.g., take key).\n";
    else sendToServer("take " + args);
}

void Cli::cmdInventory(const std::string& args) {
    (void)args;
    sendToServer("inventory");
}

void Cli::cmdDrop(const std::string& args) {
    if (args.empty()) std::cout << "Please specify an item (e.g., drop key).\n";
    else sendToServer("drop " + args);
}

void Cli::cmdTalk(const std::string& args) {
    if (args.empty()) std::cout << "Please specify an NPC (e.g., talk guard).\n";
    else sendToServer("talk " + args);
}

void Cli::cmdAttack(const std::string& args) {
    if (args.empty()) std::cout << "Please specify a target (e.g., attack goblin).\n";
    else sendToServer("attack " + args);
}

void Cli::cmdStatus(const std::string& args) {
    (void)args;
    sendToServer("status");
}

void Cli::cmdQuest(const std::string& args) {
    sendToServer(args.empty() ? "quest" : "quest " + args);
}

void Cli::cmdQuests(const std::string& args) {
    (void)args;
    sendToServer("quests");
}

void Cli::cmdWho(const std::string& args) {
    (void)args;
    sendToServer("who");
}

void Cli::cmdGroup(const std::string& args) {
    sendToServer(args.empty() ? "group" : "group " + args);
}

void Cli::cmdQuit() {
    std::cout << "Disconnecting...\n";
    sendToServer("quit");
    isRunning = false;
}

int main(int argc, char** argv) {
    std::string ip = "127.0.0.1";
    int port = 8080;

    if (argc == 3) {
        ip = argv[1];
        try {
            port = std::stoi(argv[2]);
        } catch (const std::exception&) {
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