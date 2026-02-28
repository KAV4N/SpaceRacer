#include "SpaceshipController.h"

void SpaceshipController::onStart() {
    mHoverTime   = 0.0f;
    mCurrentTilt = 0.0f;
    auto model = Strike::AssetManager::get().getAsset<Strike::Model>("spaceship");
    if (model && model->isReady()){
        Strike::PhysicsComponent& physics = scriptEntity.getComponent<Strike::PhysicsComponent>();
        glm::vec3 size = model->getBounds().getSize()*scriptEntity.getScale();
        physics.setSize(glm::vec3(size.x,size.z,size.y)); // as the model was rotated by 90 degrees we need to swap
    }

}

void SpaceshipController::onUpdate(float deltaTime) {
   

    bool left  = Strike::Input::isKeyPressed(STRIKE_KEY_LEFT);
    bool right = Strike::Input::isKeyPressed(STRIKE_KEY_RIGHT);

    glm::vec3 pos = scriptEntity.getWorldPosition();

    float finalTurn = 0.0f;

    if (left && pos.x > -kMaxX)
        finalTurn = -mTurnSpeed;

    if (right && pos.x < kMaxX)
        finalTurn = mTurnSpeed;

    scriptEntity.move(glm::vec3(finalTurn, 0.0f, -mFwdSpeed) * deltaTime);

    float targetTilt = 0.0f;
    if (left  && pos.x > -kMaxX) targetTilt =  kMaxTiltAngle;
    if (right && pos.x <  kMaxX) targetTilt = -kMaxTiltAngle;

    mCurrentTilt += (targetTilt - mCurrentTilt) * (kTiltSpeed * deltaTime);

    float hoverOffset = 0.0f;
    mHoverTime += deltaTime;
    if (!left && !right)
        hoverOffset = glm::sin(mHoverTime * kHoverFrequency) * kHoverAmplitude;

    glm::vec3 currentRot = scriptEntity.getEulerAngles();
    scriptEntity.setEulerAngles(glm::vec3(currentRot.x, currentRot.y, mCurrentTilt));

    if (!mBaseYInitialized) {
        mBaseY = pos.y;
        mBaseYInitialized = true;
    }

    glm::vec3 newPos = scriptEntity.getWorldPosition();
    newPos.y = mBaseY + hoverOffset;
    scriptEntity.setWorldPosition(newPos);
}

REGISTER_SCRIPT(SpaceshipController)