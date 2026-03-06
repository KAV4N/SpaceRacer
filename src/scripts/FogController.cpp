#include "FogController.h"
#include "GameManager.h"

void FogController::onStart() {
    Strike::Entity gmEntity = getEntity().getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("FogController: GameManager not found");
    }

    Strike::World::get().setFog(mFogStartQuiet, mFogEndQuiet, mFogDensityQuiet, mFogColor);
}

void FogController::onUpdate(float deltaTime) {
    if (mGameManager) {
        float raw         = mGameManager->getRawAmplitude();
        float smoothSpeed = (raw > mSmoothedAmplitude) ? mSmoothAttack : mSmoothRelease;
        mSmoothedAmplitude = glm::mix(mSmoothedAmplitude, raw,
                                      glm::clamp(smoothSpeed * deltaTime, 0.0f, 1.0f));
    }

    float t = glm::clamp(mSmoothedAmplitude / glm::max(mPeakRMS, 0.001f), 0.0f, 1.0f);

    float density = glm::mix(mFogDensityQuiet, mFogDensityLoud, t);
    float start   = glm::mix(mFogStartQuiet,   mFogStartLoud,   t);
    float end     = glm::mix(mFogEndQuiet,     mFogEndLoud,     t);

    Strike::World::get().setFog(start, end, density, mFogColor);
}

REGISTER_SCRIPT(FogController)