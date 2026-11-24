#include "gui/game_app.hpp"
#include "gui/hud_renderer.hpp"

void GameApp::Init() {
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
}

void GameApp::sendNetworkCommand(const std::string& cmd) {
    if (cmd.empty()) return;

    if (cmd.rfind("say ", 0) == 0) {
        _chatBox.AddMessage(_player.getName(), cmd.substr(4), WHITE);
    } else {
        _chatBox.AddSystemMessage("CMD -> " + cmd);
    }
    // TODO (branche réseau) : envoyer `cmd` au serveur TCP
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
    if (_lowEnergyTimer > 0.0f) _lowEnergyTimer -= dt;

    auto sendCb = [this](const std::string& c) { sendNetworkCommand(c); };

    // 1. Chat
    bool canOpenChat = !_miniGames.IsAnyActive() && !_inventory->IsOpen() && !_questLog.IsOpen();
    std::string chatCmd = _chatBox.Update(canOpenChat);
    if (!chatCmd.empty()) sendNetworkCommand(chatCmd);

    bool isChatFocused = _chatBox.IsFocused();
    bool canOpenMenu = !canOpenChat ? false : (!isChatFocused && !_world.IsTalking());

    // 2. Ouverture Inventaire / Quêtes
    if (canOpenMenu && !_questLog.IsOpen() && IsKeyPressed(KEY_I)) _inventory->Toggle();
    if (canOpenMenu && !_inventory->IsOpen() && IsKeyPressed(KEY_Q)) _questLog.Toggle();

    // 3. Modales prioritaires (Inventaire, Quêtes, Mini-jeux)
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

    // 4. Monde & Déplacements
    bool canMoveInWorld = !isChatFocused;
    _world.Update(_screenWidth, _screenHeight, canMoveInWorld, _player, *_inventory, _questLog, _miniGames, _droppedItems, _lowEnergyTimer, sendCb);
}

void GameApp::Draw() {
    Rooms currentRoom = _world.GetCurrentRoom();
    Texture2D buttonTexture = _manager.getButtonsTextures();

    _world.DrawRoom(_screenWidth, _screenHeight);

    // Items au sol
    for (auto& wItem : _droppedItems) {
        if (wItem.room == currentRoom) {
            DrawTextureEx(wItem.texture, wItem.pos, 0.0f, 0.25f, WHITE);
            wItem.pickupButton.Draw(buttonTexture, _world.GetButtonFrameWidth(), _world.GetButtonFrameHeight(), _world.GetCropOffset());
        }
    }

    // Joueurs distants
    HudRenderer::DrawRemotePlayers(_otherPlayers, currentRoom, _manager.getPlayerTextures().at("guy_front"), _world.GetPlayerScale());

    // Joueur local & dialogues
    _world.DrawPlayerAndDialogs(_screenWidth, _screenHeight, _miniGames.IsExercising(), !_miniGames.IsAnyActive());

    // Mini-jeux (Shop / Exercice / Combat)
    _miniGames.Draw(_screenWidth, _screenHeight, _player);

    // HUD fixe
    HudRenderer::DrawLowEnergyWarning(_screenWidth, _lowEnergyTimer);
    HudRenderer::DrawPlayerHud(_player);
    HudRenderer::DrawControlsBox(_screenHeight);

    _questLog.DrawTracker(_screenWidth);
    _chatBox.Draw(_screenWidth, _screenHeight);
    _inventory->Draw(_screenWidth, _screenHeight);
    _questLog.DrawJournal(_screenWidth, _screenHeight);
}

void GameApp::Shutdown() {
    _manager.UnloadAll();
    CloseWindow();
}