#pragma once

#include "raylib.h"
#include "player.hpp"
#include "gui/game_types.hpp"
#include <vector>

class HudRenderer {
public:
    static void DrawPlayerHud(const Player& player);
    static void DrawControlsBox(int screenHeight);
    static void DrawLowEnergyWarning(int screenWidth, float timer);
    static void DrawRemotePlayers(const std::vector<RemotePlayer>& players, Rooms currentRoom, Texture2D tex, float scale);
};