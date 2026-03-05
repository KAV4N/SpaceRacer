#include "GameManager.h"

void GameManager::onStart() {
    auto* scene = getEntity().getScene();

    mScoreText      = scene->getEntity("ScoreText");
    mLivesText      = scene->getEntity("LivesText");
    mGameOverText   = scene->getEntity("GameOverText");
    mFinalScoreText = scene->getEntity("FinalScoreText");
    mAmbientMusic   = scene->getEntity("AmbientMusic");
    mGameMusic      = scene->getEntity("GameMusic");

    initGameData();

    if (mAmbientMusic.isValid() && mAmbientMusic.hasComponent<Strike::AudioSourceComponent>()) {
        mAmbientMusic.getComponent<Strike::AudioSourceComponent>().play();
    }

    if (mGameMusic.isValid() && mGameMusic.hasComponent<Strike::AudioSourceComponent>()) {
        mGameMusic.getComponent<Strike::AudioSourceComponent>().play();
    }

    updateHUD();
}

void GameManager::onUpdate(float deltaTime) {
    // Single audio read per frame - all consumers pull getRawAmplitude() and smooth locally
    if (mGameMusic.isValid()) {
        mRawAmplitude = Strike::Application::get().getAudioAmplitude(mGameMusic);
    }

    Strike::Application::get().getWindow()
        .setWindowTitle("FPS: " + std::to_string(
            static_cast<int>(Strike::Application::get().getCurrentFPS())));
}

void GameManager::initGameData() {
    auto& data = Strike::GameData::get();
    data.setInt("lives", 50);
    data.setInt("score", 0);

    if (!data.hasKey("bestScore")) {
        data.setInt("bestScore", 0);
    }
}

void GameManager::updateHUD() {
    auto& data = Strike::GameData::get();

    if (mScoreText.isValid() && mScoreText.hasComponent<Strike::TextComponent>()) {
        mScoreText.getComponent<Strike::TextComponent>()
                  .setText("Score: " + std::to_string(data.getInt("score")));
    }

    if (mLivesText.isValid() && mLivesText.hasComponent<Strike::TextComponent>()) {
        mLivesText.getComponent<Strike::TextComponent>()
                  .setText("HP: " + std::to_string(data.getInt("lives")));
    }
}

void GameManager::saveHighScore() {
    auto& data = Strike::GameData::get();
    int score  = data.getInt("score");
    int best   = data.hasKey("bestScore") ? data.getInt("bestScore") : 0;

    if (score > best) {
        data.setInt("bestScore", score);
    }
}

void GameManager::notifyGameOver() {
    saveHighScore();

    auto& data     = Strike::GameData::get();
    int finalScore = data.getInt("score");

    if (mGameMusic.isValid() && mGameMusic.hasComponent<Strike::AudioSourceComponent>()) {
        mGameMusic.getComponent<Strike::AudioSourceComponent>().stop();
    }

    if (mGameOverText.isValid()) {
        mGameOverText.setActive(true);
        mGameOverText.getComponent<Strike::TextComponent>().setText("GAME OVER");
    }

    if (mFinalScoreText.isValid()) {
        mFinalScoreText.setActive(true);
        mFinalScoreText.getComponent<Strike::TextComponent>()
                       .setText("Score: " + std::to_string(finalScore));
    }

    Strike::World::get().loadScene("assets/scenes/menu.xml", false);
}

REGISTER_SCRIPT(GameManager)