#pragma once
#include "raylib.h"
#include <unordered_map>
#include <string>
#include <vector>

class Room {
public:
    Room(std::unordered_map<std::string, Texture2D> room_textures,
         std::unordered_map<std::string, Vector2> textures_positions,
         std::vector<Rectangle> room_hitboxes);
         
    ~Room() = default;
    
    void draw();
    
    std::vector<Rectangle> getHitboxes() { return hitboxes; }

private:
    std::unordered_map<std::string, Texture2D> textures {};
    std::unordered_map<std::string, Vector2> tpositions {};
    std::vector<Rectangle> hitboxes {};
};