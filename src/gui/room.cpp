#include "raylib.h"
#include "gui/room.hpp"

Room::Room(std::unordered_map<std::string, Texture2D> room_textures,
        std::unordered_map<std::string, Vector2> textures_positions,
        std::vector<Rectangle> room_hitboxes)
    : textures(room_textures), tpositions(textures_positions), hitboxes(room_hitboxes) 
{}

void Room::draw() {
    auto bg_it = textures.find("background");
    if (bg_it != textures.end()) {
        Texture2D bg = bg_it->second;
        Rectangle source = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
        Rectangle dest = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(bg, source, dest, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    for (auto& [t_name, texture] : textures) {
        if (t_name == "background") continue;

        auto it = tpositions.find(t_name);
        if (it != tpositions.end()){
            DrawTextureEx(texture, it->second, 0.0f, 0.2f, WHITE);
        }
    }
}