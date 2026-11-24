#include "gui/world_controller.hpp"

void WorldController::Init(AssetManager& manager, int screenWidth, int screenHeight) {
    _playerTextures = manager.getPlayerTextures();
    _currentGuy = _playerTextures.at("guy_front");

    _buttonsTex = manager.getButtonsTextures();
    _bframeWidth = (float)_buttonsTex.width / 1.9f;
    _bframeHeight = (float)_buttonsTex.height / 3.0f;

    auto makeBtn = [&](float x, float y, const std::string& dialog) {
        Rectangle pos = { x, y, _bframeWidth * 0.2f, _bframeHeight * 0.2f };
        Rectangle zone = { pos.x - 250, pos.y + 100, pos.width + 310, pos.height + 60 };
        return std::make_unique<InteractiveButton>(pos, zone, dialog);
    };

    _npcButton = makeBtn(1650.0f, 570.0f, "Bienvenue a la salle !\nTe voila inscrit. Va faire un tour au shop !");
    _shopButton = makeBtn(1680.0f, 120.0f, "");
    _fightButton = makeBtn(600.0f, 700.0f, "");
    _showerButton = makeBtn(850.0f, 450.0f, "Douche prise ! (+30 Energie)");

    _pressButton = makeBtn(900.0f, 350.0f, "");
    _benchButton = makeBtn(1350.0f, 450.0f, "");
    _bikeButton = makeBtn(500.0f, 400.0f, "");
    _treadmillButton = makeBtn(1100.0f, 400.0f, "");

    _reception = std::make_unique<Room>(manager.getReceptionTextures(), manager.getReceptionTexturesPositions(), manager.getReceptionHitboxes());
    _lockerRoom = std::make_unique<Room>(manager.getLockerRoomTextures(), manager.getLockerRoomTexturesPositions(), manager.getLockerRoomHitboxes());
    _muscu = std::make_unique<Room>(manager.getMuscuTextures(), manager.getMuscuTexturesPositions(), manager.getMuscuHitboxes());
    _cardio = std::make_unique<Room>(manager.getCardioTextures(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());
    _pool = std::make_unique<Room>(manager.getPoolTexture(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());
    _climbing = std::make_unique<Room>(manager.getClimbingTexture(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());
    _sauna = std::make_unique<Room>(manager.getSaunaTexture(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());
    _masage = std::make_unique<Room>(manager.getMasageTexture(), manager.getCardioTexturesPositions(), manager.getCardioHitboxes());

    float initW = ((float)_currentGuy.width / 2.0f) * _scale;
    float initH = (float)_currentGuy.height * _scale;
    _destRec = { screenWidth / 2.0f, screenHeight / 2.0f, initW, initH };
}

Rectangle WorldController::GetFeetHitbox(float x, float y) const {
    return { x + 5.0f, y + 172.0f, 70.0f, 20.0f };
}

void WorldController::Update(
    int screenWidth,
    int screenHeight,
    bool canInteract,
    Player& player,
    Inventory& inventory,
    QuestLog& questLog,
    MiniGames& miniGames,
    std::vector<WorldItem>& droppedItems,
    float& lowEnergyTimer,
    const MiniGames::CommandCallback& sendCmd
) {
    _framesCounter++;
    Rectangle playerHitbox = GetFeetHitbox(_destRec.x, _destRec.y);

    bool inReception = (_currentRoom == Rooms::Reception) && canInteract;
    bool inLockerRoom = (_currentRoom == Rooms::LockerRoom) && canInteract;
    bool inMuscu = (_currentRoom == Rooms::Muscu) && canInteract;
    bool inCardio = (_currentRoom == Rooms::Cardio) && canInteract;

    _npcButton->Update(playerHitbox, inReception);
    _shopButton->Update(playerHitbox, inReception);
    _showerButton->Update(playerHitbox, inLockerRoom);
    _fightButton->Update(playerHitbox, inMuscu);
    _pressButton->Update(playerHitbox, inMuscu);
    _benchButton->Update(playerHitbox, inMuscu);
    _bikeButton->Update(playerHitbox, inCardio);
    _treadmillButton->Update(playerHitbox, inCardio);

    // Ramassage des items au sol
    for (size_t i = 0; i < droppedItems.size(); ) {
        bool sameRoom = (droppedItems[i].room == _currentRoom) && canInteract;
        droppedItems[i].pickupButton.Update(playerHitbox, sameRoom);

        if (sameRoom && droppedItems[i].pickupButton.getIsTalking()) {
            inventory.AddItem(Item(droppedItems[i].name, droppedItems[i].type, droppedItems[i].texture, droppedItems[i].value, 1));
            sendCmd("take " + droppedItems[i].name);
            droppedItems.erase(droppedItems.begin() + i);
        } else {
            ++i;
        }
    }

    // Douche
    bool isShowering = _showerButton->getIsTalking();
    if (isShowering && !_wasShowering) {
        player.restoreEnergy(30);
        if (questLog.AdvanceQuest("take_shower", 1)) {
            player.addXp(40);
            sendCmd("quest complete take_shower");
        }
    }
    _wasShowering = isShowering;

    // Machines de sport
    auto checkMachine = [&](InteractiveButton& btn, ExerciseType type, const std::string& questId) {
        if (!btn.getIsTalking()) return;
        btn.setIsTalking(false);
        if (!miniGames.TryStartExercise(type, questId, player)) {
            lowEnergyTimer = 2.5f;
        }
    };
    checkMachine(*_pressButton, ExerciseType::SQUATS, "use_press");
    checkMachine(*_benchButton, ExerciseType::BENCH, "use_bench");
    checkMachine(*_bikeButton, ExerciseType::SPRINT, "use_bike");
    checkMachine(*_treadmillButton, ExerciseType::SPRINT, "use_treadmill");

    // Inscription accueil
    if (_npcButton->getIsTalking()) {
        if (questLog.AdvanceQuest("register_gym", 1)) {
            player.addXp(25);
            sendCmd("quest complete register_gym");
        }
    }

    // Ouverture Shop / Combat
    if (_shopButton->getIsTalking()) {
        _shopButton->setIsTalking(false);
        miniGames.OpenShop();
    }
    if (_fightButton->getIsTalking()) {
        _fightButton->setIsTalking(false);
        if (!miniGames.TryStartFight(player)) {
            lowEnergyTimer = 2.5f;
        }
    }

    // Déplacements & Collisions
    if (canInteract && !IsTalking()) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP)) {
            if (_framesCounter % 5 == 0) _currentFrame++;
        }

        std::vector<Rectangle> hitboxes;
        switch (_currentRoom) {
            case Rooms::Reception:  hitboxes = _reception->getHitboxes(); break;
            case Rooms::LockerRoom: hitboxes = _lockerRoom->getHitboxes(); break;
            case Rooms::Muscu:      hitboxes = _muscu->getHitboxes(); break;
            default:                hitboxes = _cardio->getHitboxes(); break;
        }

        auto checkWall = [&](float nx, float ny) {
            Rectangle future = GetFeetHitbox(nx, ny);
            for (const Rectangle& wall : hitboxes) {
                if (CheckCollisionRecs(future, wall)) return true;
            }
            return false;
        };

        if (IsKeyDown(KEY_RIGHT)) {
            if (!checkWall(_destRec.x + _speed, _destRec.y) && _destRec.x < screenWidth - 50) _destRec.x += _speed;
            _currentGuy = (_currentFrame % 3 == 1) ? _playerTextures.at("guy_right1") : ((_currentFrame % 3 == 2) ? _playerTextures.at("guy_right2") : _playerTextures.at("guy_right3"));
            _frameFaces = false;
        }
        if (IsKeyDown(KEY_LEFT)) {
            if (!checkWall(_destRec.x - _speed, _destRec.y) && _destRec.x > 3) _destRec.x -= _speed;
            _currentGuy = (_currentFrame % 3 == 1) ? _playerTextures.at("guy_left1") : ((_currentFrame % 3 == 2) ? _playerTextures.at("guy_left2") : _playerTextures.at("guy_left3"));
            _frameFaces = false;
        }
        if (IsKeyDown(KEY_DOWN)) {
            if (!checkWall(_destRec.x, _destRec.y + _speed) && _destRec.y < screenHeight - 170) _destRec.y += _speed;
            _currentGuy = _playerTextures.at("guy_front");
            _frameFaces = true;
        }
        if (IsKeyDown(KEY_UP)) {
            if (!checkWall(_destRec.x, _destRec.y - _speed) && _destRec.y > 10) _destRec.y -= _speed;
            _currentGuy = _playerTextures.at("guy_back");
            _frameFaces = true;
        }
    }

    HandleRoomTransitions();

    if (_currentRoom != _previousRoom) {
        sendCmd("move " + roomToProtocolName(_currentRoom));
        _previousRoom = _currentRoom;
    }

    if (_framesCounter >= (60 / _framesSpeed)) {
        _framesCounter = 0;
        int maxFrames = _frameFaces ? 2 : 3;
        if (_currentFrame >= maxFrames) _currentFrame = 0;
    }

    int maxFrames = _frameFaces ? 2 : 1;
    _frameRec.width = (float)_currentGuy.width / maxFrames;
    _frameRec.height = (float)_currentGuy.height;
    _frameRec.x = (float)_currentFrame * _frameRec.width;
    _destRec.width = _frameRec.width * _scale;
    _destRec.height = _frameRec.height * _scale;
}

void WorldController::HandleRoomTransitions() {
    switch (_currentRoom) {
        case Rooms::Reception:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::LockerRoom; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Muscu; _destRec.x = 5; }
            _scale = 0.6f;
            break;
        case Rooms::LockerRoom:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Pool; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Reception; _destRec.x = 220; _destRec.y = 250; }
            _scale = 0.6f;
            break;
        case Rooms::Pool:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Climbing; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::LockerRoom; _destRec.x = 5; }
            _scale = 0.6f;
            break;
        case Rooms::Climbing:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Sauna; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Pool; _destRec.x = 250; _destRec.y = 180; }
            _scale = 0.6f;
            break;
        case Rooms::Sauna:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Masage; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Climbing; _destRec.x = 5; }
            _scale = 0.8f;
            break;
        case Rooms::Masage:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Cardio; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Sauna; _destRec.x = 5; }
            _scale = 0.8f;
            break;
        case Rooms::Cardio:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Muscu; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Masage; _destRec.x = 5; }
            _scale = 0.6f;
            break;
        case Rooms::Muscu:
            if (_destRec.y < 160 && _destRec.x > 150 && _destRec.x < 270) { _currentRoom = Rooms::Reception; _destRec.x = 1650; _destRec.y = 270; }
            if (_destRec.y < 160 && _destRec.x > 1650 && _destRec.x < 1850) { _currentRoom = Rooms::Cardio; _destRec.x = 5; }
            _scale = 0.6f;
            break;
    }
}

