#pragma once

#include <string>

class Player {
public:
  Player() = default;
  Player(const std::string &name, int maxHp = 100);

  std::string username{};
  std::string group{};

  const std::string &getName() const { return username; }
  void setName(const std::string &name) { username = name; }
  int getHp() const { return _hp; }
  int getMaxHp() const { return _maxHp; }
  int getLevel() const { return _level; }
  int getXp() const { return _xp; }
  int getBonusDamage() const { return _bonusDamage; }
  bool isAlive() const { return _hp > 0; }
  void setBonusDamage(int bonus) { _bonusDamage = bonus; }

  int &hpRef() { return _hp; }
  int &bonusDamageRef() { return _bonusDamage; }

  void takeDamage(int amount);
  void heal(int amount);
  void addXp(int amount);

private:
  int _hp{100};
  int _maxHp{100};
  int _level{1};
  int _xp{0};
  int _bonusDamage{0};

  int xpForNextLevel() const { return _level * 100; }
};
