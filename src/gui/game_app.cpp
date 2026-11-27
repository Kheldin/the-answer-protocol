#include "gui/game_app.hpp"
#include "gui/hud_renderer.hpp"
#include <iostream>
#include <sstream>

bool GameApp::Init(const std::string& ip, int port, const std::string& username) {
    _cli = std::make_unique<Cli>(ip, port);
    if (!_cli->isConnected()) {
        std::cerr << "Error: cannot connect to server " << ip << ":" << port << "\n";
        return false;
    }

    _player = Player(username, 100);

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(_screenWidth, _screenHeight, "TAP - Gym Adventure");

    int monitor = GetCurrentMonitor();
    _screenWidth = GetMonitorWidth(monitor);
    _screenHeight = GetMonitorHeight(monitor);
    SetWindowSize(_screenWidth, _screenHeight);
    ToggleFullscreen();
    SetTargetFPS(60);

    _manager.LoadAll();
    _inventory = std::make_unique<Inventory>(_manager);
    _miniGames.Init(_manager);
    _world.Init(_manager, _screenWidth, _screenHeight);

    _cli->sendToServer("CONNECT " + username);
    _cli->sendToServer("LOOK");
    _chatBox.AddSystemMessage("Connecte au serveur " + ip + ":" + std::to_string(port));
    return true;
}

void GameApp::sendNetworkCommand(const std::string& cmd) {
    if (cmd.empty()) return;

    std::string serverCmd = cmd;

    // ChatBox émet "say <msg>", le serveur attend "chat <msg>"
    if (cmd.rfind("say ", 0) == 0) {
        std::string msg = cmd.substr(4);
        _chatBox.AddMessage(_player.getName(), msg, WHITE);
        serverCmd = "chat " + msg;
    } else if (cmd.rfind("whisper ", 0) == 0) {
        serverCmd = "chat " + cmd;
    } else {
        _chatBox.AddSystemMessage("CMD -> " + serverCmd);
    }

    if (_cli && _cli->isConnected()) {
        _cli->sendToServer(serverCmd);
    }
}

Rooms GameApp::parseProtocolRoom(const std::string& roomName) {
    if (roomName == "locker_room") return Rooms::LockerRoom;
    if (roomName == "muscu")       return Rooms::Muscu;
    if (roomName == "cardio")      return Rooms::Cardio;
    if (roomName == "pool")        return Rooms::Pool;
    if (roomName == "climbing")    return Rooms::Climbing;
    if (roomName == "sauna")       return Rooms::Sauna;
    if (roomName == "massage")     return Rooms::Masage;
    return Rooms::Reception;
}

void GameApp::pollServerMessages() {
    if (!_cli) return;

    std::string line;
    while (_cli->popReceivedMessage(line)) {
        if (line.rfind("OK ", 0) == 0) {
            _chatBox.AddSystemMessage(line.substr(3));
        } else if (line.rfind("ERR ", 0) == 0) {
            _chatBox.AddSystemMessage("Erreur serveur : " + line);
        } else {
            _chatBox.AddSystemMessage(line);
        }
    }
}

void GameApp::spawnDroppedItem(const std::string& rawName, Rooms room, Vector2 dropPos) {
    ItemType type = ItemType::CONSUMABLE;
    Texture2D tex = _manager.getWheyTexture();
    int val = 25;
    std::string cleanName = "Whey Protein";

    if (rawName.find("Creatine") != std::string::npos || rawName.find("creatine") != std::string::npos) {
        cleanName = "Creatine";
        tex = _manager.getCreatineTexture();
        val = 15;
    } else if (rawName.find("Testo") != std::string::npos || rawName.find("testo") != std::string::npos) {
        cleanName = "Testo";
        type = ItemType::BOOSTER;
        tex = _manager.getTestoTexture();
        val = 25;
    } else if (rawName.find("Supplements") != std::string::npos || rawName.find("supplements") != std::string::npos) {
        cleanName = "Supplements";
        tex = _manager.getSupplementsTexture();
        val = 20;
    }

    Rectangle btnPos = { dropPos.x + 15.0f, dropPos.y - 45.0f, _world.GetButtonFrameWidth() * 0.2f, _world.GetButtonFrameHeight() * 0.2f };
    Rectangle btnZone = { dropPos.x - 60.0f, dropPos.y - 60.0f, 180.0f, 180.0f };

    _droppedItems.push_back({
        room, cleanName, type, tex, val, dropPos,
        InteractiveButton(btnPos, btnZone, "")
    });
}