void WorldController::DrawRoom(int screenWidth, int screenHeight) const {
    switch (_currentRoom) {
        case Rooms::Reception:
            DrawText("RECEPTION", screenWidth / 2, screenHeight / 2, 50, BLACK);
            _reception->draw();
            _npcButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            _shopButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            break;
        case Rooms::LockerRoom:
            DrawText("LockerRoom", screenWidth / 2, screenHeight / 2, 50, BLACK);
            _lockerRoom->draw();
            _showerButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            break;
        case Rooms::Muscu:
            DrawText("Muscu", screenWidth / 2, screenHeight / 2, 50, BLACK);
            _muscu->draw();
            _fightButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            _pressButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            _benchButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            break;
        case Rooms::Cardio:
            DrawText("Cardio", screenWidth / 2, screenHeight / 2, 50, BLACK);
            _cardio->draw();
            _bikeButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            _treadmillButton->Draw(_buttonsTex, _bframeWidth, _bframeHeight, _cropOffset);
            break;
        case Rooms::Climbing: _climbing->draw(); break;
        case Rooms::Pool:     _pool->draw(); break;
        case Rooms::Sauna:    _sauna->draw(); break;
        case Rooms::Masage:   _masage->draw(); break;
    }
}

void WorldController::DrawPlayerAndDialogs(int screenWidth, int screenHeight, bool hidePlayer, bool drawDialogs) const {
    if (!hidePlayer) {
        DrawTexturePro(_currentGuy, _frameRec, _destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
        DrawRectangleLinesEx(GetFeetHitbox(_destRec.x, _destRec.y), 2.0f, RED);
    }
    if (drawDialogs) {
        _npcButton->DrawDialog(screenWidth, screenHeight);
        _showerButton->DrawDialog(screenWidth, screenHeight);
    }
}