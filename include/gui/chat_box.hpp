#pragma once

#include "raylib.h"
#include <string>
#include <vector>

struct ChatMessage {
    std::string sender;
    std::string text;
    Color color;
};

class ChatBox {
public:
    ChatBox() {
        AddSystemMessage("Bienvenue sur TAP. Appuie sur [T] pour ecrire.");
    }

    bool IsFocused() const { return _isFocused; }

    void AddMessage(const std::string& sender, const std::string& text, Color color = WHITE) {
        _messages.push_back({ sender, text, color });
        if (_messages.size() > 50) {
            _messages.erase(_messages.begin());
        }
    }

    void AddSystemMessage(const std::string& text) {
        AddMessage("[SYS]", text, GOLD);
    }

    std::string Update(bool canOpen) {
        if (!_isFocused) {
            if (canOpen && IsKeyPressed(KEY_T)) {
                _isFocused = true;
                _inputBuffer.clear();
                while (GetCharPressed() != 0) {}
            }
            return "";
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            _isFocused = false;
            _inputBuffer.clear();
            return "";
        }

        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && _inputBuffer.size() < 120) {
                _inputBuffer.push_back(static_cast<char>(key));
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (!_inputBuffer.empty()) {
                _inputBuffer.pop_back();
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            std::string submitted = _inputBuffer;
            _inputBuffer.clear();
            _isFocused = false;

            if (submitted.empty()) return "";

            if (submitted.rfind("/w ", 0) == 0) {
                AddMessage("Moi (chuchote)", submitted.substr(3), SKYBLUE);
                return "whisper " + submitted.substr(3);
            }
            if (submitted[0] == '/') {
                return submitted.substr(1);
            }

            return "say " + submitted;
        }

        return "";
    }

    void Draw(int screenWidth, int screenHeight) const {
        const int boxW = 460;
        const int boxH = 210;
        const int boxX = screenWidth - boxW - 20;
        const int boxY = screenHeight - boxH - 20;

        float bgAlpha = _isFocused ? 0.88f : 0.55f;
        DrawRectangleRounded({ (float)boxX, (float)boxY, (float)boxW, (float)boxH }, 0.08f, 8, Fade(BLACK, bgAlpha));
        DrawRectangleRoundedLines({ (float)boxX, (float)boxY, (float)boxW, (float)boxH }, 0.08f, 8, _isFocused ? GOLD : Fade(WHITE, 0.35f));

        const int maxVisible = 6;
        int startIdx = (_messages.size() > maxVisible) ? (int)_messages.size() - maxVisible : 0;
        int lineY = boxY + 12;

        for (int i = startIdx; i < (int)_messages.size(); ++i) {
            const auto& msg = _messages[i];
            std::string formatted = msg.sender.empty() ? msg.text : (msg.sender + ": " + msg.text);
            DrawText(formatted.c_str(), boxX + 14, lineY, 16, msg.color);
            lineY += 24;
        }

        Rectangle inputRec = { (float)(boxX + 10), (float)(boxY + boxH - 38), (float)(boxW - 20), 28.0f };
        DrawRectangleRec(inputRec, _isFocused ? Color{ 35, 38, 48, 255 } : Color{ 20, 22, 28, 180 });
        DrawRectangleLinesEx(inputRec, 1.0f, _isFocused ? YELLOW : GRAY);

        if (_isFocused) {
            std::string display = "> " + _inputBuffer + (((int)(GetTime() * 2) % 2 == 0) ? "_" : "");
            DrawText(display.c_str(), (int)inputRec.x + 8, (int)inputRec.y + 6, 16, WHITE);
        } else {
            DrawText("[T] Chat (/w <user> <msg>)", (int)inputRec.x + 8, (int)inputRec.y + 6, 15, LIGHTGRAY);
        }
    }

private:
    std::vector<ChatMessage> _messages;
    std::string _inputBuffer;
    bool _isFocused{false};
};