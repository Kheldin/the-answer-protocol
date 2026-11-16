#include "gui/quest_log.hpp"

QuestLog::QuestLog() {
    AddQuest(Quest("gym_tour", "First Day at the Gym", "Talk to the receptionist and explore the rooms.", 1, QuestStatus::IN_PROGRESS));
    AddQuest(Quest("protein_up", "Fuel the Machine", "Buy Whey Protein or Creatine from the shop.", 2, QuestStatus::AVAILABLE));
    AddQuest(Quest("heavy_weight", "Iron Challenger", "Defeat the sparring partner in the Muscu room.", 1, QuestStatus::AVAILABLE));

    AddQuest(Quest("use_press", "Leg Day", "Use the leg press machine\nin the Muscu room.", 1, QuestStatus::IN_PROGRESS));
    AddQuest(Quest("use_bench", "Chest Pump", "Use the bench press\nin the Muscu room.", 1, QuestStatus::IN_PROGRESS));
    AddQuest(Quest("use_bike", "Cardio Cycling", "Use the exercise bike\nin the Cardio room.", 1, QuestStatus::IN_PROGRESS));
    AddQuest(Quest("use_treadmill", "Endurance Run", "Use the treadmill\nin the Cardio room.", 1, QuestStatus::IN_PROGRESS));
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

void QuestLog::AdvanceQuest(const std::string& id, int amount) {
    for (size_t i = 0; i < _quests.size(); ++i) {
        if (_quests[i].getId() == id) {
            _quests[i].advance(amount);
            if (static_cast<int>(i) == _trackedIndex && _quests[i].isCompleted()) {
                for (size_t j = 0; j < _quests.size(); ++j) {
                    if (_quests[j].getStatus() == QuestStatus::IN_PROGRESS) {
                        _trackedIndex = static_cast<int>(j);
                        break;
                    }
                }
            }
            return;
        }
    }
}

std::string QuestLog::Update(Vector2 mousePos) {
    if (!_isOpen) return "";

    if (IsKeyPressed(KEY_ESCAPE)) {
        SetOpen(false);
        return "";
    }

    const float panelW = 820.0f;
    const float panelH = 480.0f;
    const float panelX = (1920.0f - panelW) / 2.0f;
    const float panelY = (1080.0f - panelH) / 2.0f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (size_t i = 0; i < _quests.size(); ++i) {
            Rectangle rowRec = { panelX + 30.0f, panelY + 80.0f + i * 65.0f, 320.0f, 55.0f };
            if (CheckCollisionPointRec(mousePos, rowRec)) {
                _selectedIndex = static_cast<int>(i);
                return "";
            }
        }

        if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_quests.size())) {
            Rectangle actionBtn = { panelX + 390.0f, panelY + panelH - 70.0f, 180.0f, 42.0f };
            if (CheckCollisionPointRec(mousePos, actionBtn)) {
                Quest& q = _quests[_selectedIndex];
                if (q.getStatus() == QuestStatus::AVAILABLE) {
                    q.accept();
                    _trackedIndex = _selectedIndex;
                    return "quest accept " + q.getId();
                } else if (q.getStatus() == QuestStatus::IN_PROGRESS) {
                    _trackedIndex = _selectedIndex;
                    return "quest track " + q.getId();
                }
            }
        }
    }

    return "";
}

void QuestLog::DrawJournal(int screenWidth, int screenHeight) const {
    if (!_isOpen) return;

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.55f));

    const float panelW = 820.0f;
    const float panelH = 480.0f;
    const float panelX = (screenWidth - panelW) / 2.0f;
    const float panelY = (screenHeight - panelH) / 2.0f;

    DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.04f, 8, { 28, 30, 38, 245 });
    DrawRectangleRoundedLinesEx({ panelX, panelY, panelW, panelH }, 0.04f, 8, 2.0f, LIGHTGRAY);
    DrawText("QUEST LOG [Q / ESC]", static_cast<int>(panelX + 30), static_cast<int>(panelY + 22), 24, WHITE);

    for (size_t i = 0; i < _quests.size(); ++i) {
        const Quest& q = _quests[i];
        Rectangle rowRec = { panelX + 30.0f, panelY + 80.0f + i * 65.0f, 320.0f, 55.0f };

        bool isSelected = (static_cast<int>(i) == _selectedIndex);
        bool isTracked  = (static_cast<int>(i) == _trackedIndex && q.getStatus() == QuestStatus::IN_PROGRESS);

        DrawRectangleRec(rowRec, isSelected ? Color{ 65, 85, 125, 255 } : Color{ 42, 45, 55, 255 });
        DrawRectangleLinesEx(rowRec, isSelected ? 2.5f : 1.0f, isSelected ? YELLOW : GRAY);

        Color titleColor = WHITE;
        if (q.getStatus() == QuestStatus::COMPLETED) titleColor = GREEN;
        else if (q.getStatus() == QuestStatus::AVAILABLE) titleColor = LIGHTGRAY;

        std::string label = (isTracked ? "* " : "") + q.getTitle();
        DrawText(label.c_str(), static_cast<int>(rowRec.x + 12), static_cast<int>(rowRec.y + 18), 18, titleColor);
    }

    DrawLine(static_cast<int>(panelX + 370), static_cast<int>(panelY + 75),
             static_cast<int>(panelX + 370), static_cast<int>(panelY + panelH - 25), GRAY);

    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_quests.size())) {
        const Quest& selected = _quests[_selectedIndex];
        float detailX = panelX + 390.0f;

        DrawText(selected.getTitle().c_str(), static_cast<int>(detailX), static_cast<int>(panelY + 85), 24, YELLOW);

        const char* statusText = "AVAILABLE";
        Color statusColor = SKYBLUE;
        if (selected.getStatus() == QuestStatus::IN_PROGRESS) { statusText = "IN PROGRESS"; statusColor = ORANGE; }
        else if (selected.getStatus() == QuestStatus::COMPLETED) { statusText = "COMPLETED"; statusColor = GREEN; }
        DrawText(statusText, static_cast<int>(detailX), static_cast<int>(panelY + 120), 16, statusColor);

       DrawText(selected.getDescription().c_str(), static_cast<int>(detailX), static_cast<int>(panelY + 160), 18, RAYWHITE);

        DrawText(TextFormat("Progress: %d / %d", selected.getCurrentProgress(), selected.getTargetProgress()),
                 static_cast<int>(detailX), static_cast<int>(panelY + 320), 20, WHITE);

        if (selected.getStatus() != QuestStatus::COMPLETED) {
            Rectangle actionBtn = { detailX, panelY + panelH - 70.0f, 180.0f, 42.0f };
            bool isAccept = (selected.getStatus() == QuestStatus::AVAILABLE);
            DrawRectangleRec(actionBtn, isAccept ? DARKGREEN : DARKBLUE);
            DrawRectangleLinesEx(actionBtn, 1.5f, WHITE);
            DrawText(isAccept ? "ACCEPT" : "TRACK", static_cast<int>(actionBtn.x + 48), static_cast<int>(actionBtn.y + 11), 20, WHITE);
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
        DrawText("Completed!", static_cast<int>(boxX + 15), static_cast<int>(boxY + 60), 16, GREEN);
    } else {
        DrawText(TextFormat("Progress: %d / %d", q.getCurrentProgress(), q.getTargetProgress()),
                 static_cast<int>(boxX + 15), static_cast<int>(boxY + 60), 16, LIGHTGRAY);
    }
}