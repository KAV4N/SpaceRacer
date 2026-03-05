#include "MenuManager.h"

void MenuManager::onStart() {
    mAmbientMusic  = scriptEntity.getScene()->getEntity("AmbientMusic");
    mHighScoreText = scriptEntity.getScene()->getEntity("HighScoreText");

    if (mAmbientMusic.isValid() && mAmbientMusic.hasComponent<Strike::AudioSourceComponent>()) {
        mAmbientMusic.getComponent<Strike::AudioSourceComponent>().play();
    }

    if (mHighScoreText.isValid() && mHighScoreText.hasComponent<Strike::TextComponent>()) {
        int best = 0;
        if (Strike::GameData::get().hasKey("bestScore")) {
            best = Strike::GameData::get().getInt("bestScore");
        }
        mHighScoreText.getComponent<Strike::TextComponent>()
                      .setText("Best: " + std::to_string(best));
    }
}

void MenuManager::onUpdate(float deltaTime) {
    Strike::Application::get().getWindow()
        .setWindowTitle("FPS: " + std::to_string(
            static_cast<int>(Strike::Application::get().getCurrentFPS())));
}

void MenuManager::onEvent(Strike::Event& event) {
    if (event.getEventType() != Strike::EventType::KeyPressed) return;

    auto& keyEvent = static_cast<Strike::KeyPressedEvent&>(event);
    if (keyEvent.getKeyCode() != STRIKE_KEY_SPACE) return;

    Strike::World::get().loadScene("assets/scenes/game.xml", false);
    event.handled = true;
}

REGISTER_SCRIPT(MenuManager)