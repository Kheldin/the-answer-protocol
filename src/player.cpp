#include "player.hpp"
#include <algorithm>

Player::Player(const std::string& name, int maxHp)
    : username(name),
      _hp(maxHp),
      _maxHp(maxHp),
      _level(1),
      _xp(0),
      _bonusDamage(0) {}

void Player::takeDamage(int amount) {
    if (amount <= 0) return;
    _hp = std::max(0, _hp - amount);
}

void Player::heal(int amount) {
    if (amount < 0) return;
    _hp = std::min(_maxHp, _hp + amount);
}

void Player::addXp(int amount) {
    if (amount <= 0) return;
    _xp += amount;

    while (_xp >= xpForNextLevel()) {
        _xp -= xpForNextLevel();
        _level++;
        _maxHp += 10;
        _hp = _maxHp;
    }
}