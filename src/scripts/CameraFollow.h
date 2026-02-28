#pragma once
#include "StrikeEngine.h"

class CameraFollow : public Strike::Script {
public:
    void onStart() override;
    void onUpdate(float deltaTime) override;

private:
    Strike::Entity mSpaceshipEntity;
    glm::vec3 mOffset     = glm::vec3(0.0f, 8.0f, 20.0f);
};
