#include "gui/quest_log.hpp"

QuestLog::QuestLog() {
    AddQuest(Quest("register_gym", "0. Gym Registration", "Talk to the receptionist at the front desk\nto register at the gym.", 1, QuestStatus::IN_PROGRESS));

    AddQuest(Quest("protein_up", "1. Fuel the Machine", "Buy Whey Protein or Creatine\nfrom the reception shop.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("use_bench", "2. Chest Pump", "Use the bench press machine\nin the Muscu room.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("use_press", "3. Leg Day", "Use the leg press machine\nin the Muscu room.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("use_bike", "4. Cardio Warmup", "Use the exercise bike\nin the Cardio room.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("use_treadmill", "5. Endurance Run", "Use the treadmill\nin the Cardio room.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("take_shower", "6. Post-Workout Shower", "Take a shower in the Locker Room\nto recover energy.", 1, QuestStatus::AVAILABLE));
    AddQuest(Quest("heavy_weight", "7. Iron Challenger", "Defeat the sparring partner\nin the Muscu room.", 1, QuestStatus::AVAILABLE));

    _trackedIndex = 0;
    _selectedIndex = 0;
}

void QuestLog::Toggle() {
    _isOpen = !_isOpen;
}

bool QuestLog::IsOpen() const {
    return _isOpen;
}

void QuestLog::SetOpen(bool open) {
    _isOpen = open;
}

void QuestLog::AddQuest(const Quest& quest) {
    _quests.push_back(quest);
}

bool QuestLog::AdvanceQuest(const std::string& id, int amount) {
    for (size_t i = 0; i < _quests.size(); ++i) {
        if (_quests[i].getId() == id && _quests[i].getStatus() == QuestStatus::IN_PROGRESS) {
            _quests[i].advance(amount);

            if (_quests[i].isCompleted()) {
                if (i + 1 < _quests.size()) {
                    _quests[i + 1].accept(); // Passe la suivante en IN_PROGRESS
                    _trackedIndex = static_cast<int>(i + 1);
                    _selectedIndex = static_cast<int>(i + 1);
                }
            }
            return true;
        }
    }
    return false;
}

std::string QuestLog::Update(Vector2 mousePos) {
    if (!_isOpen) return "";

    if (IsKeyPressed(KEY_ESCAPE)) {
        SetOpen(false);
        return "";
    }

    const float panelW = 820.0f;
    const float panelH = 580.0f;
    const float panelX = (1920.0f - panelW) / 2.0f;
    const float panelY = (1080.0f - panelH) / 2.0f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (size_t i = 0; i < _quests.size(); ++i) {
            if (_quests[i].getStatus() == QuestStatus::AVAILABLE) continue; // Verrouillée : non cliquable

            Rectangle rowRec = { panelX + 30.0f, panelY + 75.0f + i * 58.0f, 320.0f, 50.0f };
            if (CheckCollisionPointRec(mousePos, rowRec)) {
                _selectedIndex = static_cast<int>(i);
                return "";
            }
        }
    }

    return "";
}

void QuestLog::DrawJournal(int screenWidth, int screenHeight) const {
    if (!_isOpen) return;

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.55f));

    const float panelW = 820.0f;
    const float panelH = 580.0f;
    const float panelX = (screenWidth - panelW) / 2.0f;
    const float panelY = (screenHeight - panelH) / 2.0f;

    DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.04f, 8, { 28, 30, 38, 245 });
    DrawRectangleRoundedLinesEx({ panelX, panelY, panelW, panelH }, 0.04f, 8, 2.0f, LIGHTGRAY);
    DrawText("QUEST LOG [Q / ESC]", static_cast<int>(panelX + 30), static_cast<int>(panelY + 22), 24, WHITE);

    for (size_t i = 0; i < _quests.size(); ++i) {
        const Quest& q = _quests[i];
        Rectangle rowRec = { panelX + 30.0f, panelY + 75.0f + i * 58.0f, 320.0f, 50.0f };

        bool isLocked   = (q.getStatus() == QuestStatus::AVAILABLE);
        bool isSelected = (!isLocked && static_cast<int>(i) == _selectedIndex);
        bool isTracked  = (q.getStatus() == QuestStatus::IN_PROGRESS);

        DrawRectangleRec(rowRec, isLocked ? Color{ 32, 34, 42, 255 } : (isSelected ? Color{ 65, 85, 125, 255 } : Color{ 42, 45, 55, 255 }));
        DrawRectangleLinesEx(rowRec, isSelected ? 2.5f : 1.0f, isSelected ? YELLOW : (isLocked ? DARKGRAY : GRAY));

        if (isLocked) {
            DrawText("??? (Locked)", static_cast<int>(rowRec.x + 12), static_cast<int>(rowRec.y + 16), 18, DARKGRAY);
        } else {
            Color titleColor = (q.getStatus() == QuestStatus::COMPLETED) ? GREEN : WHITE;
            std::string label = (isTracked ? "> " : "") + q.getTitle();
            DrawText(label.c_str(), static_cast<int>(rowRec.x + 12), static_cast<int>(rowRec.y + 16), 18, titleColor);
        }
    }

    DrawLine(static_cast<int>(panelX + 370), static_cast<int>(panelY + 75),
             static_cast<int>(panelX + 370), static_cast<int>(panelY + panelH - 25), GRAY);

    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_quests.size())) {
        const Quest& selected = _quests[_selectedIndex];
        if (selected.getStatus() != QuestStatus::AVAILABLE) {
            float detailX = panelX + 390.0f;

            DrawText(selected.getTitle().c_str(), static_cast<int>(detailX), static_cast<int>(panelY + 85), 24, YELLOW);

            const char* statusText = (selected.getStatus() == QuestStatus::COMPLETED) ? "COMPLETED" : "IN PROGRESS";
            Color statusColor = (selected.getStatus() == QuestStatus::COMPLETED) ? GREEN : ORANGE;
            DrawText(statusText, static_cast<int>(detailX), static_cast<int>(panelY + 120), 16, statusColor);

            DrawText(selected.getDescription().c_str(), static_cast<int>(detailX), static_cast<int>(panelY + 160), 18, RAYWHITE);

            DrawText(TextFormat("Progress: %d / %d", selected.getCurrentProgress(), selected.getTargetProgress()),
                     static_cast<int>(detailX), static_cast<int>(panelY + 320), 20, WHITE);
        }
    }
}

