#include "DamageScript.h"
#include "GameManager.h"

void DamageScript::collectRenderersRecursive(Strike::Entity entity,
                                             std::vector<Strike::Entity>& out) {
    if (!entity.isValid()) return;

    if (entity.hasComponent<Strike::RendererComponent>()) {
        out.push_back(entity);
    }

    for (auto& child : entity.getChildren()) {
        collectRenderersRecursive(child, out);
    }
}

void DamageScript::onStart() {
    collectRenderers();

    Strike::Entity gmEntity = getEntity().getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("DamageScript: GameManager not found");
    }
}

void DamageScript::collectRenderers() {
    mRenderEntities.clear();
    mOriginalColors.clear();

    collectRenderersRecursive(scriptEntity, mRenderEntities);

    for (auto& entity : mRenderEntities) {
        if (entity.isValid() && entity.hasComponent<Strike::RendererComponent>()) {
            mOriginalColors.push_back(
                entity.getComponent<Strike::RendererComponent>().getColor());
        } else {
            mOriginalColors.push_back(glm::uvec3(255, 255, 255));
        }
    }
}

void DamageScript::onUpdate(float deltaTime) {
    if (!mIsInvulnerable) {
        for (auto& other : scriptEntity.getCollidingEntities()) {
            if (other.isValid() && other.getTag() == "Obstacle") {
                applyDamage();
                break;
            }
        }
    }

    if (!mIsInvulnerable) return;

    if (tick(mBlinkInterval)) {
        mBlinkState = !mBlinkState;
        applyBlinkColor(mBlinkState ? glm::uvec3(255, 255, 255) : glm::uvec3(255, 30, 30));
    }

    if (tick(mInvulnerabilityDuration)) {
        mIsInvulnerable = false;
        mBlinkState     = false;
        restoreOriginalColors();
    }
}

void DamageScript::applyDamage() {
    auto& data = Strike::GameData::get();
    int lives  = glm::max(data.getInt("lives") - 1, 0);
    data.setInt("lives", lives);

    if (mGameManager) {
        Strike::Entity livesText = mGameManager->getLivesTextEntity();
        if (livesText.isValid() && livesText.hasComponent<Strike::TextComponent>()) {
            livesText.getComponent<Strike::TextComponent>()
                     .setText("HP: " + std::to_string(lives));
        }
    }

    if (hasComponent<Strike::AudioSourceComponent>()) {
        getComponent<Strike::AudioSourceComponent>().play();
    }

    if (lives <= 0) {
        restoreOriginalColors();
        if (mGameManager) {
            mGameManager->notifyGameOver();
        }
        return;
    }

    mIsInvulnerable = true;
    mBlinkState     = false;
}

void DamageScript::applyBlinkColor(const glm::uvec3& color) {
    for (auto& entity : mRenderEntities) {
        if (entity.isValid() && entity.hasComponent<Strike::RendererComponent>()) {
            entity.getComponent<Strike::RendererComponent>().setColor(color);
        }
    }
}

void DamageScript::restoreOriginalColors() {
    for (size_t i = 0; i < mRenderEntities.size(); ++i) {
        auto& entity = mRenderEntities[i];
        if (entity.isValid() && entity.hasComponent<Strike::RendererComponent>()) {
            entity.getComponent<Strike::RendererComponent>().setColor(mOriginalColors[i]);
        }
    }
}

REGISTER_SCRIPT(DamageScript)