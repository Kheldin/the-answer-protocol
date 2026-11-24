#pragma once

#include "raylib.h"
#include "gui/item.hpp"
#include "gui/button.hpp"
#include <string>

enum class Rooms {
    Reception,
    LockerRoom,
    Muscu,
    Cardio,
    Pool,
    Climbing,
    Sauna,
    Masage
};

struct WorldItem {
    Rooms room;
    std::string name;
    ItemType type;
    Texture2D texture;
    int value;
    Vector2 pos;
    InteractiveButton pickupButton;
};

struct RemotePlayer {
    std::string username;
    Rooms room;
    Vector2 pos;
};

inline std::string roomToProtocolName(Rooms r) {
    switch (r) {
        case Rooms::Reception:  return "reception";
        case Rooms::LockerRoom: return "locker_room";
        case Rooms::Muscu:      return "muscu";
        case Rooms::Cardio:     return "cardio";
        case Rooms::Pool:       return "pool";
        case Rooms::Climbing:   return "climbing";
        case Rooms::Sauna:      return "sauna";
        case Rooms::Masage:     return "massage";
    }
    return "reception";
}