#include "gui/mini_games.hpp"

void MiniGames::Init(AssetManager& manager) {
    _shopTex = manager.getShopTexture();

    _benchTex = manager.getbenchPositionsTexture();
    _squatsTex = manager.getsquatsPositionsTexture();
    _sprintTex = manager.getsprintPositionsTexture();
    SetTextureFilter(_benchTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(_squatsTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(_sprintTex, TEXTURE_FILTER_POINT);

    _fightBgTex = manager.getFightTexture();
    _leftFightTex = manager.getLeftFightTexture();
    _rightFightTex = manager.getRightFightTexture();
    SetTextureFilter(_leftFightTex, TEXTURE_FILTER_POINT);
    SetTextureFilter(_rightFightTex, TEXTURE_FILTER_POINT);
}

bool MiniGames::TryStartExercise(ExerciseType type, const std::string& questId, Player& player) {
    if (!player.hasEnergy(EXERCISE_ENERGY_COST)) return false;

    player.consumeEnergy(EXERCISE_ENERGY_COST);
    _activeExercise = type;
    _pendingQuestId = questId;
    _exerciseDuration = 0.0f;
    _exerciseSwap = 0.0f;
    _exerciseFrame = 0;
    return true;
}

bool MiniGames::TryStartFight(Player& player) {
    if (!player.hasEnergy(20)) return false;

    _isFighting = true;
    _npcEnergy = 100;
    _fState = FightState::PLAYER_WAIT;
    _fightTimer = 1.0f;
    _pFrame = 0;
    _nFrame = 3;
    return true;
}

void MiniGames::Update(float dt, Player& player, Inventory& inventory, QuestLog& questLog, AssetManager& manager, const CommandCallback& sendCmd) {
    if (_isShopOpen) {
        for (int i = 0; i < 4; ++i) {
            if (_shopClickTimers[i] > 0.0f) _shopClickTimers[i] -= dt;
        }

        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePos, _item1)) {
                inventory.AddItem(Item("Whey Protein", ItemType::CONSUMABLE, manager.getWheyTexture(), 25, 1));
                if (questLog.AdvanceQuest("protein_up", 1)) player.addXp(30);
                sendCmd("buy Whey Protein");
                _shopClickTimers[0] = 0.5f;
            } else if (CheckCollisionPointRec(mousePos, _item2)) {
                inventory.AddItem(Item("Creatine", ItemType::CONSUMABLE, manager.getCreatineTexture(), 15, 1));
                if (questLog.AdvanceQuest("protein_up", 1)) player.addXp(30);
                sendCmd("buy Creatine");
                _shopClickTimers[1] = 0.5f;
            } else if (CheckCollisionPointRec(mousePos, _item3)) {
                inventory.AddItem(Item("Testo", ItemType::BOOSTER, manager.getTestoTexture(), 25, 1));
                if (questLog.AdvanceQuest("protein_up", 1)) player.addXp(30);
                sendCmd("buy Testo");
                _shopClickTimers[2] = 0.5f;
            } else if (CheckCollisionPointRec(mousePos, _item4)) {
                inventory.AddItem(Item("Supplements", ItemType::CONSUMABLE, manager.getSupplementsTexture(), 20, 1));
                if (questLog.AdvanceQuest("protein_up", 1)) player.addXp(30);
                sendCmd("buy Supplements");
                _shopClickTimers[3] = 0.5f;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            _isShopOpen = false;
        }
        return;
    }

    if (_activeExercise != ExerciseType::NONE) {
        _exerciseDuration += dt;
        _exerciseSwap += dt;

        if (_exerciseSwap >= 0.5f) {
            _exerciseSwap -= 0.5f;
            _exerciseFrame = 1 - _exerciseFrame;
        }

        if (_exerciseDuration >= 6.0f) {
            if (questLog.AdvanceQuest(_pendingQuestId, 1)) {
                sendCmd("quest complete " + _pendingQuestId);
            }
            player.addXp(50);
            _activeExercise = ExerciseType::NONE;
            _pendingQuestId.clear();
        }
        return;
    }

    if (_isFighting) {
        switch (_fState) {
            case FightState::PLAYER_WAIT:
                _fightTimer -= dt;
                _pFrame = 0; _nFrame = 3;
                if (_fightTimer <= 0.0f) {
                    _qteKey = GetRandomValue(KEY_A, KEY_Z);
                    _fightTimer = 1.5f;
                    _fState = FightState::PLAYER_QTE;
                }
                break;
            case FightState::PLAYER_QTE:
                _fightTimer -= dt;
                if (_fightTimer <= 0.0f) {
                    _fightTimer = 1.0f;
                    _fState = FightState::PLAYER_ATTACK;
                    _qteKey = 0;
                } else {
                    int key = GetKeyPressed();
                    if (key != 0) {
                        if (key == _qteKey) {
                            _npcEnergy -= (35 + player.getBonusDamage());
                            _qteKey = 1;
                        } else {
                            _qteKey = 0;
                        }
                        _fightTimer = 1.0f;
                        _fState = FightState::PLAYER_ATTACK;
                    }
                }
                break;
            case FightState::PLAYER_ATTACK:
                _fightTimer -= dt;
                if (_qteKey == 1) { _pFrame = 1; _nFrame = 0; }
                else { _pFrame = 0; _nFrame = 3; }

                if (_fightTimer <= 0.0f) {
                    if (_npcEnergy <= 0) {
                        _fState = FightState::END;
                        if (questLog.AdvanceQuest("heavy_weight", 1)) {
                            sendCmd("quest complete heavy_weight");
                        }
                        player.addXp(100);
                    } else {
                        _fightTimer = 1.0f;
                        _fState = FightState::NPC_WAIT;
                    }
                }
                break;
            case FightState::NPC_WAIT:
                _fightTimer -= dt;
                _pFrame = 0; _nFrame = 3;
                if (_fightTimer <= 0.0f) {
                    _fightTimer = 1.0f;
                    _fState = FightState::NPC_ATTACK;
                }
                break;
            case FightState::NPC_ATTACK:
                _fightTimer -= dt;
                _pFrame = 3; _nFrame = 2;
                if (_fightTimer <= 0.0f) {
                    if (!player.hasEnergy(1)) _fState = FightState::END;
                    else { _fightTimer = 1.0f; _fState = FightState::PLAYER_WAIT; }
                }
                break;
            case FightState::END:
                _pFrame = player.hasEnergy(1) ? 0 : 3;
                _nFrame = (_npcEnergy > 0) ? 3 : 0;
                if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                    _isFighting = false;
                }
                break;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            _isFighting = false;
        }
    }
}

