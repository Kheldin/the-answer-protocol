#include "gui/hud_renderer.hpp"

void HudRenderer::DrawPlayerHud(const Player& player) {
    const int hudX = 20;
    const int hudY = 20;
    const int hudW = 300;
    const int hudH = 115;

    DrawRectangleRounded({ (float)hudX, (float)hudY, (float)hudW, (float)hudH }, 0.15f, 8, Fade(BLACK, 0.75f));
    DrawRectangleRoundedLines({ (float)hudX, (float)hudY, (float)hudW, (float)hudH }, 0.15f, 8, Fade(WHITE, 0.4f));

    DrawText(player.getName().c_str(), hudX + 15, hudY + 12, 22, WHITE);
    const char* lvlText = TextFormat("LVL %d", player.getLevel());
    DrawText(lvlText, hudX + hudW - MeasureText(lvlText, 22) - 15, hudY + 12, 22, GOLD);

    float energyRatio = (player.getMaxEnergy() > 0) ? (float)player.getEnergy() / (float)player.getMaxEnergy() : 0.0f;
    Color energyColor = (energyRatio > 0.5f) ? GOLD : ((energyRatio > 0.25f) ? ORANGE : RED);
    DrawRectangle(hudX + 15, hudY + 46, hudW - 30, 22, DARKGRAY);
    DrawRectangle(hudX + 15, hudY + 46, (int)((hudW - 30) * energyRatio), 22, energyColor);
    DrawRectangleLines(hudX + 15, hudY + 46, hudW - 30, 22, WHITE);
    DrawText(TextFormat("ENERGY: %d / %d", player.getEnergy(), player.getMaxEnergy()), hudX + 22, hudY + 49, 16, BLACK);

    int xpNeeded = player.getLevel() * 100;
    float xpRatio = (xpNeeded > 0) ? (float)player.getXp() / (float)xpNeeded : 0.0f;
    if (xpRatio > 1.0f) xpRatio = 1.0f;
    DrawRectangle(hudX + 15, hudY + 78, hudW - 30, 18, DARKGRAY);
    DrawRectangle(hudX + 15, hudY + 78, (int)((hudW - 30) * xpRatio), 18, SKYBLUE);
    DrawRectangleLines(hudX + 15, hudY + 78, hudW - 30, 18, WHITE);
    DrawText(TextFormat("XP: %d / %d", player.getXp(), xpNeeded), hudX + 22, hudY + 80, 14, WHITE);
}

void HudRenderer::DrawControlsBox(int screenHeight) {
    const int boxW = 270;
    const int boxH = 165;
    const int boxX = 20;
    const int boxY = screenHeight - boxH - 20;

    DrawRectangleRounded({ (float)boxX, (float)boxY, (float)boxW, (float)boxH }, 0.15f, 8, Fade(BLACK, 0.75f));
    DrawRectangleRoundedLines({ (float)boxX, (float)boxY, (float)boxW, (float)boxH }, 0.15f, 8, Fade(WHITE, 0.4f));

    DrawText("CONTROLS", boxX + 15, boxY + 10, 18, YELLOW);
    DrawText("[Arrows]  Move", boxX + 15, boxY + 36, 16, LIGHTGRAY);
    DrawText("[E/Enter] Interact", boxX + 15, boxY + 58, 16, LIGHTGRAY);
    DrawText("[I]       Inventory", boxX + 15, boxY + 80, 16, LIGHTGRAY);
    DrawText("[Q]       Quests / Tasks", boxX + 15, boxY + 102, 16, LIGHTGRAY);
    DrawText("[T]       Chat / Whisper", boxX + 15, boxY + 124, 16, LIGHTGRAY);
    DrawText("[ESC]     Close Menu", boxX + 15, boxY + 144, 16, LIGHTGRAY);
}

void HudRenderer::DrawLowEnergyWarning(int screenWidth, float timer) {
    if (timer <= 0.0f) return;
    const char* warn = "You need recovery. No more energy.";
    int wText = MeasureText(warn, 22);
    DrawRectangle(screenWidth / 2 - wText / 2 - 15, 150, wText + 30, 40, Fade(RED, 0.85f));
    DrawText(warn, screenWidth / 2 - wText / 2, 160, 22, WHITE);
}

void HudRenderer::DrawRemotePlayers(const std::vector<RemotePlayer>& players, Rooms currentRoom, Texture2D tex, float scale) {
    for (const auto& rp : players) {
        if (rp.room != currentRoom) continue;

        Rectangle rSrc = { 0.0f, 0.0f, (float)tex.width / 2.0f, (float)tex.height };
        Rectangle rDest = { rp.pos.x, rp.pos.y, rSrc.width * scale, rSrc.height * scale };
        DrawTexturePro(tex, rSrc, rDest, { 0.0f, 0.0f }, 0.0f, SKYBLUE);

        int nameW = MeasureText(rp.username.c_str(), 18);
        DrawRectangle((int)(rp.pos.x + rDest.width / 2 - nameW / 2 - 6), (int)(rp.pos.y - 26), nameW + 12, 22, Fade(BLACK, 0.7f));
        DrawText(rp.username.c_str(), (int)(rp.pos.x + rDest.width / 2 - nameW / 2), (int)(rp.pos.y - 23), 18, WHITE);
    }
}