#include "gui/asset_manager.hpp"

void AssetManager::LoadAll()
{
    player_textures["guy_left1"] = LoadTexture("src/gui/assets/normal/walk_left1.png");
    player_textures["guy_left2"] = LoadTexture("src/gui/assets/normal/walk_left2.png");
    player_textures["guy_left3"] = LoadTexture("src/gui/assets/normal/walk_left3.png");

    player_textures["guy_right1"] = LoadTexture("src/gui/assets/normal/walk_right1.png");
    player_textures["guy_right2"] = LoadTexture("src/gui/assets/normal/walk_right2.png");
    player_textures["guy_right3"] = LoadTexture("src/gui/assets/normal/walk_right3.png");
    
    player_textures["guy_front"] = LoadTexture("src/gui/assets/normal/normal_front.png");
    player_textures["guy_back"]  = LoadTexture("src/gui/assets/normal/normal_back.png");

    reception_textures["background"] = LoadTexture("src/gui/assets/background/reception.png");
    lockerroom_textures["background"] = LoadTexture("src/gui/assets/background/lockerroom.png");
    muscu_textures["background"] = LoadTexture("src/gui/assets/background/muscu.png");
    cardio_textures["background"] = LoadTexture("src/gui/assets/background/cardio.png");

    buttonsTexture = LoadTexture("src/gui/assets/interactions/touch.png");
    
    reception_hitboxes = {
        { 288.0f, 430.0f, 560.0f, 200.0f }, // sofa down
        { 0.0f, 0.0f, 10000.0f, 300.0f }, //wall up
        { 30.0f, 960.0f, 10000.0f, 300.0f }, //wall down
        { 100.0f, 300.0f, 5.0f, 800.0f }, //wall left
        { 1510.0f, 478.0f, 1000.0f, 700.0f }, // corner right down
        { 1072.0f, 295.0f, 10000.0f, 100.0f }, // corner right down
        { 104.0f, 734.0f, 720.0f, 500.0f }, // corner left down
        { 276.0f, 297.0f, 630.0f, 110.0f }, // sofa up
    };
    
    lockerroom_hitboxes = {};
    muscu_hitboxes = {};
    cardio_hitboxes = {};
}

void AssetManager::UnloadAll()
{
    for (const auto& [name, texture] : player_textures) UnloadTexture(texture);
    for (const auto& [name, texture] : reception_textures) UnloadTexture(texture);
    for (const auto& [name, texture] : lockerroom_textures) UnloadTexture(texture);
    for (const auto& [name, texture] : muscu_textures) UnloadTexture(texture);
    for (const auto& [name, texture] : cardio_textures) UnloadTexture(texture);
}