#pragma once

#include "raylib.h"
#include "gui/asset_manager.hpp"
#include "gui/inventory.hpp"
#include "gui/quest_log.hpp"
#include "gui/chat_box.hpp"
#include "gui/mini_games.hpp"
#include "gui/world_controller.hpp"
#include "gui/game_types.hpp"
#include "player.hpp"
#include <vector>
#include <string>
#include <memory>

class GameApp {
public:
    void Init();
    void Update(float dt);
    void Draw() ;
    void Shutdown();

    void sendNetworkCommand(const std::string& cmd);
    void spawnDroppedItem(const std::string& rawName, Rooms room, Vector2 dropPos);
    void upsertRemotePlayer(const std::string& username, Rooms room, Vector2 pos);
    void removeRemotePlayer(const std::string& username);
    ChatBox& getChatBox() { return _chatBox; }

private:
    int _screenWidth{800};
    int _screenHeight{450};

    AssetManager _manager;
    std::unique_ptr<Inventory> _inventory;
    Player _player{"Antoine", 100};
    QuestLog _questLog;
    ChatBox _chatBox;
    MiniGames _miniGames;
    WorldController _world;

    std::vector<WorldItem> _droppedItems;
    std::vector<RemotePlayer> _otherPlayers;
    float _lowEnergyTimer{0.0f};
};