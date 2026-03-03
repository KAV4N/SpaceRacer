#pragma once
#include "StrikeEngine.h"
#include "SpaceshipController.h"
#include "LevelGenerator.h"

enum class GameState {
    Menu,
    Playing
};

// GameData keys
static constexpr const char* kKeyLives = "lives";
static constexpr const char* kKeyState = "state";  // 0 = Menu, 1 = Playing

class GameManager : public Strike::Script {
public:
    void onStart()  override;
    void onUpdate(float deltaTime) override;

    GameState getState() const {
        return static_cast<GameState>(Strike::GameData::get().getInt(kKeyState, 0));
    }

private:
    void enterMenu();
    void enterPlaying();

    // UI entities
    Strike::Entity mScoreText;
    Strike::Entity mLivesText;
    Strike::Entity mPressSpaceText;
    Strike::Entity mEndText;
    Strike::Entity mEndScoreText;

    // Audio entities
    Strike::Entity mAmbientMusic;
    Strike::Entity mGameMusic;

    // Gameplay entities
    Strike::Entity mSpaceship;
    Strike::Entity mGameManagerEntity;

    int mScore = 0;

    bool mSpaceWasPressed = false;
};