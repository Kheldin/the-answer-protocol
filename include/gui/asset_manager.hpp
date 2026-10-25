#pragma once

#include "raylib.h"
#include <unordered_map>
#include <string>
#include <vector>

class AssetManager {
public:
    void LoadAll();
    void UnloadAll();
    
    std::unordered_map<std::string, Texture2D> getPlayerTextures() { return player_textures; }

    std::unordered_map<std::string, Texture2D> getReceptionTextures() { return reception_textures; }
    std::unordered_map<std::string, Vector2> getReceptionTexturesPositions() { return reception_tpositions; }
    std::vector<Rectangle> getReceptionHitboxes() { return reception_hitboxes; }

    std::unordered_map<std::string, Texture2D> getLockerRoomTextures() { return lockerroom_textures; }
    std::unordered_map<std::string, Vector2> getLockerRoomTexturesPositions() { return lockerroom_tpositions; }
    std::vector<Rectangle> getLockerRoomHitboxes() { return lockerroom_hitboxes; }
    
    std::unordered_map<std::string, Texture2D> getMuscuTextures() { return muscu_textures; }
    std::unordered_map<std::string, Vector2> getMuscuTexturesPositions() { return muscu_tpositions; }
    std::vector<Rectangle> getMuscuHitboxes() { return muscu_hitboxes; }
    
    std::unordered_map<std::string, Texture2D> getCardioTextures() { return cardio_textures; }
    std::unordered_map<std::string, Vector2> getCardioTexturesPositions() { return cardio_tpositions; }
    std::vector<Rectangle> getCardioHitboxes() { return cardio_hitboxes; }

    Texture2D getButtonsTextures() { return buttonsTexture; }

private:
    std::unordered_map<std::string, Texture2D> player_textures {};

    Texture2D buttonsTexture {};

    std::unordered_map<std::string, Texture2D> reception_textures {};
    std::unordered_map<std::string, Vector2> reception_tpositions {};
    std::vector<Rectangle> reception_hitboxes {};
    
    std::unordered_map<std::string, Texture2D> lockerroom_textures {};
    std::unordered_map<std::string, Vector2> lockerroom_tpositions {};
    std::vector<Rectangle> lockerroom_hitboxes {};
    
    std::unordered_map<std::string, Texture2D> muscu_textures {};
    std::unordered_map<std::string, Vector2> muscu_tpositions {};
    std::vector<Rectangle> muscu_hitboxes {};
    
    std::unordered_map<std::string, Texture2D> cardio_textures {};
    std::unordered_map<std::string, Vector2> cardio_tpositions {};
    std::vector<Rectangle> cardio_hitboxes {};

    std::unordered_map<std::string, Texture2D> backgrounds {};
};