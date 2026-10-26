#include "raylib.h"
#include "gui/button.hpp"
#include <string>

void InteractiveButton::Update(Rectangle playerHitbox, bool inSameRoom) {
        isNear = inSameRoom && CheckCollisionRecs(playerHitbox, interactionZone);
        
        if (isNear) {
            if (IsKeyPressed(KEY_E)) {
                isTalking = !isTalking;
            }
        } else {
            isTalking = false;
        }
    }

void InteractiveButton::Draw(Texture2D texture, float bframeWidth, float bframeHeight, float cropOffset) {
    int index = isNear ? 3 : 5;
    int col = index % 2;
    int row = index / 2;

    Rectangle sourceRec = {
        (col * bframeWidth) + cropOffset,
        row * bframeHeight,
        bframeWidth - cropOffset,
        bframeHeight
    };

    DrawTexturePro(texture, sourceRec, bounds, {0.0f, 0.0f}, 0.0f, WHITE);
}

void InteractiveButton::DrawDialog(int screenWidth, int screenHeight) {
    if (isTalking) {
        DrawRectangle(50, screenHeight - 120, screenWidth - 100, 100, LIGHTGRAY);
        DrawRectangleLines(50, screenHeight - 120, screenWidth - 100, 100, BLACK);
        DrawText(dialogText.c_str(), 70, screenHeight - 100, 20, BLACK);
    }
}