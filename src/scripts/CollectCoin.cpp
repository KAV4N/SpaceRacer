#include "CollectCoin.h"

void CollectCoin::onStart() {
    mScoreText = getEntity().getScene()->getEntity("ScoreText");

    if (!mScoreText) {
        STRIKE_WARN("CollectCoin: could not find ScoreText entity");
    }
}

void CollectCoin::onUpdate(float dt) {
    for (auto& other : scriptEntity.getCollidingEntities()) {
        if (!other.isValid()) continue;
        if (other.getTag() != "Coin") continue;

        if (mScoreText.isValid() && mScoreText.hasComponent<Strike::TextComponent>()) {
            auto& text = mScoreText.getComponent<Strike::TextComponent>();

            int score = Strike::GameData::get().getInt("score");
           
            text.setText("Score: " + std::to_string(Strike::GameData::get().setInt("score", score+mScoreValue)));
        }

        other.destroy();
        return;
    }
}

REGISTER_SCRIPT(CollectCoin)