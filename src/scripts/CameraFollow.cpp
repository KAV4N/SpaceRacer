#include "CameraFollow.h"

void CameraFollow::onStart() {
    mSpaceship = scriptEntity.getScene()->getEntity("Spaceship");
}

void CameraFollow::onUpdate(float deltaTime) {
    if (!mSpaceship.isValid()) return;

    scriptEntity.setWorldPosition(mSpaceship.getWorldPosition() + mOffset);
}

REGISTER_SCRIPT(CameraFollow)