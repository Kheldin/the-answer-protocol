#pragma once
#include "raylib.h"
#include "gui/item.hpp"
#include "gui/asset_manager.hpp"
#include <string>
#include <vector>

class Inventory {
public:
    Inventory(AssetManager manager);

    void Toggle();
    bool IsOpen() const;
    void SetOpen(bool open);

    void AddItem(const Item& newItem);
    void RemoveItem(int index, int quantity = 1);

    std::string Update(Vector2 mousePos, int& playerHP, int& bonusDamage);
    void Draw(int screenWidth, int screenHeight) const;

private:
    bool isOpen = false;
    int selectedIndex = -1;
    std::vector<Item> _items;

    static constexpr int COLS = 4;
    static constexpr int ROWS = 3;
    static constexpr int MAX_SLOTS = COLS * ROWS;
};