void MiniGames::Draw(int screenWidth, int screenHeight, const Player& player) const {
    if (_isShopOpen) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
        Vector2 shopPos = {
            (screenWidth / 2.0f) - ((_shopTex.width * _shopScale) / 2.0f),
            (screenHeight / 2.0f) - ((_shopTex.height * _shopScale) / 2.0f)
        };
        DrawTextureEx(_shopTex, shopPos, 0.0f, _shopScale, WHITE);

        DrawRectangleLinesEx(_item1, 2.0f, _shopClickTimers[0] > 0.0f ? RED : WHITE);
        DrawRectangleLinesEx(_item2, 2.0f, _shopClickTimers[1] > 0.0f ? RED : WHITE);
        DrawRectangleLinesEx(_item3, 2.0f, _shopClickTimers[2] > 0.0f ? RED : WHITE);
        DrawRectangleLinesEx(_item4, 2.0f, _shopClickTimers[3] > 0.0f ? RED : WHITE);
        return;
    }

    if (_activeExercise != ExerciseType::NONE) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

        Texture2D activeTex = _benchTex;
        const char* label = "BENCH PRESS";
        if (_activeExercise == ExerciseType::SQUATS) {
            activeTex = _squatsTex;
            label = "SQUATS / LEG PRESS";
        } else if (_activeExercise == ExerciseType::SPRINT) {
            activeTex = _sprintTex;
            label = "CARDIO SPRINT";
        }

        float exFrameW = activeTex.width / 2.0f;
        float exFrameH = (float)activeTex.height;
        float exScale = 1.5f;

        Rectangle exSrc = { _exerciseFrame * exFrameW, 0.0f, exFrameW, exFrameH };
        Rectangle exDest = {
            (screenWidth / 2.0f) - ((exFrameW * exScale) / 2.0f),
            (screenHeight / 2.0f) - ((exFrameH * exScale) / 2.0f),
            exFrameW * exScale,
            exFrameH * exScale
        };

        DrawTexturePro(activeTex, exSrc, exDest, { 0.0f, 0.0f }, 0.0f, WHITE);
        DrawText(label, screenWidth / 2 - MeasureText(label, 30) / 2, 80, 30, YELLOW);

        float progress = _exerciseDuration / 6.0f;
        DrawRectangle(screenWidth / 2 - 150, screenHeight - 100, 300, 20, DARKGRAY);
        DrawRectangle(screenWidth / 2 - 150, screenHeight - 100, (int)(300 * progress), 20, GREEN);
        DrawRectangleLines(screenWidth / 2 - 150, screenHeight - 100, 300, 20, WHITE);
        return;
    }

    if (_isFighting) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

        Vector2 fightpos = {
            (screenWidth / 2.0f) - ((_fightBgTex.width * _fightBgScale) / 2.0f),
            (screenHeight / 2.0f) - ((_fightBgTex.height * _fightBgScale) / 2.0f)
        };
        DrawTextureEx(_fightBgTex, fightpos, 0.0f, _fightBgScale, WHITE);

        float pFrameW = _leftFightTex.width / 4.0f;
        float pFrameH = _leftFightTex.height / 2.0f;
        float nFrameW = _rightFightTex.width / 4.0f;
        float nFrameH = _rightFightTex.height / 2.0f;

        Rectangle pSrc = { _pFrame * pFrameW, 0.0f, pFrameW, pFrameH };
        Rectangle pDest = { screenWidth / 4.0f + 150.0f - (pFrameW * _spriteScale) / 2.0f, screenHeight / 2.0f - (pFrameH * _spriteScale) / 2.0f + 50, pFrameW * _spriteScale, pFrameH * _spriteScale };
        DrawTexturePro(_leftFightTex, pSrc, pDest, { 0, 0 }, 0.0f, WHITE);

        Rectangle nSrc = { _nFrame * nFrameW, nFrameH, nFrameW, nFrameH };
        Rectangle nDest = { 3.0f * screenWidth / 4.0f - 50.0f - (nFrameW * _spriteScale) / 2.0f, screenHeight / 2.0f - (nFrameH * _spriteScale) / 2.0f + 50, nFrameW * _spriteScale, nFrameH * _spriteScale };
        DrawTexturePro(_rightFightTex, nSrc, nDest, { 0, 0 }, 0.0f, WHITE);

        DrawText(TextFormat("TON ENERGIE: %d", player.getEnergy()), screenWidth / 4 - 120, 50, 30, GOLD);
        DrawText(TextFormat("ADVERSAIRE: %d", _npcEnergy), 3 * screenWidth / 4 - 100, 50, 30, RED);

        if (_fState == FightState::PLAYER_QTE) {
            DrawText("QTE! PRESS:", screenWidth / 2 - 100, screenHeight / 2 - 150, 30, WHITE);
            DrawText(TextFormat("%c", _qteKey), screenWidth / 2 - 30, screenHeight / 2 - 100, 70, YELLOW);
            DrawRectangle(screenWidth / 2 - 100, screenHeight / 2, 200 * (_fightTimer / 1.5f), 15, ORANGE);
        } else if (_fState == FightState::PLAYER_ATTACK) {
            if (_qteKey == 1) DrawText("HIT!", screenWidth / 2 - 50, screenHeight / 2 - 100, 50, GREEN);
            else DrawText("MISS!", screenWidth / 2 - 60, screenHeight / 2 - 100, 50, RED);
        } else if (_fState == FightState::END) {
            if (player.hasEnergy(1)) DrawText("DEFI REMPORTE !", screenWidth / 2 - 140, screenHeight / 2 - 100, 50, GREEN);
            else DrawText("EPUISE !", screenWidth / 2 - 90, screenHeight / 2 - 100, 50, RED);
            DrawText("Press ENTER or ESC to exit", screenWidth / 2 - 140, screenHeight / 2, 20, WHITE);
        }
    }
}