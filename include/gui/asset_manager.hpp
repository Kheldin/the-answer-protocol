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
    Texture2D getShopTexture() { return shop_texture; }
    Texture2D getFightTexture() { return fightTexture; }
    Texture2D getLeftFightTexture() { return left_fightTexture; }
    Texture2D getRightFightTexture() { return right_fightTexture; }
    
    Texture2D getWheyTexture() { return wheyTexture; }
    Texture2D getCreatineTexture() { return creatineTexture; }
    Texture2D getTestoTexture() { return testoTexture; }
    Texture2D getSupplementsTexture() { return supplementsTexture; }

private:
    std::unordered_map<std::string, Texture2D> player_textures {};

    Texture2D buttonsTexture {};
    Texture2D shop_texture {};
    Texture2D fightTexture {};
    Texture2D left_fightTexture {};
    Texture2D right_fightTexture {};
    
    Texture2D wheyTexture {};
    Texture2D testoTexture {};
    Texture2D supplementsTexture {};
    Texture2D creatineTexture {};

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