#include "SpaceshipController.h"
#include "GameManager.h"

void SpaceshipController::onStart() {
    mCurrentTilt = 0.0f;

    glm::vec3 pos = scriptEntity.getWorldPosition();
    scriptEntity.setWorldPosition(glm::vec3(pos.x, 10.0f, pos.z));

    auto model = Strike::AssetManager::get().getAsset<Strike::Model>("spaceship");
    if (!model || !model->isReady()) return;

    auto& physics   = scriptEntity.getComponent<Strike::PhysicsComponent>();
    glm::vec3 scale = scriptEntity.getWorldScale();
    glm::quat rot   = scriptEntity.getWorldRotation();
    Strike::Bounds bounds = model->getBounds();

    glm::vec3 size     = bounds.getSize()     * scale;
    glm::vec3 midPoint = rot * (bounds.getMidPoint() * scale);

    physics.setSize(size);
    physics.setCenter(midPoint + glm::vec3(0.0f, 0.0f, 1.6f));

    Strike::Entity gmEntity = scriptEntity.getScene()->getEntity("GameManager");
    if (gmEntity.isValid() && gmEntity.hasComponent<Strike::LogicComponent>()) {
        mGameManager = gmEntity.getComponent<Strike::LogicComponent>()
                               .getScript<GameManager>();
    }

    if (!mGameManager) {
        STRIKE_WARN("SpaceshipController: GameManager not found");
    }
}

void SpaceshipController::onUpdate(float deltaTime) {
    // Smooth the raw amplitude locally with own tuning params
    if (mGameManager) {
        float raw         = mGameManager->getRawAmplitude();
        float smoothSpeed = (raw > mSmoothedAmplitude) ? mSmoothAttack : mSmoothRelease;
        mSmoothedAmplitude = glm::mix(mSmoothedAmplitude, raw,
                                      glm::clamp(smoothSpeed * deltaTime, 0.0f, 1.0f));
    }

    float t            = glm::clamp(mSmoothedAmplitude / glm::max(mPeakRMS, 0.001f), 0.0f, 1.0f);
    float currentSpeed = glm::mix(mMinSpeed,     mMaxSpeed,     t);
    float turnSpeed    = glm::mix(mMinTurnSpeed, mMaxTurnSpeed, t);
    float tiltSpeed    = glm::mix(mMinTiltSpeed, mMaxTiltSpeed, t);

    bool movingLeft  = Strike::Input::isKeyPressed(STRIKE_KEY_LEFT);
    bool movingRight = Strike::Input::isKeyPressed(STRIKE_KEY_RIGHT);

    glm::vec3 pos = scriptEntity.getWorldPosition();

    float turnDelta  = 0.0f;
    float targetTilt = 0.0f;

    if (movingLeft && pos.x > -kMaxX) {
        turnDelta  = -turnSpeed;
        targetTilt =  kMaxTiltAngle;
    } else if (movingRight && pos.x < kMaxX) {
        turnDelta  =  turnSpeed;
        targetTilt = -kMaxTiltAngle;
    }

    mCurrentTilt += (targetTilt - mCurrentTilt) * (tiltSpeed * deltaTime);

    float newX = pos.x + (turnDelta    * deltaTime);
    float newZ = pos.z - (currentSpeed * deltaTime);

    scriptEntity.setWorldPosition(glm::vec3(newX, scriptEntity.getWorldPosition().y, newZ));

    glm::vec3 currentAngles = scriptEntity.getEulerAngles();
    scriptEntity.setEulerAngles(glm::vec3(currentAngles.x, currentAngles.y, 180.0f + mCurrentTilt));
}

REGISTER_SCRIPT(SpaceshipController)