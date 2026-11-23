#include "player.hpp"
#include <algorithm>

Player::Player(const std::string& name, int maxEnergy)
    : username(name),
      _energy(maxEnergy),
      _maxEnergy(maxEnergy),
      _level(1),
      _xp(0),
      _bonusDamage(0) {}

void Player::consumeEnergy(int amount) {
    if (amount <= 0) return;
    _energy = std::max(0, _energy - amount);
}

void Player::restoreEnergy(int amount) {
    if (amount < 0) return;
    _energy = std::min(_maxEnergy, _energy + amount);
}

void Player::addXp(int amount) {
    if (amount <= 0) return;
    _xp += amount;

    while (_xp >= xpForNextLevel()) {
        _xp -= xpForNextLevel();
        _level++;
        _maxEnergy += 10;
    }
}