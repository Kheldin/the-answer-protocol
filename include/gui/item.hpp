#pragma once
#include "raylib.h"
#include <string>

enum class ItemType {
    CONSUMABLE,
    BOOSTER,
    KEY_ITEM
};

class Item {
public:
    Item(const std::string& name, ItemType type, Texture2D texture, int value = 0, int quantity = 1);

    bool use(int& playerHP, int& bonusDamage);

    void addQuantity(int amount);
    void removeQuantity(int amount);

    const std::string& getName() const;
    ItemType getType() const;
    Texture2D getTexture() const;
    int getValue() const;
    int getQuantity() const;
    bool isUsable() const;

private:
    std::string _name;
    ItemType _type;
    Texture2D _texture;
    int _value;
    int _quantity;
};