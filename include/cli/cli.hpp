#pragma once

#include <string>
#include <thread>
#include <atomic>

class Cli {
public:
    Cli(const std::string& ip, int port);
    ~Cli();
    void run();

private:
    int fd{-1};
    std::atomic<bool> isRunning{true};
    std::thread listenerThread;
    
    void parseCommand(const std::string& input);
    void sendToServer(const std::string& msg);
    void listenToServer();
    
    void cmdConnect(const std::string& args);
    void cmdLook(const std::string& args);
    void cmdMove(const std::string& args);
    void cmdChat(const std::string& args);
    void cmdTake(const std::string& args);
    void cmdInventory(const std::string& args);
    void cmdDrop(const std::string& args);
    void cmdTalk(const std::string& args);
    void cmdAttack(const std::string& args);
    void cmdStatus(const std::string& args);
    void cmdQuest(const std::string& args);
    void cmdQuests(const std::string& args);
    void cmdWho(const std::string& args);
    void cmdGroup(const std::string& args);
    void cmdQuit();
};