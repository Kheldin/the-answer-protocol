#pragma once

#include "raylib.h"
#include "gui/asset_manager.hpp"
#include "gui/room.hpp"
#include "gui/button.hpp"
#include "gui/game_types.hpp"
#include "gui/mini_games.hpp"
#include "gui/quest_log.hpp"
#include "gui/inventory.hpp"
#include "player.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

class WorldController {
public:
    void Init(AssetManager& manager, int screenWidth, int screenHeight);

    Rooms GetCurrentRoom() const { return _currentRoom; }
    float GetPlayerScale() const { return _scale; }
    Vector2 GetPlayerPos() const { return { _destRec.x, _destRec.y }; }
    float GetButtonFrameWidth() const { return _bframeWidth; }
    float GetButtonFrameHeight() const { return _bframeHeight; }
    float GetCropOffset() const { return _cropOffset; }
    bool IsTalking() const { return _npcButton->getIsTalking() || _showerButton->getIsTalking(); }

    void Update(
        int screenWidth,
        int screenHeight,
        bool canInteract,
        Player& player,
        Inventory& inventory,
        QuestLog& questLog,
        MiniGames& miniGames,
        std::vector<WorldItem>& droppedItems,
        float& lowEnergyTimer,
        const MiniGames::CommandCallback& sendCmd
    );

    void DrawRoom(int screenWidth, int screenHeight) const;
    void DrawPlayerAndDialogs(int screenWidth, int screenHeight, bool hidePlayer, bool drawDialogs) const;

private:
    Rectangle GetFeetHitbox(float x, float y) const;
    void HandleRoomTransitions();

    std::unordered_map<std::string, Texture2D> _playerTextures;
    Texture2D _currentGuy{};
    Texture2D _buttonsTex{};
    float _bframeWidth{0.0f};
    float _bframeHeight{0.0f};
    float _cropOffset{3.0f};

    std::unique_ptr<Room> _reception, _lockerRoom, _muscu, _cardio, _pool, _climbing, _sauna, _masage;
    std::unique_ptr<InteractiveButton> _npcButton, _shopButton, _fightButton, _showerButton;
    std::unique_ptr<InteractiveButton> _pressButton, _benchButton, _bikeButton, _treadmillButton;

    Rooms _currentRoom{Rooms::Reception};
    Rooms _previousRoom{Rooms::Reception};
    bool _wasShowering{false};

    Rectangle _frameRec{};
    Rectangle _destRec{};
    bool _frameFaces{true};
    int _currentFrame{0};
    int _framesCounter{0};
    int _framesSpeed{8};
    float _speed{15.0f};
    float _scale{0.6f};
};