#include "SpaceshipController.h"

void SpaceshipController::onStart() {
    mCurrentTilt      = 0.0f;
    mCurrentSpeed     = kMinSpeed;
    mCurrentTurnSpeed = kMinTurnSpeed;

    auto model = Strike::AssetManager::get().getAsset<Strike::Model>("spaceship");
    if (model && model->isReady()){
        Strike::PhysicsComponent& physics = scriptEntity.getComponent<Strike::PhysicsComponent>();
        Strike::Bounds bounds = model->getBounds();

        glm::vec3 scale = scriptEntity.getScale();
        glm::vec3 size = bounds.getSize() * scale;
        glm::vec3 midPoint = bounds.getMidPoint() * scale;

        physics.setSize(glm::vec3(size.x, size.z, size.y));
        physics.setCenter(glm::vec3(midPoint.x, midPoint.z, midPoint.y));
    }
}

void SpaceshipController::onUpdate(float deltaTime) {

    // Speed and turn speed scale together so handling feels consistent at all speeds
    float t = (mCurrentSpeed - kMinSpeed) / (kMaxSpeed - kMinSpeed);

    mCurrentSpeed     = glm::min(mCurrentSpeed     + kAccelRate * deltaTime, kMaxSpeed);
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
    glm::vec3 currentRot = scriptEntity.getEulerAngles();

    scriptEntity.setWorldPosition(glm::vec3(
        pos.x + (finalTurn * deltaTime),
        10.0f,
        pos.z - (mCurrentSpeed * deltaTime)
    ));
    scriptEntity.setEulerAngles(glm::vec3(currentRot.x, currentRot.y, mCurrentTilt));
}

REGISTER_SCRIPT(SpaceshipController)