void GameApp::upsertRemotePlayer(const std::string& username, Rooms room, Vector2 pos) {
    for (auto& rp : _otherPlayers) {
        if (rp.username == username) {
            rp.room = room;
            rp.pos = pos;
            return;
        }
    }
    _otherPlayers.push_back({ username, room, pos });
}

void GameApp::removeRemotePlayer(const std::string& username) {
    for (size_t i = 0; i < _otherPlayers.size(); ++i) {
        if (_otherPlayers[i].username == username) {
            _otherPlayers.erase(_otherPlayers.begin() + i);
            return;
        }
    }
}

void GameApp::Update(float dt) {
    pollServerMessages();

    if (_lowEnergyTimer > 0.0f) _lowEnergyTimer -= dt;

    auto sendCb = [this](const std::string& c) { sendNetworkCommand(c); };

    bool canOpenChat = !_miniGames.IsAnyActive() && !_inventory->IsOpen() && !_questLog.IsOpen();
    std::string chatCmd = _chatBox.Update(canOpenChat);
    if (!chatCmd.empty()) sendNetworkCommand(chatCmd);

    bool isChatFocused = _chatBox.IsFocused();
    bool canOpenMenu = !canOpenChat ? false : (!isChatFocused && !_world.IsTalking());

    if (canOpenMenu && !_questLog.IsOpen() && IsKeyPressed(KEY_I)) _inventory->Toggle();
    if (canOpenMenu && !_inventory->IsOpen() && IsKeyPressed(KEY_Q)) _questLog.Toggle();

    if (_inventory->IsOpen()) {
        std::string cmd = _inventory->Update(GetMousePosition(), _player.energyRef(), _player.bonusDamageRef());
        if (_player.getEnergy() > _player.getMaxEnergy()) _player.energyRef() = _player.getMaxEnergy();

        if (!cmd.empty()) {
            if (cmd.rfind("drop ", 0) == 0) {
                Vector2 pPos = _world.GetPlayerPos();
                spawnDroppedItem(cmd.substr(5), _world.GetCurrentRoom(), { pPos.x + 20.0f, pPos.y + 130.0f });
            }
            sendNetworkCommand(cmd);
        }
        return;
    }

    if (_questLog.IsOpen()) {
        std::string qCmd = _questLog.Update(GetMousePosition());
        if (!qCmd.empty()) sendNetworkCommand(qCmd);
        return;
    }

    if (_miniGames.IsAnyActive()) {
        _miniGames.Update(dt, _player, *_inventory, _questLog, _manager, sendCb);
        return;
    }

    bool canMoveInWorld = !isChatFocused;
    _world.Update(_screenWidth, _screenHeight, canMoveInWorld, _player, *_inventory, _questLog, _miniGames, _droppedItems, _lowEnergyTimer, sendCb);
}

void GameApp::Draw() {
    Rooms currentRoom = _world.GetCurrentRoom();
    Texture2D buttonTexture = _manager.getButtonsTextures();

    _world.DrawRoom(_screenWidth, _screenHeight);

    for (auto& wItem : _droppedItems) {
        if (wItem.room == currentRoom) {
            DrawTextureEx(wItem.texture, wItem.pos, 0.0f, 0.25f, WHITE);
            wItem.pickupButton.Draw(buttonTexture, _world.GetButtonFrameWidth(), _world.GetButtonFrameHeight(), _world.GetCropOffset());
        }
    }

    HudRenderer::DrawRemotePlayers(_otherPlayers, currentRoom, _manager.getPlayerTextures().at("guy_front"), _world.GetPlayerScale());

    _miniGames.Draw(_screenWidth, _screenHeight, _player);

    HudRenderer::DrawLowEnergyWarning(_screenWidth, _lowEnergyTimer);
    HudRenderer::DrawPlayerHud(_player);
    HudRenderer::DrawControlsBox(_screenHeight);

    _questLog.DrawTracker(_screenWidth);
    _chatBox.Draw(_screenWidth, _screenHeight);
    _inventory->Draw(_screenWidth, _screenHeight);
    _questLog.DrawJournal(_screenWidth, _screenHeight);
    _world.DrawPlayerAndDialogs(_screenWidth, _screenHeight, _miniGames.IsExercising(), !_miniGames.IsAnyActive());
}

void GameApp::Shutdown() {
    if (_cli && _cli->isConnected()) {
        _cli->sendToServer("QUIT");
    }
    _cli.reset();
    _manager.UnloadAll();
    CloseWindow();
}