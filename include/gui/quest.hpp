#pragma once
#include <string>

enum class QuestStatus {
    AVAILABLE,
    IN_PROGRESS,
    COMPLETED
};

class Quest {
public:
    Quest(const std::string& id, const std::string& title, const std::string& description,
          int targetProgress, QuestStatus status = QuestStatus::AVAILABLE);

    void accept();
    void advance(int amount = 1);

    const std::string& getId() const;
    const std::string& getTitle() const;
    const std::string& getDescription() const;
    int getCurrentProgress() const;
    int getTargetProgress() const;
    QuestStatus getStatus() const;
    bool isCompleted() const;

private:
    std::string _id;
    std::string _title;
    std::string _description;
    int _currentProgress;
    int _targetProgress;
    QuestStatus _status;
};