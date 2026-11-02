#include "raylib.h"
#include "gui/asset_manager.hpp"
#include "gui/button.hpp"
#include "gui/room.hpp"
#include <unordered_map>
#include <string>
#include <vector>

int main(void)
{
    int screenWidth {800};
    int screenHeight {450};

    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "Borderless fullscreen");
    int monitor = GetCurrentMonitor();
    screenWidth = GetMonitorWidth(monitor);
    screenHeight = GetMonitorHeight(monitor);

    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    AssetManager manager {};
    manager.LoadAll();

    std::unordered_map<std::string, Texture2D> player_textures = manager.getPlayerTextures();
    Texture2D current_guy = player_textures.at("guy_front");

    Texture2D buttonsTexture = manager.getButtonsTextures();
    float bframeWidth = (float)buttonsTexture.width / 1.9f;
    float bframeHeight = (float)buttonsTexture.height / 3.0f;
    float cropOffset = 3.0f;

    Rectangle pos1 = { 1650.0f, 570.0f, bframeWidth * 0.2f, bframeHeight * 0.2f };
    Rectangle zone1 = { pos1.x - 250, pos1.y + 100, pos1.width + 310, pos1.height + 60 };
    InteractiveButton npcButton(pos1, zone1, "Hi, welcome to the gym !\nWhat can I do for you today?");

    Rectangle pos2 = { 1680.0f, 120.0f, bframeWidth * 0.2f, bframeHeight * 0.2f };
    Rectangle zone2 = { pos2.x - 250, pos2.y + 100, pos2.width + 310, pos2.height + 60 };
    InteractiveButton shopButton(pos2, zone2, "");
    
    Rectangle pos3 = { 600.0f, 700.0f, bframeWidth * 0.2f, bframeHeight * 0.2f };
    Rectangle zone3 = { pos3.x - 250, pos3.y + 100, pos3.width + 310, pos3.height + 60 };
    InteractiveButton fightButton(pos3, zone3, "");

    Room Reception(manager.getReceptionTextures(), manager.getReceptionTexturesPositions(), manager.getReceptionHitboxes());
    Room LockerRoom(manager.getLockerRoomTextures(), manager.getLockerRoomTexturesPositions(), manager.getLockerRoomHitboxes());
    Room Muscu(manager.getMuscuTextures(), manager.getMuscuTexturesPositions(), manager.getMuscuHitboxes());
    Room Cardio(manager.getCardioTextures(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());

    // --- VARIABLES SHOP ---
    Texture2D shopTexture = manager.getShopTexture(); 
    float shopScale = 0.7f; 
    bool isShopOpen = false;

    Rectangle item1 = { 615.0f, 491.0f, 310.0f, 55.0f };
    Rectangle item2 = { 615.0f, 818.0f, 310.0f, 55.0f };
    Rectangle item3 = { 1165.0f, 491.0f, 310.0f, 55.0f };
    Rectangle item4 = { 1182.0f, 818.0f, 290.0f, 55.0f };

    // --- VARIABLES COMBAT ---
    Texture2D fightTexture = manager.getFightTexture(); 
    Texture2D leftFightTexture = manager.getLeftFightTexture();
    Texture2D rightFightTexture = manager.getRightFightTexture();
    SetTextureFilter(leftFightTexture, TEXTURE_FILTER_POINT);
    SetTextureFilter(rightFightTexture, TEXTURE_FILTER_POINT);
    
    float fightBgScale = 1.5f;
    float spriteScale = 1.0f;
    bool isFighting = false;
    bool wasFighting = false;

    int playerHP = 100, npcHP = 100;
    enum class FightState { PLAYER_WAIT, PLAYER_QTE, PLAYER_ATTACK, NPC_WAIT, NPC_ATTACK, END };
    FightState fState = FightState::PLAYER_WAIT;
    int qteKey = 0; 
    float fightTimer = 0.0f;
    int pFrame = 0;
    int nFrame = 3;

    enum class Rooms { Reception, LockerRoom, Muscu, Cardio };
    Rooms currentRoom {Rooms::Reception};
    
    const float HITBOX_OFFSET_X = 5.0f;
    const float HITBOX_OFFSET_Y = 172.0f;
    const float HITBOX_WIDTH = 70.0f;  
    const float HITBOX_HEIGHT = 20.0f;

    auto get_feet_hitbox = [=](float x, float y) -> Rectangle {
        return { x + HITBOX_OFFSET_X, y + HITBOX_OFFSET_Y, HITBOX_WIDTH, HITBOX_HEIGHT };
    };

    Rectangle frameRec_faces = { 0.0f, 0.0f, (float)current_guy.width/2, (float)current_guy.height };
    bool frame_faces = true;
    int currentFrame = 0;
    Rectangle frameRec = { 0.0f, 0.0f, 0.0f, 0.0f };

    const float speed = 15.0f;
    const float scale = 0.6f;

    Rectangle destRec = {
        screenWidth / 2.0f,
        screenHeight / 2.0f,
        frameRec_faces.width * scale,
        frameRec_faces.height * scale
    };
    Vector2 origin = { 0.0f, 0.0f };

    int framesCounter {0};
    int framesSpeed {8};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        framesCounter++;
        Rectangle playerHitbox = get_feet_hitbox(destRec.x, destRec.y);
        bool inReception = (currentRoom == Rooms::Reception);
        bool inMuscu = (currentRoom == Rooms::Muscu);

        npcButton.Update(playerHitbox, inReception);
        shopButton.Update(playerHitbox, inReception);
        fightButton.Update(playerHitbox, inMuscu);

        isShopOpen = shopButton.getIsTalking();
        isFighting = fightButton.getIsTalking();

        if (isFighting && !wasFighting) {
            playerHP = 100;
            npcHP = 100;
            fState = FightState::PLAYER_WAIT;
            fightTimer = 1.0f;
            pFrame = 0;
            nFrame = 3;
        }
        wasFighting = isFighting;

        if (!(framesCounter % 5) && (IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_UP)))
            currentFrame++;

        std::vector<Rectangle> current_hitboxes;
        switch (currentRoom) {
            case Rooms::Reception:  current_hitboxes = Reception.getHitboxes(); break;
            case Rooms::LockerRoom: current_hitboxes = LockerRoom.getHitboxes(); break;
            case Rooms::Muscu:      current_hitboxes = Muscu.getHitboxes(); break;
            case Rooms::Cardio:     current_hitboxes = Cardio.getHitboxes(); break;
        }
        
        if (isShopOpen) {
            Vector2 mousePos = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, item1)) {} 
                else if (CheckCollisionPointRec(mousePos, item2)) {}
                else if (CheckCollisionPointRec(mousePos, item3)) {}
                else if (CheckCollisionPointRec(mousePos, item4)) {}
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                shopButton.setIsTalking(false);
            }
        }
        else if (isFighting)
        {
            switch (fState) {
                case FightState::PLAYER_WAIT:
                    fightTimer -= dt;
                    pFrame = 0; nFrame = 3;
                    if (fightTimer <= 0) {
                        qteKey = GetRandomValue(KEY_A, KEY_Z);
                        fightTimer = 1.5f;
                        fState = FightState::PLAYER_QTE;
                    }
                    break;
                case FightState::PLAYER_QTE:
                    fightTimer -= dt;
                    if (fightTimer <= 0) {
                        fightTimer = 1.0f;
                        fState = FightState::PLAYER_ATTACK;
                        qteKey = 0; 
                    } else {
                        int key = GetKeyPressed();
                        if (key != 0) {
                            if (key == qteKey) {
                                npcHP -= 35;
                                qteKey = 1;
                            } else {
                                qteKey = 0;
                            }
                            fightTimer = 1.0f;
                            fState = FightState::PLAYER_ATTACK;
                        }
                    }
                    break;
                case FightState::PLAYER_ATTACK:
                    fightTimer -= dt;
                    if (qteKey == 1) { pFrame = 1; nFrame = 0; }
                    else { pFrame = 0; nFrame = 3; }
                    
                    if (fightTimer <= 0) {
                        if (npcHP <= 0) fState = FightState::END;
                        else { fightTimer = 1.0f; fState = FightState::NPC_WAIT; }
                    }
                    break;
                case FightState::NPC_WAIT:
                    fightTimer -= dt;
                    pFrame = 0; nFrame = 3;
                    if (fightTimer <= 0) {
                        fightTimer = 1.0f;
                        fState = FightState::NPC_ATTACK;
                        playerHP -= 20;
                    }
                    break;
                case FightState::NPC_ATTACK:
                    fightTimer -= dt;
                    pFrame = 3; nFrame = 2;
                    if (fightTimer <= 0) {
                        if (playerHP <= 0) fState = FightState::END;
                        else { fightTimer = 1.0f; fState = FightState::PLAYER_WAIT; }
                    }
                    break;
                case FightState::END:
                    pFrame = (playerHP > 0) ? 0 : 3;
                    nFrame = (npcHP > 0) ? 3 : 0;
                    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                        fightButton.setIsTalking(false);
                    }
                    break;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                fightButton.setIsTalking(false);
            }
        }
        else 
        {
            if (!npcButton.getIsTalking())
            {
                if (IsKeyDown(KEY_RIGHT)) {
                    Rectangle future_feet = get_feet_hitbox(destRec.x + speed, destRec.y);
                    bool collision = false;
                    for (Rectangle wall : current_hitboxes) if (CheckCollisionRecs(future_feet, wall)) collision = true;
                    if (!collision && destRec.x < screenWidth - 50) destRec.x += speed;
                    
                    if (currentFrame % 3 == 1) current_guy = player_textures.at("guy_right1");
                    else if (currentFrame % 3 == 2) current_guy = player_textures.at("guy_right2");
                    else current_guy = player_textures.at("guy_right3");
                    frame_faces = false;
                }
                if (IsKeyDown(KEY_LEFT)) {
                    Rectangle future_feet = get_feet_hitbox(destRec.x - speed, destRec.y);
                    bool collision = false;
                    for (Rectangle wall : current_hitboxes) if (CheckCollisionRecs(future_feet, wall)) collision = true;
                    if (!collision && destRec.x > 3) destRec.x -= speed;
                    
                    if (currentFrame % 3 == 1) current_guy = player_textures.at("guy_left1");
                    else if (currentFrame % 3 == 2) current_guy = player_textures.at("guy_left2");
                    else current_guy = player_textures.at("guy_left3");
                    frame_faces = false;
                }
                if (IsKeyDown(KEY_DOWN)) {
                    Rectangle future_feet = get_feet_hitbox(destRec.x, destRec.y + speed);
                    bool collision = false;
                    for (Rectangle wall : current_hitboxes) if (CheckCollisionRecs(future_feet, wall)) collision = true;
                    if (!collision && destRec.y < screenHeight - 170) destRec.y += speed;
                    
                    current_guy = player_textures.at("guy_front");
                    frame_faces = true;
                }
                if (IsKeyDown(KEY_UP)) {
                    Rectangle future_feet = get_feet_hitbox(destRec.x, destRec.y - speed);
                    bool collision = false;
                    for (Rectangle wall : current_hitboxes) if (CheckCollisionRecs(future_feet, wall)) collision = true;
                    if (!collision && destRec.y > 10) destRec.y -= speed;
                    
                    current_guy = player_textures.at("guy_back");
                    frame_faces = true;
                }
            }
        }

        switch (currentRoom)
        {
            case Rooms::Reception:
                if (destRec.y < 160 && destRec.x > 150 && destRec.x < 270) {
                    currentRoom = Rooms::LockerRoom; destRec.x = 1650; destRec.y = 270;
                } else if (destRec.y < 160 && destRec.x > 900 && destRec.x < 1070) {
                    currentRoom = Rooms::Muscu; destRec.y = screenHeight - 180;
                }
                break;
            case Rooms::LockerRoom:
                if (destRec.x > 1640 && destRec.x < 1764 && destRec.y < 100) {
                    currentRoom = Rooms::Reception; destRec.x = 155; destRec.y = 200;
                }
                break;
            case Rooms::Muscu:
                if (destRec.y > screenHeight - 172) {
                    currentRoom = Rooms::Reception; destRec.y = 180;
                } else if (destRec.x < 10) {
                    currentRoom = Rooms::Cardio; destRec.x = screenWidth - 52;
                }
                break;
            case Rooms::Cardio:
                if (destRec.x > screenWidth - 51) {
                    currentRoom = Rooms::Muscu; destRec.x = 5;
                }
                break;
        }

        if (framesCounter >= (60/framesSpeed)) {
            framesCounter = 0;
            int max_frames = frame_faces ? 2 : 3;
            if (currentFrame >= max_frames) currentFrame = 0;
        }

        int max_frames = frame_faces ? 2 : 1;
        frameRec.width = (float)current_guy.width / max_frames;
        frameRec.height = (float)current_guy.height;
        frameRec.x = (float)currentFrame * frameRec.width;
        destRec.width = frameRec.width * scale;
        destRec.height = frameRec.height * scale;

        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentRoom) {
            case Rooms::Reception:
                DrawText("RECEPTION", screenWidth/2, screenHeight/2, 50, BLACK);
                Reception.draw();
                npcButton.Draw(buttonsTexture, bframeWidth, bframeHeight, cropOffset);
                shopButton.Draw(buttonsTexture, bframeWidth, bframeHeight, cropOffset);
                break;
            case Rooms::LockerRoom:
                DrawText("LockerRoom", screenWidth/2, screenHeight/2, 50, BLACK);
                LockerRoom.draw();
                break;
            case Rooms::Muscu:
                DrawText("Muscu", screenWidth/2, screenHeight/2, 50, BLACK);
                Muscu.draw();
                fightButton.Draw(buttonsTexture, bframeWidth, bframeHeight, cropOffset);
                break;
            case Rooms::Cardio:
                DrawText("Cardio", screenWidth/2, screenHeight/2, 50, BLACK);
                Cardio.draw();
                break;
        }
        
        DrawText(TextFormat("X: %d Y: %d", GetMouseX(), GetMouseY()), 10, 10, 20, WHITE);
        
        DrawTexturePro(current_guy, frameRec, destRec, origin, 0.0f, WHITE);
        DrawRectangleLinesEx(playerHitbox, 2.0f, RED);

        if (isShopOpen) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            
            Vector2 shopPos = {
                (screenWidth / 2.0f) - ((shopTexture.width * shopScale) / 2.0f),
                (screenHeight / 2.0f) - ((shopTexture.height * shopScale) / 2.0f)
            };
            
            DrawTextureEx(shopTexture, shopPos, 0.0f, shopScale, WHITE);

            Vector2 mousePos = GetMousePosition();
            DrawRectangleLinesEx(item1, 2.0f, CheckCollisionPointRec(mousePos, item1) ? RED : WHITE);
            DrawRectangleLinesEx(item2, 2.0f, CheckCollisionPointRec(mousePos, item2) ? RED : WHITE);
            DrawRectangleLinesEx(item3, 2.0f, CheckCollisionPointRec(mousePos, item3) ? RED : WHITE);
            DrawRectangleLinesEx(item4, 2.0f, CheckCollisionPointRec(mousePos, item4) ? RED : WHITE);
        }
        else if (isFighting)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));

            Vector2 fightpos = {
                (screenWidth / 2.0f) - ((fightTexture.width * fightBgScale) / 2.0f),
                (screenHeight / 2.0f) - ((fightTexture.height * fightBgScale) / 2.0f)
            };
            DrawTextureEx(fightTexture, fightpos, 0.0f, fightBgScale, WHITE);

            float pFrameW = leftFightTexture.width / 4.0f;
            float pFrameH = leftFightTexture.height / 2.0f;
            float nFrameW = rightFightTexture.width / 4.0f;
            float nFrameH = rightFightTexture.height / 2.0f;

            Rectangle pSrc = { pFrame * pFrameW, 0.0f, pFrameW, pFrameH };
            Rectangle pDest = { screenWidth/4.0f + 150.0f - (pFrameW*spriteScale)/2.0f, screenHeight/2.0f - (pFrameH*spriteScale)/2.0f + 50, pFrameW * spriteScale, pFrameH * spriteScale };
            DrawTexturePro(leftFightTexture, pSrc, pDest, {0,0}, 0.0f, WHITE);

            Rectangle nSrc = { nFrame * nFrameW, nFrameH, nFrameW, nFrameH };
            Rectangle nDest = { 3.0f*screenWidth/4.0f - 50.0f - (nFrameW*spriteScale)/2.0f, screenHeight/2.0f - (nFrameH*spriteScale)/2.0f + 50, nFrameW * spriteScale, nFrameH * spriteScale };
            DrawTexturePro(rightFightTexture, nSrc, nDest, {0,0}, 0.0f, WHITE);

            DrawText(TextFormat("PLAYER HP: %d", playerHP), screenWidth/4 - 100, 50, 30, GREEN);
            DrawText(TextFormat("NPC HP: %d", npcHP), 3*screenWidth/4 - 80, 50, 30, RED);

            if (fState == FightState::PLAYER_QTE) {
                DrawText("QTE! PRESS:", screenWidth/2 - 100, screenHeight/2 - 150, 30, WHITE);
                DrawText(TextFormat("%c", qteKey), screenWidth/2 - 30, screenHeight/2 - 100, 70, YELLOW);
                DrawRectangle(screenWidth/2 - 100, screenHeight/2, 200 * (fightTimer/1.5f), 15, ORANGE);
            } else if (fState == FightState::PLAYER_ATTACK) {
                if (qteKey == 1) DrawText("HIT!", screenWidth/2 - 50, screenHeight/2 - 100, 50, GREEN);
                else DrawText("MISS!", screenWidth/2 - 60, screenHeight/2 - 100, 50, RED);
            } else if (fState == FightState::END) {
                if (playerHP > 0) DrawText("YOU WIN!", screenWidth/2 - 100, screenHeight/2 - 100, 50, GREEN);
                else DrawText("YOU LOSE!", screenWidth/2 - 110, screenHeight/2 - 100, 50, RED);
                DrawText("Press E to exit", screenWidth/2 - 90, screenHeight/2, 20, WHITE);
            }
        }
        else {
            npcButton.DrawDialog(screenWidth, screenHeight);
        }

        EndDrawing();
    }
    
    manager.UnloadAll();
    CloseWindow();

    return 0;
}