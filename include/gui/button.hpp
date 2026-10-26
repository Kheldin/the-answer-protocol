#pragma once

#include "raylib.h"
#include <string>

class InteractiveButton {
public:
    InteractiveButton(Rectangle b, Rectangle zone, std::string text) 
        : bounds(b), interactionZone(zone), dialogText(text), isNear(false), isTalking(false) {}
    void Update(Rectangle playerHitbox, bool inSameRoom);
    void Draw(Texture2D texture, float bframeWidth, float bframeHeight, float cropOffset);
    void DrawDialog(int screenWidth, int screenHeight);

    bool getIsTalking() const { return isTalking; }
    void setIsTalking(bool state) { isTalking = state; }

private:
    Rectangle bounds;
    Rectangle interactionZone;
    std::string dialogText;
    bool isNear;
    bool isTalking;
};