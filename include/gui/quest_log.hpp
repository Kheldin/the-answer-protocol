#pragma once
#include "raylib.h"
#include "gui/quest.hpp"
#include <string>
#include <vector>

class QuestLog {
public:
    QuestLog();

    void Toggle();
    bool IsOpen() const;
    void SetOpen(bool open);

    void AddQuest(const Quest& quest);
    bool AdvanceQuest(const std::string& id, int amount = 1);

    std::string Update(Vector2 mousePos);
    void DrawJournal(int screenWidth, int screenHeight) const;
    void DrawTracker(int screenWidth) const;

private:
    bool _isOpen = false;
    int _selectedIndex = 0;
    int _trackedIndex = 0;
    std::vector<Quest> _quests;
};