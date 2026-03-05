#pragma once
#include "StrikeEngine.h"
#include "SpaceshipController.h"
#include "LevelGenerator.h"

enum class GameState {
    Menu,
    Playing
};

static constexpr const char* kKeyLives = "lives";
static constexpr const char* kKeyState = "state";
static constexpr const char* kKeyScore = "score";

class GameManager : public Strike::Script {
public:
    void onStart()  override;
    void onUpdate(float deltaTime) override;
    void onEvent(Strike::Event& event) override;

    GameState getState() const {
        return static_cast<GameState>(Strike::GameData::get().getInt(kKeyState));
    }

    Strike::Entity getLivesTextEntity() const { return mLivesText; }

    void notifyGameOver();

private:
    void enterMenu();
    void enterPlaying();
    void hardReset();

    Strike::Entity mScoreText;
    Strike::Entity mLivesText;
    Strike::Entity mPressSpaceText;
    Strike::Entity mEndText;
    Strike::Entity mEndScoreText;

    Strike::Entity mAmbientMusic;
    Strike::Entity mGameMusic;

    Strike::Entity mSpaceship;
    Strike::Entity mGameManagerEntity;

    bool mSpaceWasPressed = false;
};