#pragma once

#include <string>

class Player {
public:
    Player() = default;
    Player(const std::string& name, int maxEnergy = 100);

    std::string username{};

    const std::string& getName() const { return username; }
    void setName(const std::string& name) { username = name; }
    int getEnergy() const { return _energy; }
    int getMaxEnergy() const { return _maxEnergy; }
    int getLevel() const { return _level; }
    int getXp() const { return _xp; }
    int getBonusDamage() const { return _bonusDamage; }
    bool hasEnergy(int cost = 1) const { return _energy >= cost; }
    void setBonusDamage(int bonus) { _bonusDamage = bonus; }

    int& energyRef() { return _energy; }
    int& bonusDamageRef() { return _bonusDamage; }

    void consumeEnergy(int amount);
    void restoreEnergy(int amount);
    void addXp(int amount);

private:
    int _energy{100};
    int _maxEnergy{100};
    int _level{1};
    int _xp{0};
    int _bonusDamage{0};

    int xpForNextLevel() const { return _level * 100; }
};