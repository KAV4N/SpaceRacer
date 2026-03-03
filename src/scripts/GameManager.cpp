#include "GameManager.h"

static void setAudioPlaying(Strike::Entity& entity, bool play) {
    if (!entity.isValid()) return;
    if (!entity.hasComponent<Strike::AudioSourceComponent>()) return;
    auto& src = entity.getComponent<Strike::AudioSourceComponent>();
    if (play) src.play();
    else      src.stop();
}

void GameManager::onStart() {
    auto* scene = getEntity().getScene();

    // UI
    mScoreText      = scene->getEntity("ScoreText");
    mLivesText      = scene->getEntity("LivesText");
    mPressSpaceText = scene->getEntity("PressSpaceText");
    mEndText        = scene->getEntity("EndText");
    mEndScoreText   = scene->getEntity("EndScoreText");

    // Audio
    mAmbientMusic = scene->getEntity("AmbientMusic");
    mGameMusic    = scene->getEntity("GameMusic");

    // Gameplay
    mSpaceship         = scene->getEntity("Spaceship");
    mGameManagerEntity = scene->getEntity("GameManager");

    setAudioPlaying(mAmbientMusic, true);

    enterMenu();
}

void GameManager::onUpdate(float deltaTime) {
    if (getState() == GameState::Menu) {
        bool spaceNow = Strike::Input::isKeyPressed(STRIKE_KEY_SPACE);
        if (spaceNow && !mSpaceWasPressed) {
            enterPlaying();
        }
        mSpaceWasPressed = spaceNow;
    }
}

// ─────────────────────────────────────────────
//  State transitions
// ─────────────────────────────────────────────
void GameManager::enterMenu() {
    auto& data = Strike::GameData::get();
    data.setInt(kKeyState, static_cast<int>(GameState::Menu));
    data.setInt(kKeyLives, 3);

    mScore = 0;

    // UI – show prompt, hide HUD
    if (mPressSpaceText.isValid()) mPressSpaceText.setActive(true);
    if (mScoreText.isValid())      mScoreText.setActive(false);
    if (mLivesText.isValid())      mLivesText.setActive(false);
    if (mEndText.isValid())        mEndText.setActive(false);
    if (mEndScoreText.isValid())   mEndScoreText.setActive(false);

    // Remove SpaceshipController if present from a previous session
    if (mSpaceship.isValid() && mSpaceship.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mSpaceship.getComponent<Strike::LogicComponent>();
        if (logic.hasScript<SpaceshipController>()) {
            logic.removeScript<SpaceshipController>();
        }
    }

    // Disable LevelGenerator
    if (mGameManagerEntity.isValid() && mGameManagerEntity.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mGameManagerEntity.getComponent<Strike::LogicComponent>();
        auto* gen = logic.getScript<LevelGenerator>();
        if (gen) gen->setActive(false);
    }

    setAudioPlaying(mGameMusic, false);
}

void GameManager::enterPlaying() {
    auto& data = Strike::GameData::get();
    data.setInt(kKeyState, static_cast<int>(GameState::Playing));
    data.setInt(kKeyLives, 3);

    mScore = 0;

    // UI – hide prompt, show HUD
    if (mPressSpaceText.isValid()) mPressSpaceText.setActive(false);
    if (mScoreText.isValid()) {
        mScoreText.setActive(true);
        mScoreText.getComponent<Strike::TextComponent>().setText("Score: 0");
    }
    if (mLivesText.isValid()) {
        mLivesText.setActive(true);
        mLivesText.getComponent<Strike::TextComponent>()
                  .setText("Lives: " + std::to_string(data.getInt(kKeyLives)));
    }
    if (mEndText.isValid())      mEndText.setActive(false);
    if (mEndScoreText.isValid()) mEndScoreText.setActive(false);

    // Add SpaceshipController
    if (mSpaceship.isValid()) {
        auto& logic = mSpaceship.getOrAddComponent<Strike::LogicComponent>();
        if (!logic.hasScript<SpaceshipController>()) {
            logic.addScript<SpaceshipController>();
        }
    }

    // Enable LevelGenerator
    if (mGameManagerEntity.isValid() && mGameManagerEntity.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mGameManagerEntity.getComponent<Strike::LogicComponent>();
        auto* gen = logic.getScript<LevelGenerator>();
        if (gen) gen->setActive(true);
    }

    setAudioPlaying(mGameMusic, true);
}

REGISTER_SCRIPT(GameManager)