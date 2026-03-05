#include "GameManager.h"
#include "DamageScript.h"
#include "CollectCoin.h"

static void setAudioPlaying(Strike::Entity& entity, bool play) {
    if (!entity.isValid()) return;
    if (!entity.hasComponent<Strike::AudioSourceComponent>()) return;
    auto& src = entity.getComponent<Strike::AudioSourceComponent>();
    if (play) src.play();
    else      src.stop();
}

void GameManager::onStart() {
    auto* scene = getEntity().getScene();

    mScoreText      = scene->getEntity("ScoreText");
    mLivesText      = scene->getEntity("LivesText");
    mPressSpaceText = scene->getEntity("PressSpaceText");
    mEndText        = scene->getEntity("EndText");
    mEndScoreText   = scene->getEntity("EndScoreText");

    mAmbientMusic = scene->getEntity("AmbientMusic");
    mGameMusic    = scene->getEntity("GameMusic");

    mSpaceship         = scene->getEntity("Spaceship");
    mGameManagerEntity = scene->getEntity("GameManager");

    setAudioPlaying(mAmbientMusic, true);

    enterMenu();
}

void GameManager::onUpdate(float deltaTime) {
    auto& window = Strike::Application::get().getWindow();
    window.setWindowTitle("FPS: " + std::to_string(Strike::Application::get().getCurrentFPS()));
}

void GameManager::onEvent(Strike::Event& event) {
    if (event.getEventType() != Strike::EventType::KeyPressed) return;
    auto& e = static_cast<Strike::KeyPressedEvent&>(event);
    if (e.getKeyCode() == STRIKE_KEY_SPACE && getState() == GameState::Menu) {
        enterPlaying();
    }
}

void GameManager::notifyGameOver() {
    hardReset();
    enterMenu();
}

void GameManager::hardReset() {
    setAudioPlaying(mGameMusic, false);

    auto& data = Strike::GameData::get();
    data.setInt(kKeyState, static_cast<int>(GameState::Menu));
    data.setInt(kKeyLives, 30);
    data.setInt(kKeyScore, 0);

    if (mSpaceship.isValid() && mSpaceship.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mSpaceship.getComponent<Strike::LogicComponent>();

        if (logic.hasScript<SpaceshipController>()) {
            logic.removeScript<SpaceshipController>();
        }

        if (logic.hasScript<DamageScript>()) {
            logic.removeScript<DamageScript>();
        }
    }

    if (mGameManagerEntity.isValid() && mGameManagerEntity.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mGameManagerEntity.getComponent<Strike::LogicComponent>();

        if (logic.hasScript<LevelGenerator>()) {
            logic.removeScript<LevelGenerator>();
            logic.addScript<LevelGenerator>();
        }

        auto* gen = logic.getScript<LevelGenerator>();
        if (gen) gen->setActive(false);
    }
}

void GameManager::enterMenu() {
    auto& data = Strike::GameData::get();
    data.setInt(kKeyState, static_cast<int>(GameState::Menu));
    data.setInt(kKeyLives, 30);
    data.setInt(kKeyScore, 0);

    if (mPressSpaceText.isValid()) mPressSpaceText.setActive(true);
    if (mScoreText.isValid())      mScoreText.setActive(false);
    if (mLivesText.isValid())      mLivesText.setActive(false);
    if (mEndText.isValid())        mEndText.setActive(false);
    if (mEndScoreText.isValid())   mEndScoreText.setActive(false);
}

void GameManager::enterPlaying() {
    auto& data = Strike::GameData::get();
    data.setInt(kKeyState, static_cast<int>(GameState::Playing));
    data.setInt(kKeyLives, 30);
    data.setInt(kKeyScore, 0);

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

    if (mSpaceship.isValid()) {
        auto& logic = mSpaceship.getOrAddComponent<Strike::LogicComponent>();

        if (!logic.hasScript<SpaceshipController>()) {
            logic.addScript<SpaceshipController>();
        }

        if (!logic.hasScript<DamageScript>()) {
            logic.addScript<DamageScript>();
        }

        if (!logic.hasScript<CollectCoin>()) {
            logic.addScript<CollectCoin>();
        }
    }

    if (mGameManagerEntity.isValid() && mGameManagerEntity.hasComponent<Strike::LogicComponent>()) {
        auto& logic = mGameManagerEntity.getComponent<Strike::LogicComponent>();
        auto* gen = logic.getScript<LevelGenerator>();
        if (gen) gen->setActive(true);
    }

    setAudioPlaying(mGameMusic, true);
}

REGISTER_SCRIPT(GameManager)