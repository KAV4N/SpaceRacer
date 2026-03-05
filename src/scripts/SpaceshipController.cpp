#include "SpaceshipController.h"
#include "DamageScript.h"

void SpaceshipController::onStart() {
    mCurrentTilt      = 0.0f;
    mCurrentSpeed     = kMinSpeed;
    mCurrentTurnSpeed = kMinTurnSpeed;

    auto model = Strike::AssetManager::get().getAsset<Strike::Model>("spaceship");
    if (model && model->isReady()) {
        Strike::PhysicsComponent& physics = scriptEntity.getComponent<Strike::PhysicsComponent>();
        Strike::Bounds bounds = model->getBounds();

        glm::vec3 scale    = scriptEntity.getWorldScale();
        glm::quat rot      = scriptEntity.getWorldRotation();
        glm::vec3 size     = bounds.getSize()     * scale;
        glm::vec3 midPoint = rot * (bounds.getMidPoint() * scale);

        physics.setSize(size);
        physics.setCenter(midPoint + glm::vec3(0.0f, 0.0f, 1.6f));
    }
}

void SpaceshipController::onUpdate(float deltaTime) {
    float t = (mCurrentSpeed - kMinSpeed) / (kMaxSpeed - kMinSpeed);

    mCurrentSpeed     = glm::min(mCurrentSpeed + kAccelRate * deltaTime, kMaxSpeed);
    mCurrentTurnSpeed = glm::mix(kMinTurnSpeed, kMaxTurnSpeed, t);

    bool left  = Strike::Input::isKeyPressed(STRIKE_KEY_LEFT);
    bool right = Strike::Input::isKeyPressed(STRIKE_KEY_RIGHT);

    glm::vec3 pos = scriptEntity.getWorldPosition();

    float finalTurn  = 0.0f;
    float targetTilt = 0.0f;

    if (left && pos.x > -kMaxX) {
        finalTurn  = -mCurrentTurnSpeed;
        targetTilt =  kMaxTiltAngle;
    }
    else if (right && pos.x < kMaxX) {
        finalTurn  =  mCurrentTurnSpeed;
        targetTilt = -kMaxTiltAngle;
    }

    mCurrentTilt += (targetTilt - mCurrentTilt) * (kTiltSpeed * deltaTime);

    DamageScript* dmg = nullptr;
    if (scriptEntity.hasComponent<Strike::LogicComponent>()) {
        dmg = scriptEntity.getComponent<Strike::LogicComponent>()
                          .getScript<DamageScript>();
    }

    bool isShaking = dmg && dmg->isShaking;

    float newX = pos.x + (finalTurn  * deltaTime);
    float newZ = pos.z - (mCurrentSpeed * deltaTime);

    if (!isShaking) {
        scriptEntity.setWorldPosition(glm::vec3(newX, 10.0f, newZ));
    } else {
        // Advance X and Z but leave Y to DamageScript — update shakeBase so
        // the shake offset stays relative to the ship's current travel position
        dmg->shakeBase = glm::vec3(newX, 10.0f, newZ);
        scriptEntity.setWorldPosition(glm::vec3(
            newX,
            scriptEntity.getWorldPosition().y,
            newZ
        ));
    }

    glm::vec3 currentRot = scriptEntity.getEulerAngles();
    scriptEntity.setEulerAngles(glm::vec3(currentRot.x, currentRot.y, 180.f + mCurrentTilt));
}

REGISTER_SCRIPT(SpaceshipController)