#include "CameraFollow.h"

void CameraFollow::onStart() {
    mSpaceshipEntity = scriptEntity.getScene()->getEntity("Spaceship");
}

void CameraFollow::onUpdate(float deltaTime) {
    if (!mSpaceshipEntity.isValid()) return;

    glm::vec3 shipPos   = mSpaceshipEntity.getWorldPosition();
    glm::vec3 targetPos = shipPos + mOffset;

    scriptEntity.setWorldPosition(targetPos);
}

REGISTER_SCRIPT(CameraFollow)
