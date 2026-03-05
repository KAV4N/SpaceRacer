#include "DamageScript.h"

static void collectRenderersRecursive(Strike::Entity entity,
                                      std::vector<Strike::Entity>& outEntities)
{
    if (!entity.isValid()) return;

    if (entity.hasComponent<Strike::RendererComponent>()) {
        outEntities.push_back(entity);
    }

    for (auto& child : entity.getChildren()) {
        collectRenderersRecursive(child, outEntities);
    }
}

void DamageScript::onStart() {
    collectRenderers();

    auto gmEntity = getEntity().getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("DamageScript: could not find GameManager script");
    }
}

void DamageScript::collectRenderers() {
    mRenderEntities.clear();
    mOriginalColors.clear();

    collectRenderersRecursive(scriptEntity, mRenderEntities);

    for (auto& e : mRenderEntities) {
        if (e.isValid() && e.hasComponent<Strike::RendererComponent>()) {
            mOriginalColors.push_back(
                e.getComponent<Strike::RendererComponent>().getColor()
            );
        } else {
            mOriginalColors.push_back(glm::uvec3(255, 255, 255));
        }
    }
}

void DamageScript::onUpdate(float dt) {
    int state = Strike::GameData::get().getInt(kKeyState);
    if (state != static_cast<int>(GameState::Playing)) {
        if (mIsInvulnerable) {
            mIsInvulnerable = false;
            restoreOriginalColors();
        }
        if (isShaking) {
            isShaking = false;
            scriptEntity.setWorldPosition(shakeBase);
        }
        return;
    }

    if (!mIsInvulnerable) {
        for (auto& other : scriptEntity.getCollidingEntities()) {
            if (!other.isValid()) continue;
            if (other.getTag() == "Obstacle") {
                applyDamage();
                break;
            }
        }
    }

    updateShake(dt);

    if (!mIsInvulnerable) return;

    mInvulnerableTimer += dt;
    mBlinkTimer        += dt;

    float blinkInterval = 1.0f / glm::max(mBlinkFrequency, 0.1f);
    if (mBlinkTimer >= blinkInterval) {
        mBlinkTimer -= blinkInterval;
        mBlinkState  = !mBlinkState;

        applyBlinkColor(mBlinkState
            ? glm::uvec3(255, 255, 255)
            : glm::uvec3(255,  30,  30));
    }

    if (mInvulnerableTimer >= mInvulnerabilityDuration) {
        mIsInvulnerable    = false;
        mInvulnerableTimer = 0.0f;
        mBlinkTimer        = 0.0f;
        mBlinkState        = false;
        restoreOriginalColors();
    }
}

void DamageScript::updateShake(float dt) {
    if (!isShaking) return;

    mShakeTimer += dt;
    mShakePhase += dt * mShakeFrequency;

    if (mShakeTimer >= mShakeDuration) {
        isShaking  = false;
        mShakeTimer = 0.0f;
        mShakePhase = 0.0f;
        scriptEntity.setWorldPosition(shakeBase);
        return;
    }

    float envelope = 1.0f - (mShakeTimer / mShakeDuration);
    float offsetY  = glm::sin(mShakePhase) * mShakeMagnitude * envelope;

    scriptEntity.setWorldPosition(shakeBase + glm::vec3(0.0f, offsetY, 0.0f));
}

void DamageScript::applyDamage() {
    auto& data = Strike::GameData::get();

    int lives = glm::max(data.getInt(kKeyLives) - 1, 0);
    data.setInt(kKeyLives, lives);

    STRIKE_INFO("DamageScript: hit! lives remaining = {}", lives);

    if (mGameManager) {
        Strike::Entity livesText = mGameManager->getLivesTextEntity();
        if (livesText.isValid() && livesText.hasComponent<Strike::TextComponent>()) {
            livesText.getComponent<Strike::TextComponent>()
                     .setText("Lives: " + std::to_string(lives));
        }
    }

    isShaking  = true;
    mShakeTimer = 0.0f;
    mShakePhase = 0.0f;
    shakeBase  = scriptEntity.getWorldPosition();

    if (lives <= 0) {
        isShaking = false;
        scriptEntity.setWorldPosition(shakeBase);
        restoreOriginalColors();
        if (mGameManager) {
            mGameManager->notifyGameOver();
        }
        return;
    }

    mIsInvulnerable    = true;
    mInvulnerableTimer = 0.0f;
    mBlinkTimer        = 0.0f;
    mBlinkState        = false;
}

void DamageScript::applyBlinkColor(const glm::uvec3& color) {
    for (auto& e : mRenderEntities) {
        if (e.isValid() && e.hasComponent<Strike::RendererComponent>()) {
            e.getComponent<Strike::RendererComponent>().setColor(color);
        }
    }
}

void DamageScript::restoreOriginalColors() {
    for (size_t i = 0; i < mRenderEntities.size(); ++i) {
        auto& e = mRenderEntities[i];
        if (e.isValid() && e.hasComponent<Strike::RendererComponent>()) {
            e.getComponent<Strike::RendererComponent>().setColor(mOriginalColors[i]);
        }
    }
}

REGISTER_SCRIPT(DamageScript)