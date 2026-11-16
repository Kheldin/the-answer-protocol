#include "gui/item.hpp"
#include <algorithm>

Item::Item(const std::string& name, ItemType type, Texture2D texture, int value, int quantity)
    : _name(name), _type(type), _texture(texture), _value(value), _quantity(quantity) {}

bool Item::use(int& playerHP, int& bonusDamage) {
    if (_quantity <= 0 || !isUsable()) {
        return false;
    }

    if (_type == ItemType::CONSUMABLE) {
        playerHP = std::min(100, playerHP + _value);
    } else if (_type == ItemType::BOOSTER) {
        bonusDamage += _value;
    }

    _quantity--;
    return true;
}

void Item::addQuantity(int amount) {
    _quantity += amount;
}

void Item::removeQuantity(int amount) {
    _quantity = std::max(0, _quantity - amount);
}

const std::string& Item::getName() const { return _name; }
ItemType Item::getType() const { return _type; }
Texture2D Item::getTexture() const { return _texture; }
int Item::getValue() const { return _value; }
int Item::getQuantity() const { return _quantity; }

bool Item::isUsable() const {
    return _type == ItemType::CONSUMABLE || _type == ItemType::BOOSTER;
}