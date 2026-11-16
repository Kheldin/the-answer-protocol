#include "gui/inventory.hpp"
#include "gui/asset_manager.hpp"


Inventory::Inventory(AssetManager manager) {
    AddItem(Item("Whey Protein", ItemType::CONSUMABLE, manager.getWheyTexture(), 25, 1));
    AddItem(Item("Creatine", ItemType::CONSUMABLE, manager.getCreatineTexture(), 15, 1));
    AddItem(Item("Testo", ItemType::BOOSTER, manager.getTestoTexture(), 10, 1));
    AddItem(Item("Supplements", ItemType::CONSUMABLE, manager.getSupplementsTexture(), 20, 1));
}

void Inventory::Toggle() {
    isOpen = !isOpen;
    if (!isOpen) selectedIndex = -1;
}

bool Inventory::IsOpen() const {
    return isOpen;
}

void Inventory::SetOpen(bool open) {
    isOpen = open;
    if (!isOpen) selectedIndex = -1;
}

void Inventory::AddItem(const Item& newItem) {
    for (auto& item : _items) {
        if (item.getName() == newItem.getName()) {
            item.addQuantity(newItem.getQuantity());
            return;
        }
    }
    if (static_cast<int>(_items.size()) < MAX_SLOTS) {
        _items.push_back(newItem);
    }
}

void Inventory::RemoveItem(int index, int quantity) {
    if (index < 0 || index >= static_cast<int>(_items.size())) return;

    _items[index].removeQuantity(quantity);
    if (_items[index].getQuantity() <= 0) {
        _items.erase(_items.begin() + index);
        selectedIndex = -1;
    }
}

std::string Inventory::Update(Vector2 mousePos, int& playerHP, int& bonusDamage) {
    if (!isOpen) return "";

    if (IsKeyPressed(KEY_ESCAPE)) {
        SetOpen(false);
        return "";
    }

    const float panelW = 540.0f;
    const float panelH = 460.0f;
    const float panelX = (1920.0f - panelW) / 2.0f;
    const float panelY = (1080.0f - panelH) / 2.0f;

    const float slotSize = 100.0f;
    const float padding = 20.0f;
    const float startX = panelX + 40.0f;
    const float startY = panelY + 70.0f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // 1. Clic sur un slot de la grille
        for (int i = 0; i < MAX_SLOTS; ++i) {
            int col = i % COLS;
            int row = i / COLS;
            Rectangle slotRec = {
                startX + col * (slotSize + padding),
                startY + row * (slotSize + padding),
                slotSize,
                slotSize
            };

            if (CheckCollisionPointRec(mousePos, slotRec)) {
                selectedIndex = (i < static_cast<int>(_items.size())) ? i : -1;
                return "";
            }
        }

        // 2. Clic sur les boutons USE / DROP si un objet est sélectionné
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(_items.size())) {
            Rectangle useBtn  = { panelX + 40.0f,  panelY + panelH - 60.0f, 140.0f, 40.0f };
            Rectangle dropBtn = { panelX + 200.0f, panelY + panelH - 60.0f, 140.0f, 40.0f };

            if (CheckCollisionPointRec(mousePos, useBtn)) {
                std::string itemName = _items[selectedIndex].getName();
                if (_items[selectedIndex].use(playerHP, bonusDamage)) {
                    if (_items[selectedIndex].getQuantity() <= 0) {
                        _items.erase(_items.begin() + selectedIndex);
                        selectedIndex = -1;
                    }
                    return "use " + itemName;
                }
                return "";
            }

            if (CheckCollisionPointRec(mousePos, dropBtn)) {
                std::string itemName = _items[selectedIndex].getName();
                RemoveItem(selectedIndex, 1);
                return "drop " + itemName;
            }
        }
    }

    return "";
}

void Inventory::Draw(int screenWidth, int screenHeight) const {
    if (!isOpen) return;

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.55f));

    const float panelW = 540.0f;
    const float panelH = 460.0f;
    const float panelX = (screenWidth - panelW) / 2.0f;
    const float panelY = (screenHeight - panelH) / 2.0f;

    DrawRectangleRounded({ panelX, panelY, panelW, panelH }, 0.05f, 8, { 30, 32, 40, 245 });
    DrawRectangleRoundedLinesEx({ panelX, panelY, panelW, panelH }, 0.05f, 8, 2.0f, LIGHTGRAY);
    DrawText("INVENTORY [I]", static_cast<int>(panelX + 40), static_cast<int>(panelY + 20), 24, WHITE);

    const float slotSize = 100.0f;
    const float padding = 20.0f;
    const float startX = panelX + 40.0f;
    const float startY = panelY + 70.0f;

    for (int i = 0; i < MAX_SLOTS; ++i) {
        int col = i % COLS;
        int row = i / COLS;
        Rectangle slotRec = {
            startX + col * (slotSize + padding),
            startY + row * (slotSize + padding),
            slotSize,
            slotSize
        };

        bool isSelected = (i == selectedIndex);
        DrawRectangleRec(slotRec, isSelected ? Color{ 70, 90, 130, 255 } : Color{ 45, 48, 58, 255 });
        DrawRectangleLinesEx(slotRec, isSelected ? 3.0f : 1.5f, isSelected ? YELLOW : GRAY);

        if (i < static_cast<int>(_items.size())) {
            const auto& item = _items[i];
            Texture2D tex = item.getTexture();

            if (tex.id != 0) {
                Rectangle src = { 0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height) };
                Rectangle dst = { slotRec.x + 10.0f, slotRec.y + 10.0f, slotSize - 20.0f, slotSize - 20.0f };
                DrawTexturePro(tex, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
            } else {
                // Fallback texte si la texture n'est pas encore chargée
                std::string shortName = item.getName().substr(0, 9);
                DrawText(shortName.c_str(), static_cast<int>(slotRec.x + 8), static_cast<int>(slotRec.y + 35), 16, WHITE);
            }

            if (item.getQuantity() > 1) {
                DrawText(TextFormat("x%d", item.getQuantity()), static_cast<int>(slotRec.x + 65), static_cast<int>(slotRec.y + 75), 16, GOLD);
            }
        }
    }

    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(_items.size())) {
        const auto& selected = _items[selectedIndex];
        DrawText(selected.getName().c_str(), static_cast<int>(panelX + 360), static_cast<int>(panelY + panelH - 50), 18, YELLOW);

        Rectangle useBtn  = { panelX + 40.0f,  panelY + panelH - 60.0f, 140.0f, 40.0f };
        Rectangle dropBtn = { panelX + 200.0f, panelY + panelH - 60.0f, 140.0f, 40.0f };

        DrawRectangleRec(useBtn, selected.isUsable() ? DARKGREEN : DARKGRAY);
        DrawRectangleLinesEx(useBtn, 1.5f, WHITE);
        DrawText("USE", static_cast<int>(useBtn.x + 48), static_cast<int>(useBtn.y + 10), 20, WHITE);

        DrawRectangleRec(dropBtn, MAROON);
        DrawRectangleLinesEx(dropBtn, 1.5f, WHITE);
        DrawText("DROP", static_cast<int>(dropBtn.x + 40), static_cast<int>(dropBtn.y + 10), 20, WHITE);
    }
}