void QuestLog::DrawTracker(int screenWidth) const {
    if (_trackedIndex < 0 || _trackedIndex >= static_cast<int>(_quests.size())) return;

    const Quest& q = _quests[_trackedIndex];
    if (q.getStatus() == QuestStatus::AVAILABLE) return;

    const float boxW = 340.0f;
    const float boxH = 95.0f;
    const float boxX = screenWidth - boxW - 25.0f;
    const float boxY = 25.0f;

    DrawRectangleRounded({ boxX, boxY, boxW, boxH }, 0.1f, 6, Fade(BLACK, 0.65f));
    DrawRectangleRoundedLinesEx({ boxX, boxY, boxW, boxH }, 0.1f, 6, 1.5f, q.isCompleted() ? GREEN : GOLD);

    DrawText("CURRENT TASK [Q]", static_cast<int>(boxX + 15), static_cast<int>(boxY + 10), 14, GOLD);
    DrawText(q.getTitle().c_str(), static_cast<int>(boxX + 15), static_cast<int>(boxY + 32), 18, WHITE);

    if (q.isCompleted()) {
        DrawText("All tasks completed!", static_cast<int>(boxX + 15), static_cast<int>(boxY + 60), 16, GREEN);
    } else {
        DrawText(TextFormat("Progress: %d / %d", q.getCurrentProgress(), q.getTargetProgress()),
                 static_cast<int>(boxX + 15), static_cast<int>(boxY + 60), 16, LIGHTGRAY);
    }
}