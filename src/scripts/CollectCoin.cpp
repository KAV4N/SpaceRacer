#include "CollectCoin.h"

void CollectCoin::onStart() {
    mScoreText = getEntity().getScene()->getEntity("ScoreText");

    if (!mScoreText.isValid()) {
        STRIKE_WARN("CollectCoin: ScoreText entity not found");
    }
}

void CollectCoin::onUpdate(float deltaTime) {
    for (auto& other : scriptEntity.getCollidingEntities()) {
        if (!other.isValid() || other.getTag() != "Coin") continue;

        auto& data = Strike::GameData::get();
        int newScore = data.getInt("score") + mScoreValue;
        data.setInt("score", newScore);

        if (mScoreText.isValid() && mScoreText.hasComponent<Strike::TextComponent>()) {
            mScoreText.getComponent<Strike::TextComponent>()
                      .setText("Score: " + std::to_string(newScore));
        }
        Strike::Entity cameraEnt = scriptEntity.getScene()->getEntity("MainCamera");
        if (cameraEnt.hasComponent<Strike::AudioSourceComponent>()) {
            cameraEnt.getComponent<Strike::AudioSourceComponent>().play();
        }

        other.destroy();
        return;
    }
}

REGISTER_SCRIPT(CollectCoin)