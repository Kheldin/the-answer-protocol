#pragma once

#include "raylib.h"
#include "gui/asset_manager.hpp"
#include "gui/inventory.hpp"
#include "gui/quest_log.hpp"
#include "player.hpp"
#include <string>
#include <functional>

enum class ExerciseType { NONE, BENCH, SQUATS, SPRINT };

class MiniGames {
public:
    using CommandCallback = std::function<void(const std::string&)>;

    void Init(AssetManager& manager);

    bool IsAnyActive() const { return _isShopOpen || _isFighting || _activeExercise != ExerciseType::NONE; }
    bool IsShopOpen() const { return _isShopOpen; }
    bool IsFighting() const { return _isFighting; }
    bool IsExercising() const { return _activeExercise != ExerciseType::NONE; }

    void OpenShop() { _isShopOpen = true; }
    void CloseShop() { _isShopOpen = false; }

    bool TryStartExercise(ExerciseType type, const std::string& questId, Player& player);
    bool TryStartFight(Player& player);

    void Update(float dt, Player& player, Inventory& inventory, QuestLog& questLog, AssetManager& manager, const CommandCallback& sendCmd);
    void Draw(int screenWidth, int screenHeight, const Player& player) const;

private:
    enum class FightState { PLAYER_WAIT, PLAYER_QTE, PLAYER_ATTACK, NPC_WAIT, NPC_ATTACK, END };

    // Shop
    Texture2D _shopTex{};
    float _shopScale{0.7f};
    bool _isShopOpen{false};
    Rectangle _item1{ 615.0f, 491.0f, 310.0f, 55.0f };
    Rectangle _item2{ 615.0f, 818.0f, 310.0f, 55.0f };
    Rectangle _item3{ 1165.0f, 491.0f, 310.0f, 55.0f };
    Rectangle _item4{ 1182.0f, 818.0f, 290.0f, 55.0f };
    float _shopClickTimers[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

    // Exercices
    Texture2D _benchTex{};
    Texture2D _squatsTex{};
    Texture2D _sprintTex{};
    ExerciseType _activeExercise{ExerciseType::NONE};
    std::string _pendingQuestId{};
    float _exerciseDuration{0.0f};
    float _exerciseSwap{0.0f};
    int _exerciseFrame{0};
    static constexpr int EXERCISE_ENERGY_COST = 25;

    // Fight QTE
    Texture2D _fightBgTex{};
    Texture2D _leftFightTex{};
    Texture2D _rightFightTex{};
    float _fightBgScale{1.5f};
    float _spriteScale{0.8f};
    bool _isFighting{false};
    int _npcEnergy{100};
    FightState _fState{FightState::PLAYER_WAIT};
    int _qteKey{0};
    float _fightTimer{0.0f};
    int _pFrame{0};
    int _nFrame{3};
};