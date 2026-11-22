#include "gui/quest.hpp"
#include <algorithm>

Quest::Quest(const std::string& id, const std::string& title, const std::string& description,
             int targetProgress, QuestStatus status)
    : _id(id), _title(title), _description(description),
      _currentProgress(0), _targetProgress(targetProgress), _status(status) {}

void Quest::accept() {
    if (_status == QuestStatus::AVAILABLE) {
        _status = QuestStatus::IN_PROGRESS;
    }
}

void Quest::advance(int amount) {
    if (_status != QuestStatus::IN_PROGRESS) return;

    _currentProgress = std::min(_targetProgress, _currentProgress + amount);
    if (_currentProgress >= _targetProgress) {
        _status = QuestStatus::COMPLETED;
    }
}

const std::string& Quest::getId() const { return _id; }
const std::string& Quest::getTitle() const { return _title; }
const std::string& Quest::getDescription() const { return _description; }
int Quest::getCurrentProgress() const { return _currentProgress; }
int Quest::getTargetProgress() const { return _targetProgress; }
QuestStatus Quest::getStatus() const { return _status; }
bool Quest::isCompleted() const { return _status == QuestStatus::COMPLETED; }