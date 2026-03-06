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
        auto& src = mGameMusic.getComponent<Strike::AudioSourceComponent>();
        src.play();
        mGameMusicPlaying = true;

        // Cache the track duration so we can detect when it finishes
        if (src.hasAudio() && src.getAudio() && src.getAudio()->isReady()) {
            mMusicDuration = src.getAudio()->getDuration();
        }
    }

    updateHUD();
}

void GameManager::onUpdate(float deltaTime) {
    if (mGameEnded) return;

    // Single audio read per frame - all consumers pull getRawAmplitude() and smooth locally
    if (mGameMusic.isValid()) {
        mRawAmplitude = Strike::Application::get().getAudioAmplitude(mGameMusic);
    }

    // Track elapsed music time and detect when the non-looping track finishes
    if (mGameMusicPlaying && mMusicDuration > 0.0f) {
        mMusicElapsed += deltaTime;
        if (mMusicElapsed >= mMusicDuration) {
            mGameMusicPlaying = false;
            notifyPlayerWon();
            return;
        }
    }

    Strike::Application::get().getWindow()
        .setWindowTitle("FPS: " + std::to_string(
            static_cast<int>(Strike::Application::get().getCurrentFPS())));
}

void GameManager::initGameData() {
    auto& data = Strike::GameData::get();
    data.setInt("lives", 50);
    data.setInt("score", 0);
    data.setBool("playerWon", false);

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

void GameManager::notifyPlayerWon() {
    mGameEnded = true;

    saveHighScore();

    auto& data = Strike::GameData::get();
    data.setBool("playerWon", true);

    int finalScore = data.getInt("score");

    if (mGameOverText.isValid()) {
        mGameOverText.setActive(true);
        mGameOverText.getComponent<Strike::TextComponent>().setText("YOU WIN!");
    }

    if (mFinalScoreText.isValid()) {
        mFinalScoreText.setActive(true);
        mFinalScoreText.getComponent<Strike::TextComponent>()
                       .setText("Score: " + std::to_string(finalScore));
    }

    Strike::World::get().loadScene("assets/scenes/menu.xml", false);
}

void GameManager::notifyGameOver() {
    if (mGameEnded) return;
    mGameEnded = true;

    saveHighScore();

    auto& data     = Strike::GameData::get();
    data.setBool("playerWon", false);
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